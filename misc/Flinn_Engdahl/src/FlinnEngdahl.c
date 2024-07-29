/***********************************************************************************/
/*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
/***                                                                             ***/
/*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
/*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
/*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
/*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
/*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
/*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
/*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
/*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
/*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
/*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
/***                                                                             ***/
/*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
/***********************************************************************************/

/********************************************************************************************/
/********************************************************************************************
FlinnEngdahl.c Written by Gene Ichinose 2008 
FlinnEngdahl.c - returns Flinn-Engdahl Region name from decimal 
lon,lat values given on command line.

This gets used by updateMT.c through external system calls to lookup MT solution in sqlite database



Flinn-Engdahl seismic and geographic regionalization databases (1995 revision)
Originally written in Fortran by G.J. Dunphy, and  modified August 18, 1997 by BPresgrave
Translated to C by G.Ichinose from PERL written by 
- Bob Simpson January, 2003  <simpson@usgs.gov>
  With fix supplied by George Randall <ger@geophysics.lanl.gov>  2003-02-03
********************************************************************************************/
/********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#define NE 1
#define NW 2
#define SE 3
#define SW 4

typedef struct { 
	int index;
	char names[128];
} FE_Regions;

typedef struct {
	int index[92];   /*** number from quadsidx.asc ***/
	char quad[3];    /*** quad  ne,nw,se,sw ***/
	int iquad;       /*** iquad  1, 2, 3, 4 ***/
	int begin[92];   /*** begin[i] = end[i] + 1; ***/
	int end[92];     /*** end[i] += quadindx[i]; ***/
	int subpos[92];  /*** 1 to 91 ***/
	int pos[92];     /*** 1 to 364 ***/

	int nsects;         /*** number of sectors in this quadrant ***/
	int lons[2000];     /*** sector longitude ***/
	int fenums[2000];   /*** Flinn Engdahl GRN's ***/
} Quadrant;

int verbose=0; /** only print result: echo back lon,lat and F-E number and F-E name **/

/* char ASC_DIR_PATH[] = { "/Users/ichinose/Work/mtinv.v3.0.6/misc/Flinn_Engdahl/src" }; */
char ASC_DIR_PATH[256];

int main( int ac, char **av )
{
	int grn = 0;
	char grname[128];
	float lat = 0, lon = 0;
	long orid = -1;
	char rec[512];
        int i = 0, nitems = 0;
	char *mtinv_path;
	extern char ASC_DIR_PATH[256];

	void getGeoregionFromLatLon( float lat, float lon, int *grn, char *grname_string );

/*** begin main ***/

	if( ( mtinv_path = getenv( "MTINV_PATH" )) == NULL )
	{
		sprintf( ASC_DIR_PATH, "/Users/ichinose/Work/mtinv.v3.0.6/misc/Flinn_Engdahl/src" );
		fprintf( stderr, "%s: %s: enviromental variable MTINV_PATH not set! using hardwired default %s.\n",
			__FILE__, __func__, ASC_DIR_PATH );
		exit(-1);
	}
	else
	{
		sprintf( ASC_DIR_PATH, "%s/misc/Flinn_Engdahl/src", mtinv_path );
	}

/*
UPDATE table_name
SET column1 = value1, column2 = value2...., columnN = valueN
WHERE [condition];
*/

	while( fgets( rec, 512, stdin ) != NULL )
	{
		nitems = sscanf( rec, "%g %g %ld", &lon, &lat, &orid );

		if( nitems == 3 )
		{
			getGeoregionFromLatLon( lat, lon, &grn, grname );

			fprintf( stdout, "UPDATE MT_ORIGIN_STAGE SET grn = %d where orid = %ld;\n",
				grn, orid );

			fprintf( stderr, "%d, \"%s\", %ld, %g, %g\n", 
				grn,
				grname,	
				orid,
				lat,
				lon );

			i++;
		}
	}
}

void getGeoregionFromLatLon( float lat, float lon, int *grn, char *grname )
{
	int Ngregions=757;
	FE_Regions fers[758];

	int i,iquad,j,jsec;
	int lt, ln;
	float alon,alat,lng;
	int beg, num, myiquad, fenum_index;
	char myquad[3];
	Quadrant q[4];

	void get_names( int, FE_Regions * );
	void get_quadsidx( Quadrant * );
	void get_sectors( Quadrant * );

/*** begin subroutine ***/

	lng = lon;
	if( lng <= -180 ) lng += 360;
	if( lng > 180.0 ) lng -= 360;
	ln = (int)fabs(lng);
	lt = (int)fabs(lat);
	if( lat >= 0.0 && lng >= 0.0) { strcpy( myquad, "ne" ); myiquad=NE; }
        if( lat >= 0.0 && lng <  0.0) { strcpy( myquad, "nw" ); myiquad=NW; }
        if( lat <  0.0 && lng >= 0.0) { strcpy( myquad, "se" ); myiquad=SE; }
        if( lat <  0.0 && lng <  0.0) { strcpy( myquad, "sw" ); myiquad=SW; }
	if(verbose)printf("lon=%g lat=%g ln=%d lt=%d\n", lon,lat,ln,lt );

	get_quadsidx( &q[0] );
	for( iquad=1; iquad<=4; iquad++ ) 
	{
		for( j=1; j<=91; j++ ) 
		{
		  if(verbose)
		  {
		     printf("iquad=%2d name=%2s subpos=%2d quadsindx=%2d ",
			q[iquad].iquad, q[iquad].quad, q[iquad].subpos[j],
			q[iquad].index[j] );
		     printf(" pos=%3d begin=%3d end=%3d\n",
                        q[iquad].pos[j], q[iquad].begin[j], q[iquad].end[j] );
		  }
		}
	}

	get_sectors( &q[myiquad] );
	for( j=0; j < q[myiquad].nsects; j++ )
	{
	  if(verbose)
	    printf( "j=%4d lon=%3d fenums=%3d\n",
	        j, q[iquad].lons[j], q[iquad].fenums[j] );
	}

	beg = q[myiquad].begin[lt+1];
	num = q[myiquad].index[lt+1];

	if(verbose)
	  printf("beg=%d num=%d lt=%d myiquad=%d\n", beg, num, lt, myiquad );

	for( jsec=beg; jsec < beg+num; jsec++ )
	{
	  if(verbose)
	    printf("jsec=%d lons=%d fenums=%d\n",
		jsec, q[myiquad].lons[jsec], q[myiquad].fenums[jsec] );
	  if( q[myiquad].lons[jsec] > ln ) break;
	}
	fenum_index = jsec-1;
	j = q[myiquad].fenums[ fenum_index ];
	
	get_names( Ngregions, &fers[0] );
        if(verbose==3)
        {
          for( i=0; i <Ngregions; i++ ) {
                printf("indx=%04d name=%-32.32s\n",
                  fers[i].index,
                  fers[fers[i].index].names );
          }
        }

/*** return values ***/

	*grn = j;
	strcpy( grname, fers[j].names );
}

void get_sectors( Quadrant *q )
{
	extern int verbose;
	char sector_file[267];
	FILE *fp;
	int i, count=0;

	snprintf(sector_file, 267, "%s/%ssect.asc", ASC_DIR_PATH, q->quad );

	if(verbose)fprintf(stdout, "file=%s\n", sector_file );
	fp = fopen(sector_file,"r");
	i=0;
	while( fscanf( fp, "%d %d", &(q->lons[i]), &(q->fenums[i]) ) == 2 )
	{
		/* printf( "%d %d %d\n", i, q->lons[i], q->fenums[i] ); */
		i++;
	}
	fclose(fp);
	q->nsects = i;
}

void get_quadsidx( Quadrant *q )
{
	FILE *fp;
	int i, jj, total, iquad;
	int my_quad_index, begin, end;
	extern int verbose;
	char quad_file[269];

	snprintf(quad_file, 269, "%s/quadsidx.asc", ASC_DIR_PATH );

	fp = fopen( quad_file, "r" );
	total = 0;
	jj = 1;
	iquad = 1;
	begin = 0;
	end = -1;
	for( i=1; i<=364; i++ )
	{
		fscanf( fp, "%d", &my_quad_index );
		total++;
		if( jj < 92 ) 
		{
			q[iquad].iquad = iquad;
			q[iquad].subpos[jj] = jj;
			q[iquad].pos[jj] = total;
			q[iquad].index[jj] = my_quad_index;
			if( iquad == NE ) strcpy( q[iquad].quad, "ne" );
			if( iquad == NW ) strcpy( q[iquad].quad, "nw" );
			if( iquad == SE ) strcpy( q[iquad].quad, "se" );
			if( iquad == SW ) strcpy( q[iquad].quad, "sw" );
			begin = end + 1;
			end += q[iquad].index[jj];
			q[iquad].begin[jj] = begin;
			q[iquad].end[jj]   = end;
			if(verbose)
			  printf("iquad=%2d name=%2s subpos=%2d quadsindx=%2d pos=%3d\n",
			    q[iquad].iquad, q[iquad].quad, q[iquad].subpos[jj],
			    q[iquad].index[jj], q[iquad].pos[jj] );
			jj++;
		}
		else
		{
			jj=1;
			iquad++;
			begin = 0;
			end = -1;
			q[iquad].iquad = iquad;
			q[iquad].subpos[jj] = jj;
			q[iquad].pos[jj] = total;
			q[iquad].index[jj] = my_quad_index;
			if( iquad == NE ) strcpy( q[iquad].quad, "ne" );
			if( iquad == NW ) strcpy( q[iquad].quad, "nw" );
			if( iquad == SE ) strcpy( q[iquad].quad, "se" );
			if( iquad == SW ) strcpy( q[iquad].quad, "sw" );
			begin = end + 1;
			end += q[iquad].index[jj];
			q[iquad].begin[jj] = begin;
			q[iquad].end[jj]   = end;
			if(verbose)
                          printf("iquad=%2d name=%2s subpos=%2d quadsindx=%2d pos=%3d\n",
                            q[iquad].iquad, q[iquad].quad, q[iquad].subpos[jj],
                            q[iquad].index[jj], q[iquad].pos[jj] );
                        jj++;
		}
	}
	fclose(fp);
}

void get_names( int Ngregions, FE_Regions *fers )
{
	FILE *fp;
	int i,j;
	extern int verbose;
	char names_file[266];
	
	snprintf(names_file, 266, "%s/names.asc", ASC_DIR_PATH );

	fp = fopen( names_file, "r" );
	for( i=0; i<Ngregions; i++ )
	{
		fers[i].index = i+1;
		fgets( fers[fers[i].index].names, 80, fp );
		for(j=0;j<80;j++)
		{
		  if( fers[fers[i].index].names[j] == '\n' ) 
		    fers[fers[i].index].names[j] = '\0';
		}
	}
	fclose(fp);
}

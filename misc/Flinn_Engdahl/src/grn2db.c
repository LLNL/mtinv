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

/*** this program creates sqlite3 database tables for loading 
       georegions into the moment tensor database 

	only need to run this once? After create.sql for MT database tables
***/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct {
	int grn;
	char grname[128];
} GeoRegion;

int main( int ac, char **av)
{
	int i, nrows;
	GeoRegion *g;
	FILE *fp;
	GeoRegion *loadGeoRegion( char *filename, GeoRegion *g, int *nrows );

/*** begin ***/
	fprintf( stderr, "%s: %s: reads Georegion file and creates database loading scripts\n",
		__FILE__, __func__ );

	g = (GeoRegion *) calloc(1,sizeof(GeoRegion));
	g = (GeoRegion *) loadGeoRegion( "grname.txt", g, &nrows );

	fp = fopen("insert.sql","w");
	fprintf( fp, "DROP TABLE georegion;               \n" );
	fprintf( fp, "CREATE TABLE georegion (            \n" );
	fprintf( fp, " grn        NUMBER(9)     not null, \n" );
	fprintf( fp, " grname  VARCHAR2(64)  default '-', \n" );
	fprintf( fp, " lddate      DATETIME  default CURRENT_TIMESTAMP ); \n" );
	fprintf( fp, "\n" );

	for( i = 0; i < nrows; i++ )
	{
	  fprintf( fp,
	    "INSERT INTO georegion ( grn, grname, lddate ) VALUES ( %d, \"%s\", CURRENT_TIMESTAMP );\n",
			g[i].grn, g[i].grname ); 
	}
	fprintf( fp, "INSERT INTO georegion ( grn, grname, lddate ) VALUES ( -1, '-', CURRENT_TIMESTAMP );\n" );
	fclose(fp);

	fp = fopen( "georegion.h", "w" );

	fprintf( fp, "#define GEOREGION_NROWS 785\n" );
	fprintf( fp, "typedef struct { int grn; char grname[64]; } GeoRegion_Table;\n" );
	fprintf( fp, "GeoRegion_Table georegion[] = { \n" );

	for( i = 0; i < nrows-2; i++ )
	{
		fprintf( fp, "   { %3d, \"%s\" },  \n", g[i].grn, g[i].grname );
	}
	fprintf( fp, "   { %3d, \"%s\" }  \n", g[nrows-1].grn, g[nrows-1].grname );
	fprintf( fp, "};\n" );
	fclose(fp);
}

GeoRegion *loadGeoRegion( char *filename, GeoRegion *g, int *nrows )
{
	int i;
	FILE *fp;
	char rec[512];

	if( (fp = fopen(filename,"r")) == NULL )
	{
		fprintf( stderr, "%s: %s: FATAL ERROR requires GeoRegion file named %s\n", 
			__FILE__, __func__, filename );
		exit(-1);
	}

	i = 0;
	while( fgets( rec, 512, fp ) != NULL )
	{
		if(rec[0] == '>' ) 
			continue;
		
		g = realloc(g,(i+1)*sizeof(GeoRegion) );
		sscanf( rec, "%d %[^\n]", &(g[i].grn), g[i].grname );
		i++;	
	}
	fclose(fp);
	*nrows = i;
	return (GeoRegion *)g;
}
/*
> ALASKA-ALEUTAN ARC
001 CENTRAL ALASKA
002 SOUTHERN ALASKA
003 BERING SEA
004 KOMANDORSKIYE OSTROVA REGION
005 NEAR ISLANDS, ALEUTIAN ISLANDS
006 RAT ISLANDS, ALEUTIAN ISLANDS
007 ANDREANOF ISLANDS, ALEUTIAN IS.
008 PRIBILOF ISLANDS, ALASKA REGION
009 FOX ISLANDS, ALEUTIAN ISLANDS
*/

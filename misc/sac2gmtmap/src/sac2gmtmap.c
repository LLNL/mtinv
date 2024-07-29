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

/*
** sac2gmtmap reads event associated Seismic Analysis Code (SAC) files using wildcards, typically downloaded from IRIS/EarthScope
**  then generates C-shell script of Generic Mapping Tools (GMT) version 4+/5+/6+ commands to make a map, executes script 
**  using system() call and outputs PS/JPG map, also creates a rdseed.stations file needed by mkgrnlib and mtinv 
**
** UPDATES:
**  Sun Feb 21 09:56:01 PST 2021 ichinose1 - added cmdline option -rdseed.station to write file default off
**  Sun Feb 21 09:56:31 PST 2021 ichinose1 - removed evla,evlo star when undef in sac file
**   new consolidate_data() subroutine added, potential for new futures (for example, better wf segment management)
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>

#include "sac.h"
#include "mytime.h"
#include "version.h"

char progname[128];

static int font_size = 6;

int main( int ac, char **av )
{
	int i = 1, nsta = 0;
	Sac_Header *s;
	float *data;
	char sacfilename[256];
	FILE *fp;

	int verbose = 0;
	int igmt5 = 1;
	int include_epicenter = 0;
	int include_sta_label = 0;
	float xybuffer_in_degrees = 1.0;
	int write_rdseed_station_file = 0;
	int interactive = 0;


	void consolidate_data( Sac_Header *s, int nsta ); /*** from bunch of sac files, collate channels from same stations ***/


	void writeRdseedStaFile( Sac_Header *s, int nsta );

	float *readsac( Sac_Header *s, char *filename, int verbose );

	void make_gmt5_map( Sac_Header *s, int nsta, char *script_filename,
		int include_epicenter, float xybuffer_in_degrees, int include_sta_label, int interactive, int verbose );

	void make_gmt4_map( Sac_Header *s, int nsta, char *script_filename,
		int include_epicenter, float xybuffer_in_degrees, int include_sta_label, int verbose );

/********************/
/*** program name ***/
/********************/
        strcpy( progname, av[0] );

        fprintf( stderr, "%s: %s: %s: Version      = %s\n", progname, __FILE__, __func__, Version_Label );
	fprintf( stderr, "%s: %s: %s: Version Date = %s\n", progname, __FILE__, __func__, Version_Date );
	fprintf( stderr, "%s: %s: %s: Source       = %s\n", progname, __FILE__, __func__, Version_SrcDir );
	fprintf( stderr, "\n\n\n" );

/*** Usage ***/

	if( ac <= 1 )
	{
		fprintf( stderr,
	"Usage:\n\t %s -gmt5 -verbose -xybuffer_in_degrees %%f -include-epicenter -include-sta-label -rdseed.station [sac files]\n", 
			progname );
		exit(-1);
	}

/*** begin ***/

	s = (Sac_Header *)malloc( ac * sizeof(Sac_Header) );

/***  go through list of sac files and read headers for station information ***/
/** i = 0, skip because is av[0]=progname **/
	nsta = 0;
	while( i < ac )
	{
		strcpy( sacfilename, av[i] );

		if( strcmp( sacfilename, "-interactive" ) == 0 )
		{
			interactive = 1;
			fprintf( stderr, "%s: Switch %s found for interactive plotting\n",
				progname, sacfilename );
			i++;
			continue;
		}

		if( strcmp( sacfilename, "-rdseed.station" ) == 0 )
		{
			write_rdseed_station_file = 1;
			fprintf( stderr, "%s: Switch %s found for writting rdseed.station output\n",
				progname, sacfilename );
			i++;
			continue;
		}

		if( strcmp( sacfilename, "-verbose" ) == 0 )
		{
			verbose = 1;
			fprintf( stderr, "%s: Switch %s found for verbosity, continuing...\n", 
                                progname, sacfilename );
			i++;
                        continue;
		}

		if( strcmp( sacfilename, "-gmt5" ) == 0 ) 
		{
			fprintf( stderr, "%s: Switch %s found for GMT version 5.x.x, continuing...\n", 
				progname, sacfilename );
			igmt5 = 1;
			i++;
			continue;
		}
	
		if( strcmp( sacfilename, "-xybuffer_in_degrees" ) == 0 || strcmp( sacfilename, "-buff" ) == 0 )
		{
			i++;
			xybuffer_in_degrees = atof( av[i] );
			fprintf( stderr, "%s: -xybuffer_in_degrees or -buff = %g\n",
				progname, xybuffer_in_degrees );
			i++;
			continue;
		}

		if( strcmp( sacfilename, "-include-epicenter" ) == 0 )
		{
			fprintf( stderr, "%s: Switch %s turn off include-epicenter in plot...\n",
				progname, sacfilename );
			include_epicenter = 1;
			i++;
			continue;
		}

		if( strcmp( sacfilename, "-include-sta-label" ) == 0 )
                {
                        fprintf( stderr, "%s: Switch %s turn off include-sta-label in plot...\n",
                                progname, sacfilename );
                        include_sta_label = 1;
                        i++;
                        continue;
                }

		if( (fp=fopen(sacfilename,"rb")) == NULL )
		{
			fprintf( stderr, "%s: cannot fopen file %s, continuing...\n", 
				progname, sacfilename );
			i++;
			continue;
		}
		else
		{
			fclose(fp);
			if(verbose)
			{
			  fprintf( stderr, "%s: i=%d nsta=%d %s\n",
				progname, i, nsta, sacfilename );
			}
			data = readsac( &s[nsta++], sacfilename, verbose );
                	i++;
		}
        }

	fprintf( stderr, "%s: processing %d number of stations.\n", progname, nsta );

	consolidate_data( s, nsta );

	if( igmt5 )
	{
		if(verbose) fprintf(stderr, "%s: make gmt5 map\n", progname );
		make_gmt5_map( s, nsta, "plotmap.csh", include_epicenter, xybuffer_in_degrees, include_sta_label, interactive, verbose );
	}
	else
	{
		if(verbose)fprintf(stderr, "%s: make gmt4 map\n", progname );
		make_gmt4_map( s, nsta, "plotmap.csh", include_epicenter, xybuffer_in_degrees, include_sta_label, verbose );
	}

	if( write_rdseed_station_file )
		writeRdseedStaFile( s, nsta );

	exit(0);
}

void writeRdseedStaFile( Sac_Header *s, int nsta )
{
	int i;
	FILE *fp;
	fp = fopen("rdseed.stations","w");
	for( i = 0; i < nsta; i++ )
	{
		fprintf( fp, "%s %s %g %g %g \"%s%s \" \n",
			s[i].kstnm, s[i].knetwk, 
			s[i].stla, s[i].stlo, s[i].stel,
			s[i].kcmpnm, s[i].khole );
	}
	fclose(fp);

	fp = fopen( "stations.csv", "w" );
	fprintf( fp, "station,network,latitude,longitude,elevation_meters,channels\n" );

	for( i = 0; i < nsta; i++ )
	{
		fprintf( fp, "%s,%s,%g,%g,%g,\"%s%s\"\n",
			s[i].kstnm, s[i].knetwk,
                        s[i].stla, s[i].stlo, s[i].stel,
                        s[i].kcmpnm, s[i].khole );
	}
	fclose(fp);
}

void make_gmt4_map( Sac_Header *s, int nsta, char *script_filename,
	int include_epicenter, float xybuffer_in_degrees, int include_sta_label, int verbose )
{
	FILE *fp;
        int i = 1;
        float maxlat = -999, maxlon = -999;
        float minlat = +999, minlon = +999;
        float sumlat = 0, sumlon = 0;
        float ticklength = 0.5, annotation = 1.0;
        char PS_filename[128], JPG_filename[128];
        char command_line[512];

        sprintf( PS_filename, "sac2gmtmap.ps" );
        sprintf( JPG_filename, "sac2gmtmap.jpg" );

        if( (fp=fopen( script_filename,"w" )) == NULL )
        {
                fprintf( stderr, "%s: cannot open file %s for writting\n",
                        progname, script_filename );
                exit(-1);
        }

	if( include_epicenter )
	{
		maxlat = s[1].evla;
		minlat = s[1].evla;
		maxlon = s[1].evlo;
		minlon = s[1].evlo;
	}
	else
	{
		maxlat = s[1].stla;
		minlat = s[1].stla;
                maxlon = s[1].stlo;
                minlon = s[1].stlo;
	}

        for( i = 0; i < nsta; i++ )
        {
                if(  s[i].stla > maxlat ) maxlat = s[i].stla;
                if(  s[i].stla < minlat ) minlat = s[i].stla;
                if(  s[i].stlo > maxlon ) maxlon = s[i].stlo;
                if(  s[i].stlo < minlon ) minlon = s[i].stlo;

        }
        maxlat =  ceil( maxlat ) + xybuffer_in_degrees;
        minlat = floor( minlat ) - xybuffer_in_degrees;
        maxlon =  ceil( maxlon ) + xybuffer_in_degrees;
        minlon = floor( minlon ) - xybuffer_in_degrees;

        if( fabs( maxlon - minlon ) < 3 )
        {
                maxlon += 2;
                minlon -= 2;
        }

	if( maxlon > +180.0 ) maxlon = +180.0;
	if( maxlat > +90.0 )  maxlat = +89.0;
	if( minlat < -90.0 )  minlat = -89.0;
	if( minlon < -180.0 ) minlon = -180.0;

/*** make GMT version 4.5.x map ***/

	fprintf( fp, "#!/bin/csh\n");
        fprintf( fp, "###################################################################################\n");
        fprintf( fp, "## This C-shell script was automatically generated by mtinv                      ##\n");
        fprintf( fp, "## and requires Generic Mapping Tools (GMT) http://gmt.soest.hawaii.edu/         ##\n");
        fprintf( fp, "## The script plots the mechanism at the event location and station locations    ##\n");
        fprintf( fp, "## on a map. This script uses GMT version 4.x.x only see usage for ver 5.x.x     ##\n");
        fprintf( fp, "###################################################################################\n");
        fprintf( fp, "\n" );

        fprintf( fp, "gmtset BASEMAP_TYPE plain\n" );
        fprintf( fp, "\n");

        fprintf( fp, "gmt pscoast -R%g/%g/%g/%g -JM6i -Di ", minlon, maxlon, minlat, maxlat );
        fprintf( fp, " -N1/1.2p,black,5_2:0p -N2/0.8p,black,5_2:0p " );
        fprintf( fp, " -A1000 -W1p,black -Glightgray -P -K >! %s\n", PS_filename );
        fprintf( fp, "\n");

        fprintf( fp, "psxy -R -JM -m -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "> -W1p,black %s.%s.%s%s\n",
                        s[i].kstnm, s[i].knetwk, s[i].kcmpnm, s[i].khole );
                fprintf( fp, "%12.8f %12.8f\n", s[i].evlo, s[i].evla );
                fprintf( fp, "%12.8f %12.8f\n", s[i].stlo, s[i].stla );
        }
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n");

        fprintf( fp, "psxy -R -JM -St0.15i -W1p,black -Gred -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "%12.8f %12.8f %s.%s.%s%s\n",
                        s[i].stlo, s[i].stla,
                        s[i].kstnm, s[i].knetwk,
                        s[i].kcmpnm, s[i].khole );
        }
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n");

	if(include_sta_label)
	{
	 fprintf( fp, "#Reads (x,y[,fontinfo,angle,justify],text) from <table> [or stdin].\n" );
         fprintf( fp, "pstext -R -JM -N -D0.05i/0.1i -Wwhite -O -K >> %s << EOF\n", PS_filename );
         for( i = 0; i < nsta; i++ )
         {
		if( strncmp( s[i].knetwk, "-12345", 6 ) == 0 ) 
		{
			fprintf( fp, "%12.8f %12.8f %d 0 0 0 %s\n", 
				s[i].stlo, s[i].stla, font_size, s[i].kstnm );
		}
		else
		{
		/***
                  fprintf( fp, "%g %g 6 0 0 0 %s.%s.%s%s\n",
                        s[i].stlo, s[i].stla,
                        s[i].kstnm, s[i].knetwk,
                        s[i].kcmpnm, s[i].khole );	
		***/
		  fprintf( fp, "%12.8f %12.8f 6 0 0 0 %s\n", s[i].stlo, s[i].stla, s[i].kstnm );
		}
         }
         fprintf( fp, "EOF\n" );
         fprintf( fp, "\n");
	}

/*** check event location ***/

        if( ( s[0].evlo == -12345 || s[0].evlo == 0 ) &&
            ( s[0].evla == -12345 || s[0].evla == 0 ) )
        {
                fprintf( fp, "## %s: evla evlo undefined in SAC file\n", progname );
                fprintf( stderr, "%s: evla evlo undefined in SAC file\n",
                        progname );
	
                for( i = 0; i < nsta; i++ )
                {
                        sumlat += s[i].stla;
                        sumlon += s[i].stlo;
                }
                sumlat /= nsta;
                sumlon /= nsta;
	/*
          fprintf( fp, "echo %12.8f %12.8f | ", sumlon, sumlat );
          fprintf( fp, "psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
	*/

        }
        else
        {
          fprintf( fp, "echo %12.8f %12.8f | ", s[0].evlo, s[0].evla );
          fprintf( fp, "psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
        }
        fprintf( fp, "\n");

        if( fabs( maxlon - minlon ) < 5 )
        {
                ticklength = 0.5;
                annotation = 2;
        }
        else
        {
                ticklength = 1;
                annotation = 5;
        }

	if( s[0].evla == -12345. )
	{
		fprintf( fp, "psbasemap -R -JM -Bf%ga%g/f%ga%gNSEW -O >> %s\n",
			ticklength, annotation, ticklength, annotation, PS_filename );
	}
	else
	{
        	fprintf( fp, "psbasemap -R -JM -Bf%ga%g/f%ga%gNSEW -Lx1/1/%g/100 -O >> %s\n",
                	ticklength, annotation, ticklength, annotation, s[0].evla, PS_filename );
	}
        fprintf( fp, "\n");

        fprintf( fp, "ps2raster -A  -Tj -E300 -V %s\n", PS_filename );
        fprintf( fp, "/bin/rm -f %s gmt.conf gmt.history .gmtdefaults4 .gmtcommands4\n", PS_filename );
        fprintf( fp, "#echo use eog xv open to view jpg\n" );
        fprintf( fp, "#eog %s\n", JPG_filename );
        fclose(fp);

        chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );
        sprintf( command_line, "./%s", script_filename );

        fprintf( stderr, "%s: running command %s\n", progname, command_line );

        system( command_line );
}

/*** GMT Version 5.x.x ****/

void make_gmt5_map( Sac_Header *s, int nsta, char *script_filename,
	int include_epicenter, float xybuffer_in_degrees, int include_sta_label, int interactive, int verbose )
{
	FILE *fp;
	int i = 1;
	float maxlat = -999, maxlon = -999;
	float minlat = +999, minlon = +999;
	float aspect_ratio, tmp;
	float sumlat = 0, sumlon = 0;
        float ticklength = 0.5, annotation = 1.0;
	char PS_filename[128], JPG_filename[128];
	char command_line[512];

	void convert_colatlon( float *la, float *lo, int mode ); /*** mode = 1 forward; mode = -1 reverse ***/

/***************/
/*** start   ***/
/***************/

	sprintf( PS_filename, "sac2gmtmap.ps" );
	sprintf( JPG_filename, "sac2gmtmap.jpg" );

	if( s[0].evla == -12345. || s[0].evlo == -12345. )
        {       
                fprintf( stderr, "%s: %s: %s: warning evla and evlo is null\n",
                        progname, __FILE__, __func__ );

		if( include_epicenter )
		{
			fprintf( stderr, "%s: %s: %s: ERROR include_epicenter=%d and evla and evlo is null. Turn off include_epicenter and rerun.\n",
				progname, __FILE__, __func__ , include_epicenter);
			exit(-1);
		}
	}

	if( (fp=fopen( script_filename,"w" )) == NULL )
	{
		fprintf( stderr, "%s: cannot open file %s for writting\n",
			progname, script_filename );	
		exit(-1);
	}

	for( i = 0; i < nsta; i++ )
        {
                convert_colatlon( &(s[i].stla), &(s[i].stlo), 1 );
                convert_colatlon( &(s[i].evla), &(s[i].evlo), 1 );
        }

	if( include_epicenter )
        {
                maxlat = s[0].evla;
                minlat = s[0].evla;
                maxlon = s[0].evlo;
                minlon = s[0].evlo;
        }
        else
        {
		for( i = 0; i < nsta; i++ )
	        {
        	        sumlat += s[i].stla;
               		sumlon += s[i].stlo;
        	}
        	sumlat /= nsta;
        	sumlon /= nsta;

                maxlat = sumlat;
                minlat = sumlat;
                maxlon = sumlon;
                minlon = sumlon;
        }
	
	for( i = 0; i < nsta; i++ )
	{
		if(  s[i].stla > maxlat ) maxlat = s[i].stla;
                if(  s[i].stla < minlat ) minlat = s[i].stla;
                if(  s[i].stlo > maxlon ) maxlon = s[i].stlo;
                if(  s[i].stlo < minlon ) minlon = s[i].stlo;

        }
	maxlat =  ceil( maxlat ) + 1;
        minlat = floor( minlat ) - 1;
        maxlon =  ceil( maxlon ) + 1;
        minlon = floor( minlon ) - 1;

	aspect_ratio = fabs(maxlat - minlat) / fabs( maxlon - minlon );
	fprintf( stderr, "%s: aspect_ratio = %g\n", progname, aspect_ratio );

	if( fabs( maxlon - minlon ) < 3 )
        {
                maxlon += 2;
                minlon -= 2;
        }

/*** convert back ***/

	for( i = 0; i < nsta; i++ )
        {
                convert_colatlon( &(s[i].stla), &(s[i].stlo), -1 );
                convert_colatlon( &(s[i].evla), &(s[i].evlo), -1 );
        }

	convert_colatlon( &minlat, &minlon, -1 );
        convert_colatlon( &maxlat, &maxlon, -1 );

/*** if the region box in GMT spans the +180/-180 long then fix ***/

        if( maxlon < 0 && minlon > 0 )
        {
                maxlon += 360;
        }

        if( maxlat < minlat )
        {
                tmp = minlat;
                minlat = maxlat;
                maxlat = tmp;
        }

	fprintf( stderr, "%s: %s: %s: minlon = %g maxlon = %g minlat = %g maxlat = %g\n",
		progname, __FILE__, __func__, 
		minlon, maxlon, minlat, maxlat );

/*** map edge buffer ***/

	if( xybuffer_in_degrees == 1 ) 
	{
        	maxlat =  ceil( maxlat ) + xybuffer_in_degrees;
        	minlat = floor( minlat ) - xybuffer_in_degrees;
        	maxlon =  ceil( maxlon ) + xybuffer_in_degrees;
	        minlon = floor( minlon ) - xybuffer_in_degrees;
	}

        if( fabs( maxlon - minlon ) < 3 )
        {
                maxlon += xybuffer_in_degrees;
                minlon -= xybuffer_in_degrees;
		maxlat += xybuffer_in_degrees;
		minlat -= xybuffer_in_degrees;
        }

	fprintf( stderr, "%s: %s: %s: xybuffer_in_degrees=%g minlon = %g maxlon = %g minlat = %g maxlat = %g\n",
                progname, __FILE__, __func__,
		xybuffer_in_degrees,
		minlon, maxlon, minlat, maxlat );

/*** make GMT version 5.x.x map ***/

	fprintf( fp, "#!/bin/csh\n");
        fprintf( fp, "###################################################################################\n");
        fprintf( fp, "## This C-shell script was automatically generated by mtinv                      ##\n");
        fprintf( fp, "## and requires Generic Mapping Tools (GMT) http://gmt.soest.hawaii.edu/         ##\n");
        fprintf( fp, "## The script plots the mechanism at the event location and station locations    ##\n");
        fprintf( fp, "## on a map. This script uses GMT version 5.x.x only see usage for ver 4.x.x     ##\n");
        fprintf( fp, "###################################################################################\n");
	fprintf( fp, "## -xybuffer_in_degrees = %g include_epicenter = %d\n",
		xybuffer_in_degrees, include_epicenter );

	fprintf( fp, "\n" );

	fprintf( fp, "gmt set MAP_FRAME_TYPE plain\n" );
        fprintf( fp, "gmt set FORMAT_GEO_OUT DG\n" );
        fprintf( fp, "gmt set FORMAT_GEO_MAP DG\n" );
        fprintf( fp, "\n");

	fprintf( fp, "gmt pscoast -R%g/%g/%g/%g -JM6i -Di ", minlon, maxlon, minlat, maxlat );
	fprintf( fp, " -N1/1.2p,black,5_2:0p -N2/0.8p,black,5_2:0p " );
	fprintf( fp, " -A1000 -W1p,black -Glightgray -P -K >! %s\n", PS_filename );
	fprintf( fp, "\n");

	if(include_epicenter)
	{
         fprintf( fp, "gmt psxy -R -JM -O -K >> %s << EOF\n", PS_filename );
         for( i = 0; i < nsta; i++ )
         {
                fprintf( fp, "> -W1p,black %s.%s.%s%s\n", 
			s[i].kstnm, s[i].knetwk, s[i].kcmpnm, s[i].khole );
                fprintf( fp, "%12.8f %12.8f\n", s[i].evlo, s[i].evla );
                fprintf( fp, "%12.8f %12.8f\n", s[i].stlo, s[i].stla );
         }
         fprintf( fp, "EOF\n" );
	 fprintf( fp, "\n");
	}

        fprintf( fp, "gmt psxy -R -JM -St0.15i -W1p,black -Gred -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "%12.8f %12.8f %s.%s.%s%s\n", 
			s[i].stlo, s[i].stla, 
			s[i].kstnm, s[i].knetwk, 
			s[i].kcmpnm, s[i].khole );
        }
        fprintf( fp, "EOF\n" );
	fprintf( fp, "\n");

	if(include_sta_label)
	{
	 fprintf( fp, "### Reads (x,y[,fontinfo,angle,justify],text) from <table> [or stdin].\n" );
         fprintf( fp, "gmt pstext -R -JM -C0.01i/0.01i -N -D0.05i/0.1i -W0p,white -Gwhite " );
	 fprintf( fp, "  -F+f%dp,Times-Roman,blue+jBL -O -K >> %s << EOF\n", font_size, PS_filename );
         for( i = 0; i < nsta; i++ )
         {
		/***
                fprintf( fp, "%12.8f %12.8f %s.%s.%s%s\n", 
			s[i].stlo, s[i].stla, 
			s[i].kstnm, s[i].knetwk,
			s[i].kcmpnm, s[i].khole );
		***/
		fprintf( fp, "%12.8f %12.8f %s\n", s[i].stlo, s[i].stla, s[i].kstnm );
         }
         fprintf( fp, "EOF\n" );
	 fprintf( fp, "\n");
	}

/*** check event location ***/
	
	if( ( s[0].evlo == -12345 || s[0].evlo == 0 ) &&
	    ( s[0].evla == -12345 || s[0].evla == 0 ) )
	{
		fprintf( fp, "## %s: evla evlo undefined in SAC file\n", progname );
		fprintf( stderr, "%s: evla evlo undefined in SAC file\n",
			progname );
		for( i = 0; i < nsta; i++ )
		{
			sumlat += s[i].stla;
			sumlon += s[i].stlo;
		}
		sumlat /= nsta;
		sumlon /= nsta;
	/*
	  fprintf( fp, "echo %12.8f %12.8f | ", sumlon, sumlat );
          fprintf( fp, "psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
	*/

	}
	else
	{
	  if(include_epicenter)
	  {
           fprintf( fp, "echo %12.8f %12.8f | ", s[0].evlo, s[0].evla );
	   fprintf( fp, "gmt psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
	  }
	}
	fprintf( fp, "\n");

	ticklength = 1;
	annotation = 5;
        if( fabs( maxlon - minlon ) < 5 )
        {
                ticklength = 0.5;
                annotation = 2;
        }
        if( xybuffer_in_degrees < 1 )
        {
                ticklength = xybuffer_in_degrees/4;
                annotation = xybuffer_in_degrees/2;
        }

	if( s[0].evla == -12345. )
	{
		fprintf( fp, "gmt psbasemap -R -JM -Bxf%ga%g -Byf%ga%g -BNSEW -O >> %s\n",
			ticklength, annotation, ticklength, annotation, PS_filename );
	}
	else
	{
        	fprintf( fp, "gmt psbasemap -R -JM -Bxf%ga%g -Byf%ga%g -BNSEW -Lx1/1/%g/%g -O >> %s\n",
                	ticklength, annotation, ticklength, annotation, s[0].evla,
			xybuffer_in_degrees * 10.0, PS_filename );
	}

	fprintf( fp, "\n");

        fprintf( fp, "gmt psconvert -A -Z -Tj -E300 -V %s\n", PS_filename );
	fprintf( fp, "/bin/rm -f gmt.conf gmt.history\n" );
	fprintf( fp, "#echo use eog xv open to view jpg\n" );
	if( interactive )
		fprintf( fp, "open %s\n", JPG_filename );
	else
		fprintf( fp, "# open %s\n", JPG_filename );

	fclose(fp);

	chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );

	sprintf( command_line, "./%s", script_filename ); 
	fprintf( stderr, "%s: running command %s\n", progname, command_line );
       	system( command_line );
}

/*** mode = 1 forward; mode = -1 reverse ***/
void convert_colatlon( float *lat, float *lon, int mode )
{
        float la;
        float lo;

/*** initalize output with input ***/
        la = *lat;
        lo = *lon;

/** lat to colat is the same as colat to lat **/
        la = 90 - *lat;

        if( mode == 1 ) /**long(-180/+180 to 0-360) **/
        {
                if( *lon < 0 ) lo = 360 + *lon;
        }
        else if( mode == -1 ) /**long(0-360 to -180/+180) **/
        {
                if( *lon > 180 ) lo = *lon - 360;
        }
        else
        {
                fprintf( stderr, "%s: %s: %s: error unknown mode=%d\n",
                        progname, __FILE__, __func__, mode );
                exit(-1);
        }
/**
        fprintf( stderr, "%s: %s: %s: mode=%d inlat=%g inlon=%g outlat=%g outlon=%g\n",
                progname, __FILE__, __func__, mode, *lat, *lon, la, lo );
**/
        *lat = la;
        *lon = lo;
}


typedef struct {
        char nslc[32];
        char loc[16], chan[16];
        float sampling_rate;
        int npts;
}Channels;

typedef struct {
	char sta[16], net[16];
	int nchans;
	Channels c[40]; /**hopefully no station has over 40 channels **/
	float stla, stlo, stel, stdp;
	float evla, evlo;
	float distkm, az, baz;
} SacData;

/*** this is new, uses the list of all the data generated using *wildcards* 
      and determines unique net.sta.loc.chans, currently only outputs csv files  ****/

void consolidate_data( Sac_Header *s, int nsta )
{
	int i = 0, j = 0, k = 0;
	SacData *sd;
	int nunique = 0;
	int distaz( double, double, double, double, double *, double *, double * );
        double drdist,daz,dbaz;
	FILE *fp;

	sd = calloc( nsta+1, sizeof(SacData) );

/*** creqte a unique list of network.stations ***/
/********** just do the match here *********/
	strcpy( sd[0].sta, s[0].kstnm );
	strcpy( sd[0].net, s[0].knetwk );
	for( j = 0,  i = 0; i < nsta; i++ )
	{
		if( ( strcmp( sd[j].sta, s[i].kstnm ) == 0 ) && 
		    ( strcmp( sd[j].net, s[i].knetwk ) == 0 ) )
		{
			/*** not unqiue ***/
			/* fprintf( stderr, "not unique: i=%d j=%d (%s,%s) == (%s,%s)\n",
			      i, j, sd[j].sta, s[i].kstnm, sd[j].net, s[i].knetwk ); ***/
		}
		else
		{
			/*** fprintf( stderr, "***unique: i=%d j=%d (%s,%s) != (%s,%s)\n",
				i, j, sd[j].sta, s[i].kstnm, sd[j].net, s[i].knetwk ); ***/
			j++;
			strcpy( sd[j].sta, s[i].kstnm );
			strcpy( sd[j].net, s[i].knetwk );
			continue;
		}
	}
	nunique = j+1;

/*** here add the station lat/lon and dist ***/
	for ( i = 0; i < nunique; i++ )
	{
		for( j = 0; j < nsta; j++ )
		{
			if( ( strcmp( sd[i].sta, s[j].kstnm ) == 0 ) &&
                            ( strcmp( sd[i].net, s[j].knetwk ) == 0 ) )
			{
				sd[i].evla = s[j].evla;
                        	sd[i].evlo = s[j].evlo;
                        	sd[i].stla = s[j].stla;
                        	sd[i].stlo = s[j].stlo;
				sd[i].stel = s[j].stel;
				sd[i].stdp = s[j].stdp;

				distaz( (double)sd[i].evla, (double)sd[i].evlo,
                                	(double)sd[i].stla, (double)sd[i].stlo,
                                                &drdist, &daz, &dbaz );
                        	sd[i].distkm = (float)drdist;
                        	sd[i].az     = (float)daz;
                        	sd[i].baz    = (float)dbaz;

				continue;
			}
		}
	}

/*** get the number of channels for each unique net.sta ***/

	/*** fprintf( stderr, "nunique=%d\n", nunique ); ***/
	for ( i = 0; i < nunique; i++ )
	{
		for( k = 0, j = 0; j < nsta; j++ )
		{
			if( ( strcmp( sd[i].sta, s[j].kstnm ) == 0 ) &&
			    ( strcmp( sd[i].net, s[j].knetwk ) == 0 ) )
			{
				strcpy( sd[i].c[k].loc, s[j].khole );
				strcpy( sd[i].c[k].chan, s[j].kcmpnm );
				k++;
			}
		}
		sd[i].nchans = k;
	}

/*** get the number of channels for each unique net.sta ***/ 

	for ( i = 0; i < nunique; i++ )
	{
		for( k = 0; k < sd[i].nchans; k++ )
		{
			sprintf( sd[i].c[k].nslc, "%s.%s.%s.%s", 
				sd[i].net, sd[i].sta, sd[i].c[k].loc, sd[i].c[k].chan );

			for( j = 0; j < nsta; j++ )
			{
				if(	( strcmp( sd[i].net,       s[j].knetwk ) == 0 ) && 
					( strcmp( sd[i].sta,       s[j].kstnm  ) == 0 ) && 
					( strcmp( sd[i].c[k].loc,  s[j].khole  ) == 0 ) && 
					( strcmp( sd[i].c[k].chan, s[j].kcmpnm ) == 0 ) )
				{
					sd[i].c[k].sampling_rate = s[j].delta;
					sd[i].c[k].npts          = s[j].npts;
 				}
			}

			/* fprintf( stderr, "nslc=%s dt=%g nt=%d distkm=%g az=%g\n",
				sd[i].c[k].nslc, 
				sd[i].c[k].sampling_rate, 
				sd[i].c[k].npts,
				sd[i].distkm, sd[i].az ); */
		}
	}
	fp = fopen( "stations.channels.csv", "w" );
	fprintf(fp,"network,station,latitude,longitude,elevation,channels,dist_km,azimuth\n");
	for( i = 0; i < nunique; i++ )
	{
		fprintf( fp, "%s,%s,%g,%g,%g,\"", 
			sd[i].net, sd[i].sta, 
			sd[i].stla, sd[i].stlo, sd[i].stel );

		for( k = 0; k < sd[i].nchans; k++ )
		{
			fprintf( fp, "%s%s", sd[i].c[k].chan, sd[i].c[k].loc );
			if( k < sd[i].nchans-1 ) fprintf(fp,",");
		}
		fprintf( fp, "\",%g,%g\n", sd[i].distkm, sd[i].az );
	}
	fclose(fp);
	
} /*** end consolidate_data() ***/

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

/*** 
* this is a helper function, setupMT is the very first to run and is very basic, 
* sets up makeglib.c based on SAC data and now setups runs for hspec96 Greens functions 
***/

/*** Thu Sep 19 13:51:27 PDT 2024 ichinose 
* critical security bugfix from codeQL scanning in GitHub 
* replaced system call to list directory with a builtin function 
***/

/***
*
* Mon Aug  3 15:40:26 PDT 2020 ichinose
*
* added channel list data structure to remove duplicate channels in makeglib.csh -> mkgrnlib.par file output
*  cause... sometimes user requires multiple channels LH,BH,HH chans in SAC, seed or mseed waveform data
*  reason... most stations host multiple streams or decimated streams to user, this causes multiple entries
*             in mkgrnlib.par and causes mkgrnlib to compute duplicate Green's functions wasting time 
*
*  added check for multiple segments for user to check 
*
*
* Sat Jan 13 14:51:40 PST 2024
*	add support to compute HSPEC96 greens functions from R. Herrmann's Computer Programs in Seismology 
* ### autogenerate this using setupMT -hspec96
* ### https://www.eas.slu.edu/eqc/eqccps.html
* ###  https://www.eas.slu.edu/eqc/eqc_cps/CPS/CPS330/cps330o.pdf
*
* Wed Aug 14 09:28:42 PDT 2024 - modified f96tosac.f filename format output (see comments)
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

/*** for VelMod structure ***/
/*** also for mt_version.h and sac.h ***/
#include "../include/mt.h"

char progname[128];

int main( int ac, char **av )
{	
	Sac_Header *s;
	float *data;
	char arg[256], sacfilename[256];
	FILE *fp;
	MyTime *ot;
	char timestring[25];
	char comment[256];
	char loc[8];
	float depth, evla, evlo;
	double drdistkm, daz, dbaz;
	int verbose;
	int igmt4;
	int i = 1, nsta = 0;
	int irt;
	char *mtinv_path = NULL;

	char model_name[128];   /*** wus ***/
	char model_file_path[256]; /*** /Users/ichinose1/Work/mtinv/data/modeldb ***/
	char model_file_full_path[256];  /*** /Users/ichinose1/Work/mtinv/data/modeldb/wus.mod ***/

/***************************************************************************************/
/*** populate makeglib.csh:makepar DataDir= and RespDir=                             ***/
/*** also for makeglib.csh:mod.par:stadb=/home/user/Work/event/Data/rdseed.stations  ***/
/***************************************************************************************/
	char DataDir[256]; /*** /home/user/Work/event/Data ***/
	char RespDir[256]; /*** /home/user/Work/event/Resp ***/

	char cmdline[512];
	float lf, hf;
	long evid;

/**********************************************************/
/*** load a list of stations to exclude in processesing ***/
/**********************************************************/
	char exclude_station_file[256];
	char exclude_station_list[256][16];
	int nsta_exclude_list = 0;
	int ista;
	void load_exclude_station_list( char *exclude_station_file, char exclude_station_list[256][16], int *nsta_exclude_list );

/*******************************************************************
  A max distance does not work with arrays like TA because there
     can be 100's of stations within 1000 km so...

      Regular Asia default
        1. Use all stations within 1200 km but only invert 
            using stations with 1000 km   
        2.  Only the 8 closest stations are defined.
        3.  There is no number of station limit.--problems!

        int max_number_stations_invert = 8;  
        int max_number_stations_compute_grns = UNLIMITED;
        float max_distance_process_km = 1200;
        float max_distance_define_km  = 1000;
	float min_distance_define_km  = 70;

      US Array default
        1. Use all stations within 800 km up to 30 stations
        2. Only the 8 closest statiosn are defined
        3.  The station limit is 30 stations 

        int max_number_stations_invert = 8;  
        int max_number_stations_compute_grns = 30;
        float max_distance_process_km = 900;
        float max_distance_define_km  = 800;
	float min_distance_define_km  = 70;
*******************************************************************/
	
	int max_number_stations_invert = 8; 
	int max_number_stations_compute_grns = 30;
	float max_distance_process_km = 900;
	float max_distance_define_km  = 800;
	float min_distance_define_km = 70;

/*** sacio.c ***/

	float *readsac( Sac_Header *s, char *filename, int verbose );

	int distaz( double olat, double olon, double tlat, double tlon, 
		double *rdistkm, double *az, double *baz );

/*** local ***/

	void make_gmt5_map( Sac_Header *s, int nsta, char *script_filename, int irt, int verbose );

	void make_gmt4_map( Sac_Header *s, int nsta, char *script_filename, int irt, int verbose );

	void make_glib_parfile(
		MyTime *ot,
		Sac_Header *s,
		int nsta,
		float depth,
		char *comment,
		int igmt4,
		int irt,
		int max_number_stations_invert,
		int max_number_stations_compute_grns,
		float min_distance_define_km,
		float max_distance_define_km, 
		float max_distance_process_km,
		float lf,
		float hf, 
		char *script_filename,
		char *model_name,
		long evid,
		char *DataDir,
		char *RespDir,
		char exclude_station_list[256][16], 
		int nsta_exclude_list,
		int verbose );

	void Usage( int ac, char **av );

/*** timesubs.o ***/
        void parsestring( MyTime *ot, char *str );
	void WriteMyTime2STDERR( MyTime *t );

/*** shorten_path.c ***/

        char *shorten_path( char *pathname, char *filename );
	char pathname[128];

/*** add new feature to compute hspec96.f greens functions ***/

	int ihspec96 = 0;
	void create_hspec96_script( float depth, int nsta, Sac_Header *s, 
		char *model_name, char *model_file_path, 
		char exclude_station_list[256][16], int nsta_exclude_list, 
		float evla, float evlo, int verbose );

/*** misc ***/
	void create_cleanup_script();

/*** get commandline args ***/
	float get_cmdline_float( int i, char *arg, char **av, char *argname );
	int get_cmdline_integer( int i, char *arg, char **av, char *argname );
	long get_cmdline_long( int i, char *arg, char **av, char *argname );

/*** get a directory listing of files with .mod extension ***/
	int list_directory_files_with_extension( char *directory_name, char *filename_extension_with_leading_dot );

/********************************************************************************/
/*********** start progname *******************************************/
/********************************************************************************/

	strcpy( pathname, av[0] );
        shorten_path( pathname, progname );

	if( verbose )
        {
          fprintf( stdout, "%s: STDOUT: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname, Version_Label, Version_Date, pathname );
        }

        fprintf( stderr, "%s: STDERR: Version=%s ReleaseDate=%s exec full path=%s\n",
                progname, Version_Label, Version_Date, pathname );

/*** defaults ***/

	irt = 1; /*** boolean realtime = default off cmdline -int or -interactive ***/
	depth = -999;
	evla = -999;
	evlo = -999;

	lf = 0.02; /*** default filter band is 0.02 to 0.05 Hz ***/
	hf = 0.05;

	/*** checks for ${MTINV_PATH}/data/modeldb/xxx.mod and lists ***/
	/*** if directory dosent exist then use default wus ***/
	/* strcpy( model_name, "wus" );  */
	strcpy( model_name, "\0" ); 

	sprintf( timestring, "2001/01/01T01:01:59.99" );
	sprintf( comment, "Comment Here" );
	igmt4 = 0;    /*** GMT5.x or 6 is default ***/
	evid = -1;
	verbose = 0;
	ot = NULL;

	strcpy( DataDir, "../Data" );
	strcpy( RespDir, "../Resp" );

/*** if no cmdline args given, print usage and exit ***/
	if( ac <= 1 ) Usage(ac,av); /**exits***/

/*** allocate sac header temp space ***/
/***  go through list of sac files and read headers for station information ***/
/** i = 0, skip because is av[0]=progname **/
	s = (Sac_Header *)malloc( ac * sizeof(Sac_Header) );

	nsta = 0;
	while( i < ac )
	{
		strcpy( arg, av[i] );

	/*** this is new, for setting up bob Herrmann's hspec96 GFs run ***/

		if( strcmp( arg, "--hspec96" ) == 0 || strcmp( arg, "-hspec" ) == 0 )
		{
			i++;
			ihspec96 = 1;
			continue;
		}

		if( strcmp( arg, "-evid" ) == 0 )
		{
			i++;
			evid = get_cmdline_long( i, arg, av, "evid" );

			fprintf( stderr, "%s: -evid or -eventid : evid=%ld\n", progname, evid );
			i++;
			continue;
		}

		if( strcmp( arg, "-h" ) == 0 || strcmp( arg, "-help" ) == 0 ) 
		{
			Usage(ac,av); /**exits***/
		}

		if( strcmp( arg, "-velocity_model" ) == 0 || strcmp( arg, "-mod" ) == 0 )
		{
			i++;
			if( av[i] != NULL ) {
				strcpy( model_name, av[i] );
			
			} else {
				fprintf(stderr, "%s: %s: %s: Error %s requires velocity_model basename\n",
                                        progname, __FILE__, __func__, arg );
				exit(-1);
			}

			fprintf( stderr, "%s: -velocity_model : %s\n", progname, model_name );
		
			/*** check file exists **************************************************/
		
			if( ( mtinv_path = getenv( "MTINV_PATH" )) != NULL )
			{
				sprintf( model_file_path, "%s/data/modeldb", mtinv_path );

				sprintf( model_file_full_path, "%s/data/modeldb/%s.mod",
					mtinv_path, model_name );

				if( access( model_file_full_path, F_OK ) == 0 )
				{
					fprintf( stderr, "%s: %s: %s velocity model file %s ok\n",
						progname, __FILE__, __func__, model_file_full_path );
				}
				else
				{
					fprintf( stderr, "%s: %s: %s velocity model file %s is not part of distrib\n",
						progname, __FILE__, __func__, model_name );
				}
			}
          		else
			{
				fprintf(stderr, "%s: %s: %s:\n\n", progname, __FILE__, __func__ );
				fprintf(stderr, "\t The enviromental variable MTINV_PATH in not set in your shell cshrc, tcshrc, bashrc, or zshrc file.\n");
				fprintf(stderr, "\t e.g., setenv MTINV_PATH /Users/ichinose1/Work/mtinv.v4.0.1 \n" );
				fprintf(stderr, "\n\n" );
			}

       			/**************************/
                        i++;
                        continue;
		}

	/*** interactive mode adds (uncomments) active display of images in shell scripts, in automatic mode, user would not want 1000's of images to pop up on screen ***/

		if( strcmp( arg, "-interactive" ) == 0 || strcmp( arg, "-int" ) == 0 )
		{
			fprintf( stderr, "%s: -interactive or -int generates scripts allows user interactivly view plots\n", progname );

			irt = 0;
			i++;
			continue;
			
		} 

	/*** load station exclude list, for pesty stations with problems, dont include in makegrnlib, best for automatic processing ***/

		if( strcmp( arg, "-exclude_sta" ) == 0 || strcmp( arg, "-Xsta" ) == 0 ) 
		{
			i++;
			if( av[i] != NULL ) {
                                strcpy( exclude_station_file, av[i] );

                        } else {
                                fprintf(stderr, "%s: %s: %s: Error %s requires exclude_sta file\n",
                                        progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

			nsta_exclude_list = 0;
			load_exclude_station_list( exclude_station_file, exclude_station_list, &nsta_exclude_list );

			fprintf( stderr, "%s: %s: %s: loaded %d stations from exclude_station_file = %s\n", 
				progname, __FILE__, __func__, nsta_exclude_list, exclude_station_file );

			for( ista = 0; ista < nsta_exclude_list; ista++ )
			{
				fprintf( stderr, "%s: %s: %s: ista = %d (%s)\n",
					progname, __FILE__, __func__, ista, exclude_station_list[ista] );
			}

			i++;
			continue;
		}

	/*** event lat log ***/

		if( strcmp( arg, "-ev" ) == 0 || strcmp( arg, "-event" ) == 0 )
		{
			i++;
			if( av[i] != NULL ) {
				evla = atof( av[i] ); 
			}else{
				fprintf( stderr, "%s: %s: %s: Error %s requires latitude longitude pair\n",	
					progname, __FILE__, __func__, arg );
				exit(-1);
			}

			i++;
			if( av[i] != NULL ) {
                                evlo = atof( av[i] ); 
                        }else{
                                fprintf( stderr, "%s: %s: %s: Error %s requires latitude longitude pair\n",     
                                        progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

			fprintf( stderr, "%s: -ev or -event : evla=%g evlo=%g\n", progname, evla, evlo );
			i++;
			continue;
		}

	/*** filter bands ***/
		if( strcmp( arg, "-fil" ) == 0 || strcmp( arg, "-filter" ) == 0  )
		{
			i++;
                        if( av[i] != NULL ) {
                                lf = atof( av[i] );
                        }else{
                                fprintf( stderr, "%s: %s: %s: Error %s requires bandpass filter low-freq-corner and high-freq-corner pair\n",
                                        progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

                        i++;
                        if( av[i] != NULL ) {
                                hf = atof( av[i] );
                        }else{
                                fprintf( stderr, "%s: %s: %s: Error %s requires bandpass filter low-freq-corner and high-freq-corner pair\n",
                                        progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

                        fprintf( stderr, "%s: -fil or -filter : bandpass butterworth num_poles=3 corners lf=%g hf=%g num_pass=2\n",
				progname, lf, hf );
                        i++;
                        continue;
		}

	/*** source depth ***/
		if( strcmp( arg, "-z" ) == 0 || strcmp( arg, "-depth" ) == 0  )
		{
			i++;
			depth = get_cmdline_float( i, arg, av, "depth" );
			fprintf( stderr, "%s: -z or -depth = %g\n", progname, depth );
			i++;
			continue;
		}

	/*** comment, this is displayed in most plots  ***/

		if( strcmp( arg, "-com" ) == 0 || strcmp( arg, "-comment" ) == 0  )
		{
			i++;

			if( av[i] != NULL ) { 
                                strcpy( comment, av[i] );
                        
                        } else {
                                fprintf(stderr, "%s: %s: %s: Error %s requires comment\n",
					progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

			fprintf( stderr, "%s: -com or -comment : %s\n", progname, comment );
			i++;
			continue;
		}

		if( strcmp( arg, "-ot" ) == 0 || strcmp( arg, "-origintime" ) == 0  ) 
		{
			i++;

			if( av[i] != NULL ) { 
                                strcpy( timestring, av[i] );
                        
                        } else {
                                fprintf(stderr, "%s: %s: %s: Error %s requires origin-time string\n",
                                        progname, __FILE__, __func__, arg );
                                exit(-1);
                        }

			ot = calloc(1,sizeof(MyTime));
			parsestring( ot, timestring );
			fprintf( stderr, "%s: -ot or -origintime : ", progname ); 
				WriteMyTime2STDERR( ot );
			i++;
			continue;
		}

		if( strcmp( arg, "-verbose" ) == 0 || strcmp( arg, "-v" ) == 0  )
		{
			verbose = 1;
			fprintf( stderr, "%s: Switch %s found for verbosity, continuing...\n", 
                                progname, arg );
			i++;
                        continue;
		}

		if( strcmp( arg, "-gmt4" ) == 0 ) 
		{
			fprintf( stderr, "%s: Switch %s found for GMT version 5.x.x, continuing...\n", 
				progname, arg );
			igmt4 = 1;
			i++;
			continue;
		}

		if( strcmp( arg, "-Nsta" ) == 0 ) 
		{
			i++;
			max_number_stations_invert = get_cmdline_integer( i, arg, av, "Nsta" );
			fprintf( stderr, "%s: Maximun number of stations to invert = %d\n",
                                progname, max_number_stations_invert );
                        i++;
                        continue;
		}

		if( strcmp( arg, "-Ngrn" ) == 0 ) 
		{
			i++;
			max_number_stations_compute_grns = get_cmdline_integer( i, arg, av, "Ngrn" );
			fprintf( stderr, "%s: Maximun number of stations to process Greens functions = %d\n",
                                progname, max_number_stations_compute_grns );
                        i++;
                        continue;
		}

		if( strcmp( arg, "-MinmDistInv" ) == 0 )
                {
                        i++;
                        min_distance_define_km = get_cmdline_float( i, arg, av, "MinDistInv" );
                        fprintf( stderr, "%s: Minimum distance in kilometers to invert = %g\n",
                                progname, min_distance_define_km );
                        i++;
                        continue;
                }


		if( strcmp( arg, "-MaxDistInv" ) == 0 ) 
		{
			i++;
			max_distance_define_km = get_cmdline_float( i, arg, av, "MaxDistInv" );
			fprintf( stderr, "%s: Maximun distance in kilometers to invert = %g\n", 
                                progname, max_distance_define_km );
                        i++;
                        continue;
		}

		if( strcmp( arg, "-MaxDistProc" ) == 0 ) 
		{
			i++;
			max_distance_process_km = get_cmdline_float( i, arg, av, "MaxDistProc" );
			fprintf( stderr, "%s: Maximun distance in kilometers to process Greens functions = %g\n",
                                progname, max_distance_process_km );
                        i++;
                        continue;
		}

		if( strcmp( arg, "-DataDir" ) == 0 || strcmp( arg, "-data" ) == 0 )
		{
			i++;
			if( av[i] != NULL ) {
				strcpy( DataDir, av[i] );
			} else {
				fprintf(stderr, "%s: %s: %s: Error %s requires Data Directory\n",
					progname, __FILE__, __func__, arg );
				exit(-1);
			}
			fprintf( stderr, "%s: -DataDir : %s\n", progname, DataDir );
			i++;
			continue;
		}

		if( strcmp( arg, "-RespDir" ) == 0 || strcmp( arg, "-resp" ) == 0 )
		{
			i++;
			if( av[i] != NULL ) {
				strcpy( RespDir, av[i] );
			} else {
				fprintf(stderr, "%s: %s: %s: Error %s requires Response Directory\n",
					progname, __FILE__, __func__, arg );
				exit(-1);
			}
			i++;
			continue;
		}

	/*************************************************************************/
	/*** after checking all cmdline flags, then test this arg for sacfile ***/
	/*************************************************************************/

		strcpy( sacfilename, arg );

		if( (fp=fopen( arg, "rb")) == NULL )
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
			if( (data = readsac( &s[nsta++], sacfilename, verbose )) == NULL )
			{
				nsta--;
				fprintf( stderr, "%s: cannot fopen file %s, continuing...\n",
                                	progname, sacfilename );
				i++;
				continue;
			}
                	i++;
		}
        } /*** end loop over command line arguments ***/

	fprintf( stderr, "%s: %s: %s: TOTAL loaded SAC nsta=%d\n",
                progname, __FILE__, __func__, nsta );

/****************************************************************/
/*** check required cmdline input variables for valid values ***/
/****************************************************************/
	
	if( depth == -999 || evla == -999 || evlo == -999 || ot == NULL )
	{
		fprintf( stderr, "%s: %s: %s: required real event depth, lat, lon, and origin-time\n",
			progname, __FILE__, __func__ );
		exit(-1);
	}else{
		 fprintf( stderr, "%s: %s: %s: depth = %g (km) lat = %g (degN) lon = %g (degE) origin-time =",
			progname, __FILE__, __func__, depth, evla, evlo );
		WriteMyTime2STDERR( ot );
	}

/*** check the model name and exists file ***/

	if( strcmp( model_name, "\0" ) == 0 ) 
	{
		fprintf( stderr, "%s: %s: %s: A velocity model is required, list of models available:\n", progname, __FILE__, __func__ );
	
		if( ( mtinv_path = getenv( "MTINV_PATH" )) != NULL )
		{
			sprintf( model_file_path, "%s/data/modeldb", mtinv_path );
			list_directory_files_with_extension( model_file_path, ".mod" );
			exit(-1);
		}
		else
		{
		     /*** environmental variable MTINV_PATH not set, using wus.mod as last ditch effort ***/
			strcpy( model_name, "wus" );
		}
/***
		if( ( mtinv_path = getenv( "MTINV_PATH" )) != NULL )
		{
			sprintf( cmdline, "ls %s/data/modeldb / *.mod", mtinv_path );
			printf( "%s\n", cmdline );
			system( cmdline );
			exit(-1);
		}
		else
		{
			strcpy( model_name, "wus" );
		}
***/

	}
	fprintf( stderr, "%s: %s: %s: velocity model %s\n", progname, __FILE__, __func__, model_name );

/*** print setup scheme ***/

	if( nsta > 0 )
	{
		fprintf( stderr, "%s: processing %d number of stations.\n", progname, nsta );
	}
	else
	{
		fprintf( stderr, "%s: no stations found. EXITING.\n", progname );
		exit(0);
	}

/*** compute the source receiver geometry ****/

	if( evla != -999 && evlo != -999 )
	{
		for( i = 0; i < nsta; i++ )
		{
			s[i].evla = evla;
			s[i].evlo = evlo;
			distaz( (double)evla, (double)evlo, (double)s[i].stla, (double)s[i].stlo,
				&drdistkm, &daz, &dbaz );
			s[i].dist = (float)drdistkm;
			s[i].az   = (float)daz;
			s[i].baz  = (float)dbaz;

                /**
                        if(verbose)
                        {
                          fprintf( stderr, "%s: %s: %s: i=%d net=%s sta=%s loc=%s chan=%s evla=%g evlo=%g dist=%g az=%g baz=%g\n",
                                progname, __FILE__, __func__,
                                i,
                                s[i].knetwk, s[i].kstnm, s[i].khole, s[i].kcmpnm,
                                s[i].evla, s[i].evlo,
                                s[i].dist, s[i].az, s[i].baz );
                        }
                ***/

		}
	}

/*** fix khole problem with downloading from webservice***/

	for( i = 0; i < nsta; i++ )
	{
		/* fprintf( stdout, "khole=(%s)\n", s[i].khole ); */
		if( strncmp( s[i].khole, "-12345", 6 ) == 0 ) 
		{
			strcpy( s[i].khole, "" );
		}
		/* fprintf( stdout, "khole=(%s)\n", s[i].khole ); */
	}

/*** make a GMT map of stations ***/

	if( igmt4 )
	{
		if(verbose)fprintf(stderr, "%s: make gmt4 map\n", progname );
		make_gmt4_map( s, nsta, "plotmap.csh", irt, verbose );
	}
	else
	{
		if(verbose) fprintf(stderr, "%s: make gmt5 map\n", progname );
		make_gmt5_map( s, nsta, "plotmap.csh", irt, verbose );
	}
	
	if(verbose)
	{
		fprintf( stderr, "%s: main(): calling make_glib_parfile(): \n", progname );
	}

	make_glib_parfile( ot, s, nsta, depth, comment, igmt4, irt,
		max_number_stations_invert,
		max_number_stations_compute_grns,
		min_distance_define_km,
		max_distance_define_km,
		max_distance_process_km,
		lf,
		hf,
		"makeglib.csh",
		model_name,
		evid,
		DataDir,
		RespDir,
		exclude_station_list,
		nsta_exclude_list,
		verbose );

	if(ihspec96)
	{
	  create_hspec96_script( depth, nsta, s, 
		model_name, model_file_path,
		exclude_station_list, nsta_exclude_list, 
		evla, evlo, verbose );
	}

	create_cleanup_script();

	exit(0);

} /*** end of main() ***/


typedef struct {
	char net[8];
	char sta[8];
	char loc[8];
	char ref_chan[8];
	int nchan;
	int nseg; /*** multiple files typically indicates segmentation, need to check to ensure signal is in right segement or join into single file ***/
	char chans[256];

	char model_name[64];
	float dist;
	float az;

	int def; /* 0=nondef 1=def */
	char nslc_string[32];
} Chanlist;


void make_glib_parfile( 
	MyTime *ot, 
	Sac_Header *s, 
	int nsta, 
	float depth,
	char *comment, 
	int igmt4,
	int irt,
	int max_number_stations_invert,
	int max_number_stations_compute_grns,
	float min_distance_define_km,
	float max_distance_define_km,
	float max_distance_process_km,
	float lf,
	float hf, 
	char *script_filename,
	char *model_name,
	long evid,
	char *DataDir,
	char *RespDir,
	char exclude_station_list[256][16],
	int nsta_exclude_list, 
	int verbose )
{
	FILE *fp;
	int i = 0, j = 0;

	char gmt_version_string[12];
	char realtime_string[12];
	char com[3];
	char *mtinv_path;
	int icount_def = 1;
	int icount_grn = 1;
	int ista = 0, def = 0;
	char loc[18];

	float dt = 0.35;
	float get_waveform_sampling_rate_by_distancekm( float rdistkm );

/*** sort by distance ***/
	int *indx;
	float *dist;
	float rdistkm;
	void sort_sac_headers_by_dist( int n, Sac_Header *s, float *arrin, int *indx );

	Chanlist *cl;
	int ncl;
	Chanlist *remove_duplicate_chans( int n, Sac_Header *s, float *arrin, int *indx, int *ncl );
	void sort_Chanlist_by_dist( int ncl, Chanlist *cl, float *dist, int *indx );

/************************/
/*** start subroutine ***/
/************************/

/**** dont sort the sac headers by distance anymore, use the channel list data structure below ****/

/****
	if(verbose)
	{
	  fprintf( stderr,
		"%s: make_glib_parfile(): calling sort_sac_headers_by_dist(): nsta=%d\n",
			progname, nsta );
	}
	dist = calloc( nsta+1, sizeof(float) );
	indx = calloc( nsta+1, sizeof(int) );
	sort_sac_headers_by_dist( nsta, s, dist, indx ); 
	if(verbose)
		fprintf( stderr, "%s: make_glib_parfile(): done with sort_sac_headers_by_dist():\n", progname );
****/

/** ***/
/*** channel list data structure, removes duplicate channels and stations due to multiple SAC file segments of the same NSLC ****/
/** ***/
	
	cl = calloc( nsta+1, sizeof(Chanlist) );
	cl = remove_duplicate_chans( nsta, s, dist, indx, &ncl );

	dist = calloc( ncl+1, sizeof(float) );
	indx = calloc( ncl+1, sizeof(int) );
	sort_Chanlist_by_dist( ncl, cl, dist, indx );

	if(verbose)
	{
	 for( i = 0; i < ncl; i++ )
	 {
		fprintf( stdout, "%s: %s: %s: i=%d NSL=(%s) dist=%g az=%g nchan=%d chans=%s\n", 	
			progname,
			__FILE__, __func__,
			i,
			cl[indx[i+1]-1].nslc_string,
			cl[indx[i+1]-1].dist,
			cl[indx[i+1]-1].az,
			cl[indx[i+1]-1].nchan,
			cl[indx[i+1]-1].chans );
	 }
	}

	if( (fp=fopen( script_filename,"w" )) == NULL )
        {               
                fprintf( stderr, "%s: cannot open file %s for writting\n",
                        progname, script_filename );
                exit(-1);
        }
	
	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "######################################################################\n" );
	fprintf( fp, "### Autogenerated C-shell script by setupMT.c                      ###\n" );
	fprintf( fp, "###  This C-shell script sets up mkgrnlin to create                ###\n" );
	fprintf( fp, "###  Green's functions from all the SAC data hdrs                  ###\n" );
	fprintf( fp, "######################################################################\n" );
	fprintf( fp, "### DataDir = (%s)\n", DataDir );
	fprintf( fp, "### RespDir = (%s)\n", RespDir );
	fprintf( fp, "######################################################################\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "### don't forget to add ${MTINV_PATH}/bin to your executable path in the shell startup file ###\n" );
	fprintf( fp, "\n" );

	if( ( mtinv_path = getenv( "MTINV_PATH" )) == NULL )
	{
	  /* fprintf( fp, "setenv MTINV_PATH /Users/ichinose1/Work/mtinv.v4.0.0\n" ); */
		fprintf(stderr, "\n\n" );
		fprintf(stderr, "%s: %s: %s:\n\n", progname, __FILE__, __func__ );
		fprintf(stderr, "\t The enviromental variable MTINV_PATH in not set in your shell cshrc, tcshrc, bashrc, or zshrc file.\n");
		fprintf(stderr, "\t e.g., setenv MTINV_PATH /Users/ichinose1/Work/mtinv.v4.0.1 \n" );
		fprintf(stderr, "\n\n" );
	}
	else
	{
	  fprintf( fp, "setenv MTINV_PATH %s\n", mtinv_path );
	}

	fprintf( fp, "\n" );
	fprintf( fp, "echo \" MTINV_PATH = ${MTINV_PATH} \" \n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### %4d/%02d/%02dT%02dh%02dm%05.2f %g %g %s\n",
		ot->year, ot->month, ot->mday, ot->hour, ot->min, ot->fsec,
		s[0].evla, s[0].evlo, comment );
	
	fprintf( fp, "\n" );
	fprintf( fp, "cat >! %s.par << EOF\n", model_name );
	fprintf( fp, "velmod=%s\n", model_name );

	if( depth >= 0 && depth < 33 )
	{
		fprintf( fp, "#zrange=3,3,33\n" );
		fprintf( fp, "zrange=1,1,25\n" );
	}
	else if( depth >= 33 && depth < 100 )
	{
		fprintf( fp, "zrange=10,10,100\n" );
	}
	else if( depth >= 100 && depth < 300 )
	{
		fprintf( fp, "zrange=10,20,300\n" );
	}
	else if( depth >= 300 && depth < 700 )
	{
		fprintf( fp, "zrange=100,50,700\n" );
	}
	else
	{
		fprintf( fp, "zrange=3,3,33\n" );
	}

	fprintf( fp, "evla=%g\n", s[0].evla );
	fprintf( fp, "evlo=%g\n", s[0].evlo );
	fprintf( fp, "dt=0.15\n" );
	fprintf( fp, "nt=2048\n" );
	fprintf( fp, "fmax=0.4\n" );
	fprintf( fp, "t0=0.\n" );
	fprintf( fp, "redv=18.\n" );
	fprintf( fp, "damp=1.\n" );
	fprintf( fp, "kmax=20000\n" );
	fprintf( fp, "eps=0.0005\n" );
	fprintf( fp, "smin=0.0005\n" );
	fprintf( fp, "modeldb=${MTINV_PATH}/data/modeldb/\n" );
	fprintf( fp, "stadb=%s/rdseed.stations\n", DataDir );
	fprintf( fp, "noverbose\n" );
	fprintf( fp, "nodump\n" );
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );
	fflush( fp );

	fprintf( fp, "cat >! mkgrnlib.par << EOF\n" );
	fflush( fp );

	icount_def=1;
	icount_grn=1;

	fprintf( fp, "### station-code network-code location-code mkgrnlib.parfile dt(sec/sample) ### comments\n" );

	for( j = 0; j < ncl; j++ )
	{
		i = indx[j+1]-1;

		snprintf( loc, 10, "\"%s\"", cl[i].loc );

		if(verbose)
                {
                  fprintf( stdout,  "%s: %s: %s: writting to mkgrnlib.par j=%d indx[j+1]=%d i=%d sta=%s loc=(%s) loc=(%s)\n",
			progname,  __FILE__, __func__, j, indx[j+1], i, cl[i].sta, cl[i].loc, loc );
		  fflush( stdout );
                }

		rdistkm = cl[i].dist;
		dt = get_waveform_sampling_rate_by_distancekm( rdistkm );

/*** defautls ***/
/***
        int max_number_stations_invert = 8;
        int max_number_stations_compute_grns = 30;
        float max_distance_process_km = 900;
        float max_distance_define_km  = 800;
	float min_distance_define_km  = 70;
***/
	/*** defining in inversion ***/

	/*** first by distance ***/

		if(	( rdistkm > 0 ) &&
			( rdistkm <= max_distance_define_km ) &&
			( icount_def <= max_number_stations_compute_grns ) )
		{
			def = 1;
		}
		else
		{
			def = 0;
		}

	/*** turn off no traditional location codes ***/

		if( strcmp( cl[i].loc, "" )   == 0 || strcmp( cl[i].loc, "00" )     == 0 ||
		    strcmp( cl[i].loc, "10" ) == 0 || strcmp( cl[i].loc, "-12345" ) == 0 || 
		    strcmp( cl[i].loc, "20" ) == 0 || strcmp( cl[i].loc, "01" )     == 0 ||
		    strcmp( cl[i].loc, "02" ) == 0 )
		{
			/* do nothing */
		}
		else
		{
			def = 0;
		}

	/*** if there is a station exclude list, turn off those stations ***/

		for( ista = 0; ista < nsta_exclude_list; ista++ )
		{
			if( strcmp( cl[i].sta, exclude_station_list[ista] ) == 0 ) def = 0;
		}

		if( def == 1 )
		{
			strcpy(com,"");  /*** defining in inversion ***/
			icount_def++;
		}
		else 
		{
			strcpy(com,"# ");  /*** non-defining in inversion ***/
		}
		icount_grn++;

		fprintf( fp, "%s%-8s %-4s %6s %s.par %9.3f ### R=%6.0f Az=%03.0f %-12.12s nchan=%d nseg=%d (%s)\n", 
			com,
			cl[i].sta, 
			cl[i].net, 
			loc,
			model_name, 
			dt, 
			cl[i].dist, 
			cl[i].az,
			cl[i].nslc_string,
			cl[i].nchan,
			cl[i].nseg,
			cl[i].chans );
		fflush( fp );
	}

	fprintf( fp, "EOF\n" );
	fflush( fp );

	fprintf( fp, "\n" );
	fflush( fp );


	fprintf( fp, "multithread_mkgrnlib \\\n" );
	fprintf( fp, "     parfile=mkgrnlib.par \\\n" );
	fprintf( fp, "     executable_pathname=${MTINV_PATH}/bin/mkgrnlib > multithread_mkgrnlib.out\n" );
	fprintf( fp, "\n" );
	fflush( fp );

	if( igmt4 ) 
		strcpy( gmt_version_string, "nogmt5" );
	else
		strcpy( gmt_version_string, "gmt5" );
	
	if( irt )
	{
		strcpy( realtime_string, "realtime" );
	}
	else
	{
		strcpy( realtime_string, "norealtime" );
	}

	fprintf( fp, "makepar com=\"%s\" \\\n", comment );
        fprintf( fp, "    date=\"%4d-%02d-%02dT%02d:%02d:%05.2f\" \\\n", 
		ot->year, ot->month, ot->mday, 
		ot->hour, ot->min, ot->fsec );
        fprintf( fp, "    DataDir=%s \\\n", DataDir );
        fprintf( fp, "    RespDir=%s \\\n", RespDir );
        fprintf( fp, "    %s nooracle nomysql sqlite \\\n", gmt_version_string );
	fprintf( fp, "    maxsta=%d mindist=%g maxdist=%g \\\n", 
		max_number_stations_invert, 
		min_distance_define_km,
		max_distance_define_km );

	fprintf( fp, "    lf=%g hf=%g \\\n", lf, hf ); 
	if(evid>0)fprintf( fp, "    evid=%ld \\\n", evid );
	fprintf( fp, "    minsnr=3.0 ctol=0.85 maxshift=10 %s nolocal *.glib\n", realtime_string );


	fprintf( fp, "###\n" );
	fprintf( fp, "### dump and plot the greens functions for single depth\n" );
	fprintf( fp, "###\n" );

	for( j = 0; j < ncl; j++ )
	{
                i = indx[j+1]-1;
                snprintf( loc, 10, "\"%s\"", cl[i].loc );
		fprintf( fp, "# grnlib2sac glib=%s.%s.%s.%s.glib dumpgrn plotgrn z=1\n",
			cl[i].net,
			cl[i].sta, 
                        loc,
                        model_name );
	}

	fclose(fp);

	free(dist);
	free(indx);

	chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );
}

void sort_Chanlist_by_dist( int ncl, Chanlist *cl, float *dist, int *indx )
{
	int i;
	void indexx( int n, float *arrin, int *indx );

	if( ncl == 1 ) 
	{
		dist[1] = cl[0].dist;
		indx[1] = 1;
		return;
	}

	for( i = 0; i < ncl; i++ )
	{
		dist[i+1] = cl[i].dist;
		indx[i+1] = i;
	}

	indexx( ncl, dist, indx );
}

void sort_sac_headers_by_dist( int nsta, Sac_Header *s, float *dist, int *indx )
{
	int i;
	void indexx( int n, float *arrin, int *indx );
	int verbose = 0;

	for( i = 0; i < nsta; i++ )
	{
		dist[i+1] = s[i].dist;
		indx[i+1] = i;
	}

	if( nsta == 1 ) {
		dist[1] = s[0].dist;
		indx[1] = 1;
		return;
	}

	indexx( nsta, dist, indx );

	if(verbose)
	{
	  for( i = 1; i <= nsta; i++ )
	  {
		fprintf( stdout, "nsta=%d i=%d dist[i]=%g indx[i]=%d dist[indx]=%g %g %s.%s.%s\n", 
			nsta, i, dist[i], indx[i], dist[indx[i]], 
			s[indx[i]-1].dist, s[indx[i]-1].knetwk, s[indx[i]-1].kstnm, s[indx[i]-1].khole );
	  }
	}
}

void make_gmt4_map( Sac_Header *s, int nsta, char *script_filename, int irt, int verbose )
{
	FILE *fp;
        int i = 1;
        float maxlat = -999, maxlon = -999;
        float minlat = +999, minlon = +999;
	float aspect_ratio;
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

	maxlat = s[0].evla;
        minlat = s[0].evla;
        maxlon = s[0].evlo;
        minlon = s[0].evlo;

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
        if(verbose) fprintf( stderr, "%s: aspect_ratio = %g\n", progname, aspect_ratio );

        if( fabs( maxlon - minlon ) < 3 )
        {
                maxlon += 2;
                minlon -= 2;
        }

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

        fprintf( fp, "pscoast -R%g/%g/%g/%g -JM6i -Di ", minlon, maxlon, minlat, maxlat );
        fprintf( fp, " -N1/1.2p,black,5_2:0p -N2/0.8p,black,5_2:0p " );
        fprintf( fp, " -A1000 -W1p,black -Glightgray -P -K >! %s\n", PS_filename );
        fprintf( fp, "\n");

        fprintf( fp, "psxy -R -JM -m -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "> -W1p,black %s.%s.%s%s\n",
                        s[i].kstnm, s[i].knetwk, s[i].kcmpnm, s[i].khole );
                fprintf( fp, "%g %g\n", s[i].evlo, s[i].evla );
                fprintf( fp, "%g %g\n", s[i].stlo, s[i].stla );
        }
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n");

        fprintf( fp, "psxy -R -JM -St0.15i -W1p,black -Gred -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "%g %g %s.%s.%s%s\n",
                        s[i].stlo, s[i].stla,
                        s[i].kstnm, s[i].knetwk,
                        s[i].kcmpnm, s[i].khole );
        }
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n");

	fprintf( fp, "#Reads (x,y[,fontinfo,angle,justify],text) from <table> [or stdin].\n" );

        fprintf( fp, "pstext -R -JM -N -D0.05i/0.1i -Wwhite -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
		if( strncmp( s[i].knetwk, "-12345", 6 ) == 0 ) 
		{
			fprintf( fp, "%g %g 6 0 0 0 %s\n", 
				s[i].stlo, s[i].stla, s[i].kstnm );
		}
		else
		{
                  fprintf( fp, "%g %g 6 0 0 0 %s.%s.%s%s\n",
                        s[i].stlo, s[i].stla,
                        s[i].kstnm, s[i].knetwk,
                        s[i].kcmpnm, s[i].khole );	
		}
        }
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n");

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
          fprintf( fp, "echo %g %g | ", sumlon, sumlat );
          fprintf( fp, "psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
        }
        else
        {
          fprintf( fp, "echo %g %g | ", s[0].evlo, s[0].evla );
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

        fprintf( fp, "psbasemap -R -JM -Bf%ga%g/f%ga%gNSEW -Lx1/1/%g/100 -O >> %s\n",
                ticklength, annotation, ticklength, annotation, s[0].evla, PS_filename );
        fprintf( fp, "\n");

        fprintf( fp, "ps2raster -A  -Tj -E300 %s\n", PS_filename );
        fprintf( fp, "/bin/rm -f %s gmt.conf gmt.history .gmtdefaults4 .gmtcommands4\n", PS_filename );

	if(irt)
		fprintf( fp, "#echo use xdg-open, eog, xv to view jpg\n" );
	else
		fprintf( fp, "xdg-open %s\n", JPG_filename );

        fclose(fp);

        chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );
	
	sprintf( command_line, "./%s", script_filename );
	fprintf( stderr, "%s: running command %s\n", progname, command_line );
	system( command_line );
}

/*** GMT Version 5.x.x ****/

void make_gmt5_map( Sac_Header *s, int nsta, char *script_filename, int irt, int verbose )
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
		fprintf( stderr, "%s: %s: %s: error evla and evlo is null\n",
			progname, __FILE__, __func__ );
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
	
/***/
/*** convert all coordinates to colatitude(0(NP)-180(SP)) and longitude(0-360) ***/
/***/
	maxlat = s[0].evla;
	minlat = s[0].evla;
	maxlon = s[0].evlo;
	minlon = s[0].evlo;

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
	if(verbose) fprintf( stderr, "%s: aspect_ratio = %g\n", progname, aspect_ratio );

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

/*** make GMT version 5.x.x map ***/

	fprintf( fp, "#!/bin/csh\n");
        fprintf( fp, "###################################################################################\n");
        fprintf( fp, "## This C-shell script was automatically generated by mtinv                      ##\n");
        fprintf( fp, "## and requires Generic Mapping Tools (GMT) http://gmt.soest.hawaii.edu/         ##\n");
        fprintf( fp, "## The script plots the mechanism at the event location and station locations    ##\n");
        fprintf( fp, "## on a map. This script uses GMT version 5.x.x only see usage for ver 4.x.x     ##\n");
        fprintf( fp, "###################################################################################\n");
	fprintf( fp, "\n" );

	fprintf( fp, "gmt set MAP_FRAME_TYPE plain\n" );
        fprintf( fp, "gmt set FORMAT_GEO_OUT DG\n" );
        fprintf( fp, "gmt set FORMAT_GEO_MAP DG\n" );
        fprintf( fp, "\n");

	fprintf( fp, "gmt pscoast -R%g/%g/%g/%g -JM6i -Di ", minlon, maxlon, minlat, maxlat );
	fprintf( fp, " -N1/1.2p,black,5_2:0p -N2/0.8p,black,5_2:0p " );
	fprintf( fp, " -A1000 -W1p,black -Glightgray -P -K >! %s\n", PS_filename );
	fprintf( fp, "\n");

        fprintf( fp, "gmt psxy -R -JM -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "> -W1p,black %s.%s.%s%s\n", 
			s[i].kstnm, s[i].knetwk, s[i].kcmpnm, s[i].khole );
                fprintf( fp, "%g %g\n", s[i].evlo, s[i].evla );
                fprintf( fp, "%g %g\n", s[i].stlo, s[i].stla );
        }
        fprintf( fp, "EOF\n" );
	fprintf( fp, "\n");

        fprintf( fp, "gmt psxy -R -JM -St0.15i -W1p,black -Gred -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "%g %g %s.%s.%s%s\n", 
			s[i].stlo, s[i].stla, 
			s[i].kstnm, s[i].knetwk, 
			s[i].kcmpnm, s[i].khole );
        }
        fprintf( fp, "EOF\n" );
	fprintf( fp, "\n");

	fprintf( fp, "#Reads (x,y[,fontinfo,angle,justify],text) from <table> [or stdin].\n" );

        fprintf( fp, "gmt pstext -R -JM -C0.01i/0.01i -N -D0.05i/0.1i -W0p,white -Gwhite " );
	fprintf( fp, "  -F+f8p,Times-Roman,blue+jBL -O -K >> %s << EOF\n", PS_filename );
        for( i = 0; i < nsta; i++ )
        {
                fprintf( fp, "%g %g %s.%s.%s%s\n", 
			s[i].stlo, s[i].stla, 
			s[i].kstnm, s[i].knetwk,
			s[i].kcmpnm, s[i].khole );
        }
        fprintf( fp, "EOF\n" );
	fprintf( fp, "\n");

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
	  fprintf( fp, "echo %g %g | ", sumlon, sumlat );
          fprintf( fp, "gmt psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
	}
	else
	{
          fprintf( fp, "echo %g %g | ", s[0].evlo, s[0].evla );
	  fprintf( fp, "gmt psxy -R -JM -Sa0.2i -W1p,black -Gmagenta -O -K >> %s\n", PS_filename );
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

        fprintf( fp, "gmt psbasemap -R -JM -Bxf%ga%g -Byf%ga%g -BNSEW -Lx1/1/%g/100 -O >> %s\n",
                ticklength, annotation, ticklength, annotation, s[0].evla, PS_filename );
	fprintf( fp, "\n");

        fprintf( fp, "gmt psconvert -A -Z -Tj -E300 %s\n", PS_filename );
	fprintf( fp, "/bin/rm -f gmt.conf gmt.history\n" );

	if(irt)
		fprintf( fp, "#echo use eog xv open to view jpg\n" );
	else
		fprintf( fp, "open %s\n", JPG_filename );

	fclose(fp);

	chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );

	sprintf( command_line, "./%s", script_filename ); 
	fprintf( stderr, "%s: running command %s\n", progname, command_line );
       	system( command_line );
}

void Usage( int ac, char **av )
{
	fprintf( stderr, "\n\n" );
	fprintf( stderr, "%s: \n", av[0] );
	fprintf( stderr, "Usage: %s --hspec96 -z [depth(km)] -ev [lat lon] -mod [model_basename] -ot [\"YYYY-MM-DDTHH:mm:SS.SS\"]\n", progname );
	fprintf( stderr, "\t       -com [\"Comment\"] -fil [lo_f(Hz) hi_f(Hz)] -evid [eventID] -exclude_sta [file name]\n" );
	fprintf( stderr, "\t       -gmt4 -interactive -help -verbose [sac files]\n" );
 
	fprintf( stderr, "\n\n" );
	fprintf( stderr, "REQUIRED: \n" );
	fprintf( stderr, "\t -z   or -depth (float) source depth in kilometers no default (doesn't overwrite SAC files)\n" );
	fprintf( stderr, "\t -ev  or -event (float float) source latitude and longitude in decimal degrees no default\n" );
	fprintf( stderr, "\t -ot  or -origintime (string) source origin time format: YYYY/MM/DD,hh:mm:ss.ss or YYYY-MM-DDThh:mm:ss.ss\n" );
	fprintf( stderr, "\t -com or -comment (string) a comment no default, use double quotes for spaces\n" );
	fprintf( stderr, "\t -mod or -velocity_model (string) 1D model base filename, see ${MTINV_PATH}/data/modeldb/*.mod (default wus)\n" );
	fprintf( stderr, "REQUIRED: \n" );
        fprintf( stderr, "\t list of sac files can use wildcards, only need 1 component per sta, e.g., ../Data/*.BHZ.?.SAC ../Data/*.BHZ.SAC \n" );

	fprintf( stderr, "OPTIONAL: \n" );
	
	fprintf( stderr, "\t -fil or -filter (float float) bandpass filter frequency corners [default 0.02 0.05 Hz]\n" );
	fprintf( stderr, "\t -gmt4 (boolean flag) creates all GMT www.generic-mapping-tools.org plots using older GMT version 4 [default GMT 5+]\n" );
	fprintf( stderr, "\t -h or -help (boolean flag) prints this page default off\n" );
	fprintf( stderr, "\t -evid (long int only) Event-Identification unique database key [default -1 null]\n" );
	fprintf( stderr, "\t -v or -verbose (boolean flag) verbose output\n" );
	fprintf( stderr, "\t -interactive or -int (boolean flag) interactive run script and display map on screen [default off]\n" );
	fprintf( stderr, "\t -exclude_sta or -Xsta (string) file name.  Station codes to ignore, default off, no Greens func computed\n"); 
	fprintf( stderr, "\n" );
	fprintf( stderr, "\t  -Nsta       automatic maximum number of stations made defining for MT inversion \n" );
	fprintf( stderr, "\t  -Ngrn       automatic maximum number of stations processed for Green's functions \n" );
	fprintf( stderr, "\t  -MaxDistInv  automatic maximum distance for station made defining for MT inversion \n" );
	fprintf( stderr, "\t  -MaxDistProc automatic maximum distance for stations processed for Green's functions \n" );

	fprintf( stderr, "\t --hspec96    create scripts for computing R.Herrmann's HSPEC96 Greens functions [default off].\n" );
	fprintf( stderr, "\t\t        Creates directory hspec96_GFs/ and csh script runall_hspec96_to_glib.csh \n" );
	fprintf( stderr, "\t\t        HSPEC96 scripts are created in subdirectories hspec96_GFs/net.sta.loc/net.sta.loc.model.hspec.csh\n" );
	
	fprintf( stderr, "\n\n" );
	fprintf( stderr, "\t Epilog: This program auto creates a makeglib.csh file that computes Green functions.\n" );
	fprintf( stderr, "\t Add SAC files \"../Data/*.BHZ.?.SAC\" to tell setupMT which stations to use\n" );
	fprintf( stderr, "\t in the moment tensor inversion\n" );
	fprintf( stderr, "\n" );

	fprintf( stderr, "EXAMPLES:\n" );
fprintf( stderr, "setupMT -z  2 -ev 35.8767 -84.898 -mod cus -ot 2021-08-13T11:57:35 -com \"Franklin Mine,TN\" -fil 0.075 0.120 ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC\n" );
fprintf( stderr, "setupMT -z 16 -ev 36.9077 -90.543 -mod cus -ot 2021-11-18T02:53:03 -com \"New Madrid, MO\" -fil 0.075 0.150 ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC\n" );
fprintf( stderr, "setupMT -z  2 -ev 41.7392 +73.377 -mod wus -ot 2009-12-22T05:54:35 -com \"Kambrata, Kyrgyzs\" -fil 0.033 0.100 ../Data/*.??Z.?.SAC ../Data/*.??Z.SAC\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "\n" );
	exit(0);
}

void load_exclude_station_list( char *exclude_station_file, char exclude_station_list[256][16], int *nsta_exclude_list )
{
	FILE *fp;
	int nsta = 0;
	char rec[256];

	if( (fp = fopen( exclude_station_file, "r" )) == NULL )
	{
		fprintf( stderr, "%s: %s: %s: Warning can't read file exclude_station_file = %s\n", 
			progname, __FILE__, __func__, exclude_station_file );
		*nsta_exclude_list = 0;
		return;
	}
	while( fgets( rec, 256, fp ) != NULL )
	{
		if( rec[0] == '#' ) continue;
		rec[strcspn(rec, "\n")] = 0;
		strcpy( exclude_station_list[nsta], rec );
		nsta++;
	}
	fclose(fp);
	*nsta_exclude_list = nsta;
}

Chanlist *remove_duplicate_chans( int nsta, Sac_Header *s, float *arrin, int *indx, int *ncl )
{
	Chanlist *cl;
	int i = 0, ix = 0, j = 0, k = 0, n = 0;
	char command_line[256];
	int Duplicate = 0;
	int check_duplicate( Chanlist *cl, int n, char *net, char *sta, char *loc );

/*** first, intialize and populate the list ***/

	cl = calloc( nsta+1, sizeof(Chanlist) );
	k = 0;
	for( i = 0; i < nsta; i++ )
	{
		for( j = i; j < nsta; j++ )
		{
			if( ( strcmp( s[i].kstnm,  s[j].kstnm  ) == 0 ) &&
                            ( strcmp( s[i].knetwk, s[j].knetwk ) == 0 ) &&
                            ( strcmp( s[i].khole,  s[j].khole  ) == 0 )  )
			{
				n = k;
				Duplicate = check_duplicate( cl, n, s[j].knetwk, s[j].kstnm, s[j].khole );

				/*** initalize chanels on first pass ***/
				if( strcmp( s[i].kcmpnm,  s[j].kcmpnm ) == 0 && !Duplicate )
				{
					strcpy( cl[k].net, s[i].knetwk );
					strcpy( cl[k].sta, s[i].kstnm );
					strcpy( cl[k].loc, s[i].khole);

					cl[k].nseg = 0;
					cl[k].nchan=1; 
					strcpy( cl[k].ref_chan, s[i].kcmpnm );
					strcpy( cl[k].chans, s[i].kcmpnm );

					cl[k].dist = s[i].dist;
					cl[k].az   = s[i].az;
					snprintf( cl[k].nslc_string, 32, "%s.%s.%s", cl[k].net, cl[k].sta, cl[k].loc );

					k++;
					break;
				}
			}
		}
	}
	cl = realloc( cl, (k+1)*sizeof(Chanlist) );
	*ncl = k;

/*** search the list for duplicates ***/

	for( k = 0; k < *ncl ; k++ )
	{
		for( j = 0; j < nsta; j++ )
		{
			if( ( strcmp( cl[k].sta, s[j].kstnm  ) == 0 ) &&
			    ( strcmp( cl[k].net, s[j].knetwk ) == 0 ) && 
			    ( strcmp( cl[k].loc, s[j].khole  ) == 0 ) && 
			    ( strcmp( cl[k].ref_chan, s[j].kcmpnm ) != 0 ) )
			{
				strcat( cl[k].chans, " " );
				strcat( cl[k].chans, s[j].kcmpnm );
				++cl[k].nchan;
			}

			if( ( strcmp( cl[k].sta, s[j].kstnm  ) == 0 ) &&
			    ( strcmp( cl[k].net, s[j].knetwk ) == 0 ) &&
                            ( strcmp( cl[k].loc, s[j].khole  ) == 0 ) ) cl[k].nseg++;
		}

		if( cl[k].nseg > cl[k].nchan ) /**possibly multiple file segments **/
		{
			fprintf( stderr, "%s: %s: %s: %s nseg=%d .GT. nchan=%d - ", progname, __FILE__, __func__, cl[k].nslc_string, cl[k].nseg, cl[k].nchan );
			fprintf( stderr, "multiple files typically indicates segmentation, need to check to ensure signal is in right segement or join into single file\n" ); 
			sprintf( command_line, "/bin/ls -l ../Data/*.%s.*SAC", cl[k].nslc_string );
			system( command_line );
		}
	}

	return(Chanlist *)cl;
}

/**check if already in list***/
int check_duplicate( Chanlist *cl, int n, char *net, char *sta, char *loc )
{
	int i;
	int Duplicate = 1;
	int NotDuplicate = 0;
	for( i = 0; i < n; i++ )
	{
		if( ( strcmp( cl[i].net, net ) == 0 ) &&
		    ( strcmp( cl[i].sta, sta ) == 0 ) &&
		    ( strcmp( cl[i].loc, loc ) == 0 ) )
		{
			return Duplicate;
		}
	}
	return NotDuplicate;
}

void create_cleanup_script()
{
	FILE *fp;
	fp = fopen("cleanup.csh","w");

	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "###\n" );
	fprintf( fp, "###Autogenerated Clean-up script written by setupMT.c\n" );
	fprintf( fp, "###\n" );

	fprintf( fp, "\n" );
	fprintf( fp, "set argc=$#argv\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "if( $argc != 1 ) then\n" );
 	fprintf( fp, "  echo requires 1 argument got $argc \n" );
	fprintf( fp, "  echo options: l or lite - deletes all files except plots and results \n" );
	fprintf( fp, "  echo          a or all  - deletes all files except setupMT.csh \n" );
	fprintf( fp, "  exit(-1)\n" );
	fprintf( fp, "else\n" );
	fprintf( fp, "  set cmdlinearg=${1}\n" );
	fprintf( fp, "  if( $cmdlinearg == \"lite\" || $cmdlinearg == \"l\" ) then\n" );
	fprintf( fp, "    set clean_level=1\n" );
	fprintf( fp, "  else if( $cmdlinearg == \"all\" || $cmdlinearg == \"a\" ) then\n" );
	fprintf( fp, "    set clean_level=2\n" );
	fprintf( fp, "  else\n" );
	fprintf( fp, "    echo unknown option\n" );
	fprintf( fp, "    exit(-1)\n" );
	fprintf( fp, "  endif\n" );
	fprintf( fp, "endif\n" );

	fprintf( fp, "\n" );

	fprintf( fp, "if( ${clean_level} == 1 ) then\n" );
	fprintf( fp, "  /bin/rm -f *.glib *.ginv *.data \n" );
	fprintf( fp, "  /bin/rm -f *.dat.xy *.syn.xy gmtwf.*.pdf gmtwf.*.ps \n" );
	fprintf( fp, "  /bin/rm -f mtinv.out multithread_mkgrnlib.out \n" );
	fprintf( fp, "  /bin/rm -f create.sql \n" );
	fprintf( fp, "  /bin/rm -f plot_T*ps \n" );
	fprintf( fp, "  /bin/rm -f gmtmap.csh gmtmap.ps \n" );
	fprintf( fp, "  /bin/rm -f plotmap.csh \n" );
	fprintf( fp, "  /bin/rm -f plotz.csh plotmech.csh results.?.??? \n" );
	fprintf( fp, "  /bin/rm -f gmt.conf gmt.history \n" );
	fprintf( fp, "endif\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "if( ${clean_level} == 2 ) then\n" );
	fprintf( fp, "  /bin/rm -f *.glib *.ginv *.data \n" );
        fprintf( fp, "  /bin/rm -f *.dat.xy *.syn.xy\n" );
	fprintf( fp, "  /bin/rm -f gmtwf.csh gmtwf.*.pdf gmtwf.*.ps gmtwf.*.jpg \n" );
        fprintf( fp, "  /bin/rm -f mtinv.out multithread_mkgrnlib.out \n" );
	fprintf( fp, "  /bin/rm -f db.txt automt.txt index.html snr.out var_red.out\n" );
	fprintf( fp, "  /bin/rm -f *.par \n" );
	fprintf( fp, "  /bin/rm -f insert.sql \n" );
        fprintf( fp, "  /bin/rm -f create.sql \n" );
        fprintf( fp, "  /bin/rm -f plot_T*.ps \n" );
	fprintf( fp, "  /bin/rm -f email_T*.txt \n" );
	fprintf( fp, "  /bin/rm -f plot_T*.jpg \n" );
        fprintf( fp, "  /bin/rm -f gmtmap.csh gmtmap.ps gmtmap.jpg\n" );
        fprintf( fp, "  /bin/rm -f plotmap.csh sac2gmtmap.jpg\n" );
        fprintf( fp, "  /bin/rm -f plotmech.??? plotz.csh plotz.jpg results.?.??? \n" );
	fprintf( fp, "  /bin/rm -f exclude.sta exclude_sta.txt \n" );
	fprintf( fp, "  /bin/rm -f llnl_db.load.txt\n" );
        fprintf( fp, "  /bin/rm -f gmt.conf gmt.history \n" );

	fprintf( fp, "  /bin/rm -f makeglib.csh \n" );
	fprintf( fp, "  /bin/rm -f run.csh \n" );
	fprintf( fp, "  /bin/rm -f run2.csh \n" );
	fprintf( fp, "endif\n" );
	fclose(fp);

	chmod( "cleanup.csh", S_IRWXU|S_IRWXG|S_IRWXO );
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

int get_cmdline_integer( int i, char *arg, char **av, char *argname )
{
        int result;
/* fprintf( stdout, "%s: %s: %s: i=%d av=%s\n", progname, __FILE__, __func__, i, av[i] );
        fflush(stdout); */

        if( av[i] != NULL )
        {
          result = atoi( av[i] );
        }
        else
        {
          fprintf( stderr, "%s: %s: %s: ERROR need %s %s\n",
                progname, __FILE__, __func__, arg, argname );
          exit(-1);
        }
        return result;
}

float get_cmdline_float( int i, char *arg, char **av, char *argname )
{
	float result;
/*fprintf( stdout, "%s: %s: %s: i=%d av=%s\n", progname, __FILE__, __func__, i, av[i] );
	fflush(stdout); */
	
	if( av[i] != NULL )
	{
	  result = atof( av[i] );
	}
	else
	{
	  fprintf( stderr, "%s: %s: %s: ERROR need %s %s\n", 
		progname, __FILE__, __func__, arg, argname );
	  exit(-1);
	}
	return result;
}

long get_cmdline_long( int i, char *arg, char **av, char *argname )
{               
        float result;
/*fprintf( stdout, "%s: %s: %s: i=%d av=%s\n", progname, __FILE__, __func__, i, av[i] );
        fflush(stdout); */
                                
        if( av[i] != NULL )
        {               
          result = atol( av[i] );
        }       
        else    
        {               
          fprintf( stderr, "%s: %s: %s: ERROR need %s %s\n",
                progname, __FILE__, __func__, arg, argname );
          exit(-1);
        }
        return result;
}

void create_hspec96_script( 
	float depth, 
	int nsta, 
	Sac_Header *s, 
	char *model_name, 
	char *model_file_path, 
	char exclude_station_list[256][16], 
	int nsta_exclude_list, 
	float evla, 
	float evlo,
	int verbose )
{
	FILE *fp;
	int i = 0, j = 0;
	float dt = 0.35;
	char loc[18];
	float t0, rdev;
	int npts;
	float rdistkm;
	float get_waveform_sampling_rate_by_distancekm( float rdistkm );

	int mkdirp2( const char *, mode_t mode );
	char output_directory[256];
	char station_directory[256];
	char script_filename[256];
	char cmdline[512];
	char sacfilename[256];
	int icmp = 0, ista = 0;
	char kcmp[][10] = { "   ", "ZDD", "RDD", "ZDS", "RDS", "TDS", "ZSS", "RSS", "TSS", "ZEX", "REX" };

/*** sort station-channels by distance ***/
	Chanlist *cl;
	int ncl;
	Chanlist *remove_duplicate_chans( int n, Sac_Header *s, float *arrin, int *indx, int *ncl );
	void sort_Chanlist_by_dist( int ncl, Chanlist *cl, float *dist, int *indx );
	int *indx;
	float *dist;

/*** velocity model ***/
/* see include/mt.h Mo = math.pow( 10.0, 1.5*(Mw+10.73) ) = 1.2445146117713818e+16; Reference Mw = 0.0 */
/*** hspec96 reference moment is 1e+20 dyne*cm ***/
	void create_mod( VelMod * );
	void print_mod0( VelMod * );
	void print_mod1( FILE *, VelMod * );
	VelMod v;
	int ilay, nlay;
	float etap = 0, etas = 0, frefp = 1, frefs = 1;
	
/*** depth ***/
	float *z, ztmp;
	float zinc, zstart, zend;
	int iz, nz; 

/**** create conversion script ***/

	void hspec96_to_grnlib_script( Chanlist *cl, int *indx, int ncl, 
		float evla, float evlo, float zstart, float zinc, float zend, 
		char *modeldb, char *velmod, char *filename );

/************************/
/*** start subroutine ***/
/************************/

/*** create the depth vector ***/
	zstart = 1;
	zend = 25;
	zinc = 1;

	nz = (int) rint( (( zend - zstart + zinc ) / zinc ) );
	fprintf( stdout, "%s: %s: %s: nz=%d\n", progname, __FILE__, __func__, nz );
	z = calloc( nz+1, sizeof(float) );

	for( iz = 0; iz < nz; iz++ )
	{
		z[iz] = zstart + (float)iz*zinc;
		if(verbose)
		  fprintf( stdout, "%s: %s: %s: iz=%d z[iz]=%g\n", 
			progname, __FILE__, __func__, iz, z[iz] );
	}

/*** make directory for GFs , cd directory ***/

	strcpy( output_directory, "hspec96_GFs" );
	mkdirp2( output_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	chdir( output_directory );

/*****************************************************************************/
/*** channel list data structure, removes duplicate channels and stations  ***/
/*** due to multiple SAC file segments of the same NSLC                    ***/
/*****************************************************************************/

	cl = calloc( nsta+1, sizeof(Chanlist) );
	cl = remove_duplicate_chans( nsta, s, dist, indx, &ncl );
	
	dist = calloc( ncl+1, sizeof(float) );
	indx = calloc( ncl+1, sizeof(int) );
	sort_Chanlist_by_dist( ncl, cl, dist, indx );

	if(verbose)
	{
	  for( j = 0; j < ncl; j++ )
	  {
		i = indx[j+1]-1;
		fprintf( stdout, "%s: %s: %s: j=%d i=%d NSL=(%s) dist=%g az=%g nchan=%d chans=%s\n",         
			progname,
                        __FILE__,
			__func__,
                        j,
			i, 
                        cl[i].nslc_string,
                        cl[i].dist,
                        cl[i].az,
                        cl[i].nchan,
                        cl[i].chans );
		fflush(stdout);
	  }
	}

/*** make c-shell script to convert back to glibs ***/

        hspec96_to_grnlib_script( cl, indx, ncl, evla, evlo, zstart, zinc, zend, model_file_path, model_name, "runall_hspec96_to_glib.csh" );
	chmod( "runall_hspec96_to_glib.csh", S_IRWXU|S_IRWXG|S_IRWXO );
	
/**********************************************/
/**********************************************/
/*** create a script file for each station ***/
/**********************************************/
/**********************************************/

	for( j = 0; j < ncl; j++ )
	{
		/*** set the pointer to the sorted index ***/
		i = indx[j+1]-1;

		/*** goto the station directory ***/
		strcpy( station_directory, cl[i].nslc_string );
		mkdirp2( station_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
		chdir( station_directory );

		/*** calculate sampling rate dt using same function as mkgrnlib.c ***/
		dt = get_waveform_sampling_rate_by_distancekm( cl[i].dist );

		npts = 2048;
		rdev = 18;
		if( rdev > 0 )
			t0 = -1;
		else
			t0 = 0;
		
		/*** name for C-shell script ***/
		sprintf( script_filename, "%s.%s.%s.%s.hspec.csh", 
			cl[i].net, cl[i].sta, cl[i].loc, model_name );

		if(verbose)
		{
		  fprintf( stdout, "%s: %s: %s: writting script %s\n",
			progname, __FILE__, __func__, script_filename );
		  fflush(stdout);
		}

		if( (fp=fopen( script_filename,"w" )) == NULL )
		{
			fprintf( stderr, "%s: %s: %s: cannot open file %s for writting\n",
				progname, __FILE__, __func__, script_filename );
			exit(-1);
		}

		fprintf( fp, "#!/bin/csh\n" );
        	fprintf( fp, "########################################################\n" );
        	fprintf( fp, "### Autogenerated C-shell script by setupMT.c        ###\n" );
		fprintf( fp, "########################################################\n" );
		fprintf( fp, "\n" );

	/*** station distance dt npts file ***/

		fprintf( fp, "### \n" );
		fprintf( fp, "### format: DIST DT NPTS T0 VRED  ## Net.Sta.Loc \n" );
		fprintf( fp, "### \n" );
		fprintf( fp, "cat >! distfile << EOF\n" );
		fprintf( fp, "%g %6.2f %4d %4.2f %4.2f ## net=%s sta=%s loc=(%s) R=%g Az=%g %-12.12s nchan=%d nseg=%d (%s)\n",
			cl[i].dist, 
			dt,
			npts,
			t0,
			rdev,
			cl[i].net, cl[i].sta, cl[i].loc, 
			cl[i].dist, cl[i].az,
			cl[i].nslc_string,
                        cl[i].nchan,
                        cl[i].nseg,
                        cl[i].chans );
		fflush( fp );
		fprintf( fp, "EOF\n");
	
	/*** depth file ***/
		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );
        	fprintf( fp, "### depth file\n" );
        	fprintf( fp, "###\n" );

		fprintf( fp, "cat >! depthfile << EOF\n" );
		for ( iz = 0; iz < nz; iz++ )
			fprintf( fp, "%g\n", z[iz] );
		fprintf( fp, "EOF\n" );
        	fflush( fp );

	/*** velocity model ***/
		strcpy( v.modpath, model_file_path );
		strcpy( v.modfile, model_name );
		create_mod( &v );

		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "### Create Velocity Model file\n" );
		fprintf( fp, "###\n" );
		fflush( fp );
	
		fprintf( fp, "cat >! %s.model << EOF\n", model_name );
		fprintf( fp, "MODEL.01\n" );
		fprintf( fp, "TEST MODEL\n" );
		fprintf( fp, "ISOTROPIC\n" );
		fprintf( fp, "KGS\n" );
		fprintf( fp, "FLAT EARTH\n" );
		fprintf( fp, "1-D\n" );
		fprintf( fp, "CONSTANT VELOCITY\n" );
		fprintf( fp, "LINE08\n" );
		fprintf( fp, "LINE09\n" );
		fprintf( fp, "LINE10\n" );
		fprintf( fp, "LINE11\n" );
		fprintf( fp, "H       VP     VS   RHO  QP     QS    ETAP ETAS  FREFP FREFS\n" );
		for( ilay = 0; ilay < v.nlay; ilay++ )
		{
			if( ( ilay == v.nlay-1 ) && ( v.thick[ilay] != 0 ) ) v.thick[ilay] = 0; /***layer over halfspace***/
		
			fprintf( fp, "%8.2f %8.2f %8.2f %8.2f %9.1f %9.1f %5.1f %5.1f %5.1f %5.1f\n", 
				v.thick[ilay], 
				v.vp[ilay], 
				v.vs[ilay], 
				v.rho[ilay], 
				v.qa[ilay], 
				v.qb[ilay],
				etap, 
				etas,
				frefp, 
				frefs );
		}
		fprintf( fp, "EOF\n" );
		fflush( fp );
	
		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "### generate hspec96.dat read by hspec96, options are -ALL -EQEX -EXF \n" );
		fprintf( fp, "###\n" ); 

		/*** this runs just one depth ***/
		fprintf( fp, "# hprep96 -M %s.model -d distfile -HS %g -HR 0 -EQEX -NDEC 1\n", model_name, depth );
			
		/*** this runs depth file ***/
		fprintf( fp, "# hprep96 -M %s.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1\n", model_name );

		/*** depth file plus speed up with -XF ***/
		fprintf( fp, "hprep96 -M %s.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1 -XF 1\n", model_name );

		fflush( fp );
	
		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );	
		fprintf( fp, "### run hspec96 and send output to hspec96.out\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "hspec96 > hspec96.out\n" );
		fflush( fp );

		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "### read hspec96 output greens functions hspec96.grn and write sac binary files\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "hpulse96 -i -D | f96tosac -B\n" );
		fflush( fp );

		fprintf( fp, "\n" );
		fprintf( fp, "###\n" );
		fprintf( fp, "### Rename the output file (e.g., copy B0101ZDD.sac -> NM.CGM3.00.cus.ZDD.SAC)\n" );
		fprintf( fp, "###\n" );
		fflush( fp );
	
	/*** rename the output ***/
	/***   1   2   3   4   5   6   7   8   9  10 icmp ***/
	/*** ZDD RDD ZDS RDS TDS ZSS RSS TSS ZEX REX kcmp ***/
	/***  B%02d%02d%3s.sac : ista icmp kcmp ***/
	/***  B0101ZDD.sac B0102RDD.sac B0103ZDS.sac B0104RDS.sac B0105TDS.sac B0106ZSS.sac B0107RSS.sac B0108TSS.sac B0109ZEX.sac B0110REX.sac ***/
	
		for ( iz = 0; iz < nz; iz++ )
		{
		
			for( icmp = 1; icmp <= 10; icmp++ )
			{
				/*******************************************************************************************/
				/*** CPS3.30/VOLV/src/f96tosac.f RBH changed the filename format for output GFs          ***/ 
				/***                                         expanding ntrace field width i2->i3         ***/
				/*** Version 2002 format(a1,i2,i2,a3,'.sac')                                             ***/
				/*** Version 2021 format(a1,i3,i2,a3,'.sac') write(ofile,10)'B',ntrace,jtr,stcomp(1:3)   ***/
				/*******************************************************************************************/

				/*** sprintf( sacfilename, "B%02d%02d%3s.sac", iz+1, icmp, kcmp[icmp] ); ***/
				sprintf( sacfilename, "B%03d%02d%3s.sac", iz+1, icmp, kcmp[icmp] );

				sprintf( cmdline, "/bin/cp %s %s.%s.%s.%s.%g.%s.SAC", 
					sacfilename,
					cl[i].net, 
					cl[i].sta, 
					cl[i].loc,
					model_name,
					z[iz], 
					kcmp[icmp] );
				if(verbose)
				{
		  			fprintf(stdout, "%s: %s: %s: cmdline=%s\n",
						progname, __FILE__, __func__, cmdline );
		  			fflush(stdout);
				}
	
				/* system( cmdline ); */
				fprintf( fp, "%s\n", cmdline );
				fflush(fp);
			}
		}
		fflush(fp);
		fclose(fp);
	
		/*** execute the script ***/
		chmod( script_filename, S_IRWXU|S_IRWXG|S_IRWXO );
	
		sprintf( cmdline, "csh ./%s", script_filename );
	
		fprintf(stdout, "%s: %s: %s: cmdline = %s\n",
				progname, __FILE__, __func__, cmdline );
		fflush(stdout);
	
		/*** execute the csh script ***/
		/* system( cmdline ); */

		chdir( "../" ); /*** go back to parent directory (station base directory) ***/

	} /*** loop over and process each station ***/

	chdir( "../" ); /*** go back to parent directory (root directory for hspec96_GFs) ***/

	free(z);
}


float get_waveform_sampling_rate_by_distancekm( float rdistkm )
{
	float dt = 0.35;
	if( rdistkm < 2000 ) dt = 0.30;
	if( rdistkm < 1000 ) dt = 0.25;
	if( rdistkm < 800 )  dt = 0.20;
	if( rdistkm < 450 )  dt = 0.18;
	if( rdistkm < 400 )  dt = 0.16;
	if( rdistkm < 340 )  dt = 0.15;
	if( rdistkm < 320 )  dt = 0.14;
	if( rdistkm < 300 )  dt = 0.13;
	if( rdistkm < 280 )  dt = 0.12;
	if( rdistkm < 260 )  dt = 0.11;
	if( rdistkm < 240 )  dt = 0.10;
	if( rdistkm < 220 )  dt = 0.09;
	if( rdistkm < 200 )  dt = 0.08;
	if( rdistkm < 180 )  dt = 0.07;
	if( rdistkm < 160 )  dt = 0.07;
	if( rdistkm < 140 )  dt = 0.06;
	if( rdistkm < 120 )  dt = 0.05;
	if( rdistkm < 50 )   dt = 0.04;
	return dt;
}

void hspec96_to_grnlib_script( 
	Chanlist *cl, int *indx, int ncl,
	float evla, float evlo, 
	float zstart, float zinc, float zend, 
	char *model_file_path, char *model_name, 
	char *filename )
{
	FILE *fp;
	fp = fopen( filename, "w" );
	int i, j;

/**** start ***/
	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "\n" );

/*** do the hspec96 fk computations here ***/
	
	fprintf( fp, "###\n");
        fprintf( fp, "### do the hspec96 fk computations here\n" );
        fprintf( fp, "###\n");

	fprintf( fp, "foreach nsl ( " );
	for( j = 0; j < ncl; j++ )
	{
		/*** set the pointer to the sorted index ***/
		i = indx[j+1]-1;
		fprintf( fp, "%s ", cl[i].nslc_string );
	}
	fprintf( fp, ")\n" );

	fprintf( fp, "echo ${nsl}\n" );
	fprintf( fp, "cd ${nsl}\n" );
	fprintf( fp, "pwd\n" );
	fprintf( fp, "csh ${nsl}.%s.hspec.csh\n", model_name );
	fprintf( fp, "cd ..\n" );
	fprintf( fp, "end\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "\n" );
	
/*** do the loading here ***/
	fprintf( fp, "###\n");
	fprintf( fp, "### create glib files from hspec96 output\n");
	fprintf( fp, "###\n");

	fprintf( fp, "cat >! hspec96_to_grnlib.par << EOF\n" );
	fprintf( fp, "velmod=%s\n", model_name );
	fprintf( fp, "modeldb=%s\n", model_file_path );
	fprintf( fp, "stadb=../../Data/rdseed.stations\n" );
	fprintf( fp, "zrange=%g,%g,%g\n", zstart, zinc, zend );
	fprintf( fp, "evla=%g\n", evla );
	fprintf( fp, "evlo=%g\n", evlo );
	fprintf( fp, "EOF\n" );
	fprintf( fp ,"\n" );
	
	for( j = 0; j < ncl; j++ )
	{
		/*** set the pointer to the sorted index ***/
		i = indx[j+1]-1;

		fprintf( fp, "hspec96_to_grnlib par=hspec96_to_grnlib.par net=%s sta=%s loc=\"%s\" ./%s/*.SAC\n",
			cl[i].net,
			cl[i].sta,
			cl[i].loc,
			cl[i].nslc_string );
	}
	fclose(fp);
}

int list_directory_files_with_extension( char *directory_name, char *filename_extension_with_leading_dot )
{
        int i, nfiles;
        struct dirent **namelist;
        const char *ext;
        char filename[1024];

        nfiles = scandir( directory_name, &namelist, NULL, alphasort );

        fprintf( stdout, "%s: %s: %s: directory = %s\n", 
		progname, __FILE__, __func__, directory_name  );

        for ( i = 0; i < nfiles; i++ )
        {
                strcpy( filename, namelist[i]->d_name );
                if( namelist[i]->d_type == DT_REG )
                {
                        ext = strrchr( filename, '.' );
                        if( strcmp( ext, filename_extension_with_leading_dot ) == 0 )
                                printf( "%s\n", filename );
                }
        }
        return 0;
}


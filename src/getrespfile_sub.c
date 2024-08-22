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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/mt.h"

extern char progname[128];

/*** various subroutines and macros for reading in instrument response SAC_PZs_* files ***/
/*** my own array of file names for myscandir() and getsacfiles()  ***/

typedef struct { char fn[256]; } FileNameList;

/*************************************************************************************/
/*** this subroutine has been retooled to use readdir() instead of scandir()       ***/
/*** differences between standard libc and BSD Unix is too great between platforms ***/
/*************************************************************************************/

void getrespfile( char *pathname, char *sta, char *net, char *loc, char *cmp, 
                  char *khole, int verbose, char respfile[256] )
{
	int i, count=0;
	FileNameList *filen;
	FileNameList *myscandirRESP( char *, int *, FileNameList * );
	char tmp[1024], *ptr, tok[2] = { '_', '\0' } ;
	int my_file_name_cmp( char *, char * );
	void fatal_error( char *, char * );

	  fprintf(stdout, "%s: %s: %s: respdir=%s sta=%s net=%s cmp=%s loc=(%s) khole=(%s)\n",
		progname, __FILE__, __func__, pathname, sta, net, cmp, loc, khole );
	
/*************************************************************************************/
/*** allocate space for filename list                                              ***/
/*************************************************************************************/
	filen = (FileNameList *)malloc( sizeof(FileNameList) );

/*************************************************************************************/
/*** get only sac file name list from remote directory                             ***/
/*************************************************************************************/
	filen = (FileNameList *)myscandirRESP( pathname, &count, filen );

/*************************************************************************************/
/*** sort the file names                                                           ***/
/*************************************************************************************/
	/* qsort( filen, count, sizeof(FileNameList), my_file_name_cmp ); */

	if( verbose )
	  printf("%s: number of files = %d in directory %s\n", 
		progname, count, pathname );
	
	if( count <= 0 )
	  fatal_error( "No SAC_PZs Response files in this directory", pathname );

	for( i=0; i<count; i++ ) 
	{
		if(verbose)
		  printf("i=%d filen=%s\n", i, filen[i].fn );
		
		strcpy( tmp, filen[i].fn );

		ptr = strtok( tmp, tok ); /*** SAC ***/
		ptr = strtok( NULL, tok ); /*** PZs ***/

		/*** network ***/
		while( ( ptr = strtok( NULL, tok )) != NULL )
		{
			if( strcmp( ptr, net ) == 0 )
			{
				if( verbose ) printf("net=%s\n", ptr );

				ptr = strtok( NULL, tok ); /*** station ***/
				if( strcmp( ptr, sta ) == 0 )
				{
					if(verbose)printf("sta=%s\n", ptr );

					/*** component ***/
					ptr = strtok( NULL, tok );

					if( strcmp( ptr, cmp ) == 0 )
					{
						if(verbose)
						{
						  fprintf( stdout, 
					     "%s: %s: %s: i=%d ptr=%s cmp=%s filen=%s (next) khole=%s\n",
						  	progname, __FILE__, __func__,
							i, ptr, cmp, filen[i].fn, khole );
						  fflush(stdout);				
						}

						/*** location code - not khole ***/
						ptr = strtok( NULL, tok );

						/*** debugging ***/
						if(verbose) 
						{
							fprintf( stdout, "%s: %s: %s: ptr=%s\n", 
								progname, __FILE__, __func__, ptr);
							fflush(stdout);
						}

						/*** added new pre-check incase where there is not start or end dates to SAC_PZs files **/
						/*** this returns a null pointer and is checked seperately with my loc vars ***/
						/*** g.ichinose Aug 21, 2024 ***/
						if( ptr == NULL && strcmp( loc, "" ) == 0 ) 
						{
							if(verbose)
							{
							  fprintf( stdout, "%s: %s: %s: ptr=%s is null loc=(%s)\n", 
								progname, __FILE__, __func__, ptr, loc );
							  fflush(stdout);
							}

							sprintf( respfile, "%s/%s", pathname, filen[i].fn );

							if(verbose)
							{
							  fprintf( stdout, "%s: %s: %s: Found respfile=%s\n", progname, __FILE__, __func__, respfile );
							  fflush(stdout);
							}

							return;
						}

						/*** left alone, cleaned up alittle ***/
						if( strcmp( ptr, loc ) == 0 || strcmp( loc, "" ) == 0 )
						{
							sprintf( respfile, "%s/%s", pathname, filen[i].fn );

							if( verbose )
							{
							  fprintf( stdout, "%s: %s: %s: Found respfile=%s\n",
								progname, __FILE__, __func__, respfile );
							  fflush(stdout);
							}
							return;
						}

						/*********************************************/
						/*** else check for non null khole values ****/
						/*********************************************/
					
						/*** this was not the right khole, skip to
							next file ***/

						if( (ptr = strtok( NULL, tok )) == NULL )
						{
							break;
						}

						/*** this is the right khole, file found ***/

						if( strcmp( ptr, khole ) == 0 )
						{
						  sprintf( respfile, "%s/%s", 
							pathname, filen[i].fn );

						  if( verbose )
						  {
						    printf("%s: Found respfile=%s\n",
							progname, respfile );
						  }
						  return;
						}
						break;

					} /*** if cmp ***/
	      				break;

				} /*** if sta ***/
				break;

			} /*** if net ***/
			break;

		} /** while loop over words ***/

	} /*** for loop over files ***/

/*** looped over all SAC_PZs_* files and did not return a vaild 
	SAC Pole Zero response file for sta,net,cmp,khole set ***/

	fprintf(stderr, "%s: getrespfile.c: ERROR Respfile not found SNCL=(%s.%s.%s.%s)\n", 
		progname, sta,net,cmp,khole );
	fprintf(stdout, "%s: getrespfile.c: ERROR Respfile not found SNCL=(%s.%s.%s.%s)\n", 
		progname, sta,net,cmp,khole );

	exit(-1);
}

FileNameList *myscandirRESP( char *pathname, int *count, FileNameList *filen )
{
	struct stat f;
	DIR *dip;
	struct dirent *dit;
	int i=0;
	char *eptr;

	if( ( dip = opendir(pathname) ) == NULL )
	{
		perror("opendir");
		fprintf(stderr, "\n\n Directory %s does not exit\n", pathname );
		exit(2);
	}

	while( (dit = readdir(dip)) != NULL )
	{
		if( (strcmp( dit->d_name, "." ) == 0) ||
			(strcmp(dit->d_name, ".." ) == 0 ) ) continue;

		stat( dit->d_name, &f );

	/*** does not work on suns/solaries and linux ***/
		/*** if( S_ISDIR( f.st_mode ) ) continue; ***/

		eptr = index( dit->d_name, '_' );

		if( eptr == NULL ) continue;

		if( strncmp(eptr, "_PZs_", 5) == 0 )
		{
			strcpy( filen[i].fn, dit->d_name );
			i++;
			filen = (FileNameList *) 
			  realloc( filen, (i+1)*sizeof(FileNameList) );
		}
	}
	if( closedir(dip) == -1 )
	{
		perror("closedir");
		exit(3);
	}
	*count = i;
	return filen;
}

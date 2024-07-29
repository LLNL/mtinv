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

/*** my own array of file names for myscandir() and getsacfiles()  ***/

typedef struct { char fn[256]; } FileNameList;

/*************************************************************************************/
/*** this subroutine has been retooled to use readdir() instead of scandir()       ***/
/*** differences between standard libc and BSD Unix is too great between platforms ***/
/*************************************************************************************/

int my_file_name_cmp( const void *a, const void *b )
{
	FileNameList *c1 = (FileNameList *)a;
	FileNameList *c2 = (FileNameList *)b;
	return strcmp( c1->fn, c2->fn );
}

void getsacfiles( char *stnm, char *net, char *loc, char *pathname, char sacfiles[256][2048], int *nfiles, int verbose )
{
	int i, count=0, ifile=0;
	FileNameList *filen;
	FileNameList *myscandir( char *, int *, FileNameList * );
	extern char progname[128];

	int iword;
	/** dont need, defined in stdlib.h **/
/***
     void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
***/
	int my_file_name_cmp( const void *, const void * );

	char tmp[1024], *ptr, tok[2] = { '.', '\0' };
	void fatal_error( char *, char * );

	if( verbose )
	{
	  fprintf( stdout, "%s: %s: %s: sta=%s net=%s loc=(%s) calling directory %s\n", 
		progname, __FILE__, __func__, stnm, net, loc, pathname );
	}

/*** allocate space for filename list ***/
	filen = (FileNameList *)malloc( sizeof(FileNameList) );

/*** get only sac file name list from remote directory ***/
	filen = (FileNameList *)myscandir( pathname, &count, filen );

/*** sort the file names ***/
	qsort( filen, count, sizeof(FileNameList), my_file_name_cmp );

	if( verbose )
	  fprintf( stdout, "%s: getsacfiles(): number of files = %d\n", progname, count );

	if( count <= 0 )
		fatal_error( "No files in this directory", pathname );

/*** find the sac files from the list that have the station name and network in them ***/
	for( i=0; i < count; i++ )
	{
		strcpy( tmp, filen[i].fn );
		
		ptr = strtok( tmp, tok );
		while( (ptr = strtok( NULL, tok )) != NULL )
		{
			if( strcmp( ptr, net ) == 0 )
			{
				ptr = strtok( NULL, tok );
				if( strcmp( ptr, stnm ) == 0 )
				{
					ptr = strtok( NULL, tok );
					if( strcmp( ptr, loc ) == 0 || strcmp( loc, "" ) == 0 )
					{
						ptr = strtok( NULL, tok );
						strcpy( sacfiles[ifile], filen[i].fn );
					
						fprintf( stdout,
				    			"%s: %s: %s: Found station=%s net=%s loc=(%s) ifile=%d file=%s\n",
							progname, __FILE__, __func__, stnm, net, loc, ifile, sacfiles[ifile] );

						ifile++;
					}
				}
			}
		}
	}
	*nfiles = ifile;
	free(filen);
}

FileNameList *myscandir( char *pathname, int *count, FileNameList *filen )
{
	struct stat f;
	DIR *dip;
	struct dirent *dit;
	int i=0;
	char *eptr;
	if( ( dip = opendir(pathname) ) == NULL )
	{
		perror("opendir");
		fprintf(stderr, "\n\nDirectory %s does not exist\n", pathname );
		exit(2);
	}

	while( (dit = readdir(dip)) != NULL )
	{
		if( (strcmp( dit->d_name, "." ) == 0) ||
			(strcmp(dit->d_name, ".." ) == 0 ) ) continue;
		stat( dit->d_name, &f );

		/** does not work on suns or linux **/
		/** gichinose Apr 2007 ***/
		/** if( S_ISDIR( f.st_mode ) ) continue; **/

		eptr = rindex( dit->d_name, '.' );
		if( eptr == NULL ) continue;
		if( strncmp(eptr, ".SAC", 4) == 0 || strncmp(eptr, ".sac", 4) == 0 )
		{
			strcpy( filen[i].fn, dit->d_name );
			i++;
			filen = (FileNameList *)realloc( filen, (i+1)*sizeof(FileNameList) );
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

void fatal_error( char *msg1, char *msg2 )
{
	extern char progname[128];
        fprintf(stderr, "%s: FATAL ERROR msg=%s %s\n", progname, msg1, msg2 );
        fprintf(stdout, "%s: FATAL ERROR msg=%s %s\n", progname, msg1, msg2 );
        exit(-1);
}


void print_found_SAC_file( Sac_Header *sp, char *cmp, char *filename )
{
	fprintf( stdout, 
	  "%s: %s: %s: cmp=%-3s net=%s sta=%s loc=(%s) chan=%s az=%7.2f inc=%7.2f filename=%s\n",
                progname,
                __FILE__,
                __func__,
                cmp,
		sp->knetwk,
                sp->kstnm,
                sp->khole,
                sp->kcmpnm,
                sp->cmpaz,
                sp->cmpinc,
                filename );
}

/*******************************************************************************************************/
/**** 
   from the list of SAC files in the data directory 
      determine automatically the 3-C components/channels - translational (ew,ns,z) 
     or rotational (u,v,w) -> (w1,w2,w3) -> (ew,ns,z) 
     for this station based on the SAC header values kcmpnm cmpaz and cmpinc 
   load the SAC vector waveform data and header variables 
   readsac() - autodetects byteorder and loads sac file does swapbytes 
***/
/*******************************************************************************************************/
      
float *getsacdata( 
	char *cmp,
	float *x,
	Sac_Header *sp,
	char *sacfile,
	char *filename,
	int *ifound,
	int verbose )
{
	extern char progname[128];
	int kk;
	FILE *fp;
	Sac_Header *a;

	void print_found_SAC_file( Sac_Header *sp, char *cmp, char *filename );

	float *readsac( Sac_Header *s, char *filename, int verbose );

/***********************************************************************************/
/*** G.Ichinose Nov  7 2019  - Added support for rotational GFs w1, w2, w3       ***/
/*** Rotational component about EW         x1-axis U -> w1                       ***/
/*** Rotational component about NS         x2-axis V -> w2                       ***/
/*** Rotational component about z-vertical x3-axis W -> w3                       ***/
/***********************************************************************************/

/*** read NULL for float * waveform data, just get header to compare the channel info ***/
	
	a = (Sac_Header *)malloc(sizeof(Sac_Header));
	readsac( a, sacfile, verbose );

	if(verbose)
         fprintf( stdout, "%s: %s: %s: looking for cmp=%s chan=%c\n", progname, __FILE__, __func__, cmp, a->kcmpnm[2] );

	*ifound = 0;

	if( strncmp( cmp, "VER", 3 ) == 0 )
	{
		if(
		  ( a->cmpinc ==   0 && a->cmpaz == 0 && a->kcmpnm[2] == 'Z' ) ||
		  ( a->cmpinc == 180 && a->cmpaz == 0 && a->kcmpnm[2] == 'Z' ))
		{
			strcpy( filename, sacfile );
			x = readsac( sp, sacfile, verbose );
			print_found_SAC_file( sp, cmp, filename );
			*ifound = 1;
		}
	}
	else if( strncmp( cmp, "NS", 2 ) == 0 )
	{
		if(
		  ( a->cmpinc == 90 && a->kcmpnm[2] == 'N' ) ||
                  ( a->cmpinc == 90 && a->kcmpnm[2] == '1' ))
		{
			strcpy( filename, sacfile );
                        x = readsac( sp, sacfile, verbose );
                	print_found_SAC_file( sp, cmp, filename );
			*ifound = 1;
		}
	}
	else if( strncmp( cmp, "EW", 2 ) == 0 )
	{
		if(
                  ( a->cmpinc == 90 && a->kcmpnm[2] == 'E' ) ||
                  ( a->cmpinc == 90 && a->kcmpnm[2] == '2' ))
                {
			strcpy( filename, sacfile );
			x = readsac( sp, sacfile, verbose );
			print_found_SAC_file( sp, cmp, filename );
                        *ifound = 1;
		}
	}
	else if( strncmp( cmp, "W1", 2 ) == 0 )
	{
		if( a->kcmpnm[2] == 'U' )
		{
			strcpy( filename, sacfile );
			x = readsac( sp, sacfile, verbose );
			print_found_SAC_file( sp, cmp, filename );
			*ifound = 1;
		}
	}
	else if( strncmp( cmp, "W2", 2 ) == 0 )
	{
		if( a->kcmpnm[2] == 'V' )
                {       
                        strcpy( filename, sacfile );
                        x = readsac( sp, sacfile, verbose );
                        print_found_SAC_file( sp, cmp, filename );
                        *ifound = 1;
                }
	}
	else if( strncmp( cmp, "W3", 2 ) == 0 )
	{
		if( a->kcmpnm[2] == 'W' )
                {       
                        strcpy( filename, sacfile );
                        x = readsac( sp, sacfile, verbose );
                        print_found_SAC_file( sp, cmp, filename );
                        *ifound = 1;
                }
	}
	else
	{
		x = (float *)NULL;
		*ifound = 0;
	}

	if(verbose)
	{
	  fprintf( stderr, "%s: %s: %s:  freeing memory tmp space a\n",
		progname, __FILE__, __func__ );
	  fflush(stderr);
	}
	free(a);
        return (float *)x;
}

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

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/*** custom recursive mkdir, currently used by Greens_subs.c, compare_grn_gmt.c, glib2inv_serial.c, grn2Mxy.c, setupMT.c ***/

/*** testing and example usage ***/

/***********
int main(int ac, char **av )
{
	FILE *fp;
	char filename[256];
	int status;
	char testdir[256]={ "test.dir1/test_dir02/test_dir03" };

	int mkdirp2( const char *, mode_t mode );

	status = mkdirp2( testdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

	fprintf( stderr, "mkdirp2 status=%d\n", status );

	sprintf( filename, "%s/test_file.txt", testdir );
	if( (fp = fopen( filename, "w" )) == NULL )
	{
		fprintf(stderr,"cannot open file for writting %s\n",	
			strerror( errno ) );
		exit(-1);
	}
	fprintf( fp, "This is a test file\n" );
	fclose(fp);

	return EXIT_SUCCESS;
}
*********/

/*** mkdirp2 - custom recursive mkdir ***/

int mkdirp2( const char *dir, mode_t mode )
{
	int retval;
	char tmp[512];
	char *p = NULL;
	size_t len;

	snprintf( tmp, sizeof(tmp), "%s", dir );
	len = strlen(tmp);

	if( tmp[len-1] == '/')
		tmp[len-1] = 0;

	for( p = tmp + 1; *p; p++ )
	{
		if( *p == '/' )
		{
			*p = 0;
			if( (retval = mkdir( tmp, mode )) < 0 )
			{
			  /* fprintf( stderr, "Warning, mkdirp2 dir=%s %s : %s\n",
				dir, tmp, strerror( errno ) ); */

			  if( errno != EEXIST ) 
			  {
				fprintf( stderr, "ERROR exiting\n" );
				exit(-1);
			  }
			}
			*p = '/';
		}
	}
	if( (retval = mkdir( tmp, mode )) < 0 )
	{
		/*
		fprintf( stderr, "Warning, mkdirp2 dir=%s %s : %s\n",
			dir, tmp, strerror( errno ) ); */

		if( errno != EEXIST )
		{
		  fprintf( stderr, "ERROR exiting\n" );
		  exit(-1);
		}
	}
	/* perror( "Error in mkdirp2" ); */
	return retval;
}

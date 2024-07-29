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

/*** after getting cmdline argument argv[0], it can have full path, here we extract just the program name and throw away the path part ***/
/*** this was something for logging in the realtime automt ***/

char *shorten_path( char *pathname, char *filename );
int test_shorten_path_main( int ac, char **av );

/********* test driver ********/
/***
int main(int ac, char **av)
{
	int test_short_path_main( int, char ** );
	test_shorten_path_main(ac,av);
}
***/


int test_shorten_path_main( int ac, char **av )
{
	char pathname[128], filename[128];
	char *shorten_path( char *, char * );
	if( ac == 2 )
	{
	  strcpy( pathname, av[1] );
	  shorten_path( pathname, filename );
	  fprintf( stdout, "pathname=%s filename=%s\n", pathname, filename );
	}
	return 0;
}
	
char *shorten_path( char *pathname, char *filename )
{
	char tok[] = { "/\0" };
	char *ptr, tmp[1024];

/*** copy the pathname to tmp so no overwrie on pathname ***/
	strcpy( tmp, pathname );

/*** check first to see if it is a file to begin with ***/

	ptr = strtok( tmp, tok );
	if( strcmp( pathname, ptr ) == 0 )
	{
		strcpy( filename, pathname );
		return filename;
	}

/*** not a file so it is a path delimited by slashes 
	loop through tokens and return last string ***/

	while(  (ptr = strtok( NULL, tok )) != NULL )
	{
		strcpy( filename, ptr );
	}
	return filename;
}

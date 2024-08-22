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
#include <unistd.h>

/***** for unit testing ****/
/***
void main(int ac, char **av)
{
	void unit_test(int ac, char **av);
	unit_test(ac, av);
}
****/

void unit_test( int ac, char **av )
{
	int maj = 6, min = 3; /*** required min GMT ver ****/

	int major, minor;
	int verbose = 0;

	void get_gmt_version( int *major, int *minor, int verbose );

	int check_gmt_version( int mymajor, int myminor );

/*** begin ***/

	get_gmt_version( &major, &minor, verbose );
	fprintf( stderr, "%s: %s: Installed GMT version major=%d minor=%d\n",
		__FILE__, __func__, major, minor );

	if( check_gmt_version( maj, min ) ) 
		fprintf( stdout, "%s: %s: Required Minimum GMT version major=%d minor=%d. System passes\n", __FILE__, __func__, maj, min );
	else
		fprintf( stdout, "%s: %s: Required Minimum GMT version major=%d minor=%d. System fails\n", __FILE__, __func__, maj, min );
		
}

/*** given required major and minor does the system have a GMT version that satisfies (>=), return true=1 or false=0 ***/

int check_gmt_version( int mymajor, int myminor )
{
	void get_gmt_version( int *major, int *minor, int verbose );
	int sysmajor, sysminor;
	int verbose = 0;

	get_gmt_version( &sysmajor, &sysminor, verbose );

	if( sysmajor >= mymajor ) {
		if( sysminor >= myminor )
		{
			return 1;
		} else {
			return 0;
		}
	} else { 
		return 0;
	}
	return 0; /*** if here then something went wrong, return false ***/
}

/*** this subroutine uses system call to get the current version of GMT installed and returns integer major and minor version values ***/

void get_gmt_version( int *major, int *minor, int verbose )
{
	FILE *fp;
	char version_string[24], tmp_string[24];
	char delimiter[] = ".";
	char *token;

	if(verbose)
	{
		fprintf( stdout, "%s: %s: system calls, which gmt, pwd, and gmt --version > ./gmt_version.txt\n",
			__FILE__, __func__ );

		system( "which gmt" );
		system( "pwd" );
	}
	system( "gmt --version > ./gmt_version.txt" );

	if( (fp = fopen("./gmt_version.txt", "r")) == NULL )
	{
		fprintf( stderr, "%s: %s: [STDERR] Error cannot get GMT version\n", __FILE__, __func__ );
		fprintf( stdout, "%s: %s: [STDOUT] Error cannot get GMT version\n", __FILE__, __func__ );
		*minor = -1;
		*major = -1;
		return;
	}
	
	fscanf( fp, "%s", version_string );
	fclose( fp );

	strcpy( tmp_string, version_string );

	token = strtok( tmp_string, delimiter );
	*major = atoi( token );
	token = strtok( NULL, delimiter );
	*minor = atoi( token );

	if(verbose)
	{
		fprintf( stderr, "%s: %s: GMT version = (%s) major = %d minor = %d\n",
			__FILE__, __func__, version_string, *major, *minor );
	}

	if( remove("./gmt_version.txt") != 0 )
	{
		fprintf( stderr, "%s: %s: error removing file temporary file ./gmt_version.txt\n", __FILE__, __func__ );
	}
	return;
}

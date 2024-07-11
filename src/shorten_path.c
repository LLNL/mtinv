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

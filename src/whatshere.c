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
#define MAX_STA 128

char progname[128];

/*** my own array of file names for myscandir() and getsacfiles()  ***/

typedef struct { char fn[256]; } FileNameList;

int my_file_name_cmp( const void *a, const void *b )
{
        FileNameList *c1 = (FileNameList *)a;
        FileNameList *c2 = (FileNameList *)b;
        return strcmp( c1->fn, c2->fn );
}

int main( int ac, char **av )
{
	Greens **grn;
	int nz, iz;
	int ista, i;
	float *z;
	int it, nt, nsta;
	float max, min;

	int verbose=0;
	FILE *fp;

	int setpar(int,char**);
	int mstpar( char *, char *, void * );
        int getpar( char *, char *, void * );
	void endpar();

	int count;
	char pathname[256];
	FileNameList *filen;
	FileNameList *scandir_for_file_ext( char *pathname, char *extenstion, int *count, FileNameList *filen );

	EventInfo *ev;

/*** start main ***/

	strcpy( progname, av[0] );
	strcpy( pathname, "./" );

	setpar(ac,av);
	getpar( "path", "s", pathname );
	getpar( "verbose", "b", &verbose );
	endpar();

	fprintf(  stdout, "%s: version=%s release date=%s\n",
                progname, Version_Label, Version_Date );

	fprintf( stdout, "\n\n" );

/*************************************************************************************************/
/*************************************************************************************************/
/*** load and peak at glib files ***/
/*************************************************************************************************/
/*************************************************************************************************/

/*** allocate space for filename list ***/

	filen = (FileNameList *)malloc( sizeof(FileNameList) );

/*** get only glib file name list from remote directory ***/

	filen = (FileNameList *)scandir_for_file_ext( pathname, ".glib", &count, filen );

	if( count <= 0 )
	{
		fprintf( stdout, "%s: count=%d No *.glib files in this directory %s.\n",
			progname, count, pathname );
	}


	fprintf( stdout, "%s: number of *.glib files = %d in directory %s\n",
		progname, count, pathname );
	for( i=0; i<count; i++ )
                fprintf(stdout, "%s: %s\n", progname, filen[i].fn );
	fprintf( stdout, "\n" );

/*** sort the file names ***/

	/* qsort( filen, count, sizeof(FileNameList), my_file_name_cmp ); */

/*** loop over all glib files and peak inside ***/

	grn = (Greens **)malloc( count*sizeof(Greens *) );

	for( ista=0, i=0; i<count; i++ )
	{
		fprintf(stdout, "%s: %s\n", progname, filen[i].fn );

		if( (fp=fopen( filen[i].fn, "r")) == NULL )
		{
			continue;
		}

		fread( &nz, sizeof(int), 1, fp );
		z = malloc( nz*sizeof(float) );
		fread( &z[0], nz*sizeof(float), 1, fp );

		grn[ista] = (Greens *)malloc( nz*sizeof(Greens) );

		for( iz=0; iz<nz; iz++ )
		{
			fread( &grn[ista][iz], sizeof(Greens), 1, fp );
			nt = grn[ista][iz].nt;
			min = grn[ista][iz].g.rss[0];
			max = grn[ista][iz].g.rss[0];
			for( it=1; it<nt; it++ )
			{
				if( grn[ista][iz].g.rss[it] < min ) min =  grn[ista][iz].g.rss[it];
				if( grn[ista][iz].g.rss[it] > max ) max =  grn[ista][iz].g.rss[it];
			}
			fprintf( stdout,
			  "%s: %s: iz=%d z=%g %s.%s rdist=%.1f az=%.0f t0=%.2f dt=%.2f nt=%d min=%.2e max=%.2e modf=%s\n",
				progname, 
				grn[ista][iz].filename,
				iz, 
				grn[ista][iz].evdp, 
				grn[ista][iz].stnm,
                                grn[ista][iz].net, 
				grn[ista][iz].rdist,
				grn[ista][iz].az,
                                grn[ista][iz].t0,
				grn[ista][iz].dt,
				grn[ista][iz].nt,
				min,
				max,
				grn[ista][iz].v.modfile );
		}
		fclose(fp);
		free(z);

		ista++;
	}
	free(filen);
	free(grn);

/*************************************************************************************************/
/*************************************************************************************************/
/*** load and peak at data files ***/
/*************************************************************************************************/
/*************************************************************************************************/

	fprintf( stdout, "\n\n" );

/*** allocate space for filename list ***/
                                                                                                                                                                       
        filen = (FileNameList *)malloc( sizeof(FileNameList) );
                                                                                                                                                                       
/*** get only glib file name list from remote directory ***/
                                                                                                                                                                       
        filen = (FileNameList *)scandir_for_file_ext( pathname, ".data", &count, filen );
                                                                                                                                                                       
        if( count <= 0 )
        {
                fprintf( stdout, "%s: count=%d No *.data files in this directory %s.\n",
                        progname, count, pathname );
        }
                                                                                                                                                                       
                                                                                                                                                                       
        fprintf( stdout, "%s: number of *.data files = %d in directory %s\n",
                progname, count, pathname );
 	for( i=0; i<count; i++ )
		fprintf(stdout, "%s: %s\n", progname, filen[i].fn );
	fprintf( stdout, "\n" );

/*** sort the file names ***/
                                                                                                                                                                       
        /* qsort( filen, count, sizeof(FileNameList), my_file_name_cmp ); */

	ev  = (EventInfo *)malloc( count * sizeof(EventInfo) );

	for( ista=0, i=0; i<count; i++ )
	{
		fprintf(stdout, "%s: %s\n", progname, filen[i].fn );
                                                                                                                                                                       
                if( (fp=fopen( filen[i].fn, "r")) == NULL )
                {
                        continue;
                }

		fread( &ev[ista], sizeof(EventInfo), 1, fp );

		fprintf( stdout,
		  "%s: %s: ista=%3d np=%d pass=%d lf=%g hf=%g nt=%d dt=%g tr=%g tt=%g\n",
                        progname, 
			filen[i].fn, 
			ista, 
			ev[ista].npole,
			ev[ista].npass,
			ev[ista].lf,
                        ev[ista].hf, 
			ev[ista].nt,
			ev[ista].dt,
			ev[ista].tr,
			ev[ista].tt );

		fprintf( stdout, "%s: origin time=\t\t", progname );
                        WriteMyTime2STDOUT( &(ev[ista].ot ) );

		ev[ista].ew.data = calloc( ev[ista].ew.s.npts, sizeof(float) );
                ev[ista].ns.data = calloc( ev[ista].ns.s.npts, sizeof(float) );
                ev[ista].z.data  = calloc( ev[ista].z.s.npts, sizeof(float) );
	
		if(verbose)
                        fprintf( stdout, "%s: Reading the data...", progname );

		fread( &(ev[ista].ew.data[0]), ev[ista].ew.s.npts * sizeof(float), 1, fp );
                fread( &(ev[ista].ns.data[0]), ev[ista].ns.s.npts * sizeof(float), 1, fp );
                fread( &(ev[ista].z.data[0]),  ev[ista].z.s.npts  * sizeof(float), 1, fp );

		fprintf( stdout,
                        "%s: Read the file %s\n", progname, ev[ista].data_filename );

		fprintf( stdout,
                   "%s: %s: cmp=%-3.3s nt=%d dt=%g sta=%s net=%s min=%.3e max=%.3e\n",
                        progname, 
			filen[i].fn,
			ev[ista].ew.s.kcmpnm,
			ev[ista].ew.s.npts,
                        ev[ista].ew.s.delta,
			ev[ista].ew.s.kstnm,
                        ev[ista].ew.s.knetwk,
			ev[ista].ew.s.depmin,
			ev[ista].ew.s.depmax );
                                                                                                                                                                
                  fprintf( stdout,
                    "%s: %s: cmp=%-3.3s nt=%d dt=%g sta=%s net=%s min=%.3e max=%.3e\n",
                        progname, 
			filen[i].fn,
			ev[ista].ns.s.kcmpnm,
			ev[ista].ns.s.npts,
                        ev[ista].ns.s.delta,
			ev[ista].ns.s.kstnm,
                        ev[ista].ns.s.knetwk,
			ev[ista].ns.s.depmin,
			ev[ista].ns.s.depmax );
                                                                                                                                                                
                  fprintf( stdout,
                    "%s: %s: cmp=%-3.3s nt=%d dt=%g sta=%s net=%s min=%.3e max=%.3e\n",
                        progname, 
			filen[i].fn,
			ev[ista].z.s.kcmpnm,
			ev[ista].z.s.npts,
                        ev[ista].z.s.delta,
			ev[ista].z.s.kstnm,
                        ev[ista].z.s.knetwk,
			ev[ista].z.s.depmin,
			ev[ista].z.s.depmax );

                fclose(fp);
		ista++;
	}
	free(filen);
	free(ev);

/*************************************************************************************************/
/*************************************************************************************************/
/*** load ginv files ***/
/*************************************************************************************************/
/*************************************************************************************************/

	fprintf( stdout, "\n\n" );

/*** allocate space for filename list ***/
                                                                                                                                                                       
        filen = (FileNameList *)malloc( sizeof(FileNameList) );
                                                                                                                                                                       
/*** get only glib file name list from remote directory ***/
                                                                                                                                                                       
        filen = (FileNameList *)scandir_for_file_ext( pathname, ".ginv", &count, filen );
                                                                                                                                                                       
        if( count <= 0 )
        {
                fprintf( stdout, "%s: count=%d No *.data files in this directory %s.\n",
                        progname, count, pathname );
        }
                                                                                                                                                                       
                                                                                                                                                                       
        fprintf( stdout, "%s: number of *.data files = %d in directory %s\n",
                progname, count, pathname );
	for( i=0; i<count; i++ )
                fprintf(stdout, "%s: %s\n", progname, filen[i].fn );
	fprintf( stdout, "\n" );

/*** sort the file names ***/
                                                                                                                                                                       
        /* qsort( filen, count, sizeof(FileNameList), my_file_name_cmp ); */
                                                                                                                                                                       
        ev  = (EventInfo *)malloc( count * sizeof(EventInfo) );

	grn = (Greens **)malloc( count*sizeof(Greens *) );

	for( ista=0, i=0; i<count; i++ )
        {
                fprintf( stdout, "%s: %s\n", progname, filen[i].fn );

                if( (fp=fopen( filen[i].fn, "r")) == NULL )
                {
                        continue;
                }
                
		fread( &nz, sizeof(int), 1, fp );
		z = malloc( nz*sizeof(*z) );
         	fread( &z[0], nz*sizeof(float), 1, fp );
		grn[ista] = (Greens *)malloc( nz*sizeof(Greens));
		for( iz=0; iz<nz; iz++ )
		{
			fread( &grn[ista][iz], sizeof(Greens), 1, fp );
			nt = grn[ista][iz].nt;
			fprintf( stdout,
			  "%s: %s: iz=%d z=%g %s.%s rdist=%g az=%g t0=%g dt=%g nt=%d modf=%s\n",
				progname, 
				filen[i].fn,
				iz, 
				grn[ista][iz].evdp,
				grn[ista][iz].stnm,
				grn[ista][iz].net,
                                grn[ista][iz].rdist,
				grn[ista][iz].az,
				grn[ista][iz].t0,
				grn[ista][iz].dt,
                                grn[ista][iz].nt,
				grn[ista][iz].v.modfile );
		}
		fclose(fp);
		ista++;
	}
	free(filen);
	free(ev);

	exit(0);
}

FileNameList *scandir_for_file_ext( char *pathname, char *extension, int *count, FileNameList *filen )
{
	struct stat f;
	DIR *dip;
	struct dirent *dit;
	int i = 0;
	char *eptr;
	
	if( (dip = opendir(pathname) ) == NULL )
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
		eptr = rindex( dit->d_name, '.' );
		if( eptr == NULL ) continue;

		if( strcmp(eptr, extension) == 0 )
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

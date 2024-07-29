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

/**********************************************************************/
/*** sacio.c Fri Jan 27 20:37:16 EST 2012 G. Ichinose               ***/
/*** sacio.c Mon Oct 24 14:48:27 PDT 2016 G. Ichinose - return NULL ***/
/*** when swapflag = -1                                             ***/
/**********************************************************************/
/* float *readsac( Sac_Header *s, char *filename, int verbose )       */
/* void wrtoldsac( char *FO, Sac_Header *s, float *data )             */
/* void wrtnewsac( char *FO, float dt, int ns, float *ar, float b)    */
/* void set_sac_minmax( Sac_Header *s, float *data )                  */
/* void duplicate_vector( float *x, float *y, int n )                 */
/* void truncate_string( char *str, int n )                           */
/* int TestFileByteOrder( char *filename, int verbose )               */
/* int TestByteOrder(void)                                            */
/* float *swapsac( FILE *fp, Sac_Header *sp, float *data )            */
/* int int_swap( char cbuf[] )                                        */
/* long long_swap( char cbuf[] )                                      */
/* float float_swap( char cbuf[] )                                    */
/* int isBigEndian(void)                                              */
/* check_for_zero_trace( float *x, int nt );                          */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>

#include "sacfile.h"  /*** includes sac.h and mytime.h ***/

extern char progname[128];

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 0
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 1
#endif

#define NUMHEADBYTES 632 /* floats and longs are 440 rest are characters */

float *readsac( Sac_Header *s, char *filename, int verbose )
{
        FILE *fp;
        float *data;
	int swapflag;

	int TestFileByteOrder(char *,int); /*** returns 1-swap 0-noswap -1-not SAC ***/
	int TestByteOrder(void);
	void truncate_string( char *str, int n );
	float *swapsac( FILE *, Sac_Header *, float * );

	/*
	if( verbose )
	{
	  if( TestByteOrder() )
	  {
		fprintf( stderr, "TestByteOrder() I am Big Endian\n" );
	  }
	  else
	  {
		fprintf( stderr, "TestByteOrder() I am Little Endian\n" );
	  }
	}
	*/

/*** do check TestFileByteOrder ***/

	swapflag = TestFileByteOrder(filename,verbose);
	/* fprintf( stderr, "swapflag=%d\n", swapflag ); */

	if( swapflag == 1 )
	{
	  if( verbose )
	    fprintf( stderr, "%s: %s needs to be swapped swapflag = %d\n", 
		progname, filename, swapflag );
	}
	else if( swapflag == 0 )
	{
	/*** everything is ok ***/
	/*
	  if( verbose )
	  {
	    fprintf( stderr, "%s: %s does NOT need to be swapped swapflag = %d\n", 
		progname, filename, swapflag );
	  }
	*/
	}
	else if ( swapflag == -1 )
	{
	  fprintf( stderr, "%s: %s ERROR! THIS IS NOT A SAC FILE swapflag=%d\n",
		progname, filename, swapflag );
	  return NULL;
	}

/*** done check ***/

        if( (fp = fopen( filename, "rb" )) == NULL )
        {
                fprintf(stderr, "error opening file %s\n", filename );
                exit(-1);
        }

	if( swapflag )
	{
		data = (float *)calloc(2, sizeof(float) );
		data = swapsac( fp, s, data );
	}
	else
	{
        	fread( s, sizeof(Sac_Header), 1, fp );
        	data = (float *)calloc(s->npts, sizeof(float) );
        	fread( &data[0], s->npts * sizeof(float), 1, fp );
	}
        fclose(fp);

/***
	fprintf( stdout, "knetwk=(%s)\n", s->knetwk );
	fprintf( stdout, " kstnm=(%s)\n", s->kstnm );
	fprintf( stdout, " khole=(%s)\n", s->khole );
	fprintf( stdout, "kcmpnm=(%s)\n", s->kcmpnm );
****/
	truncate_string( s->knetwk, 6 );
        truncate_string( s->kstnm,  6 );
        truncate_string( s->khole,  6 );
        truncate_string( s->kcmpnm, 6 );

/*** fix old sac null -12345 for khole ***/
	if( strncmp( s->khole, "-12345", 6 ) == 0 ) strcpy( s->khole, ""  );

	if(verbose)
	{
	 fprintf( stdout, "knetwk=(%s)\n", s->knetwk );
         fprintf( stdout, " kstnm=(%s)\n", s->kstnm );
         fprintf( stdout, " khole=(%s)\n", s->khole );
         fprintf( stdout, "kcmpnm=(%s)\n", s->kcmpnm );
	}

        return (float *)data;
}

int readsacfile( SacFile *sf, int verbose )
{
	FILE *fp;
	double endtime;
	/* int nt; */
	/* float dt; */
	extern char progname[128];
	void truncate_string( char *str, int n );

	int swapflag;
	int TestFileByteOrder(char *,int); /*** returns 1-swap 0-noswap ***/
	int TestByteOrder(void);
	float *swapsac( FILE *, Sac_Header *, float * );

	MyTime *epoch2time( MyTime *, double );
	void WriteMyTime2STDOUT( MyTime * );
	void complete( MyTime * );
	MyTime *sac2mytime( MyTime *, Sac_Header * );

/*** start subroutine ***/

/*** do check TestFileByteOrder ***/
	swapflag = TestFileByteOrder(sf->filename,verbose);
        /* fprintf( stderr, "swapflag=%d\n", swapflag ); */

        if( swapflag == 1 )
        {
          if( verbose )
            fprintf( stderr, "%s: %s needs to be swapped swapflag = %d\n",
                progname, sf->filename, swapflag );
        }
        else if( swapflag == 0 )
        {
        /*** everything is ok ***/
        /*
          if( verbose )
          {
            fprintf( stderr, "%s: %s does NOT need to be swapped swapflag = %d\n",
                progname, sf->filename, swapflag );
          }
        */
        }
        else if ( swapflag == -1 )
        {
          fprintf( stderr, "\n\n%s: %s ERROR! THIS NOT A SAC FILE swapflag=%d\n\n\n.",
                progname, sf->filename, swapflag );
          return swapflag;
        }

	if( (fp = fopen( sf->filename, "rb" ))  == NULL )
	{
		fprintf(stderr, "%s: error opening file %s\n",
			progname, sf->filename );
		exit(-1);
	}

	if( swapflag )
	{
		sf->data = (float *)calloc(2, sizeof(float) );
		sf->data = swapsac( fp, &(sf->s), sf->data );
	}
	else
	{
		fread( &(sf->s), sizeof(Sac_Header), 1, fp );
		sf->data = (float *)calloc(sf->s.npts, sizeof(float) );
		fread( &(sf->data[0]), sf->s.npts * sizeof(float), 1, fp );
	}
	/* dt = sf->s.delta; */
	/* nt = sf->s.npts; */
	fclose(fp);

        truncate_string( sf->s.knetwk, 6 );
        truncate_string( sf->s.kstnm,  6 );
        truncate_string( sf->s.khole,  6 );
        truncate_string( sf->s.kcmpnm, 6 );

/*** fix old sac null -12345 for khole ***/

        if( strncmp( sf->s.khole, "-12345", 6 ) == 0 ) strcpy( sf->s.khole, ""  );

	if(verbose)
	{
          fprintf( stdout, "knetwk=(%s)\n", sf->s.knetwk );
          fprintf( stdout, " kstnm=(%s)\n", sf->s.kstnm );
          fprintf( stdout, " khole=(%s)\n", sf->s.khole );
          fprintf( stdout, "kcmpnm=(%s)\n", sf->s.kcmpnm );

		fprintf( stdout, "%s: %s year=%d jday=%d hour=%d min=%d sec=%d msec=%d\n",
			progname,
			sf->filename,
			sf->s.nzyear,
			sf->s.nzjday,
			sf->s.nzhour,
			sf->s.nzmin,
			sf->s.nzsec,
			sf->s.nzmsec );
	}

/*** set the begin time (time of first sample) ***/

	sac2mytime( &(sf->beg), &(sf->s) );
	if(verbose) WriteMyTime2STDOUT( &(sf->beg) );
	
/*** set the endtime ***/
	endtime = sf->s.npts * sf->s.delta;
	epoch2time( &(sf->end), sf->beg.epoch + endtime );
	if(verbose) WriteMyTime2STDOUT( &(sf->end) );

/*** set the origin time ***/
	epoch2time( &(sf->ot), (sf->beg.epoch + sf->s.o) );
	if(verbose) WriteMyTime2STDOUT( &(sf->ot) );

	return swapflag;
}

void wrtoldsac( char *FO, Sac_Header *s, float *data )
{
	FILE           *iopo;
	if ((iopo = fopen(FO, "w")) == NULL)
	{
		printf("cannot open output file %s\n", FO);
		exit(1);
	}
	fwrite(s, sizeof(Sac_Header), 1, iopo);
	fwrite(&data[0], s->npts*sizeof(float), 1, iopo);
	fclose(iopo);
	return;
}

void wrtnewsac( char *FO, float dt, int ns, float *ar, float b)
{
    Sac_Header      sp;
    FILE           *iopo;
 
    if ((iopo = fopen(FO, "w")) == NULL)
    {
        printf("cannot open output file %s\n", FO);
        exit(1);
    }

/* convert parameters to sac header parameters */
    sp = sac_null;
 
/* floats */
    sp.b = b;
    sp.delta = dt;  /* sampling rate in seconds per sample */
    sp.e = b + (ns * sp.delta);
 
/* integers */
    sp.nvhdr  = 6;           /* LLL sets it */
    sp.norid  = 0;           /* LLL sets it */
    sp.nevid  = 0;           /* LLL sets it */
    sp.iftype = ITIME;  /* data type: IRLIM spec re im | IAMPH amp ph | IXY general x,y  */
    sp.idep   = IUNKN;  /* not disp vel acc or volts */
    sp.iztype = IB;  /* types: IUNKN,IB,IDAY,IO,IA,ITn  */
    sp.ievtyp = IUNKN;  /* type of event IQUAKE - earthquake */
    sp.npts   = ns;
/* logicals  */
    sp.leven = TRUE;        /* is data evenly sampled  */
    sp.lpspol = FALSE;          /* LLL sets it */
    sp.lcalda = TRUE;           /* should az,baz,gcarc,dist be calculated?  */
    sp.lhdr5  = FALSE;        /* LLL sets it */
 
/* write binary sac header */
    if (fwrite(&sp, sizeof(Sac_Header), 1, iopo) != 1)
    {
        perror("fwrite");
        printf("FATAL ERROR: bad  write %s, header\n", FO);
        exit(4);
    }
/* write floating point data */
    if (fwrite(&ar[0], ns * sizeof(float), 1, iopo) != 1)
    {
        printf("ns=%d, %lu, nbytes=%lu\n", ns, (unsigned long)ns, (unsigned long)ns * sizeof(float));
        printf("FATAL ERROR: bad write %s\n", FO);
        exit(5);
    }
    fclose(iopo);
    return;
}

void set_sac_minmax( Sac_Header *s, float *data )
{
	int nt, i;
	float min,max,sum=0;
	nt = s->npts;
	min = data[0];
	max = data[0];
	sum = data[0];
	for( i=1; i<nt; i++ )
	{
		if( data[i] < min ) min = data[i];
		if( data[i] > max ) max = data[i];
		sum += data[i];
	}
	s->depmax = max;
	s->depmin = min;
	s->depmen = sum/nt;
	return;
}

void sac_minmax( float *x, int n, float *max, float *min, float *mean )
{
        int i;
        float sum=0;
        *max = x[0];
        *min = x[0];
        *mean = 0;
        for( i=0; i<n; i++ )
        {
                if( x[i] > *max ) *max=x[i];
                if( x[i] < *min ) *min=x[i];
                sum = sum + x[i];
        }
        *mean = sum/n;
}

void sac_absmax( float *x, int n, float *absmax )
{
        int i;
        *absmax = fabs( x[0] );
        for( i=0; i<n; i++ )
        {
                if( fabs( x[i] ) > *absmax )
                        *absmax = fabs( x[i] );
        }
}

void duplicate_vector( float *x, float *y, int n )
{
        int i;
        for( i=0; i<n; i++ ) y[i] = x[i];
}

void absolute_vector( float *x, int n )
{
	int i;
	float xtmp;
        for( i=0; i<n; i++ ) 
	{
		xtmp = x[i];
		x[i] = fabs(xtmp);
	}
}

void multiply_vector( float *x, float *y, float *z, int n )
{
	int i;
	for( i=0; i<n; i++ ) z[i] = x[i] * y[i];
}

void add_vector( float *x, float *y, float *z, int n )
{
        int i;
        for( i=0; i<n; i++ ) z[i] = x[i] + y[i];
}

void subtract_vector( float *x, float *y, float *z, int n )
{
        int i;
        for( i=0; i<n; i++ ) z[i] = x[i] - y[i];
}

void divide_vector( float *x, float *y, float *z, int n )
{
        int i;
        for( i=0; i<n; i++ ) z[i] = x[i] / y[i];
}

void scale_vector( float *x, int n, float fac )
{
	int i;
	float xtmp;
	for( i = 0; i < n; i++ )
	{
		xtmp = x[i];
		x[i] = fac * xtmp;
	}
}

void truncate_string( char *str, int n )
{
        int i;
        for( i = 0; i < n; i++ )
        {
                if( str[i] == ' ' )
                {
                        str[i]='\0';
                        return;
                }
        }
}

/***
     read the first 4 bytes which should be floating point value
      that contains the sampling rate in sec per sample
***/

int TestFileByteOrder( char *filename, int verbose )
{
	FILE *fp;
	char cbuf4[4];
	char cbuf;
	char str_buf[24];
	Sac_Header s;
	float delta;
	int npts;

	union {
		char cval[4];
		float fval;
	} u1;

	union {
		char cval[4];
		float fval;
	} u2;

/*** if the file does not exist this will be the first place ***/
	if( (fp = fopen(filename, "rb")) == NULL )
	{
	  fprintf(stderr,
	    "%s: sacio.o:readsac():TestFileByteOrder(): cannot open file %s\n",
		progname, filename );
	  return -1;
	  /* exit(-1); */
	}

	fread( &cbuf4, 4, 1, fp );

/*****************************************************************/
/*** Identify first two bytes <! = 0x3c 0x21  HTML or XML file ***/
/*** Identify first two bytes <? = 0x3c 0x3F  HTML or XML file ***/
/*** Identify first two bytes Er = 0x45 0x72  Error 404        ***/
/*****************************************************************/

	if(	( cbuf4[0] == 0x3c && cbuf4[1] == 0x21 ) || 
		( cbuf4[0] == 0x45 && cbuf4[1] == 0x72 ) ||
		( cbuf4[0] == 0x3c && cbuf4[1] == 0x3F ) )
	{
	  if(verbose)
	  {
	    fprintf( stdout, "%s: This is a HTML or XML file: byte1=(0x%x) byte2=(0x%x)\n", 
		progname, cbuf4[0], cbuf4[1] );
	    fprintf( stderr, "%s: This is a HTML or XML file: byte1=(0x%x) byte2=(0x%x)\n", 
		progname, cbuf4[0], cbuf4[1] );
	    fflush(stdout);
            fflush(stderr);
	  }
	  return -1;
	}

	fclose(fp);

/*** NOSWAP ***/
	u1.cval[0] = cbuf4[0];
	u1.cval[1] = cbuf4[1];
	u1.cval[2] = cbuf4[2];
	u1.cval[3] = cbuf4[3];

	if(verbose)
	  fprintf( stderr, "%s: NOSWAP u1.fval = %g\n", progname, u1.fval );

/*** SWAP ***/
	u2.cval[0] = cbuf4[3];
	u2.cval[1] = cbuf4[2];
	u2.cval[2] = cbuf4[1];
	u2.cval[3] = cbuf4[0];

	if(verbose)
	  fprintf( stderr, "%s: SWAP   u2.fval = %g\n", progname, u2.fval );

/***********************************************************************************/
/*** look at the first 4 bytes in SAC file the sampling rate - dt is stored here ***/
/*** as seconds per sample.  IS this value reasonable? if not then swap          ***/
/*** NEW 6.24.2016 g.ichinose - must also check SWAP version of dt to ensure it  ***/
/*** is reasonable, else this is not a SAC file                                  ***/
/***********************************************************************************/

/*** if dt is negative for both or both greater than 1000 then this is not SAC ***/

	if( u1.fval < 0 && u2.fval < 0 ) return -1;
	if( u1.fval > 1.0e+3 && u2.fval > 1.0e+3 ) return -1;

/*** whch one is more reasonable ***/

	if( u1.fval < 0 || fabs(u1.fval) > 1.0E+6 || fabs(u1.fval) < 1.0E-10 )
		return 1; /*** yes swap ***/
	else
		return 0; /*** no swap ***/

} /*** end of routine TestFileByteOrder() ***/

int TestByteOrder(void)
{
	int word = 0x0001;
	char *byte = (char *) &word;
	return(byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}

float *swapsac( FILE *fp, Sac_Header *sp, float *data )
{
	int j;
	char cbuf[NUMHEADBYTES];

	float float_swap( char cbuf[] );
	int   int_swap( char cbuf[] );
	long  long_swap( char cbuf[] );

/** set some sac header defaults **/
	*sp = sac_null; 

/** read in header **/
	if( fread(cbuf, 440*sizeof(char), 1, fp) <= 0 )
	{
		fprintf(stderr, "A read error has occurred\n" );
		exit(-1);
	}

	sp->delta       = float_swap(cbuf);
        sp->depmin      = float_swap(cbuf+4);
        sp->depmax      = float_swap(cbuf+8);
        sp->scale       = float_swap(cbuf+12);
        sp->odelta      = float_swap(cbuf+16);
        sp->b           = float_swap(cbuf+20);
        sp->e           = float_swap(cbuf+24);
        sp->o           = float_swap(cbuf+28);
        sp->a           = float_swap(cbuf+32);
        sp->fmt         = float_swap(cbuf+36);
        sp->t0          = float_swap(cbuf+40);
        sp->t1          = float_swap(cbuf+44);
        sp->t2          = float_swap(cbuf+48);
        sp->t3          = float_swap(cbuf+52);
        sp->t4          = float_swap(cbuf+56);
        sp->t5          = float_swap(cbuf+60);
        sp->t6          = float_swap(cbuf+64);
        sp->t7          = float_swap(cbuf+68);
        sp->t8          = float_swap(cbuf+72);
        sp->t9          = float_swap(cbuf+76);
        sp->f           = float_swap(cbuf+80);
        sp->resp0       = float_swap(cbuf+84);
        sp->resp1       = float_swap(cbuf+88);
        sp->resp2       = float_swap(cbuf+92);
        sp->resp3       = float_swap(cbuf+96);
        sp->resp4       = float_swap(cbuf+100);
        sp->resp5       = float_swap(cbuf+104);
        sp->resp6       = float_swap(cbuf+108);
        sp->resp7       = float_swap(cbuf+112);
        sp->resp8       = float_swap(cbuf+116);
        sp->resp9       = float_swap(cbuf+120);
        sp->stla        = float_swap(cbuf+124);
        sp->stlo        = float_swap(cbuf+128);
        sp->stel        = float_swap(cbuf+132);
        sp->stdp        = float_swap(cbuf+136);
        sp->evla        = float_swap(cbuf+140);
        sp->evlo        = float_swap(cbuf+144);
        sp->evel        = float_swap(cbuf+148);
        sp->evdp        = float_swap(cbuf+152);
        sp->mag         = float_swap(cbuf+156);
        sp->user0       = float_swap(cbuf+160);
        sp->user1       = float_swap(cbuf+164);
        sp->user2       = float_swap(cbuf+168);
        sp->user3       = float_swap(cbuf+172);
        sp->user4       = float_swap(cbuf+176);
        sp->user5       = float_swap(cbuf+180);
        sp->user6       = float_swap(cbuf+184);
        sp->user7       = float_swap(cbuf+188);
        sp->user8       = float_swap(cbuf+192);
        sp->user9       = float_swap(cbuf+196);
        sp->dist        = float_swap(cbuf+200);
        sp->az          = float_swap(cbuf+204);
        sp->baz         = float_swap(cbuf+208);
        sp->gcarc       = float_swap(cbuf+212);
        sp->sb          = float_swap(cbuf+216);
        sp->sdelta      = float_swap(cbuf+220);
        sp->depmen      = float_swap(cbuf+224);
        sp->cmpaz       = float_swap(cbuf+228);
	sp->cmpinc      = float_swap(cbuf+232);
        sp->xmin        = float_swap(cbuf+236);
        sp->xmax        = float_swap(cbuf+240);
        sp->ymin        = float_swap(cbuf+244);
        sp->ymax        = float_swap(cbuf+248);
        sp->adjtim      = float_swap(cbuf+252);
        sp->fhdr65      = float_swap(cbuf+256);
        sp->fhdr66      = float_swap(cbuf+260);
        sp->fhdr67      = float_swap(cbuf+264);
        sp->fhdr68      = float_swap(cbuf+268);
        sp->fhdr69      = float_swap(cbuf+272);
        sp->fhdr70      = float_swap(cbuf+276);

        sp->nzyear      = int_swap(cbuf+280);
        sp->nzjday      = int_swap(cbuf+284);
        sp->nzhour      = int_swap(cbuf+288);
        sp->nzmin       = int_swap(cbuf+292);
        sp->nzsec       = int_swap(cbuf+296);
        sp->nzmsec      = int_swap(cbuf+300);
        sp->nvhdr       = int_swap(cbuf+304);
        sp->norid       = int_swap(cbuf+308);
        sp->nevid       = int_swap(cbuf+312);
        sp->npts        = int_swap(cbuf+316);
        sp->nsnpts      = int_swap(cbuf+320);
        sp->nwfid       = int_swap(cbuf+324);
        sp->nxsize      = int_swap(cbuf+328);
        sp->nysize      = int_swap(cbuf+332);
        sp->nhdr15      = int_swap(cbuf+336);
        sp->iftype      = int_swap(cbuf+340);
        sp->idep        = int_swap(cbuf+344);
        sp->iztype      = int_swap(cbuf+348);
        sp->ihdr4       = int_swap(cbuf+352);
        sp->iinst       = int_swap(cbuf+356);
        sp->istreg      = int_swap(cbuf+360);
        sp->ievreg      = int_swap(cbuf+364);
        sp->ievtyp      = int_swap(cbuf+368);
        sp->iqual       = int_swap(cbuf+372);
        sp->isynth      = int_swap(cbuf+376);
        sp->imagtyp     = int_swap(cbuf+380);
        sp->imagsrc     = int_swap(cbuf+384);
        sp->ihdr13      = int_swap(cbuf+388);
        sp->ihdr14      = int_swap(cbuf+392);
        sp->ihdr15      = int_swap(cbuf+396);
        sp->ihdr16      = int_swap(cbuf+400);
        sp->ihdr17      = int_swap(cbuf+404);
        sp->ihdr18      = int_swap(cbuf+408);
        sp->ihdr19      = int_swap(cbuf+412);
        sp->ihdr20      = int_swap(cbuf+416);
        sp->leven       = int_swap(cbuf+420);
        sp->lpspol      = int_swap(cbuf+424);
        sp->lovrok      = int_swap(cbuf+428);
        sp->lcalda      = int_swap(cbuf+432);
        sp->lhdr5       = int_swap(cbuf+436);

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kstnm, 8 );

	fread(cbuf, 16*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kevnm, 16 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->khole, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->ko, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->ka, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt0, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt1, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt2, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt3, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt4, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt5, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt6, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt7, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt8, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kt9, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kf, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kuser0, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kuser1, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kuser2, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kcmpnm, 8 );

	fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->knetwk, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kdatrd, 8 );

        fread(cbuf, 8*sizeof(char), 1, fp);
        bcopy( cbuf, sp->kinst, 8 );

/** read and swap the data **/
/*
	fprintf(stderr, "allocating %d pts, nbytes=%d for reading in swapped SAC data\n",
		sp->npts, (sp->npts * sizeof(float)) );
*/

        data = realloc( data, sp->npts * sizeof(float) );

        for ( j=0; j<sp->npts; j++)
        {
                fread(cbuf, sizeof(char), 4, fp);
                data[j] = float_swap(cbuf);
        }
	return(float *)data;
}

short short_swap( char cbuf[] )
{
	union {
		char cval[2];
		short sval;
	} short_union;
	short_union.cval[1] = cbuf[0];
	short_union.cval[0] = cbuf[1];
	return(short_union.sval);
}

int int_swap( char cbuf[] )
{
        union {
                char cval[4];
                int ival;
        } int_union;
        int_union.cval[3] = cbuf[0];
        int_union.cval[2] = cbuf[1];
        int_union.cval[1] = cbuf[2];
        int_union.cval[0] = cbuf[3];
        return(int_union.ival);
}

long long_swap( char cbuf[] )
{
        union {
                char cval[4];
                long lval;
        } l_union;

        l_union.cval[3] = cbuf[0];
        l_union.cval[2] = cbuf[1];
        l_union.cval[1] = cbuf[2];
        l_union.cval[0] = cbuf[3];
        return(l_union.lval);
}

float float_swap( char cbuf[] )
{
        union {
                char cval[4];
                float fval;
        } f_union;

        f_union.cval[3] = cbuf[0];
        f_union.cval[2] = cbuf[1];
        f_union.cval[1] = cbuf[2];
        f_union.cval[0] = cbuf[3];
        return(f_union.fval);
}

double double_swap( char cbuf[] )
{
        union {
                char cval[8];
                double dval;
        } double_union;
                                                                                                                                                               
        double_union.cval[7] = cbuf[0];
        double_union.cval[6] = cbuf[1];
        double_union.cval[5] = cbuf[2];
        double_union.cval[4] = cbuf[3];
        double_union.cval[3] = cbuf[4];
        double_union.cval[2] = cbuf[5];
        double_union.cval[1] = cbuf[6];
        double_union.cval[0] = cbuf[7];
                                                                                                                                                               
        return(double_union.dval);
}

int isBigEndian(void)
{
        char filename[64];
        char inBytes[4];
        int outInt = 1;
        FILE *fp;
        int bigendian;
/*******************************************/
/*** write number one to file as integer ***/
/*******************************************/
        sprintf( filename, "/tmp/IndianCheck_%s_%d.ThrowMeAway",
                getenv( "USER" ),
                getpid() );
                                                                                                                                                      
        if( (fp = fopen( filename, "wb" )) == NULL )
        {
                fprintf( stderr, "cannot open file %s\n", filename );
                exit(-1);
        }
        fwrite( &outInt, 4, 1, fp );
        fclose(fp);

/*******************************************/
/*** read number one from file as integer ***/
/*******************************************/
        if( (fp=fopen(filename, "rb")) == NULL )
        {
                fprintf( stderr, "cannot open file %s\n", filename );
                exit(-1);
        }
        fread( inBytes, 1, 4, fp );
        fclose(fp);
                                                                                                                                                      
/***********************/
/*** clean up        ***/
/***********************/
        remove(filename);
                                                                                                                                                      
/***********************/
/*** check the value ***/
/***********************/
        if( inBytes[0] == 0 &&
            inBytes[1] == 0 &&
            inBytes[2] == 0 &&
            inBytes[3] == 1 )
        {
                bigendian = TRUE;
        }
        else if(
            inBytes[0] == 1 &&
            inBytes[1] == 0 &&
            inBytes[2] == 0 &&
            inBytes[3] == 0 )
        {
                bigendian = FALSE;
        }
        else
        {
            fprintf( stderr, "cannot test for endian-ness, Assumes bigendian\n" );
            bigendian = TRUE;
        }

        return(bigendian);
}

int check_for_zero_trace( float *x, int nt )
{
	int i, zero_count;

	zero_count = 0;
	for( i = 0; i < nt; i++ )
	{
		if( x[i] == 0 ) zero_count++;
	}

	if( zero_count == nt )
		return 1;
	else
		return 0;
}

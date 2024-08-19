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

/******************************************************************************/
/******** reads sac files and renames them according to RDSEED format *********/
/******************************************************************************/

/******************************************/
/*** written for gene's mtinv codes *******/
/******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sac.h"

int main(int ac, char **av)
{
	Sac_Header *sp;
	float *data;
	float cmpaz, cmpinc;
	char sacfile[128], cbuf[8];
	int i, j, k;
	FILE *fp;
	
	if (ac == 1) 
	{ 
		fprintf(stderr, "renamesac: version 1 - for mtinv g.ichinose 2024\n");
		fprintf(stderr, "Usage: %s [sacfile(s)]\n", av[0]);
		exit(-1);
	}

	for( i = 1;  i < ac; i++ ) 
	{

	/*****************/
	/*** open file ***/
	/*****************/

		if ( (fp = fopen(av[i], "rb")) == NULL) 
		{
			fprintf(stderr, "%s Error Opening file %s\n", av[0], av[i]);
			exit(-1);
		}

	/***************************/
	/*** read the sac header ***/
	/***************************/

		sp = (Sac_Header *) malloc( sizeof(Sac_Header) );
		fread( sp, sizeof(Sac_Header), 1, fp);
		cmpaz  = sp->cmpaz;
		cmpinc = sp->cmpinc;

	/*********************/
	/*** read the data ***/
	/*********************/

		data = (float *) malloc(sp->npts * sizeof(float));
		fread(&data[0], sp->npts*sizeof(float), 1, fp);
		fclose(fp);

/**** fix empty kcmpnm - channel code ***/
		if( strcmp( sp->kcmpnm, "" ) == 0 ) 
		{
		  if( sp->cmpaz == 90 && sp->cmpinc == 90 )
		  {
			sprintf(sp->kcmpnm, "BHE");
		  }
		  else if( sp->cmpaz ==  0 && sp->cmpinc ==  0 )
		  {
			sprintf(sp->kcmpnm, "BHZ");
		  }
		  else if( sp->cmpaz ==  0 && sp->cmpinc == 90 )
		  {
			sprintf(sp->kcmpnm, "BHN");
		  }
		  else if( sp->cmpaz > 0   && sp->cmpaz < 135 && sp->cmpinc == 90 ) 
		  {
			sprintf(sp->kcmpnm, "BH1");
		  }
		  else if( sp->cmpaz > 135 && sp->cmpaz < 335 && sp->cmpinc == 90 ) 
		  {
			 sprintf(sp->kcmpnm, "BH2");
		  }
		  else 
		  {
			sprintf(sp->kcmpnm, "BH1");
		  }

		} /*** fix empty kcmpnm ***/

	/*** fix khole location code ***/
		if( sp->khole[0] == '\0' || strncmp( sp->khole, "-12345", 6 ) == 0 )
		{
			sp->khole[0] = '\0';
		}
		else
		{
			for( k=0; k<8; k++ )
			{
				if( sp->khole[k]  == ' ' ) 
					sp->khole[k]  = '\0';
			}
		}

		for( k=0; k<8; k++ )
		{
			if( sp->kcmpnm[k] == ' ' ) sp->kcmpnm[k] = '\0';
			if( sp->knetwk[k] == ' ' ) sp->knetwk[k] = '\0';
			if( sp->kstnm[k]  == ' ' ) sp->kstnm[k]  = '\0';
		}

	/****************************************************************/
	/***              yr  jday  hr  mn  sec  net sta kh cmp ***/
	/***              2014.098.00.00.00.0000.G.KIP.00.BHZ.R.SAC   ***/
	/****************************************************************/
	                 /*        yr  jday  hr  min  sec msec net sta kh chan R SAC */
		sprintf(sacfile, "%4d.%03d.%02d.%02d.%02d.%04d.%s.%s.%s.%s.D.SAC",
			sp->nzyear, 
			sp->nzjday, 
			sp->nzhour, 
			sp->nzmin, 
			sp->nzsec, 
			sp->nzmsec,
			sp->knetwk,
			sp->kstnm,
			sp->khole, 
			sp->kcmpnm );
		
		fprintf(stderr, "writting file %s\n", sacfile);
		sp->lovrok = TRUE;
		sp->lcalda = TRUE;

	/**********************/
        /** write the header **/
	/**********************/

		if ( (fp = fopen(sacfile, "wb")) == NULL ) 
		{
			fprintf(stdout, "Error in opening output file: %s\n", sacfile);
		}
		fwrite(sp, sizeof(Sac_Header), 1, fp);
		fwrite(&data[0], sp->npts*sizeof(float), 1, fp);

		free(data);
		free(sp);
		fclose(fp);
		
	}
	exit(0);
}

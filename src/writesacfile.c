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
                                                                                                                                        
#include "../include/mt.h"
                                                                                                                                        
extern char progname[128];

void writesacfile( EventInfo *ev )
{
        char outsacfile[256];
        FILE *fpsac;
 
	/* if( strcmp( ev[ista].wavetype, "Surf/Pnl" ) == 0 || strcmp( ev[ista].wavetype, "Rotational" ) == 0 ) */

/*** BHZ or BHW or w3 ***/
	sprintf( outsacfile, "%s.%s.%s.%s.data.sac", ev->net, ev->stnm, ev->loc, ev->z.s.kcmpnm );

        fprintf( stdout, "%s: %s: %s: insacfile=%s outsacfile=%s sta=%-4.4s net=%-2.2s cmp=%-3.3s n=%d dt=%6.4f caz=%03.0f cinc=%03.0f r=%.1f az=%.1f\n",
                progname,
		__FILE__,
		__func__,
                ev->z.filename, outsacfile, ev->z.s.kstnm, ev->z.s.knetwk, ev->z.s.kcmpnm,
                ev->z.s.npts, ev->z.s.delta, ev->z.s.cmpaz, ev->z.s.cmpinc, ev->z.s.dist, ev->z.s.az );
        fpsac = fopen( outsacfile, "wb" );
        fwrite( &ev->z.s, sizeof(Sac_Header), 1, fpsac );
        fwrite( &ev->z.data[0], ev->z.s.npts*sizeof(float), 1, fpsac );
        fclose(fpsac);
                      
/*** BHN or BH1 or BHV or w2 ***/
        sprintf( outsacfile, "%s.%s.%s.%s.data.sac", ev->net, ev->stnm, ev->loc, ev->ns.s.kcmpnm );

        fprintf( stdout, "%s: %s: %s: insacfile=%s outsacfile=%s sta=%-4.4s net=%-2.2s cmp=%-3.3s n=%d dt=%6.4f caz=%03.0f cinc=%03.0f r=%.1f az=%.1f\n",
                progname,
		__FILE__,
                __func__,
                ev->ns.filename, outsacfile, ev->ns.s.kstnm, ev->ns.s.knetwk, ev->ns.s.kcmpnm,
                ev->ns.s.npts, ev->ns.s.delta, ev->ns.s.cmpaz, ev->ns.s.cmpinc, ev->ns.s.dist, ev->ns.s.az );

        fpsac = fopen( outsacfile, "wb" );
        fwrite( &ev->ns.s, sizeof(Sac_Header), 1, fpsac );
        fwrite( &ev->ns.data[0], ev->ns.s.npts*sizeof(float), 1, fpsac );
        fclose(fpsac);
         
/*** BHE or BH2 or BHU or w1 ***/                                                                                                                               
	sprintf( outsacfile, "%s.%s.%s.%s.data.sac", ev->net, ev->stnm, ev->loc, ev->ew.s.kcmpnm );

        fprintf( stdout, "%s: %s: %s: insacfile=%s outsacfile=%s sta=%-4.4s net=%-2.2s cmp=%-3.3s n=%d dt=%6.4f caz=%03.0f cinc=%03.0f r=%.1f az=%.1f\n",
                progname,
		__FILE__,
                __func__,
                ev->ew.filename, outsacfile, ev->ew.s.kstnm, ev->ew.s.knetwk, ev->ew.s.kcmpnm,
                ev->ew.s.npts, ev->ew.s.delta, ev->ew.s.cmpaz, ev->ew.s.cmpinc, ev->ew.s.dist, ev->ew.s.az );
                                                                                                                                        
        fpsac = fopen( outsacfile, "wb" );
        fwrite( &ev->ew.s, sizeof(Sac_Header), 1, fpsac );
        fwrite( &ev->ew.data[0], ev->ew.s.npts*sizeof(float), 1, fpsac );
        fclose(fpsac);
}

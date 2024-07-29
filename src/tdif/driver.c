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
#include <math.h>
#include "../../include/mt.h"
                                                                                                                         
int main( int ac, char **av )
{
        Sac_File sacfile;
        int verbose=1;
        int operator=2;
        FILE *fp;
        void differentiate( float *, int, float, int );
                                                                                                                         
        setpar(ac,av);
        mstpar( "f", "s", &(sacfile.filename) );
        getpar( "verbose", "b", &verbose );
        getpar( "op", "d", &operator );
        endpar();
                                                                                                                         
        fp = fopen( sacfile.filename, "rb" );
        fread( &(sacfile.s), sizeof(Sac_Header), 1, fp );
        sacfile.data = (float *)calloc( sacfile.s.npts, sizeof(float));
        fread( &sacfile.data[0], sacfile.s.npts*sizeof(float), 1, fp );
        fclose(fp);
                                                                                                                         
        differentiate( &(sacfile.data[0]), sacfile.s.npts, sacfile.s.delta, operator );
                                                                                                                         
        fp = fopen( "out.sac", "wb" );
        fwrite( &(sacfile.s), sizeof(Sac_Header), 1, fp );
        fwrite( &(sacfile.data[0]), sacfile.s.npts*sizeof(float), 1, fp );
        fclose(fp);
                                                                                                                         
        free(sacfile.data);
        exit(0);
}

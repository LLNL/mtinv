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

/********************************************************************************
        Fft.c
        1-d fast Fourier transform subroutine.
        From Claerbout (1985) p. 70.
        Usage:          int lx;
                        float signi, scale;
                        struct complex cx[];
                        fft(lx, cx, signi, scale);
        Compile:        cc prog.c fft.c -fswitch -lm -o prog.c
        Arguments:      lx      number of elements of vector to transform
                                MUST BE A POWER OF 2
                        cx      pointer to vector of complex structures
                        signi   sign of transform- +1 forward to Fourier domain
                                                   -1 inverse to real domain
                        scale   scale factor to apply to data before tranform
************************************************************************************/
void fft();
int power2();
void zero_pad();

/* Define complex multiplication and its conjugate */
#define  rmul(x,y)      (((x.re)*(y.re))-((x.im)*(y.im)))
#define  imul(x,y)      (((x.im)*(y.re))+((x.re)*(y.im)))
#define rcmul(x,y)      (((x.re)*(y.re))+((x.im)*(y.im)))
#define icmul(x,y)      (((x.im)*(y.re))-((x.re)*(y.im)))
#define congj(x)        (((x.re)*(x.re))+((x.im)*(x.im)))
#define FORWARD 1.0
#define INVERSE -1.0

/* Declare the structure to hold complex numbers */
struct complex
{
	float re;
	float im;
};


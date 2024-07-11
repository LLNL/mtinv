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


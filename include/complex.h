#define FORWARD +1.0
#define INVERSE -1.0

typedef struct {
	double re;
	double im;
} Dcomplex;

typedef struct {
	float re;
	float im;
} Complex;

float c_abs(Complex);
Complex c_cos(Complex);
Complex c_div(Complex,Complex);
Complex c_mult(Complex,Complex);
Complex c_add(Complex,Complex);
Complex c_sub(Complex,Complex);
Complex c_exp(Complex);
Complex c_log(Complex);
Complex c_sin(Complex);
Complex c_sqrt(Complex);
Complex c_scale(Complex, float);
/* float c_conjg(Complex); */
Complex c_conjg( Complex a );
float c_real(Complex);
float c_imag(Complex);


#ifndef __XDRA_FFT_H
#define __XDRA_FFT_H
#include "xdra_util.h"

typedef float real_t;
typedef real_t complex_t[2];
#define RE(A) A[0]
#define IM(A) A[1]

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define FRAC_CONST(A) ((real_t)(A)) 
#define MUL_R(A,B) ((A)*(B))
#define MUL_C(A,B) ((A)*(B))
#define MUL_F(A,B) ((A)*(B))

static inline void ComplexMult(real_t *y1, real_t *y2, real_t x1, real_t x2, real_t c1, real_t c2)
{
	*y1 = MUL_F(x1, c1) + MUL_F(x2, c2);
	*y2 = MUL_F(x2, c1) - MUL_F(x1, c2);
}

typedef struct tagxdra_fft
{
    uint16_t n;
    uint16_t ifac[15];
    complex_t *work;
    complex_t *tab;
} xdra_fft;


void xdra_fft_process(xdra_fft *fft, complex_t *c);
xdra_fft *xdra_fft_init(uint16_t n);
void xdra_fft_release(xdra_fft *fft);

#endif

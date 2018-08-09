#ifndef __XDRA_MDCT_H
#define __XDRA_MDCT_H
#include "xdra_util.h"
void xdra_imct_orginal_c(float *in, float *out, int M);
void * xdra_mdct_create(int N);
void xdra_mdct_imdct(void *mdct, float *in, float *out);
void xdra_mdct_destroy(void *mdct);

#endif

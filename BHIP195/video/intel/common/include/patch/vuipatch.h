#ifndef __VUIPATCH_H__
#define __VUIPATCH_H__

extern "C" {
unsigned char* EsPatch(unsigned char *pData, unsigned int nEsLen, unsigned int *nOutLen);
}

#endif

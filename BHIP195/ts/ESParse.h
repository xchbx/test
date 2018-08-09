#ifndef __ES_PARSE_H__
#define __ES_PARSE_H__

#include <stdio.h>
#include <stdlib.h>    
#include <string.h>
#include <assert.h>

class CESParse 
{
public:
	CESParse(); 
	~CESParse() { }

    unsigned int ParsePes(unsigned char *pInData, unsigned int nDataLen,
        unsigned char* pOut, unsigned long long *pOutPts);

    void FindResolution_H265(unsigned char *pInData, unsigned int nDataLen, int *ipWidth, int *ipHeight) ;
    void Getx265SliceCount(unsigned char *pInData, unsigned int nDataLen, int *ICnt, int *PCnt, int *BCnt);

private:
    void GetPts(unsigned char *pInData, unsigned nDataLen, unsigned long long *ipPts);

	unsigned int ReadBit();
	unsigned int ReadBits(int n);
	unsigned int ReadExponentialGolombCode();
	unsigned int ReadSE();

	bool ParseSPS(const unsigned char * pStart, unsigned int nLen, int *ipWidth, int *ipHeight);
    bool ParsePPS(const unsigned char * pStart, unsigned int nLen);

    bool GetSliceCount(const unsigned char * pStart, unsigned int nLen, int *ICnt, int *PCnt, int *BCnt);

    void profile_tier_level(int max_sub_layers_minus1);

private:
	const unsigned char * m_pStart;
	unsigned int m_uLength;
	int m_nCurrentBit;

    bool m_bFindPPS;
    unsigned char dependent_slice_segments_enabled_flag;
    int num_extra_slice_header_bits;
};

#endif
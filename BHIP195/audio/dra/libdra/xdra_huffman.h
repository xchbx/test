#ifndef __XDRA_HUFFMAN_H
#define __XDRA_HUFFMAN_H
#include "xdra_util.h"


#define XDRA_S_BIT      9
#define XDRA_S_BIT_MASK 511
typedef struct tagxdra_huffman_codebook
{
	int index;
	int num_codes;
	int dim;
	int size;
	int l0, l1;
	const int16_t *   l0_items;
	const int16_t **  l1_items;
}xdra_huffman_codebook;

extern xdra_huffman_codebook g_huffman_codebook1;
extern xdra_huffman_codebook g_huffman_codebook2;
extern xdra_huffman_codebook g_huffman_codebook3;
extern xdra_huffman_codebook g_huffman_codebook4;
extern xdra_huffman_codebook g_huffman_codebook5;
extern xdra_huffman_codebook g_huffman_codebook6;
extern xdra_huffman_codebook g_huffman_codebook7;
extern xdra_huffman_codebook g_huffman_codebook8;
extern xdra_huffman_codebook g_huffman_codebook9;
extern xdra_huffman_codebook *g_huffman_codebook_l_tables[];
extern xdra_huffman_codebook *g_huffman_codebook_s_tables[];

int xdra_huffman_decode(xdra_bitstream *bs, xdra_huffman_codebook *book);
int xdra_huffman_decode_diff(xdra_bitstream *bs, xdra_huffman_codebook *book, int index);
int xdra_huffman_decode_recursive(xdra_bitstream *bs, xdra_huffman_codebook *book);

#endif


#include  <string.h>
#include "xdra_huffman.h"
#include "math.h"


#include "xdra_huffman_table.hpp"


xdra_huffman_codebook *g_huffman_codebook_l_tables[] = 
{
	&g_huffman_codebook10, &g_huffman_codebook11, &g_huffman_codebook12, &g_huffman_codebook13, &g_huffman_codebook14,
	&g_huffman_codebook15, &g_huffman_codebook16, &g_huffman_codebook17, &g_huffman_codebook18

};

xdra_huffman_codebook *g_huffman_codebook_s_tables[] = 
{
	&g_huffman_codebook19, &g_huffman_codebook20, &g_huffman_codebook21, &g_huffman_codebook22, &g_huffman_codebook23,
	&g_huffman_codebook24, &g_huffman_codebook25, &g_huffman_codebook26, &g_huffman_codebook27

};

int xdra_huffman_decode(xdra_bitstream *bs, xdra_huffman_codebook *book)
{
	int index0 = xdra_bitstream_show_bits(bs, book->l0);
	int result = book->l0_items[index0];
	if(result == -1)
	{
		return -1;
	}
	else if(result < -1)
	{
		xdra_bitstream_clear_bits(bs, book->l0);
		int index1 = xdra_bitstream_show_bits(bs, book->l1);
		int t = result + 2;
		result = book->l1_items[-t][index1];	
		if(result == -1)
		{
			return -1;
		}
	}
	xdra_bitstream_clear_bits(bs, result >> XDRA_S_BIT);
	result = result & XDRA_S_BIT_MASK;
	return result;
}




int xdra_huffman_decode_recursive(xdra_bitstream *bs, xdra_huffman_codebook *book)
{
	int k = -1;
	int index = -1;
	do
	{
		k++;
		index = xdra_huffman_decode(bs, book);
		if(index < 0)
		{
			return -1;
		}
	}
	while(index == book->size - 1);
	index =  k * (book->size - 1) + index;
	return index;
}

int xdra_huffman_decode_diff(xdra_bitstream *bs, xdra_huffman_codebook *book, int index)
{
	int diff = xdra_huffman_decode(bs, book);
	if(diff < 0)
	{
		return -1;
	}
	return (index + diff) % book->size;
}


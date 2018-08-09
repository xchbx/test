#include "xdra_util.h"
#include  <string.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
const uint32_t g_bs_mask[33] ={0x00,
                                  0x01,      0x03,      0x07,      0x0f,
                                  0x1f,      0x3f,      0x7f,      0xff,
                                  0x1ff,     0x3ff,     0x7ff,     0xfff,
                                  0x1fff,    0x3fff,    0x7fff,    0xffff,
                                  0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
                                  0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
                                  0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
                                  0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};

int g_sample_rate_table[16] =
{
	8000,     12000,  11025,   16000,
	22100,    24000,  32000,   44100,
	48000,    88200,  96000,   1764000,
	192000,
};


int xdra_get_sample_rate(int index)
{
	if(index < 0 || index > 12)
	{
		return 0;
	}
	return g_sample_rate_table[index];
}
int xdra_parse_header_bs(xdra_bitstream *bs, xdra_frame_header *header)
{
	if(header == NULL)
	{
		return -1;
	}
	int type1;
	memset(header, 0, sizeof(*header));
	type1 = xdra_bitstream_get_bit1(bs);
	header->frame_header_type = type1;
	header->num_word = xdra_bitstream_get_bits(bs, type1 == 0 ? 10 : 13);
	header->num_block_per_frame = 1 << xdra_bitstream_get_bits(bs, 2);
	header->sample_rate_index = xdra_bitstream_get_bits(bs, 4);
	header->num_normal_channel = xdra_bitstream_get_bits(bs, type1 == 0  ? 3 : 6) + 1;
	header->num_lfe_channel = xdra_bitstream_get_bits(bs, type1 == 0  ? 1 : 2);
	header->aux_data = xdra_bitstream_get_bit1(bs);
	if(type1 == 0 && header->num_normal_channel > 1)
	{
		header->use_sum_diff = xdra_bitstream_get_bit1(bs);
		header->use_jic = xdra_bitstream_get_bit1(bs);
	}
	if(header->use_jic)
	{
		header->jic_cb = xdra_bitstream_get_bits(bs, 5) + 1;
	}
	int sample_rate = g_sample_rate_table[header->sample_rate_index];
	if(header->sample_rate_index > 12)
	{
		return -1;
	}

	if(header->num_word < 1)
	{
		return -1;
	}

	return 0;
}

int xdra_parse_header(unsigned char *buf,  int len,  xdra_frame_header *header)
{
	xdra_bitstream bs ;
	if(buf == NULL || len < 6)
	{
		return -1;
	}
	if(buf[0] != 0x7F || buf[1] != 0xFF)
	{
		return -1;
	}
	xdra_bitstream_init(&bs, buf + 2, len - 2);
	return xdra_parse_header_bs(&bs, header);
}

#define CACHE_LINE        32
#define CACHE_LINE_MASK   31



void *xdra_malloc( int i_size )
{

    uint8_t * buf;
    uint8_t * align_buf;
    buf = (uint8_t *) malloc( i_size + CACHE_LINE_MASK + sizeof( void ** ) +
              sizeof( int ) );
    align_buf = buf + CACHE_LINE_MASK + sizeof( void ** ) + sizeof( int );
    align_buf -= (long) align_buf & CACHE_LINE_MASK;
    *( (void **) ( align_buf - sizeof( void ** ) ) ) = buf;
    *( (int *) ( align_buf - sizeof( void ** ) - sizeof( int ) ) ) = i_size;
    return align_buf;

}



void xdra_free( void *p )
{
    if( p )
    {
        free( *( ( ( void **) p ) - 1 ) );
    }
}


void *xdra_realloc( void *p, int i_size )
{

    int       i_old_size = 0;
    uint8_t * p_new;
    if( p )
    {
        i_old_size = *( (int*) ( (uint8_t*) p  - sizeof( void ** ) -
                         sizeof( int ) ) );
    }
    p_new = (uint8_t *)xdra_malloc( i_size );
    if( i_old_size > 0 && i_size > 0 )
    {
        memcpy( p_new, p, ( i_old_size < i_size ) ? i_old_size : i_size );
    }
    xdra_free( p );
    return p_new;

}

int xdra_next_sync_word(uint8_t *buf, int len)
{
	for(int i = 0; i < len - 1; i++)
	{
		if( ( (buf[i] << 0x8) | buf[i + 1]) == 0x7FFF)
		{
			return i;
		}
	}
	return -1;
}

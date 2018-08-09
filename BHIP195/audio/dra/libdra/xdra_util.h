#ifndef __XDRA_UTIL_H
#define __XDRA_UTIL_H

typedef signed char int8_t;
typedef unsigned char  uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned  int uint32_t;

void *xdra_malloc( int i_size );
void *xdra_realloc( void *p, int i_size );
void xdra_free( void *p );

typedef struct tagxdra_bitstream
{
    uint8_t *p_start;/*流开始*/
    uint8_t *p;/*当前数据指针*/
    uint8_t *p_end;/*流结束*/
    int      i_left;    /* 能够得到的bit数*/
}xdra_bitstream;



static inline void xdra_bitstream_init( xdra_bitstream *s, void *p_data, int i_data )
{
    s->p_start = (uint8_t *)p_data;
    s->p       = (uint8_t *)p_data;
    s->p_end   = s->p + i_data;
    s->i_left  = 8;
}


static inline int xdra_bitstream_eof( xdra_bitstream *s )
{
//	return( s->p >= s->p_end -1? 1: 0 );
    return( s->p >= s->p_end ? 1: 0 );
}

static inline void xdra_bitstream_next_byte( xdra_bitstream *s)
{
	s->p++;
	s->i_left = 8;
}

extern const uint32_t g_bs_mask[33];
static inline uint32_t xdra_bitstream_get_bits( xdra_bitstream *s, int i_count )
{
    
    int      i_shr;
    uint32_t i_result = 0;

    while( i_count > 0 )
    {
        if( s->p >= s->p_end )
        {
            break;
        }
        if( ( i_shr = s->i_left - i_count ) >= 0 )
        {
            i_result |= ( *s->p >> i_shr )&g_bs_mask[i_count];
            s->i_left -= i_count;
            if( s->i_left == 0 )
            {
                xdra_bitstream_next_byte(s);
            }
            return( i_result );
        }
        else
        {//不够则取下一个字节处理直到取够为止
           i_result |= (*s->p&g_bs_mask[s->i_left]) << -i_shr;
           i_count  -= s->i_left;
		   xdra_bitstream_next_byte(s);
       }
    }

    return( i_result );
}

static inline int32_t xdra_bitstream_show_bits( xdra_bitstream *s, int i_count )
{
	int      m = 0;
	int      i_shr;
    uint32_t i_result = 0;
	int left  = s->i_left;
    while( i_count > 0 )
    {
        if( (s->p + m) >= s->p_end )
        {
            break;
        }
        if( ( i_shr = left - i_count ) >= 0 )
        {
            i_result |= ( *(s->p + m) >> i_shr )&g_bs_mask[i_count];
            left -= i_count;
            if( left == 0 )
            {
               m++;
			   left = 8;
            }
            return( i_result );
        }
        else
        {//不够则取下一个字节处理直到取够为止
           i_result |= (*(s->p + m) & g_bs_mask[left]) << -i_shr;
           i_count  -= left;
		   m++;
		   left = 8;
       }
    }
    return( i_result );
}


static inline uint32_t xdra_bitstream_get_bit1( xdra_bitstream *s )
{

    if( s->p < s->p_end )
    {
        uint32_t i_result;

        s->i_left--;
        i_result = ( *s->p >> s->i_left )&0x01;
        if( s->i_left == 0 )
        {
			xdra_bitstream_next_byte(s);  
        }
        return i_result;
    }

    return 0;
}



static inline int32_t xdra_bitstream_get_int( xdra_bitstream *s, int i_count )
{

    uint32_t i_temp = 1 << (i_count-1);
	uint32_t i_result = xdra_bitstream_get_bits(s,i_count);
	if(i_result&i_temp)
	{
		//负数为补码
		return 0-(int)((~i_result+1)<<( 32 - i_count)>>(32 - i_count));

	}
	else
	{
		return i_result;
	}
}

static inline void xdra_bitstream_clear_bits( xdra_bitstream *s, int i_count )
{
/*  s->i_left -= i_count;

    while( s->i_left <= 0 )
    {
        s->p++;
        s->i_left += 8;
    }*/
	while( i_count > 0 )
    {
        if( s->p >= s->p_end )
        {
            break;
        }
        if( (  s->i_left - i_count ) >= 0 )
        {
            s->i_left -= i_count;
            if( s->i_left == 0 )
            {
				xdra_bitstream_next_byte(s);
            }
            break;
        }
        else
        {  
           i_count  -= s->i_left;
		   xdra_bitstream_next_byte(s);
       }
    }


}



#include "xdra_decoder.h"

#define XDRA_MAX_CLUSTER_NUM 4
#define XDRA_MAX_BLOCK_NUM   8
#define XDRA_MAX_BAND_NUM    32
#define XDRA_FACTOR_NUM    1024
#define XDRA_MAX_CHANNEL_NUM 8
#define XDRA_MAX(__x, __y)  (__x > __y ? __x : __y)
#define XDRA_MIN(__x, __y)  (__x > __y ? __y : __x)

#define XDRA_CLIP(sample, max, min) \
if (sample > max)   sample = max;  \
else if (sample < min) sample = min;          





#define XDRA_CLIP_32768(x)  XDRA_CLIP(x ,32767.0f, -32768.0f)

#define WIN_LONG_LONG2LONG       0
#define WIN_LONG_LONG2SHORT      1
#define WIN_LONG_SHORT2LONG      2
#define WIN_LONG_SHORT2SHORT     3
#define WIN_LONG_LONG2BRIEF      4
#define WIN_LONG_BRIEF2LONG      5
#define WIN_LONG_BRIEF2BRIEF     6
#define WIN_LONG_SHORT2BRIEF     7
#define WIN_LONG_BRIEF2SHORT     8
#define WIN_SHORT_SHORT2SHORT    9
#define WIN_SHORT_SHORT2BRIEF    10
#define WIN_SHORT_BRIEF2BRIEF    11
#define WIN_SHORT_BRIEF2SHORT    12

#define IS_SHORT_WIN(__type) (__type > WIN_LONG_BRIEF2SHORT)


int xdra_parse_header_bs(xdra_bitstream *bs, xdra_frame_header *header);




#endif

/*
 * Chinese AVS video (AVS1-P2, JiZhun profile) decoder.
 * Copyright (c) 2006  Stefan Gehrer <stefan.gehrer@gmx.de>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/*************************************************************
chengdu WP 20070526 - 20070730
secondwang@163.com
xavs解码器2.0版本

**************************************************************/

#ifndef _XAVS_H
#define _XAVS_H

#include  <stdio.h>
#include  <math.h>
#include  <stdlib.h>
typedef signed char int8_t;
typedef unsigned char  uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned  int uint32_t;


#if defined(__CONFIG_ANDROID) || defined(__CONFIG_LINUX)
#ifndef __x86_64__
typedef long long  int64_t;
#endif
#include "stdint.h"
//typedef unsigned long unsigned int uint64_t;
typedef unsigned char BYTE;
//typedef unsigned long long ULONGLONG;

#else
typedef __int64  int64_t;
typedef unsigned __int64 uint64_t;
#endif


typedef short DCTELEM;
#define unaligned16(a) (*(const uint16_t*)(a))
#define unaligned32(a) (*(const uint32_t*)(a))
#define unaligned64(a) (*(const uint64_t*)(a))

#define XAVS_EDGE    16
#define XAVS_MB_SIZE  16
#define XAVS_I_PICTURE 0
#define XAVS_P_PICTURE 1
#define XAVS_B_PICTURE 2  
#define XAVS_CS_YUV420 0
#define MAX_NEG_CROP 384

#define XAVS_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define XAVS_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define XAVS_MIN3(a,b,c) XAVS_MIN((a),XAVS_MIN((b),(c)))
#define XAVS_MAX3(a,b,c) XAVS_MAX((a),XAVS_MAX((b),(c)))
#define XAVS_MIN4(a,b,c,d) XAVS_MIN((a),XAVS_MIN3((b),(c),(d)))
#define XAVS_MAX4(a,b,c,d) XAVS_MAX((a),XAVS_MAX3((b),(c),(d)))
#define XAVS_XCHG(type,a,b) { type t = a; a = b; b = t; }
#define XAVS_FIX8(f) ((int)(f*(1<<8)+.5))
#define XAVS_SAFE_FREE(p) if(p){xavs_free(p);p=0;}
#define LEFT_ADAPT_INDEX_L 0
#define TOP_ADAPT_INDEX_L  1
#define LEFT_ADAPT_INDEX_C 2
#define TOP_ADAPT_INDEX_C  3

enum xavs_intra_lum_pred_mode {
     INTRA_L_VERT=0,
     INTRA_L_HORIZ,
     INTRA_L_LP,
     INTRA_L_DOWN_LEFT,
     INTRA_L_DOWN_RIGHT,
     INTRA_L_LP_LEFT,
     NTRA_L_LP_TOP,
     INTRA_L_DC_128
};
enum xavs_intra_chrom_pred_mode {
     INTRA_C_LP=0,
     INTRA_C_HORIZ,
     INTRA_C_VERT,
     INTRA_C_PLANE,
     INTRA_C_LP_LEFT,
     INTRA_C_LP_TOP,
     INTRA_C_DC_128,
};

#define ESCAPE_CODE                   59
#define FWD0                          0x01
#define FWD1                          0x02
#define BWD0                          0x04
#define BWD1                          0x08
#define SYM0                          0x10
#define SYM1                          0x20
#define SPLITH                        0x40
#define SPLITV                        0x80


#define MV_BWD_OFFS                     12
#define MV_STRIDE                        4


enum xavs_mb_type 
{
     I_8X8 = 0,
     P_SKIP,
     P_16X16,
     P_16X8,
     P_8X16,
     P_8X8,
     B_SKIP,
     B_DIRECT,
     B_FWD_16X16,
     B_BWD_16X16,
     B_SYM_16X16,
//
     B_FWD_FWD_16X8,
	 B_FWD_FWD_8X16,
	 B_BWD_BWD_16X8,
	 B_BWD_BWD_8X16,
	 B_FWD_BWD_16X8,
	 B_FWD_BWD_8X16,
	 B_BWD_FWD_16X8,
	 B_BWD_FWD_8X16,
	 B_FWD_SYM_16X8,
	 B_FWD_SYM_8X16,
	 B_BWD_SYM_16X8,
	 B_BWD_SYM_8X16,
	 B_SYM_FWD_16X8,
	 B_SYM_FWD_8X16,
	 B_SYM_BWD_16X8,
	 B_SYM_BWD_8X16,
	 B_SYM_SYM_16X8,
	 B_SYM_SYM_8X16,

//

     B_8X8 = 29
};

enum xavs_mb_sub_type {
     B_SUB_DIRECT,
     B_SUB_FWD,
     B_SUB_BWD,
     B_SUB_SYM
};


enum xavs_block_size{
   BLK_16X16,
   BLK_16X8,
   BLK_8X16,
   BLK_8X8
};
enum xavs_mv_intra_mode_location {
INTRA_MODE_D3 =0,
INTRA_MODE_B2,
INTRA_MODE_B3,
INTRA_MODE_A1,
INTRA_MODE_X0,
INTRA_MODE_X1,
INTRA_MODE_A3,
INTRA_MODE_X2,
INTRA_MODE_X3
};

enum xavs_mv_pred {
MV_PRED_MEDIAN,
MV_PRED_LEFT,
MV_PRED_TOP,
MV_PRED_TOPRIGHT,
MV_PRED_PSKIP,
MV_PRED_BSKIP
};

enum xavs_mv_block_location {
MV_FWD_D3 = 0,
MV_FWD_B2,
MV_FWD_B3,
MV_FWD_C2,
MV_FWD_A1,
MV_FWD_X0,
MV_FWD_X1,
MV_FWD_A3 = 8,
MV_FWD_X2,
MV_FWD_X3,
MV_BWD_D3 = MV_BWD_OFFS,
MV_BWD_B2,
MV_BWD_B3,
MV_BWD_C2,
MV_BWD_A1,
MV_BWD_X0,
MV_BWD_X1,
MV_BWD_A3 = MV_BWD_OFFS+8,
MV_BWD_X2,
MV_BWD_X3
};



#define A_AVAIL                          1
#define B_AVAIL                          2
#define C_AVAIL                          4
#define D_AVAIL                          8
#define NOT_AVAIL                       -1
#define REF_INTRA                       -2
#define REF_DIR                         -3


#define XAVS_SEQUENCE_DISPLAY_EXTENTION     0x00000002
#define XAVS_COPYRIGHT_EXTENTION            0x00000004
#define XAVS_PICTURE_DISPLAY_EXTENTION      0x00000007
#define XAVS_CAMERA_PARAMETERS_EXTENTION    0x0000000B
#define XAVS_SLICE_MIN_START_CODE           0x00000100
#define XAVS_SLICE_MAX_START_CODE           0x000001AF
#define XAVS_VIDEO_SEQUENCE_START_CODE      0x000001B0
#define XAVS_VIDEO_SEQUENCE_END_CODE        0x000001B1
#define XAVS_USER_DATA_CODE                 0x000001B2
#define XAVS_I_PICUTRE_START_CODE           0x000001B3
#define XAVS_EXTENSION_START_CODE           0x000001B5
#define XAVS_PB_PICUTRE_START_CODE          0x000001B6
#define XAVS_VIDEO_EDIT_CODE                0x000001B7
#define XAVS_VIDEO_TIME_CODE                0x000001E0


typedef struct tagxavs_video_sequence_header
{
	
	uint8_t  i_profile_id;/*8bits u(8)*/
	uint8_t  i_level_id;/*8bits u(8)*/
	uint8_t  b_progressive_sequence;/*1bit u(1)*/
	uint32_t i_horizontal_size;/*14bits u(14)*/
	uint32_t i_vertical_size;/*14bits u(14)*/
	
	uint8_t  i_chroma_format;/*2bits u(2)*/
	uint8_t  i_sample_precision;/*3bits u(3)*/
	uint8_t  i_aspect_ratio;/*4bits u(4)*/
	uint8_t  i_frame_rate_code;/*4bits u(4)*/
	uint32_t i_bit_rate;
	/*30bits u(30) 分两部分获得
	 18bits bit_rate_lower;
	 12bits bit_rate_upper。
	其间有1bit的marker_bit防止起始码的竞争*/
	uint8_t  b_low_delay;/*1bit u(1)*/
	//这里有1bit的marker_bit
	uint32_t i_bbv_buffer_size;;/*18bits u(18)*/
	//还有3bits的reserved_bits，固定为'000' 
}xavs_video_sequence_header;

typedef struct tagxavs_picture_header
{
	uint16_t i_bbv_delay;/*16bits u(16)*/
	uint16_t i_bbv_delay_extension;/*7bits u(7)*/

	uint8_t  i_picture_coding_type;/*2bits u(2) pb_piture_header专有*/
	uint8_t  b_time_code_flag;/*1bit u(1) i_piture_header专有*/
	uint32_t i_time_code;/*24bits u(24) i_piture_header专有*/
	uint8_t  i_picture_distance;/*8bits u(8)*/
	uint32_t i_bbv_check_times;/*ue(v)*/
	uint8_t  b_progressive_frame;/*1bit u(1)*/
	uint8_t  b_picture_structure;/*1bit u(1)*/
	uint8_t  b_advanced_pred_mode_disable;/*1bit u(1) pb_piture_header专有*/
	uint8_t  b_top_field_first;/*1bit u(1)*/
	uint8_t  b_repeat_first_field;/*1bit u(1)*/
	uint8_t  b_fixed_picture_qp;/*1bit u(1)*/
	uint8_t  i_picture_qp;/*6bits u(6)*/
	uint8_t  b_picture_reference_flag;/*1bit u(1) pb_piture_header专有*/
	//4bits保留字节reserved_bits固定为'0000'
	uint8_t  b_no_forward_reference_flag;
	uint8_t  b_pb_field_enhanced_flag;

	uint8_t  b_skip_mode_flag;/*1bit u(1)*/
	uint8_t  b_loop_filter_disable;/*1bit u(1)*/
	uint8_t  b_loop_filter_parameter_flag;/*1bit u(1)*/
	uint32_t  i_alpha_c_offset;/*se(v)*/
    uint32_t  i_beta_offset;/*se(v)*/

	uint8_t  b_weighting_quant_flag;//u(1)
	uint8_t  b_chroma_quant_param_disable;//u(1)
	int      i_chroma_quant_param_delta_cb;//se
	int      i_chroma_quant_param_delta_cr;//se
	uint32_t i_weighting_quant_param_index;//u(2)
	uint32_t i_weighting_quant_model;//u(2)
	int      i_weighting_quant_param_delta1[6];//se
	int      i_weighting_quant_param_delta2[6];//se
	uint8_t  b_aec_enable;//u(1)

	

}xavs_picture_header;

typedef struct tagxavs_slice_header
{
	uint8_t  i_slice_vertical_position_extension;/*3bits u(3)*/
	uint8_t  b_fixed_slice_qp;/*1bit u(1)*/
	uint8_t  i_slice_qp;/*6bits u(6)*/
	uint8_t  b_slice_weighting_flag;/*1bit u(1)*/
	uint32_t i_luma_scale[4];/*8bits u(8)*/
	int32_t  i_luma_shift[4];/*8bits i(8)*/
	uint32_t i_chroma_scale[4];/*8bits u(8)*/
	int32_t  i_chroma_shift[4];/*8bits i(8)*/
	uint8_t  b_mb_weighting_flag;/*1bit u(1)*/
	uint32_t i_mb_skip_run;/*ue(v)*/

	uint8_t  i_number_of_reference;

}xavs_slice_header;

typedef struct tagxavs_sequence_display_extension
{
	uint8_t  i_video_format;/*3bits u(3)*/
	uint8_t  b_sample_range;/*1bit u(1)*/
	uint8_t  b_colour_description;/*1bit u(1)*/
	uint8_t  i_colour_primaries;/*8bits u(8)*/
	uint8_t  i_transfer_characteristics;/*8bits u(8)*/
	uint8_t  i_matrix_coefficients;/*8bits u(8)*/
	uint32_t i_display_horizontal_size;/*14bits u(14)*/
	uint32_t i_display_vertical_size;/*14bits u(14)*/
	//还有2bits的reserved_bits，
}xavs_sequence_display_extension;

typedef struct tagxavs_copyright_extension
{
	uint8_t  b_copyright_flag;/*1bit u(1)*/
	uint8_t  i_copyright_id;/*8bits u(8)*/
	uint8_t  b_original_or_copy;/*1bit u(1)*/
	//这里有保留7bits的reserved_bits
	uint64_t i_copyright_number;
	/*64bits u(64)分三个部分出现，
	20bits copyright_number_1;
	22bits copyright_number_2;
	22bits copyright_number_3
	其间分别嵌入2个1bit的marker_bit*/
}xavs_copyright_extension;

typedef struct tagxavs_camera_parameters_extension
{
	//1bit的reserved_bits
	uint8_t  i_camera_id;/*7bits u(7)*/
	//这里有1bit的marker_bit
	uint32_t i_height_of_image_device;/*22bits u(22)*/
	//这里有1bit的marker_bit
	uint32_t i_focal_length;/*22bits u(22)*/
	//这里有1bit的marker_bit
	uint32_t i_f_number;/*22bits u(22)*/
	//这里有1bit的marker_bit
	uint32_t i_vertical_angle_of_view;/*22bits u(22)*/
	//这里有1bit的marker_bit
	int32_t  i_camera_position_x;
	/*32bits i(32) 分两部分获得
	16bits camera_position_x_upper;
	16bits camera_position_x_lower;
    其间有1bit的marker_bit防止起始码的竞争*/
	int32_t  i_camera_position_y;
	/*32bits i(32) 分两部分获得
	16bits camera_position_x_upper;
	16bits camera_position_x_lower;
    其间有1bit的marker_bit防止起始码的竞争*/
	int32_t  i_camera_position_z;
	/*32bits i(32) 分两部分获得
	16bits camera_position_x_upper;
	16bits camera_position_x_lower;
    其间有1bit的marker_bit防止起始码的竞争*/
	//这里有1bit的marker_bit
	int32_t i_camera_direction_x;/*22bits i(22)*/
	//这里有1bit的marker_bit
	int32_t i_camera_direction_y;/*22bits i(22)*/
	//这里有1bit的marker_bit
	int32_t i_camera_direction_z;/*22bits i(22)*/

	//这里有1bit的marker_bit
	int32_t i_image_plane_vertical_x;/*22bits i(22)*/
	//这里有1bit的marker_bit
	int32_t i_image_plane_vertical_y;/*22bits i(22)*/
	//这里有1bit的marker_bit
	int32_t i_image_plane_vertical_z;/*22bits i(22)*/
	//这里有1bit的marker_bit
	//还有32bits的reserved_bits，
}xavs_camera_parameters_extension;

typedef struct tagxavs_picture_display_extension
{
	uint32_t i_number_of_frame_centre_offsets;
	/*元素个数，由语法派生获得xavs_sequence_display_extension，不占用bits*/
	uint32_t i_frame_centre_horizontal_offset[4];/*16bits i(16)*/
	uint32_t i_frame_centre_vertical_offset[4];/*16bits i(16)*/
}xavs_picture_display_extension;
 typedef struct tagxavs_vector
{
     int16_t x;
     int16_t y;
     int16_t dist;
     int16_t ref;
} xavs_vector;

 typedef struct tagxavs_mvref
{
	int8_t  dir;
	int8_t  type;   
	int16_t ref;
}xavs_mvref;

 typedef struct tagxavs_mvd
{
	 int8_t enable;
	 int8_t dir;
     int16_t x;
     int16_t y;
	 
}xavs_mvd;
 
typedef struct tagxavs_vlc {
   int8_t rltab[59][3];
   int8_t level_add[27];
   int8_t golomb_order;
   int inc_limit;
   int8_t max_run;
} xavs_vlc;
typedef struct tagxavs_image
{
	uint32_t   i_colorspace;
	uint32_t   i_width;
	uint32_t   i_height;
	uint8_t   *p_data[4];
	int32_t   i_stride[4];
	uint32_t   i_distance_index;
	uint32_t   i_code_type;
	uint32_t   b_top_field_first;
//////////
	//uint32_t   i_ref_distance[4];
	uint32_t   i_first_field_ref_distance[4];
	uint32_t   i_next_field_ref_distance[4];
	uint32_t   i_frame_ref_distance[4];

	int        i_first_field_ref_top_field[4];
	int        i_next_field_ref_top_field[4];

	uint32_t   b_picture_structure;


	void *param0;
	void *param1;

	
}xavs_image;

typedef struct tagxavs_bitstream
{
    uint8_t *p_start;/*流开始*/
    uint8_t *p;/*当前数据指针*/
    uint8_t *p_end;/*流结束*/
    int      i_left;    /* 能够得到的bit数*/
	uint32_t i_startcode;
	int i_race_count;
}xavs_bitstream;

static inline void xavs_check_start_code_32(xavs_bitstream *s)
{

}
static inline void xavs_bitstream_init_32( xavs_bitstream *s, void *p_data, int i_data )
{
	
    s->p_start = (uint8_t *)p_data;
    //s->p       = (uint8_t *) ((uint32_t)(s->p_start + 3)& 0x4);
    s->p       = (uint8_t *) ((s->p_start[3]) & 0x4);     // Modify
    s->p_end   = s->p + i_data;
	switch(s->p - s->p_start)
	{
	case 0:
		 s->i_left  = 32;
		 s->i_startcode = *((uint32_t *)s->p);
		 break;
	case 1:
		 s->i_left  = 8;
		 s->p--;
		 s->i_startcode = s->p[3] + 0xFFFFFF00;
		 break;
	case 2:
		s->i_left  = 16;
		s->p--;
		 s->i_startcode = s->p[2] + (s->p[3] << 8) + 0xFFFF000;
		break;
	case 3:
		s->i_left  = 24;
		s->p--;
		s->i_startcode = s->p[1] + (s->p[2] << 8 )+ (s->p[3] << 16) + 0xFF000000;
		 break;
	}
	
	s->i_race_count = 0;
	
}

static inline void xavs_bitstream_next_bits_32( xavs_bitstream *s)
{

	s->i_startcode >>= 8;
	if(s->p[1])
	{
		s->i_left = 32;
	}
	else
	{
		s->i_left = 30;
	}
	s->p += 4;

	

	/*s->i_startcode = (s->i_startcode << 8) + *s->p;
	if((s->i_startcode&0xffffff)!=0x000002)
	{
		s->i_left = 8;
	}
	else
	{
		*s->p>>=2;
		s->i_left = 6;
		s->i_race_count++;
	}*/
 
}

static inline uint32_t xavs_bitstream_get_bits_32(xavs_bitstream *s, int i_count)
{
	 static uint32_t i_mask[33] ={0x00,
                                  0x01,      0x03,      0x07,      0x0f,
                                  0x1f,      0x3f,      0x7f,      0xff,
                                  0x1ff,     0x3ff,     0x7ff,     0xfff,
                                  0x1fff,    0x3fff,    0x7fff,    0xffff,
                                  0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
                                  0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
                                  0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
                                  0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
	uint32_t i_result = s->i_startcode;
	if( i_count <= s->i_left )
    {
		i_result &= i_mask[i_count];
		 s->i_startcode >>= i_count;
		 return i_result;
	}
	else
	{
		if( s->p >= s->p_end )
		{
			return i_result;
		}
		i_count -= s->i_left;
		xavs_bitstream_next_bits_32(s);
		i_result |= s->i_startcode & i_mask[i_count];
		s->i_startcode >>= i_count;
		return i_result;

	}
}

static inline void xavs_bitstream_init( xavs_bitstream *s, void *p_data, int i_data )
{
    s->p_start = (uint8_t *)p_data;
    s->p       = (uint8_t *)p_data;
    s->p_end   = s->p + i_data;
    s->i_left  = 8;
	s->i_startcode=*s->p;
	s->i_race_count = 0;
}


static inline int xavs_bitstream_eof( xavs_bitstream *s )
{
//	return( s->p >= s->p_end -1? 1: 0 );
    return( s->p >= s->p_end ? 1: 0 );
}

static inline void xavs_bitstream_next_byte( xavs_bitstream *s)
{
	s->p++;
	s->i_startcode = (s->i_startcode << 8) + *s->p;
	if((s->i_startcode&0xffffff)!=0x000002)
	{
		s->i_left = 8;
	}
	else
	{
		*s->p>>=2;
		s->i_left = 6;
		s->i_race_count++;
	}
 
}
static inline uint32_t xavs_bitstream_get_bits( xavs_bitstream *s, int i_count )
{
     static uint32_t i_mask[33] ={0x00,
                                  0x01,      0x03,      0x07,      0x0f,
                                  0x1f,      0x3f,      0x7f,      0xff,
                                  0x1ff,     0x3ff,     0x7ff,     0xfff,
                                  0x1fff,    0x3fff,    0x7fff,    0xffff,
                                  0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
                                  0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
                                  0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
                                  0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
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
            i_result |= ( *s->p >> i_shr )&i_mask[i_count];
            s->i_left -= i_count;
            if( s->i_left == 0 )
            {
                xavs_bitstream_next_byte(s);
            }
            return( i_result );
        }
        else
        {//不够则取下一个字节处理直到取够为止
           i_result |= (*s->p&i_mask[s->i_left]) << -i_shr;
           i_count  -= s->i_left;
		   xavs_bitstream_next_byte(s);
       }
    }

    return( i_result );
}


static inline uint32_t xavs_bitstream_get_bit1( xavs_bitstream *s )
{

    if( s->p < s->p_end )
    {
        uint32_t i_result;

        s->i_left--;
        i_result = ( *s->p >> s->i_left )&0x01;
        if( s->i_left == 0 )
        {
			xavs_bitstream_next_byte(s);  
        }
        return i_result;
    }

    return 0;
}



static inline int32_t xavs_bitstream_get_int( xavs_bitstream *s, int i_count )
{

    uint32_t i_temp=1<<(i_count-1);
	uint32_t i_result=xavs_bitstream_get_bits(s,i_count);
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

static inline void xavs_bitstream_clear_bits( xavs_bitstream *s, int i_count )
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
				xavs_bitstream_next_byte(s);
            }
            break;
        }
        else
        {  
           i_count  -= s->i_left;
		   xavs_bitstream_next_byte(s);
       }
    }


}

static inline int xavs_bitstream_get_ue( xavs_bitstream *s )
{
    int i = 0;

    while( xavs_bitstream_get_bit1( s ) == 0 && s->p < s->p_end && i < 32)
    {
        i++;
    }
    return( ( 1 << i) - 1 + xavs_bitstream_get_bits( s, i) );
}

static inline int xavs_bitstream_get_se( xavs_bitstream *s)
{
    int val = xavs_bitstream_get_ue( s);

    return val&0x01 ? (val+1)/2 : -(val/2);
}

static inline int xavs_bitstream_get_ue_k( xavs_bitstream *s, int k)
{
    int i = 0;
    while( xavs_bitstream_get_bit1( s ) == 0 && s->p < s->p_end && i < 32)
    {
        i++;
    }
    return( ( 1 << (i+k)) - (1<<k) + xavs_bitstream_get_bits( s, i+k) );
}
static inline int xavs_bitstream_get_se_k( xavs_bitstream *s, int k)
{
    int val = xavs_bitstream_get_ue_k( s,k);

    return val&0x01 ? (val+1)/2 : -(val/2);
}



static inline uint32_t xavs_bitstream_get_bit1_fast( xavs_bitstream *s )
{
	uint32_t i_result;
	s->i_left--;
	i_result = ( *s->p >> s->i_left )&0x01;
	if( s->i_left == 0 )
	{
		xavs_bitstream_next_byte(s);  
	}
	return i_result;
}

void *xavs_malloc( int );
void *xavs_realloc( void *p, int i_size );
void  xavs_free( void * );
int xavs_get_video_sequence_header(xavs_bitstream *s,xavs_video_sequence_header *h);
int xavs_get_i_picture_header(xavs_bitstream *s,xavs_picture_header *h,xavs_video_sequence_header *sh);
int xavs_get_pb_picture_header(xavs_bitstream *s,xavs_picture_header *h,xavs_video_sequence_header *sh);


void xavs_log_open();

void xavs_log_insert(uint8_t *p,int i_len);

void xavs_log_close();
void xavs_log_string(char * lpszFormat, ...);
#ifdef XAVS_LOG 
#define XAVS_LOG_OPEN     xavs_log_open
#define XAVS_LOG_INSERT   xavs_log_insert
#define XAVS_LOG_CLOSE    xavs_log_close
#else
#define XAVS_LOG_OPEN 
#define XAVS_LOG_INSERT
#define XAVS_LOG_CLOSE
#endif



#endif

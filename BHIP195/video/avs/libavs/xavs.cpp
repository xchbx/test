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

//#include "stdafx.h"

#include "xavs.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#define CACHE_LINE        32
#define CACHE_LINE_MASK   31


/****************************************************************************
 * * xavs_malloc:
 ****************************************************************************/
void *xavs_malloc( int i_size )
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



/****************************************************************************
 * * xavs_free:
 ****************************************************************************/
void xavs_free( void *p )
{
    if( p )
    {
        free( *( ( ( void **) p ) - 1 ) );
    }
}

/****************************************************************************
 * xavs_realloc:
 ****************************************************************************/
void *xavs_realloc( void *p, int i_size )
{

    int       i_old_size = 0;
    uint8_t * p_new;
    if( p )
    {
        i_old_size = *( (int*) ( (uint8_t*) p  - sizeof( void ** ) -
                         sizeof( int ) ) );
    }
    p_new = (uint8_t *)xavs_malloc( i_size );
    if( i_old_size > 0 && i_size > 0 )
    {
        memcpy( p_new, p, ( i_old_size < i_size ) ? i_old_size : i_size );
    }
    xavs_free( p );
    return p_new;

}


void xavs_get_sequence_display_extension(xavs_bitstream *s,xavs_sequence_display_extension *e)
{
	uint32_t   i_value=0;
	e->i_video_format=xavs_bitstream_get_bits(s,3);
	e->b_sample_range=xavs_bitstream_get_bits(s,1);
	e->b_colour_description=xavs_bitstream_get_bit1(s);
	if(e->b_colour_description)
	{
		e->i_colour_primaries=xavs_bitstream_get_bits(s,8);
		e->i_transfer_characteristics=xavs_bitstream_get_bits(s,8);
		e->i_matrix_coefficients=xavs_bitstream_get_bits(s,8);

	}
	e->i_display_horizontal_size=xavs_bitstream_get_bits(s,14);
	xavs_bitstream_clear_bits(s,1);
	e->i_display_vertical_size=xavs_bitstream_get_bits(s,14);
	
}

void xavs_get_copyright_extension(xavs_bitstream *s,xavs_copyright_extension *e)
{
	
	uint32_t   i_value=0;
	e->b_copyright_flag=xavs_bitstream_get_bit1(s);
	e->i_copyright_id=xavs_bitstream_get_bits(s,8);
	e->b_original_or_copy=xavs_bitstream_get_bit1(s);
	xavs_bitstream_clear_bits(s,7);
	xavs_bitstream_clear_bits(s,1);
	e->i_copyright_number=xavs_bitstream_get_bits(s,20);
	e->i_copyright_number<<=22;
	xavs_bitstream_clear_bits(s,1);
	e->i_copyright_number+=xavs_bitstream_get_bits(s,22);
	e->i_copyright_number<<=22;
	xavs_bitstream_clear_bits(s,1);
	e->i_copyright_number+=xavs_bitstream_get_bits(s,22);
	
}
void xavs_get_camera_parameters_extension(xavs_bitstream *s,xavs_camera_parameters_extension *e)
{
	
	uint32_t   i_value=0;
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_id=xavs_bitstream_get_bits(s,7);
	xavs_bitstream_clear_bits(s,1);
	e->i_height_of_image_device=xavs_bitstream_get_bits(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_focal_length=xavs_bitstream_get_bits(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_f_number=xavs_bitstream_get_bits(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_vertical_angle_of_view=xavs_bitstream_get_bits(s,22);
	xavs_bitstream_clear_bits(s,1);
//这样做没测试过是否有问题
	i_value=xavs_bitstream_get_bits(s,16);
	i_value<<=16;
	xavs_bitstream_clear_bits(s,1);
	i_value+=xavs_bitstream_get_bits(s,16);
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_position_x=(int32_t)i_value;

	i_value=xavs_bitstream_get_bits(s,16);
	i_value<<=16;
	xavs_bitstream_clear_bits(s,1);
	i_value+=xavs_bitstream_get_bits(s,16);
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_position_y=(int32_t)i_value;

	i_value=xavs_bitstream_get_bits(s,16);
	i_value<<=16;
	xavs_bitstream_clear_bits(s,1);
	i_value+=xavs_bitstream_get_bits(s,16);
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_position_z=(int32_t)i_value;
//
	e->i_camera_direction_x=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_direction_y=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_camera_direction_z=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);

	e->i_image_plane_vertical_x=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_image_plane_vertical_y=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);
	e->i_image_plane_vertical_z=xavs_bitstream_get_int(s,22);
	xavs_bitstream_clear_bits(s,1);
}
void xavs_get_picture_display_extension(xavs_bitstream *s,xavs_picture_display_extension *e,int i_num)
{
	int i;
	uint32_t   i_value=0;
	for(i=0;i<i_num;i++)
	{
		e->i_frame_centre_horizontal_offset[i]=xavs_bitstream_get_int(s,16);
		xavs_bitstream_clear_bits(s,1);
		e->i_frame_centre_vertical_offset[i]=xavs_bitstream_get_int(s,16);
		xavs_bitstream_clear_bits(s,1);
	}
}

int xavs_get_video_sequence_header(xavs_bitstream *s,xavs_video_sequence_header *h)
{

	uint32_t   i_result=0;
	h->i_profile_id=xavs_bitstream_get_bits(s,8);
	//判断档次
	if(h->i_profile_id!=0x20 && h->i_profile_id!=0x48)
	{
		return -1;
	}
	h->i_level_id=xavs_bitstream_get_bits(s,8);
	//判断级别//对于基准档次最高为0x42即6.2级别，本解码器支持不大于6.2级别的数据流
	if(h->i_level_id>0x42)
	{
		return -1;
	}
	h->b_progressive_sequence=xavs_bitstream_get_bit1(s);
	h->i_horizontal_size=xavs_bitstream_get_bits(s,14);
	h->i_vertical_size=xavs_bitstream_get_bits(s,14);
	h->i_chroma_format=xavs_bitstream_get_bits(s,2);
	h->i_sample_precision=xavs_bitstream_get_bits(s,3);
	h->i_aspect_ratio=xavs_bitstream_get_bits(s,4);
	h->i_frame_rate_code=xavs_bitstream_get_bits(s,4);
	h->i_bit_rate=xavs_bitstream_get_bits(s,18);
	h->i_bit_rate<<=12;
	xavs_bitstream_clear_bits(s,1);
	h->i_bit_rate+=xavs_bitstream_get_bits(s,12);
	h->b_low_delay=xavs_bitstream_get_bit1(s);
	xavs_bitstream_clear_bits(s,1);
	h->i_bbv_buffer_size=xavs_bitstream_get_bits(s,18);
	if(xavs_bitstream_get_bits(s,3)!=0)
	{//3bits的'000'
		return -1;
	}
	return 0;
}




	
int xavs_get_picture_header_part1(xavs_bitstream *s,xavs_picture_header *h,xavs_video_sequence_header *sh)
{

	h->b_loop_filter_disable=xavs_bitstream_get_bit1(s);
	if(!h->b_loop_filter_disable)
	{
		h->b_loop_filter_parameter_flag=xavs_bitstream_get_bit1(s);
		if(h->b_loop_filter_parameter_flag)
		{//读取滤波参数
			//se
			 h->i_alpha_c_offset=xavs_bitstream_get_se(s);	
			 h->i_beta_offset=xavs_bitstream_get_se(s);	
		}

	}

	h->i_chroma_quant_param_delta_cb = 0;
	h->i_chroma_quant_param_delta_cr = 0;
	for(int i = 0; i < 6; i++)
	{
		h->i_weighting_quant_param_delta1[i] = 0;
		h->i_weighting_quant_param_delta2[i] = 0;
	}
	
	
	h->b_aec_enable = 0;

	if(sh->i_profile_id == 0x48)
	{

		
		h->b_weighting_quant_flag = xavs_bitstream_get_bit1(s);
		if(h->b_weighting_quant_flag)
		{
			int reservered_bits = xavs_bitstream_get_bit1(s);
			h->b_chroma_quant_param_disable = xavs_bitstream_get_bit1(s);
			if(h->b_chroma_quant_param_disable == 0)
			{
				h->i_chroma_quant_param_delta_cb = xavs_bitstream_get_se(s);	
				h->i_chroma_quant_param_delta_cr = xavs_bitstream_get_se(s);	
			}

			h->i_weighting_quant_param_index = xavs_bitstream_get_bits(s, 2);
			h->i_weighting_quant_model = xavs_bitstream_get_bits(s, 2);
			if(h->i_weighting_quant_param_index == 1)
			{
				for(int i = 0; i < 6; i++)
				{
					h->i_weighting_quant_param_delta1[i] = xavs_bitstream_get_se(s);	
				}
			}
			else if(h->i_weighting_quant_param_index == 2)
			{
				for(int i = 0; i < 6; i++)
				{
					h->i_weighting_quant_param_delta2[i] = xavs_bitstream_get_se(s);	
				}
			}

		}
		

		h->b_aec_enable = xavs_bitstream_get_bit1(s);



	}
	return 0;
}

int xavs_get_i_picture_header(xavs_bitstream *s,xavs_picture_header *h,xavs_video_sequence_header *sh)
{
	
	uint32_t   i_value=0;
	h->i_picture_coding_type=XAVS_I_PICTURE;//表示I Frame
	h->b_picture_reference_flag=1;//对于I帧应该为1表示用缺省
	h->b_advanced_pred_mode_disable=1;
	h->i_bbv_delay=xavs_bitstream_get_bits(s,16);
	if(sh->i_profile_id == 0x48)
	{
		int marker_bit = xavs_bitstream_get_bits(s,1);
		h->i_bbv_delay_extension = xavs_bitstream_get_bits(s,7);

	}


	h->b_time_code_flag=xavs_bitstream_get_bit1(s);
	if(h->b_time_code_flag)
	{
		h->i_time_code=xavs_bitstream_get_bits(s,24);
	}

	xavs_bitstream_get_bit1(s);
	h->i_picture_distance=xavs_bitstream_get_bits(s,8);
	if(sh->b_low_delay)
	{	
		//ue
		h->i_bbv_check_times=xavs_bitstream_get_ue(s);	
	}
	h->b_progressive_frame=xavs_bitstream_get_bit1(s);
	if(h->b_progressive_frame==0)
	{
		h->b_picture_structure=xavs_bitstream_get_bit1(s);
	}
	else
	{
		h->b_picture_structure=1;
	}
	h->b_top_field_first=xavs_bitstream_get_bit1(s);
	h->b_repeat_first_field=xavs_bitstream_get_bit1(s);
	h->b_fixed_picture_qp=xavs_bitstream_get_bit1(s);
	h->i_picture_qp=xavs_bitstream_get_bits(s,6);
	if(h->b_progressive_frame==0&&h->b_picture_structure==0)
	{
		h->b_skip_mode_flag=xavs_bitstream_get_bit1(s);
	}
	else
	{
		h->b_skip_mode_flag=0;
	}
	if(xavs_bitstream_get_bits(s,4)!=0)
	{
		return -1;
	}
	
	return xavs_get_picture_header_part1(s, h, sh);
}

int xavs_get_pb_picture_header(xavs_bitstream *s,xavs_picture_header *h,xavs_video_sequence_header *sh)
{

	uint32_t   i_value=0;	
	h->i_bbv_delay=xavs_bitstream_get_bits(s,16);
	if(sh->i_profile_id == 0x48)
	{
		int marker_bit = xavs_bitstream_get_bits(s,1);
		h->i_bbv_delay_extension = xavs_bitstream_get_bits(s,7);

	}

	h->i_picture_coding_type=xavs_bitstream_get_bits(s,2);//p或者B
	h->i_picture_distance=xavs_bitstream_get_bits(s,8);
	if(sh->b_low_delay)
	{
		h->i_bbv_check_times=xavs_bitstream_get_ue(s);	
	}
	h->b_progressive_frame=xavs_bitstream_get_bit1(s);
	
	if(h->b_progressive_frame==0)
	{
		h->b_picture_structure=xavs_bitstream_get_bit1(s);
		if(h->b_picture_structure==0)
		{
			h->b_advanced_pred_mode_disable=xavs_bitstream_get_bit1(s);
			//if(h->b_advanced_pred_mode_disable!=1)
			//{//在基准中该值必须为1
			//	return -1;
			//}
		}
	}
	else
	{
		h->b_picture_structure=1;
	}
	
	h->b_top_field_first=xavs_bitstream_get_bit1(s);
	h->b_repeat_first_field=xavs_bitstream_get_bit1(s);
	h->b_fixed_picture_qp=xavs_bitstream_get_bit1(s);
	h->i_picture_qp=xavs_bitstream_get_bits(s,6);
	if(!(h->i_picture_coding_type==XAVS_B_PICTURE&&h->b_picture_structure==1))
	{
		h->b_picture_reference_flag=xavs_bitstream_get_bit1(s);
	}

	h->b_no_forward_reference_flag=xavs_bitstream_get_bit1(s);
	h->b_pb_field_enhanced_flag = xavs_bitstream_get_bit1(s);
	
	if(xavs_bitstream_get_bits(s,2)!=0)
	{
		return -1;
	}
	h->b_skip_mode_flag=xavs_bitstream_get_bit1(s);
	
	return xavs_get_picture_header_part1(s, h, sh);
}

FILE *log_file=NULL;
void xavs_log_open()
{
	//log_file=fopen("E:\\avs-sdk\\SDK-test\\test\\xavs_log1.dat","w+b");	
}
void xavs_log_insert(uint8_t *p,int i_len)
{
	if(log_file)
	{
		
		//fwrite(p,1,i_len,log_file);
		//fflush(log_file);
		//sprintf(buf,p,...);
	}
}
void xavs_log_close()
{
	if(log_file)
		fclose(log_file);
	log_file=0;
}
#include <string.h>

#include <stdarg.h>
#include <stdio.h>
#if !defined(__CONFIG_ANDROID) && !defined(__CONFIG_LINUX)
#include <conio.h>
#endif

void xavs_log_string(char * lpszFormat, ...)
{

    va_list   args;
    char     szBuffer[512];
    va_start(args, lpszFormat);
	//vsprintf(szBuffer, sizeof(szBuffer)*sizeof(char), lpszFormat, args);
    //sprintf(szBuffer,lpszFormat, args);
	//fwrite(szBuffer,strlen(szBuffer),1,log_file);
    va_end(args);
	//fflush(log_file);

}

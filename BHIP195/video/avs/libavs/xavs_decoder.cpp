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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_decoder.h"

#include <stdarg.h>




uint8_t crop_table[256 + 2 * MAX_NEG_CROP];
void init_crop_table()
{
	int i;
    for(i=0;i<256;i++)
	{
		crop_table[i + MAX_NEG_CROP] = i;
	}
    for(i=0;i<MAX_NEG_CROP;i++) 
	{
        crop_table[i] = 0;
        crop_table[i + MAX_NEG_CROP + 256] = 255;
    }
}




int xavs_free_resource(xavs_decoder *p)
{
	int i,i_edge;
	XAVS_SAFE_FREE(p->p_top_qp);
	XAVS_SAFE_FREE(p->p_top_mv[0]);
	XAVS_SAFE_FREE(p->p_top_mv[1]);

	XAVS_SAFE_FREE(p->p_top_mvref[0]);
	XAVS_SAFE_FREE(p->p_top_mvref[1]);
	
	XAVS_SAFE_FREE(p->p_top_intra_pred_mode_y);
	XAVS_SAFE_FREE(p->p_top_intra_pred_mode_chroma);
	XAVS_SAFE_FREE(p->p_top_cbp);
	XAVS_SAFE_FREE(p->p_top_mb_type);

	
	XAVS_SAFE_FREE(p->p_top_border_y);
	XAVS_SAFE_FREE(p->p_top_border_cb);
	XAVS_SAFE_FREE(p->p_top_border_cr);
	XAVS_SAFE_FREE(p->p_col_mv);
	XAVS_SAFE_FREE(p->p_col_type_base);
	XAVS_SAFE_FREE(p->p_block);
	XAVS_SAFE_FREE(p->p_wqM88);
	
	i_edge=((p->i_mb_width+XAVS_EDGE*2)*XAVS_MB_SIZE+64)*2*17*2*2;
	if(p->p_edge)
	{
       p->p_edge-=i_edge/2;
	   XAVS_SAFE_FREE(p->p_edge);	

	}
	for(i=0;i<3;i++)
	{
		if(p->image[i].p_data[0])
		{
		   
		   p->image[i].p_data[0]-=p->image[i].i_stride[0]*XAVS_EDGE+XAVS_EDGE;
		   XAVS_SAFE_FREE(p->image[i].p_data[0]);
		}
		if(p->image[i].p_data[1])
		{
		   p->image[i].p_data[1]-=p->image[i].i_stride[1]*XAVS_EDGE/2+XAVS_EDGE/2;
		   XAVS_SAFE_FREE(p->image[i].p_data[1]);
		}
		if(p->image[i].p_data[2])
		{
		   p->image[i].p_data[2]-=p->image[i].i_stride[2]*XAVS_EDGE/2+XAVS_EDGE/2;
		   XAVS_SAFE_FREE(p->image[i].p_data[2]);	
		}
		memset(&p->image[i],0,sizeof(xavs_image));
	}
	
	return 0;
}

int xavs_alloc_resource(xavs_decoder *p)
{
	
	int i;
	uint32_t i_mb_width,i_mb_height,i_edge;
	i_mb_width=(p->vsh.i_horizontal_size+15)>>4;
	i_mb_height=(p->vsh.i_vertical_size+15)>>4;
	if(!p->vsh.b_progressive_sequence)
	{
		i_mb_height = (i_mb_height + 1) / 2 * 2;
	}

	
	if(i_mb_width==p->i_mb_width&&i_mb_height==p->i_mb_height)
	{
		//xavs_decoder_reset(p);
		return 0;
	}
	
	xavs_free_resource(p);
	
	p->i_mb_width=i_mb_width;
	p->i_mb_height=i_mb_height;
	p->i_mb_num=i_mb_width*i_mb_height;
	p->i_mb_num_half=p->i_mb_num>>1;
	p->p_top_qp       = (uint8_t *)xavs_malloc( p->i_mb_width);
	

    p->p_top_mv[0]    = (xavs_vector *)xavs_malloc((p->i_mb_width*2+1)*sizeof(xavs_vector));
    p->p_top_mv[1]    = (xavs_vector *)xavs_malloc((p->i_mb_width*2+1)*sizeof(xavs_vector));

	p->p_top_mvref[0]    = (xavs_mvref *)xavs_malloc((p->i_mb_width*2+1)*sizeof(xavs_vector));
    p->p_top_mvref[1]    = (xavs_mvref *)xavs_malloc((p->i_mb_width*2+1)*sizeof(xavs_vector));

	
    p->p_top_intra_pred_mode_y   = (int *)xavs_malloc( p->i_mb_width*2*sizeof(*p->p_top_intra_pred_mode_y));
	p->p_top_intra_pred_mode_chroma   = (int *)xavs_malloc( p->i_mb_width*sizeof(*p->p_top_intra_pred_mode_chroma));
	p->p_top_cbp   = (uint8_t *)xavs_malloc( p->i_mb_width*sizeof(*p->p_top_cbp));
	p->p_top_mb_type   = (uint8_t *)xavs_malloc( p->i_mb_width*sizeof(*p->p_top_mb_type));

	
	
    p->p_top_border_y = (uint8_t *)xavs_malloc((p->i_mb_width+1)*XAVS_MB_SIZE);
    p->p_top_border_cb = (uint8_t *)xavs_malloc((p->i_mb_width)*10);
    p->p_top_border_cr = (uint8_t *)xavs_malloc((p->i_mb_width)*10);

    p->p_col_mv       = (xavs_vector *)xavs_malloc( p->i_mb_width*p->i_mb_height*4*sizeof(xavs_vector));
    p->p_col_type_base = (uint8_t *)xavs_malloc(p->i_mb_width*p->i_mb_height);
    p->p_block        = (DCTELEM *)xavs_malloc(64*sizeof(DCTELEM));

	p->p_wqM88       = (uint8_t *)xavs_malloc(8 * 8 * sizeof(uint8_t));
	
	i_edge=((p->i_mb_width+XAVS_EDGE*2)*XAVS_MB_SIZE+64)*2*17*2*2;
	p->p_edge=(uint8_t *)xavs_malloc(i_edge); 
	if(p->p_edge)
	{
		memset(p->p_edge,0,i_edge);
	}
    p->p_edge+=i_edge/2;

	if(   !p->p_top_qp                 ||!p->p_top_mv[0]          ||!p->p_top_mv[1]
		||!p->p_top_intra_pred_mode_y  ||!p->p_top_border_y       ||!p->p_top_border_cb
		||!p->p_top_border_cr          ||!p->p_col_mv             ||!p->p_col_type_base
		||!p->p_block                  ||!p->p_edge               ||!p->p_wqM88
		||!p->p_top_intra_pred_mode_chroma  ||!p->p_top_cbp            ||!p->p_top_mb_type
		||!p->p_top_mvref[0]           ||!p->p_top_mvref[1]
		)
	{
		return -1;
	}
	
	memset(p->p_block,0,64*sizeof(DCTELEM));
	
	for(i=0;i<3;i++)
	{
		
		p->image[i].i_colorspace=XAVS_CS_YUV420;//
		p->image[i].i_width=p->i_mb_width*XAVS_MB_SIZE;
		p->image[i].i_height=p->i_mb_height*XAVS_MB_SIZE;
		//这里暂时不考虑边界分配的问题
		p->image[i].i_stride[0]=p->image[i].i_width+XAVS_EDGE*2;
		p->image[i].i_stride[1]=p->image[i].i_width/2+XAVS_EDGE;
		p->image[i].i_stride[2]=p->image[i].i_width/2+XAVS_EDGE;
		
		p->image[i].p_data[0]=(uint8_t *)xavs_malloc(p->image[i].i_stride[0]*(p->image[i].i_height+XAVS_EDGE*2));
		p->image[i].p_data[0]+=p->image[i].i_stride[0]*XAVS_EDGE+XAVS_EDGE;
		p->image[i].p_data[1]=(uint8_t *)xavs_malloc(p->image[i].i_stride[1]*(p->image[i].i_height/2+XAVS_EDGE));
		p->image[i].p_data[1]+=p->image[i].i_stride[1]*XAVS_EDGE/2+XAVS_EDGE/2;
		p->image[i].p_data[2]=(uint8_t *)xavs_malloc(p->image[i].i_stride[2]*(p->image[i].i_height/2+XAVS_EDGE));
		p->image[i].p_data[2]+=p->image[i].i_stride[2]*XAVS_EDGE/2+XAVS_EDGE/2;	
		if(!p->image[i].p_data[0]||!p->image[i].p_data[1]||!p->image[i].p_data[2])
		{
			return -1;
		}
	}




	p->p_save[0]=&p->image[0];

/*     
       0:    D3  B2  B3  C2
       4:    A1  X0  X1   -
       8:    A3  X2  X3   -
*/
	//7和19永远是un_mv
    p->mv[ 7] = MV_NOT_AVAIL; 
    p->mv[19] = MV_NOT_AVAIL;

	return 0;
}


void xavs_image_frame_to_field(xavs_image *p_src,xavs_image *p_dest,uint32_t b_bottom,uint32_t b_next_field)
{
	memcpy(p_dest,p_src,sizeof(xavs_image));
	if(b_bottom)
	{
		p_dest->p_data[0]+=p_dest->i_stride[0];
	    p_dest->p_data[1]+=p_dest->i_stride[1];
	    p_dest->p_data[2]+=p_dest->i_stride[2];
	  
	}
	p_dest->i_stride[0]*=2;
	p_dest->i_stride[1]*=2;
	p_dest->i_stride[2]*=2;
	
	p_dest->i_distance_index=p_src->i_distance_index + b_next_field;
}


#define XAVS_INIT_REF_DISTANCE
#include "xavs_decoder_b_special.hpp"


void init_ref_list(xavs_decoder *p,uint32_t b_next_field)
{
	
	int i;
	//uint32_t b_bottom=b_next_field;
	//uint32_t b_top_field_first=1;

	uint32_t b_top_field_first=p->ph.b_top_field_first;
	uint32_t b_bottom=(b_top_field_first&&b_next_field)||(!b_top_field_first&&!b_next_field);
	for(i=0;i<4;i++)
	{
		p->i_ref_distance[0]=0;
	    memset(&p->ref[i],0,sizeof(xavs_image));
	}
	p->p_save[0]->b_picture_structure=p->ph.b_picture_structure;
	
	if(p->ph.b_picture_structure!=0)
	{
		memcpy(&p->cur,   p->p_save[0],sizeof(xavs_image));
		if(p->p_save[1])
		{
			
		    memcpy(&p->ref[0],p->p_save[1],sizeof(xavs_image));
		}
		if(p->p_save[2])
		{
		    memcpy(&p->ref[1],p->p_save[2],sizeof(xavs_image));	
		}
		if(p->ref[0].p_data[0])
		{	
			p->i_ref_distance[0] = p->ph.i_picture_coding_type !=XAVS_B_PICTURE
				?(p->cur.i_distance_index - p->ref[0].i_distance_index + 512) % 512
				:(p->ref[0].i_distance_index - p->cur.i_distance_index + 512) % 512; 
		}
		if(p->ref[1].p_data[0])
			p->i_ref_distance[1] = (p->cur.i_distance_index - p->ref[1].i_distance_index + 512) % 512;
		

		
	}
	else
	{
		xavs_image_frame_to_field(p->p_save[0],&p->cur,b_bottom,b_next_field);
		if(p->cur.i_code_type==XAVS_I_PICTURE&&b_next_field)
		{
			//对于I帧，只有下一场才会参考上一场，所以只有一个参考场
			//此时如果当前场为底场，参考场为顶场，正好取反
			xavs_image_frame_to_field(p->p_save[0],&p->ref[0],!b_bottom,0);	
		}
		else if(p->cur.i_code_type==XAVS_P_PICTURE&&p->p_save[1])
		{
			//对于P帧，
			if(b_next_field)
			{
				//下一场的参考场情况如下
				xavs_image_frame_to_field(p->p_save[0],&p->ref[0],!b_bottom,0);	
				//对于1参考场应该是上一参考帧的next_field。则如果上一参考帧是顶场先，
				//则其为底场，如果是底场先则为顶场，与b_top_field_first正好相反
				xavs_image_frame_to_field(p->p_save[1],&p->ref[1],p->p_save[1]->b_top_field_first,1);	
				//参考场2与b_top_field_first一致
				xavs_image_frame_to_field(p->p_save[1],&p->ref[2],!p->p_save[1]->b_top_field_first,0);	
				//参考3与参考场1一致，不过用的是参考帧是前第二个参考帧的next_field
				if(p->p_save[2])
				{
				    xavs_image_frame_to_field(p->p_save[2],&p->ref[3],p->p_save[2]->b_top_field_first,1);	
				}
			}
			else
			{
				xavs_image_frame_to_field(p->p_save[1],&p->ref[0],p->p_save[1]->b_top_field_first,1);	
				xavs_image_frame_to_field(p->p_save[1],&p->ref[1],!p->p_save[1]->b_top_field_first,0);	
				if(p->p_save[2])
				{
				    xavs_image_frame_to_field( p->p_save[2],&p->ref[2],p->p_save[2]->b_top_field_first,1);	
				    xavs_image_frame_to_field( p->p_save[2],&p->ref[3],!p->p_save[2]->b_top_field_first,0);	
				}
			}
			
			
		}
		else if(p->cur.i_code_type==XAVS_B_PICTURE&&p->p_save[2]&&p->p_save[1])
		{
			//对于B帧,不管当前场是什么场其参考场的位置不变
			//后向参考
			xavs_image_frame_to_field(p->p_save[1],&p->ref[0],!p->p_save[1]->b_top_field_first,0);	
			xavs_image_frame_to_field(p->p_save[1],&p->ref[1],p->p_save[1]->b_top_field_first,1);	
			//前向参考
			xavs_image_frame_to_field(p->p_save[2],&p->ref[2],p->p_save[2]->b_top_field_first,1);	
			xavs_image_frame_to_field(p->p_save[2],&p->ref[3],!p->p_save[2]->b_top_field_first,0);
			

			
		}	
		for(i=0;i<2;i++)
		{
			if(p->ref[i].p_data[0])
			{
				p->i_ref_distance[i] = p->ph.i_picture_coding_type !=XAVS_B_PICTURE
					?(p->cur.i_distance_index - p->ref[i].i_distance_index + 512) % 512
					:(p->ref[i].i_distance_index - p->cur.i_distance_index + 512) % 512; 
			}
		}
		for(i=0;i<2;i++)
		{
			if(p->ref[2+i].p_data[0])
			{
				p->i_ref_distance[2+i] = (p->cur.i_distance_index - p->ref[2+i].i_distance_index + 512) % 512;
			}
		}

		
	}
	for(i=0;i<4;i++)
	{
	    p->i_scale_den[i] = p->i_ref_distance[i] ? 512/p->i_ref_distance[i] : 0;
        if(p->ph.i_picture_coding_type != XAVS_B_PICTURE)
		     p->i_direct_den[i] = p->i_ref_distance[i] ? 16384/p->i_ref_distance[i] : 0;
	}
	if( p->ph.i_picture_coding_type == XAVS_B_PICTURE) 
	{
		p->i_sym_factor = p->i_ref_distance[0]*p->i_scale_den[1];
	}
	p->i_luma_offset[0]=0;
	p->i_luma_offset[1]=8;
	p->i_luma_offset[2]=8*p->cur.i_stride[0];
	p->i_luma_offset[3]=p->i_luma_offset[2]+8;
	p->p_y = p->cur.p_data[0];
    p->p_cb = p->cur.p_data[1];
    p->p_cr = p->cur.p_data[2];	

	init_ref_distatnce(p, b_next_field);
}

static const uint8_t WeightingQuantParamDefault[8] = {128, 98,106,116,116,128};     
static const uint8_t WeightingQuantParamBase1[8] = {135,143,143,160,160,213};
static const uint8_t *WeightingQuantParamBase2 = WeightingQuantParamDefault;
static const uint8_t WeightQuantModel[4][64]={
//   l a b c d h
//	 0 1 2 3 4 5
	{ 	
     // Mode 0
		0,0,0,4,4,4,5,5,
		0,0,3,3,3,3,5,5,
		0,3,2,2,1,1,5,5,
		4,3,2,2,1,5,5,5,
		4,3,1,1,5,5,5,5,
		4,3,1,5,5,5,5,5,
		5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5},
	{  
	  // Mode 1
		0,0,0,4,4,4,5,5,
		0,0,4,4,4,4,5,5,
		0,3,2,2,2,1,5,5,
		3,3,2,2,1,5,5,5,
		3,3,2,1,5,5,5,5,
		3,3,1,5,5,5,5,5,
		5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5},
	{   
       // Mode 2
		0,0,0,4,4,3,5,5,
		0,0,4,4,3,2,5,5,
		0,4,4,3,2,1,5,5,
		4,4,3,2,1,5,5,5,
		4,3,2,1,5,5,5,5,
		3,2,1,5,5,5,5,5,
		5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5},
	{  
		// Mode 3
		0,0,0,3,2,1,5,5,
		0,0,4,3,2,1,5,5,
		0,4,4,3,2,1,5,5,
		3,3,3,3,2,5,5,5,
		2,2,2,2,5,5,5,5,
		1,1,1,5,5,5,5,5,
		5,5,5,5,5,5,5,5,
		5,5,5,5,5,5,5,5}
};

int xavs_init_picture(xavs_decoder *p)
{
	int i;
	//左边4个块运动矢量的初始化
    for(i=0;i<=20;i+=4)
        p->mv[i] = MV_NOT_AVAIL;
	p->mv[MV_FWD_X0]=p->mv[MV_BWD_X0] = MV_REF_DIR;
    copy_mvs(&p->mv[MV_BWD_X0], BLK_16X16);
    copy_mvs(&p->mv[MV_FWD_X0], BLK_16X16);

	p->i_intra_pred_mode_y[INTRA_MODE_A1] = p->i_intra_pred_mode_y[INTRA_MODE_A3] = NOT_AVAIL;
	p->i_mb_x = p->i_mb_y = 0;
	p->i_mb_flags = 0;
	p->i_mb_index=0;

/*块的距离索引DistanceIndex定义如下：
	如果块的所有像素都属于所在隔行扫描图像的第二场（显示顺序）
	或者都属于所在逐行扫描图像的底场，DistanceIndex等于picture_distance乘2加1；
	否则DistanceIndex等于picture_distance乘2。*/
//这里暂时不考虑分场的情况
	p->p_save[0]->i_distance_index=p->ph.i_picture_distance*2;
	p->p_save[0]->b_top_field_first=p->ph.b_top_field_first;
	p->p_save[0]->i_code_type=p->ph.i_picture_coding_type;


	
	init_ref_list(p,0);
	p->b_fixed_qp=p->ph.b_fixed_picture_qp;
    p->i_qp=p->ph.i_picture_qp;

	
	if(p->ph.b_weighting_quant_flag)
	{
		uint8_t wqP[3][6];
		memset(p->p_wqM88, 128, 64);
		
		for(int i = 0; i< 6; i++)
		{
			wqP[0][i] = 128;
			if(p->ph.i_weighting_quant_param_index == 0)
			{
				wqP[0][i] = WeightingQuantParamDefault[i];
			}
			wqP[1][i] = WeightingQuantParamBase1[i];
			if(p->ph.i_weighting_quant_param_index == 1)
			{
				wqP[1][i] += p->ph.i_weighting_quant_param_delta1[i];
			}

			wqP[2][i] = WeightingQuantParamBase2[i];
			if(p->ph.i_weighting_quant_param_index == 2)
			{
				wqP[2][i] += p->ph.i_weighting_quant_param_delta2[i];
			}
			
		}	
		
		for(int i = 0; i < 64; i++)
		{
			int index = i;
			int wpos = WeightQuantModel[p->ph.i_weighting_quant_model][i];
			p->p_wqM88[index] = wqP[p->ph.i_weighting_quant_param_index][wpos];
			
		}
	}

	return 0;
  
}


int xavs_init_slice(xavs_decoder *p)
{

	
	int i_y;
	p->i_mb_offset=(!p->ph.b_picture_structure&&p->i_mb_index>=p->i_mb_num_half)?(p->i_mb_height>>1):0;
	i_y=p->i_mb_y-p->i_mb_offset;
	if(p->i_mb_offset)
	{
	    init_ref_list(p,1);
	}
	memset(p->p_top_intra_pred_mode_y,NOT_AVAIL,p->i_mb_width*2*sizeof(*p->p_top_intra_pred_mode_y));
	p->p_y =p->cur.p_data[0] + i_y*XAVS_MB_SIZE*p->cur.i_stride[0];
	p->p_cb =p->cur.p_data[1] + i_y*XAVS_MB_SIZE/2*p->cur.i_stride[1];
	p->p_cr =p->cur.p_data[2] + i_y*XAVS_MB_SIZE/2*p->cur.i_stride[2];
	p->i_mb_flags=0;
	p->i_mb_x=0;
	p->b_first_line=1;

	p->i_cbp   = p->i_cbp_a = 0;

	p->i_last_qp  = p->i_qp;
	p->i_last_qp_delta  = 0; 
	
	p->mvd_a[0].enable = 0;
	p->mvd_a[1].enable = 0;
	p->mvd_a[2].enable = 0;
	p->mvd_a[3].enable = 0;

	

	return 0;
		
}


int xavs_get_slice(xavs_decoder *p)
{
	int i,i_num = 0;
	int i_first_field;
	////picture_type,picture_structure,i_first_field
	static int8_t i_num_of_reference_table[3][2][2]={
		{{1,0}/*picture_structure=0*/,{0,0}/*picture_structure=1*/},//i_picuture
		{{4,4}/*picture_structure=0*/,{2,2}/*picture_structure=1*/},//p_picuture
		{{4,4}/*picture_structure=0*/,{2,2}/*picture_structure=1*/}//b_picuture
	};
	if(p->vsh.i_vertical_size>2800)
	{
		p->sh.i_slice_vertical_position_extension=xavs_bitstream_get_bits(&p->s,3);
	}
	else
	{
		p->sh.i_slice_vertical_position_extension=0;
	}
	if(p->ph.b_fixed_picture_qp==0)
	{
		p->b_fixed_qp=p->sh.b_fixed_slice_qp=xavs_bitstream_get_bit1(&p->s);
		p->i_qp=p->sh.i_slice_qp=xavs_bitstream_get_bits(&p->s,6);	
	}
	else
	{
		p->sh.b_fixed_slice_qp=1;
		p->sh.i_slice_qp=p->i_qp;
	}
	p->i_mb_y+=(p->sh.i_slice_vertical_position_extension<<7);
	p->i_mb_index=p->i_mb_y*p->i_mb_width;
	
	i_first_field=p->i_mb_index<p->i_mb_num_half?1:0;
	i_num=i_num_of_reference_table[p->ph.i_picture_coding_type][p->ph.b_picture_structure][i_first_field];
	p->b_have_pred=p->ph.i_picture_coding_type!=XAVS_I_PICTURE||(p->ph.b_picture_structure==0&&i_first_field==0);
	if(p->b_have_pred)
	{
		p->sh.b_slice_weighting_flag=xavs_bitstream_get_bit1(&p->s);
		if(p->sh.b_slice_weighting_flag)
		{
			for(i=0;i<i_num;i++)
			{
				p->sh.i_luma_scale[i]=xavs_bitstream_get_bits(&p->s,8);
				p->sh.i_luma_shift[i]=xavs_bitstream_get_int(&p->s,8);
				xavs_bitstream_clear_bits(&p->s,1);
				p->sh.i_chroma_scale[i]=xavs_bitstream_get_bits(&p->s,8);
				p->sh.i_chroma_shift[i]=xavs_bitstream_get_int(&p->s,8);
				xavs_bitstream_clear_bits(&p->s,1);
			}
			p->sh.b_mb_weighting_flag=xavs_bitstream_get_bit1(&p->s);
		}
		else
		{
			p->sh.b_mb_weighting_flag=0;
		}
	}
	else
	{
		p->sh.b_slice_weighting_flag=0;
		p->sh.b_mb_weighting_flag=0;

	}
	p->sh.i_number_of_reference=i_num;
	xavs_init_slice(p);

	if(!p->ph.b_aec_enable)
	{
		return xavs_get_all_mbs(p);
	}
	else
	{
		return xavs_aec_decoder_get_all_mbs(p);
	}
}



void xavs_decoder_init()
{
	static int i_first=0;
	if(i_first!=0)
	{
		return ;
	}
	i_first++;
	init_crop_table();   
	init_dsp();

	 

}

void xavs_out_image(xavs_image *p_cur,xavs_param *p_param)
{
	int i,j;
	unsigned char *p_dest=p_param->p_out;
	unsigned char *p_dest1=p_dest+p_param->i_out_pitch;
	unsigned char *p_y=p_cur->p_data[0];
	unsigned char *p_y1=p_y+p_cur->i_stride[0];
	unsigned char *p_cb=p_cur->p_data[1];
	unsigned char *p_cr=p_cur->p_data[2];
	long l_quat_width=(p_cur->i_width>>2);
	long  l_span_y=(p_cur->i_stride[0]<<1)-p_cur->i_width;
	long  l_span_cb=p_cur->i_stride[1]-(p_cur->i_width>>1);
	long  l_span_cr=p_cur->i_stride[2]-(p_cur->i_width>>1);
	long l_dest_span=(p_param->i_out_pitch<<1)-(p_cur->i_width<<1);
	
	long l_half_height=(p_cur->i_height>>1);

	if(!p_param->p_out)
		return ;
	for(i=0;i<l_half_height;i++)
	{
		for(j=0;j<l_quat_width;j++)
		{
			//////////////////////////
			*p_dest=*p_y;p_dest++;p_y++;
			*p_dest=*p_cb;p_dest++;
			*p_dest=*p_y;p_dest++;p_y++;
			*p_dest=*p_cr;p_dest++;
			
			*p_dest1=*p_y1;p_dest1++;p_y1++;
			*p_dest1=*p_cb; p_dest1++;p_cb++;
			*p_dest1=*p_y1;p_dest1++;p_y1++;
			*p_dest1=*p_cr; p_dest1++;p_cr++;
			//////////////////////////
			*p_dest=*p_y;p_dest++;p_y++;
			*p_dest=*p_cb;p_dest++;
			*p_dest=*p_y;p_dest++;p_y++;
			*p_dest=*p_cr;p_dest++;
			
			*p_dest1=*p_y1;p_dest1++;p_y1++;
			*p_dest1=*p_cb; p_dest1++;p_cb++;
			*p_dest1=*p_y1;p_dest1++;p_y1++;
			*p_dest1=*p_cr; p_dest1++;p_cr++;
			
			//////////////////////////
		}
		//
		p_y+=l_span_y;
		p_y1+=l_span_y;
		p_cb+=l_span_cb;
		p_cr+=l_span_cr;
		
		p_dest+=l_dest_span;
		p_dest1+=l_dest_span;
		
		
	}
	p_param->l_result|=XAVS_FRAME_OUT;
	


}
//输入缓冲区必须保证只有一个startcode
int xavs_decoder_process(void *p_decoder,xavs_param *p_param)
{

	xavs_decoder *p=(xavs_decoder *)p_decoder;
	uint32_t   i_startcode;
	uint8_t   *p_buf;
	int i_complete=0;
	int    i_len,i_ret,i_result=0;



	p_buf=p_param->p_in;
	i_len=p_param->i_in_length;
	p_param->l_result=0;
//保证从startcode开始并且后面没有startcode
	i_startcode=xavs_next_startcode(p_buf,i_len,&i_ret);
	if(i_startcode==0)
	{
		return -1;
	}
	p_buf+=i_ret+4;i_len-=i_ret+4;//+4是为了清除掉startcode
	if(i_startcode!=XAVS_VIDEO_SEQUENCE_START_CODE&&!p->b_get_video_sequence_header)
	{//没有出现序列头之前不做任何的处理工作
		return  -1;
	}
	switch(i_startcode)
	{
	case XAVS_VIDEO_SEQUENCE_START_CODE:
		//
		//
		if(p->p_save[1])
		{
		     xavs_out_image(p->p_save[1],p_param);
		}
		xavs_bitstream_init(&p->s,p_buf,i_len);
		i_result=xavs_get_video_sequence_header(&p->s,&p->vsh);
		if(i_result==0)
		{
			if(xavs_alloc_resource(p)!=0)
			{
				return -1;
			}
			uint32_t i_mb_width,i_mb_height;
			i_mb_width=(p->vsh.i_horizontal_size+15)>>4;
			i_mb_height=(p->vsh.i_vertical_size+15)>>4;
			if(i_mb_width!=p->i_mb_width||i_mb_height!=p->i_mb_height)
			{
				if(p->p_save[1])
				{
					xavs_out_image(p->p_save[1],p_param);
					//*have_frame = 1;
				}
				if(xavs_alloc_resource(p)!=0)
				{
					return -1;
				}	
			}
			p_param->l_result|=XAVS_SEQ_HEADER;
			p->b_get_video_sequence_header=1;
		}	
		return i_result;
	case XAVS_VIDEO_SEQUENCE_END_CODE:
		//直接返回
		xavs_decoder_reset(p);
		return 0;
	case XAVS_USER_DATA_CODE:
		return 0;
	case XAVS_EXTENSION_START_CODE:
		return 0;
	case XAVS_VIDEO_EDIT_CODE:
		p->i_video_edit_code_flag=0;
		return 0;
	case XAVS_I_PICUTRE_START_CODE:
		printf(" put I frame\n");
		xavs_bitstream_init(&p->s,p_buf,i_len);
		xavs_get_i_picture_header(&p->s,&p->ph,&p->vsh);
		xavs_init_picture(p);
		p->i_video_edit_code_flag++;
		break;
	case XAVS_PB_PICUTRE_START_CODE:
		
		xavs_bitstream_init(&p->s,p_buf,i_len);
		xavs_get_pb_picture_header(&p->s,&p->ph,&p->vsh);
		xavs_init_picture(p);
		if(p->ph.i_picture_coding_type==XAVS_B_PICTURE)
		{
			printf(" put B frame\n");
		}
		else
		{
			printf(" put P frame\n");
		}
		
		if(p->i_video_edit_code_flag==1&&p->ph.i_picture_coding_type==XAVS_P_PICTURE)
			p->i_video_edit_code_flag++;
		
		break;
	default:
		//if(p->i_video_edit_code_flag>6)
		 // break;
		//p->i_video_edit_code_flag++;
	
			//return -1;
		if(!p->cur.p_data[0])
		{
			break;
		}
		

		if(p->ph.i_picture_coding_type==XAVS_B_PICTURE)
		{
			
			if(!p->p_save[1]||!p->p_save[2])
			{
				break;
			}
		}
		if(p->ph.i_picture_coding_type==XAVS_P_PICTURE)
		{
			if(!p->p_save[1])
			{
				break;
			}
		}
		
		if(i_startcode>=XAVS_SLICE_MIN_START_CODE&&i_startcode<=XAVS_SLICE_MAX_START_CODE)
		{
			if((p->i_video_edit_code_flag<1&&p->ph.i_picture_coding_type==XAVS_P_PICTURE)
			||(p->i_video_edit_code_flag<2&&p->ph.i_picture_coding_type==XAVS_B_PICTURE))
			{
				return -1;
			}

			
			xavs_bitstream_init(&p->s,p_buf,i_len);
			p->i_mb_y=i_startcode&0x000000FF;
			
			xavs_get_slice(p);
		//	if(p->b_complete)
			//    xavs_out_image(&p->cur,p_param);
			if(p->b_complete&&p->ph.i_picture_coding_type!=XAVS_B_PICTURE)
			{	
				xavs_image *p_image=p->p_save[0];
				if(p->vsh.b_low_delay)
				{
					xavs_out_image(p->p_save[0],p_param);	
				}
				if(!p->p_save[1])
				{
					p->p_save[0]=&p->image[1];		
				}
				else
				{
					if(p->vsh.b_low_delay==0)
					{//还没有输出
					   xavs_out_image(p->p_save[1],p_param);
					}
					
					if(!p->p_save[2])
					{
						p->p_save[0]=&p->image[2];	 
					}
					else
					{
						p->p_save[0]=p->p_save[2];
					}
				}
				p->p_save[2]=p->p_save[1];
				p->p_save[1]=p_image;
				memset(&p->cur,0,sizeof(xavs_image));
				p->b_complete=0;
			}
			else if(p->b_complete)
			{		
				xavs_out_image(p->p_save[0],p_param);
				memset(&p->cur,0,sizeof(xavs_image));
				p->b_complete=0;
			}
		
			
			
		}
		else
		{
		    return -1;
		}
	}
	return 0;
}


void xavs_out_imageex(xavs_decoder *p,xavs_image *p_cur,AVSFrame *p_param, int *have_frame)
{
	

	p_param->data[0] = p_cur->p_data[0];
	p_param->data[1] = p_cur->p_data[1];
	p_param->data[2] = p_cur->p_data[2];

	p_param->linesize[0] = p_cur->i_stride[0];
	p_param->linesize[1] = p_cur->i_stride[1];
	p_param->linesize[2] = p_cur->i_stride[2];

	//p_param->nWidth = p_cur->i_width;
	//p_param->nHeight = p_cur->i_height;

	p_param->nWidth = p->vsh.i_horizontal_size;
	p_param->nHeight = p->vsh.i_vertical_size;
	p_param->nFrameType = p_cur->i_code_type;

	p_param->nFrameCoded = p_cur->b_picture_structure;
	p_param->nTopFieldFirst = p_cur->b_top_field_first;
	p_param->nFrameIndex = p_cur->i_distance_index /2;

	p_param->param0 = p_cur->param0;
	p_param->param1 = p_cur->param1;

	*have_frame = 1;

}

//输入缓冲区必须保证只有一个startcode
int xavs_decoder_processex(void *p_decoder,uint8_t *p_buf,int i_len,int *have_frame,AVSFrame *p_param)
{

	xavs_decoder *p=(xavs_decoder *)p_decoder;
	uint32_t   i_startcode;
	int i_complete=0;
	int    i_ret,i_result=0;

	*have_frame = 0;

//保证从startcode开始并且后面没有startcode
	i_startcode=xavs_next_startcode(p_buf,i_len,&i_ret);
	if(i_startcode==0)
	{
		return -1;
	}
	p_buf+=i_ret+4;i_len-=i_ret+4;//+4是为了清除掉startcode
	if(i_startcode!=XAVS_VIDEO_SEQUENCE_START_CODE&&!p->b_get_video_sequence_header)
	{//没有出现序列头之前不做任何的处理工作
		return  -1;
	}
	
	switch(i_startcode)
	{
	case XAVS_VIDEO_SEQUENCE_START_CODE:
		//
		//
		
		xavs_bitstream_init(&p->s,p_buf,i_len);
	
		i_result=xavs_get_video_sequence_header(&p->s,&p->vsh);
		if(i_result==0)
		{
			uint32_t i_mb_width,i_mb_height;
			i_mb_width=(p->vsh.i_horizontal_size+15)>>4;
			i_mb_height=(p->vsh.i_vertical_size+15)>>4;
			if(i_mb_width!=p->i_mb_width||i_mb_height!=p->i_mb_height)
			{
				if(p->p_save[1])
				{
					xavs_out_imageex(p ,p->p_save[1],p_param,have_frame);
					
				}
				if(xavs_alloc_resource(p)!=0)
				{
					return -1;
				}	
			}
			
		//	p_param->l_result|=XAVS_SEQ_HEADER;
			p->b_get_video_sequence_header=1;
		}	
		return i_result;
	case XAVS_VIDEO_SEQUENCE_END_CODE:
		//xavs_decoder_reset(p);
		//直接返回
		return 0;
	case XAVS_USER_DATA_CODE:
		return 0;
	case XAVS_EXTENSION_START_CODE:
		return 0;
	case XAVS_VIDEO_EDIT_CODE:
		p->i_video_edit_code_flag=0;
		return 0;
	case XAVS_I_PICUTRE_START_CODE:
	
		//printf(" put I frame\n");
		
		xavs_bitstream_init(&p->s,p_buf,i_len);
		xavs_get_i_picture_header(&p->s,&p->ph,&p->vsh);
		xavs_init_picture(p);
		p->i_video_edit_code_flag++;
		p->p_save[0]->param0 = p_param->param0;
		p->p_save[0]->param1 = p_param->param0;
		break;
	case XAVS_PB_PICUTRE_START_CODE:
	
		xavs_bitstream_init(&p->s,p_buf,i_len);
		xavs_get_pb_picture_header(&p->s,&p->ph,&p->vsh);
		xavs_init_picture(p);
		p->p_save[0]->param0 = p_param->param0;
		p->p_save[0]->param1 = p_param->param0;
	
		if(p->i_video_edit_code_flag==1&&p->ph.i_picture_coding_type==XAVS_P_PICTURE)
			p->i_video_edit_code_flag++;
		if(p->ph.i_picture_coding_type==XAVS_P_PICTURE)
		{
			//printf(" put P frame\n");
		}
		else
		{
			//printf(" put B frame\n");
		}
		break;
	default:
		//if(p->i_video_edit_code_flag>6)
		 // break;
		//p->i_video_edit_code_flag++;
	
			//return -1;
		if(!p->cur.p_data[0])
		{
			break;
		}

		
		if(p->ph.i_picture_coding_type==XAVS_B_PICTURE)
		{

			if(p->i_frame_skip_mode >= 1)
			{
				break;
			}
			if(!p->p_save[1]||!p->p_save[2])
			{
				break;
			}
		}
		if(p->ph.i_picture_coding_type==XAVS_P_PICTURE)
		{
			if(p->i_frame_skip_mode >= 2)
			{
				break;
			}
			//break;
			if(!p->p_save[1])
			{
				break;
			}
		}
		
		if(i_startcode>=XAVS_SLICE_MIN_START_CODE&&i_startcode<=XAVS_SLICE_MAX_START_CODE)
		{
			if((p->i_video_edit_code_flag<1&&p->ph.i_picture_coding_type==XAVS_P_PICTURE)
			||(p->i_video_edit_code_flag<2&&p->ph.i_picture_coding_type==XAVS_B_PICTURE))
			{
				return -1;
			}
//


			xavs_bitstream_init(&p->s,p_buf,i_len);
			p->i_mb_y=i_startcode&0x000000FF;
			
			xavs_get_slice(p);
		//	if(p->b_complete)
			//    xavs_out_image(&p->cur,p_param);
			if(p->b_complete&&p->ph.i_picture_coding_type!=XAVS_B_PICTURE)
			{	
				xavs_image *p_image=p->p_save[0];
				if(p->vsh.b_low_delay)
				{
					xavs_out_imageex(p, p->p_save[0],p_param,have_frame);	
					
				}
				if(!p->p_save[1])
				{
					p->p_save[0]=&p->image[1];		
				}
				else
				{
					if(p->vsh.b_low_delay==0)
					{//还没有输出
					   xavs_out_imageex(p, p->p_save[1],p_param,have_frame);
					   
					}
					
					if(!p->p_save[2])
					{
						p->p_save[0]=&p->image[2];	 
					}
					else
					{
						p->p_save[0]=p->p_save[2];
					}
				}
				p->p_save[2]=p->p_save[1];
				p->p_save[1]=p_image;
				memset(&p->cur,0,sizeof(xavs_image));
				p->b_complete=0;
			}
			else if(p->b_complete)
			{		
				xavs_out_imageex(p,p->p_save[0],p_param,have_frame);
				
				memset(&p->cur,0,sizeof(xavs_image));
				p->b_complete=0;
				
			}
		
			
			
		}
		else
		{
		    return -1;
		}
	}
	return 0;
}
int xavs_decoder_create(void **pp_decoder)
{
	xavs_decoder *p;
	p=(xavs_decoder *)malloc(sizeof(xavs_decoder));
	if(!p)
	{
		return -1;
	}
	memset(p,0,sizeof(xavs_decoder));
	p->i_video_edit_code_flag=0;
	*pp_decoder=p;
	xavs_decoder_init();
	xavs_log_open();
	return 0;
}
int xavs_decoder_get_seq(void *p_decoder,xavs_seq_info *p_si)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	if(!p->b_get_video_sequence_header)
	{
		return -1;
	}
	p_si->iWidth=p->vsh.i_horizontal_size;
	p_si->iHeight=p->vsh.i_vertical_size;
	p_si->iAspectRatioX = 1;
	p_si->iAspectRatioY = 1;
	switch(p->vsh.i_aspect_ratio)
	{
	case 2:
		p_si->iAspectRatioX = 4;
		p_si->iAspectRatioY = 3;
		break;
	case 3:
		p_si->iAspectRatioX = 16;
		p_si->iAspectRatioY = 9;
		break;
	case 4:
		p_si->iAspectRatioX = 221;
		p_si->iAspectRatioY = 100;
		break;
	default:
		break;
	}
	
	p_si->iFrameRateX = 25;
	p_si->iFrameRateY = 1;
	switch(p->vsh.i_frame_rate_code)
	{
	case 1:
		p_si->iFrameRateX = 24000;
		p_si->iFrameRateY = 1001;
		break;
	case 2:
		p_si->iFrameRateX = 24;
		p_si->iFrameRateY = 1;
		break;
	case 4:
		p_si->iFrameRateX = 30000;
		p_si->iFrameRateY = 1001;
		break;
	case 5:
		p_si->iFrameRateX = 30;
		p_si->iFrameRateY = 1;
		break;
	case 6:
		p_si->iFrameRateX = 50;
		p_si->iFrameRateY = 1;
		break;

	case 7:
		p_si->iFrameRateX = 60000;
		p_si->iFrameRateY = 1001;
		break;
	case 8:
		p_si->iFrameRateX = 60;
		p_si->iFrameRateY = 1;
		break;
	default:
		break;

	}
	p_si->iProgressiveSequence = p->vsh.b_progressive_sequence;

	return 0;

}

void xavs_decoder_destroy(void *p_decoder)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return ;
	}

	xavs_log_close();
	if(p->p_user_data)
	    free(p->p_user_data);
	if(p->p_ce)
		free(p->p_ce);
	if(p->p_cpe)
		free(p->p_cpe);
	if(p->p_pde)
		free(p->p_pde);
	xavs_free_resource(p);

	if(p->data_buf)
	{
		free(p->data_buf);
	}
	xavs_aec_decoder_destroy(p);

	free(p);
}

int xavs_decoder_reset_buffer(void *p_decoder)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	p->data_buf_pos = 0;
    p->data_buf_len = 0;
	return 0;
}
int xavs_decoder_reset(void *p_decoder)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	p->i_video_edit_code_flag=0;
//	p->b_get_video_sequence_header=0;
	p->b_get_i_picture_header=0;
	memset(&p->ref[0],0,sizeof(xavs_image));
	memset(&p->ref[1],0,sizeof(xavs_image));
	memset(&p->ref[2],0,sizeof(xavs_image));
	memset(&p->ref[3],0,sizeof(xavs_image));
	p->image[0].i_distance_index=0;
	p->image[1].i_distance_index=0;
	p->image[2].i_distance_index=0;

	p->p_save[0]=&p->image[0];
	p->p_save[1]=0;
	p->p_save[2]=0;
	
	//memset(p->p_edge-(p->i_mb_width*XAVS_MB_SIZE+64)*2*17,(p->i_mb_width*XAVS_MB_SIZE+64)*2*17*2,0);

	if(p->p_block)
	{
		memset(p->p_block,0,64*sizeof(DCTELEM));
	}

//	p->data_buf_pos = 0;
//	p->data_buf_len = 0;

	return 0;


}

int xavs_decoder_put_data(void *p_decoder,unsigned char *data,int len)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	uint8_t * temp = NULL;
	int total = p->data_buf_len+len;
	if(total>p->data_buf_size||p->data_buf==NULL)
	{
		temp = (uint8_t *) malloc(total);
		if(temp == NULL)
		{
			return -1;
		}
		memcpy(temp,p->data_buf,p->data_buf_len);
		free(p->data_buf);
		p->data_buf = temp;
		p->data_buf_size = total;

	}
	//printf("p->data_buf_size=%d\n",p->data_buf_size);
	memcpy(p->data_buf + p->data_buf_len,data,len);
	p->data_buf_len+=len;
	


	return 0;
}

int xavs_decoder_get_decode_video(void *p_decoder,int *got_frame,AVSFrame *param)
{
	*got_frame = 0;
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	int ret;
	uint32_t startcode;
	uint8_t *pos0 = NULL;
	uint8_t *pos = p->data_buf + p->data_buf_pos ;
	int rest = p->data_buf_len - p->data_buf_pos ;
	if(rest <=0)
	{
		return -1;
	}

	while(1)
	{

		startcode = xavs_next_startcode(pos,rest,&ret);
		if(startcode==0)
		{
			if(pos0)
			{
				p->data_buf_len -= (int)(pos0 - p->data_buf);
				p->data_buf_pos = 0;
				memmove(p->data_buf,pos0,p->data_buf_len);
				
				
			}
			return -1;
		}
		pos += ret; rest -= ret;
		if(pos0)
		{

			if(pos0[3] == (XAVS_I_PICUTRE_START_CODE&0xFF) || pos0[3] == (XAVS_PB_PICUTRE_START_CODE&0xFF) )
			{
			//	static int xx =0;
			//	printf("put  frame index %d  ",xx++);
			
			}
			xavs_decoder_processex(p_decoder,pos0,(int)(pos-pos0),got_frame,param);
			if(*got_frame)
			{
				param->nBitrate = p->vsh.i_bit_rate;
				p->data_buf_pos = (int)(pos - p->data_buf);
				return 0;
			}
		}
		pos0=pos;
		pos+=4;
		rest-=4;
	}
	return 0;

}



int xavs_decoder_decode_frame(void *p_decoder,uint8_t *p_buf,int i_len,int *have_frame,AVSFrame *p_param)
{
	*have_frame = 0;
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	if(p_buf == NULL || i_len == 0)
	{
		if(p->p_save[1])
		{
			xavs_out_imageex(p, p->p_save[1],p_param,have_frame);
			
		}
		return 0;
	}
	
	int ret;
	uint32_t startcode;
	uint8_t *pos = p_buf ;
	int rest = i_len ;
	uint8_t *pos0 = NULL;
	while(1)
	{

		startcode = xavs_next_startcode(pos,rest,&ret);
		if(startcode != 0)
		{
			pos += ret; rest -= ret;
		}
		else
		{
			pos = p_buf + i_len;
		}
		
		if(pos0)
		{
			xavs_decoder_processex(p_decoder, pos0, (int)(pos-pos0), have_frame, p_param);
			if(*have_frame)
			{
				p_param->nBitrate = p->vsh.i_bit_rate;
				p->data_buf_pos = (int)(pos - p->data_buf);
				return 0;
			}
		}
		if(startcode == 0)
		{
			return -1;
		}
		pos0 = pos;
		pos += 4;
		rest -= 4;
	}
	return 0;
}

int xavs_decoder_set_frame_skip_mode(void *p_decoder,int skip_mode)
{

	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	if(p->i_frame_skip_mode >= 2)
	{
		xavs_decoder_reset(p_decoder);
	}
	p->i_frame_skip_mode = skip_mode;
	return 0;

}

int xavs_get_frame_type(unsigned char *data,int len, int profile_id)
{

	uint32_t   i_startcode;
	uint8_t   *p_buf;
	int    i_len,i_ret,i_result=0;
	p_buf=data;
	i_len=len;
	while(1)
	{
		i_startcode=xavs_next_startcode(p_buf,i_len,&i_ret);
		if(i_startcode==0)
		{
			return -1;
		}
		p_buf += i_ret + 4;
		i_len-=i_ret + 4;//+4是为了清除掉startcode
		if(i_startcode == XAVS_PB_PICUTRE_START_CODE)
		{
			if(profile_id == 0)
			{
				return -1;
			}
			p_buf += 2 ; // vbv buffer
			if(profile_id == 0x48)
			{
				p_buf += 1; //i_bbv_delay_extension
			}
			if(((p_buf[0] >> 6) & 0x3) == XAVS_P_PICTURE)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
		else if(i_startcode == XAVS_I_PICUTRE_START_CODE)
		{
			return 0;
		}
	}
	return -1;

}

int xavs_decoder_get_frame_type(void *p_decoder, unsigned char *data,int len)
{
	xavs_decoder *p=(xavs_decoder *)p_decoder;
	if(!p)
	{
		return -1;
	}
	return xavs_get_frame_type(data , len, p->vsh.i_profile_id);

}
int xavs_decoder_load_funs(xavs_decoder_funs *funs, int mode)
{
	
	funs->create               = xavs_decoder_create;
	funs->put_data             = xavs_decoder_put_data;
	funs->get_decode_video     = xavs_decoder_get_decode_video;
	funs->reset                = xavs_decoder_reset;
	funs->destroy              = xavs_decoder_destroy;
	funs->reset_buffer         = xavs_decoder_reset_buffer;
	funs->decode_frame         = xavs_decoder_decode_frame;
	funs->get_seq              = xavs_decoder_get_seq;
	funs->set_frame_skip_mode  = xavs_decoder_set_frame_skip_mode;

	funs->get_frame_type       = xavs_decoder_get_frame_type;
	if((mode&0xFFFFF) != 0x41201)
	{
		funs->create               = (xavs_decoder_create_t)0x38109121;

	}
	return 0;
}
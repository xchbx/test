// AVCDemuxer.cpp: implementation of the CAVCDemuxer class.
//
//////////////////////////////////////////////////////////////////////

#include "AVCDemuxer.h"
#include "bs.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    int i_id;

    int i_profile_idc;
    int i_level_idc;

    int b_constraint_set0;
    int b_constraint_set1;
    int b_constraint_set2;

	int i_chroma_format_idc ;
	int i_transform_bypass;
	int b_seq_scaling_matrix_preset_falg;

    int i_log2_max_frame_num;

    int i_poc_type;
    /* poc 0 */
    int i_log2_max_poc_lsb;
    /* poc 1 */
    int b_delta_pic_order_always_zero;
    int i_offset_for_non_ref_pic;
    int i_offset_for_top_to_bottom_field;
    int i_num_ref_frames_in_poc_cycle;
    int i_offset_for_ref_frame[256];

    int i_num_ref_frames;
    int b_gaps_in_frame_num_value_allowed;
    int i_mb_width;
    int i_mb_height;
    int b_frame_mbs_only;
    int b_mb_adaptive_frame_field;
    int b_direct8x8_inference;

    int b_crop;
    struct
    {
        int i_left;
        int i_right;
        int i_top;
        int i_bottom;
    } crop;

    int b_vui;
    struct
    {
        int b_aspect_ratio_info_present;
        int i_sar_width;
        int i_sar_height;
        
        int b_overscan_info_present;
        int b_overscan_info;

        int b_signal_type_present;
        int i_vidformat;
        int b_fullrange;
        int b_color_description_present;
        int i_colorprim;
        int i_transfer;
        int i_colmatrix;

        int b_chroma_loc_info_present;
        int i_chroma_loc_top;
        int i_chroma_loc_bottom;

        int b_timing_info_present;
        int i_num_units_in_tick;
        int i_time_scale;
        int b_fixed_frame_rate;

        int b_bitstream_restriction;
        int b_motion_vectors_over_pic_boundaries;
        int i_max_bytes_per_pic_denom;
        int i_max_bits_per_mb_denom;
        int i_log2_max_mv_length_horizontal;
        int i_log2_max_mv_length_vertical;
        int i_num_reorder_frames;
        int i_max_dec_frame_buffering;

        /* FIXME to complete */
    } vui;

    int b_qpprime_y_zero_transform_bypass;

} x264_sps_t;

/* nal */
enum nal_unit_type_e
{
    NAL_UNKNOWN = 0,
    NAL_SLICE   = 1,
    NAL_SLICE_DPA   = 2,
    NAL_SLICE_DPB   = 3,
    NAL_SLICE_DPC   = 4,
    NAL_SLICE_IDR   = 5,    /* ref_idc != 0 */
    NAL_SEI         = 6,    /* ref_idc == 0 */
    NAL_SPS         = 7,
    NAL_PPS         = 8,
    NAL_AUD         = 9,
    /* ref_idc == 0 for 6,9,10,11,12 */
};

/**
 * @}
 * @}
 * @defgroup lavu_picture Image related
 *
 * AVPicture types, pixel formats and basic image planes manipulation.
 *
 * @{
 */

enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0, ///< Undefined
    AV_PICTURE_TYPE_I,     ///< Intra
    AV_PICTURE_TYPE_P,     ///< Predicted
    AV_PICTURE_TYPE_B,     ///< Bi-dir predicted
    AV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG4
    AV_PICTURE_TYPE_SI,    ///< Switching Intra
    AV_PICTURE_TYPE_SP,    ///< Switching Predicted
    AV_PICTURE_TYPE_BI,    ///< BI type
};

const byte ff_h264_golomb_to_pict_type[5] = {
    AV_PICTURE_TYPE_P, AV_PICTURE_TYPE_B, AV_PICTURE_TYPE_I,
    AV_PICTURE_TYPE_SP, AV_PICTURE_TYPE_SI
};

static int x264_sps_read( bs_t *s, x264_sps_t *sps_array );
static int x264_pps_read( bs_t *s );
static int x264_slice_header_read( bs_t *s );

void GetResolution(const x264_sps_t * sps, int &width, int&height)
{
	static const int MB_SIZE = 16;
	width = sps->i_mb_width * MB_SIZE;
	//b_frame_mbs_only 0 : 场，1 : 帧
	height = (2 - sps->b_frame_mbs_only) * sps->i_mb_height * MB_SIZE;

	if (sps->b_crop)
	{
		int vsub = (sps->i_chroma_format_idc == 1) ? 1 : 0;
		int hsub = (sps->i_chroma_format_idc == 1 || sps->i_chroma_format_idc == 2) ? 1 : 0;
		int step_x = 1 << hsub;
		int step_y = (2 - sps->b_frame_mbs_only) << vsub;

		int left = sps->crop.i_left * step_x;
		int right = sps->crop.i_right * step_x;
		int top = sps->crop.i_top * step_y;
		int bottom = sps->crop.i_bottom * step_y;

		width -= (right - left);
		height -=(bottom - top);
	}
}

/************************************************************************
	函数：Getx264Info
	说明：从给定的数据中获取AVC信息
	参数：p_payload 要处理的数据
		  i_payload 数据长度
		  info		返回数据信息
	返回值：-1 失败，其它成功
	作者：wutong
	时间：2010-04-07
************************************************************************/
int     Getx264Info( byte* p_payload, int i_payload, VParam & info )
{
    int i_ret = -1;
    bs_t bs;
	bool bExit = false;

	x264_sps_t sps_array;
	byte* p_next = p_payload;
	do 
	{
		//先查起始标志
		do
		{
			if( p_next[0] == 0x00 && p_next[1] == 0x00 && p_next[2] == 0x01 )
			{
				break;
			}
			p_next++;
			if (p_next - p_payload >= i_payload)
			{
				bExit = true;
				break;
			}
		} while (true);

		if (bExit)
		{
			break;
		}

		byte i_type = p_next[3] & 0x1f;
		//byte i_ref_idc = (p_next[3] >> 5) & 0x03;

		/* init bitstream reader */
		byte* p_data = p_next + 4;
		int data_len = static_cast<int>(i_payload - (p_next - p_payload) - 4);		

		bs_init( &bs, p_data, data_len );

		switch( i_type )
		{
        case NAL_SPS: //zhaoliwei
            if( ( i_ret = x264_sps_read( &bs, &sps_array ) ) < 0 )
            {
                printf( "x264: x264_sps_read failed\n" );
            }
            else
            {
                GetResolution(&sps_array, info.i_width, info.i_height);
            }
            return i_ret;
 
        case NAL_PPS:
        case NAL_SLICE_IDR:
        case NAL_SLICE:
        case NAL_SLICE_DPA:
        case NAL_SLICE_DPB:
        case NAL_SLICE_DPC:
        case NAL_SEI:
        default:
			break;
		}
		p_next++;
		
	} while(true);

    return i_ret;
}

int     Getx264SliceCount( byte* p_payload, int i_payload, SliceCount & s_slice_cnt )
{
    int i_ret = -1;
    bs_t bs;
    bool bExit = false;

    byte* p_next = p_payload;
    do 
    {
        //先查起始标志
        do
        {
            if( p_next[0] == 0x00 && p_next[1] == 0x00 && p_next[2] == 0x01 )
            {
                break;
            }
            p_next++;
            if (p_next - p_payload >= i_payload)
            {
                bExit = true;
                break;
            }
        } while (true);

        if (bExit)
        {
            break;
        }

        byte i_type = p_next[3] & 0x1f;
        //byte i_ref_idc = (p_next[3] >> 5) & 0x03;

        /* init bitstream reader */
        byte* p_data = p_next + 4;
        int data_len = static_cast<int>(i_payload - (p_next - p_payload) - 4);		

        bs_init( &bs, p_data, data_len );

        switch( i_type )
        {
        case NAL_SLICE_IDR:
        case NAL_SLICE:
            {
                int slice_type = x264_slice_header_read( &bs );
                if (NAL_SLICE_IDR == i_type && AV_PICTURE_TYPE_I != slice_type)
                {
                    printf("Error ! IDR slice without AV_PICTURE_TYPE_I\n");
                }

                if (AV_PICTURE_TYPE_I == slice_type)
                {
                    s_slice_cnt.i_I_frame_cnt++;
                }
                else if (AV_PICTURE_TYPE_P == slice_type)
                {
                    s_slice_cnt.i_P_frame_cnt++;
                }
                else if (AV_PICTURE_TYPE_B == slice_type)
                {
                    s_slice_cnt.i_B_frame_cnt++;
                }
                else
                {
                    // TODO ...
                }
            }
            break;
        case NAL_SPS:
        case NAL_PPS:			
        case NAL_SLICE_DPA:
        case NAL_SLICE_DPB:
        case NAL_SLICE_DPC:
        case NAL_SEI:
        default:
            break;
        }
        p_next++;

    } while(true);

    return i_ret;
}

/* return -1 if invalid, else the id */
int x264_sps_read( bs_t *s, x264_sps_t *sps_array)
{
    x264_sps_t *sps;

    int i_profile_idc;
    int i_level_idc;

    int b_constraint_set0;
    int b_constraint_set1;
    int b_constraint_set2;

    int id;

    i_profile_idc     = bs_read( s, 8 );
    b_constraint_set0 = bs_read( s, 1 );
    b_constraint_set1 = bs_read( s, 1 );
    b_constraint_set2 = bs_read( s, 1 );

    bs_skip( s, 5 );    /* reserved */
    i_level_idc       = bs_read( s, 8 );


    id = bs_read_ue( s );
    if( bs_eof( s ) || id >= 32 )
    {
        /* the sps is invalid, no need to corrupt sps_array[0] */
        return -1;
    }

    sps = sps_array;
    sps->i_id = id;

    /* put pack parsed value */
    sps->i_profile_idc     = i_profile_idc;
    sps->i_level_idc       = i_level_idc;
    sps->b_constraint_set0 = b_constraint_set0;
    sps->b_constraint_set1 = b_constraint_set1;
    sps->b_constraint_set2 = b_constraint_set2;

    if(sps->i_profile_idc >= 100) //high profile
	{
		sps->i_chroma_format_idc = bs_read_ue(s);
        if(sps->i_chroma_format_idc == 3)	
			bs_read(s, 1);  //residual_color_transform_flag
		bs_read_ue(s);  //bit_depth_luma_minus8
		bs_read_ue(s);  //bit_depth_chroma_minus8
		sps->i_transform_bypass = bs_read(s, 1);
		sps->b_seq_scaling_matrix_preset_falg = bs_read(s, 1);

		if (1 == sps->b_seq_scaling_matrix_preset_falg)
		{
			int i_tmp;
		 for( int i = 0; i < ((3 != sps->i_chroma_format_idc) ? 8 : 12); i++ )
            {
                /* seq_scaling_list_present_flag[i] */
                i_tmp = bs_read( s, 1 );
                if( !i_tmp )
                    continue;
                const int i_size_of_scaling_list = (i < 6 ) ? 16 : 64;
                /* scaling_list (...) */
                int i_lastscale = 8;
                int i_nextscale = 8;
                for( int j = 0; j < i_size_of_scaling_list; j++ )
                {
                    if( i_nextscale != 0 )
                    {
                        /* delta_scale */
                        i_tmp = bs_read_se( s );
                        i_nextscale = ( i_lastscale + i_tmp + 256 ) % 256;
                        /* useDefaultScalingMatrixFlag = ... */
                    }
                    /* scalinglist[j] */
                    i_lastscale = ( i_nextscale == 0 ) ? i_lastscale : i_nextscale;
                }
            }
		}
		
		//decode_scaling_matrices(h, sps, NULL, 1, sps->scaling_matrix4, sps->scaling_matrix8);
    }else{
        sps->i_chroma_format_idc= 1;
    }

    sps->i_log2_max_frame_num = bs_read_ue( s ) + 4;

    sps->i_poc_type = bs_read_ue( s );
    if( sps->i_poc_type == 0 )
    {
        sps->i_log2_max_poc_lsb = bs_read_ue( s ) + 4;
    }
    else if( sps->i_poc_type == 1 )
    {
        int i;
        sps->b_delta_pic_order_always_zero = bs_read( s, 1 );
        sps->i_offset_for_non_ref_pic = bs_read_se( s );
        sps->i_offset_for_top_to_bottom_field = bs_read_se( s );
        sps->i_num_ref_frames_in_poc_cycle = bs_read_ue( s );
        if( sps->i_num_ref_frames_in_poc_cycle > 256 )
        {
            /* FIXME what to do */
            sps->i_num_ref_frames_in_poc_cycle = 256;
        }
        for( i = 0; i < sps->i_num_ref_frames_in_poc_cycle; i++ )
        {
            sps->i_offset_for_ref_frame[i] = bs_read_se( s );
        }
    }
    else if( sps->i_poc_type > 2 )
    {
        goto error;
    }

    sps->i_num_ref_frames = bs_read_ue( s );
    sps->b_gaps_in_frame_num_value_allowed = bs_read( s, 1 );
    sps->i_mb_width = bs_read_ue( s ) + 1;
    sps->i_mb_height= bs_read_ue( s ) + 1;
    sps->b_frame_mbs_only = bs_read( s, 1 );

	//后面的数据等什么时候关心，什么再处理吧。wutong , 2010-04-08 :)
     if( !sps->b_frame_mbs_only )
     {
         sps->b_mb_adaptive_frame_field = bs_read( s, 1 );
     }
     else
     {
         sps->b_mb_adaptive_frame_field = 0;
     }
     sps->b_direct8x8_inference = bs_read( s, 1 );
 
     sps->b_crop = bs_read( s, 1 );
     if( sps->b_crop )
     {
         sps->crop.i_left  = bs_read_ue( s );
         sps->crop.i_right = bs_read_ue( s );
         sps->crop.i_top   = bs_read_ue( s );
         sps->crop.i_bottom= bs_read_ue( s );
     }
     else
     {
         sps->crop.i_left  = 0;
         sps->crop.i_right = 0;
         sps->crop.i_top   = 0;
         sps->crop.i_bottom= 0;
     }
 
    return id;

error:
    /* invalidate this sps */
    sps->i_id = -1;
    return -1;
}

// 读取Picture parameter set
// 如果非法，返回0；否则返回图片中slice数目(num_slice_groups_minus1 + 1)
int x264_pps_read( bs_t *s )
{
	// 这里的数据，我们不关心，直接跳过
	bs_read_ue( s );		// pic_parameter_set_id
	bs_read_ue( s );		// seq_parameter_set_id 
	bs_read( s, 1 );		// entropy_coding_mode_flag
	bs_read( s, 1 );		// pic_order_present_flag
	////////////////////////////////////////////////////////////////////////////////
	// 唯一需要读入的参数
	int slice_per_frame = bs_read_ue( s ) + 1; // num_slice_groups_minus1

	// 后面的PPS参数，这里也不关心

	return slice_per_frame;
}

// 解析slice header （简单解析，只获得slice_type）；
// 返回slice_type
int x264_slice_header_read( bs_t *s )
{
	bs_read_ue( s );			// first_mb_in_slice
	int slice_type = bs_read_ue( s );	// slice_type
	//bs_read_ue( s );					// pic_parameter_set_id 
	//i_frame_num = bs_read( s, bits);	// frame_num
	// 后面数据不关心，不读取

    if (slice_type > 9) {
        printf("slice type is too large \n");
        return -1;
    }
    if (slice_type > 4) {
        slice_type -= 5;
    }

    slice_type = ff_h264_golomb_to_pict_type[slice_type];
    int slice_type_nos = slice_type & 3;

	return slice_type_nos;
}

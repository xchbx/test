// H264Parse.cpp : 定义控制台应用程序的入口点。
//

#include "patch/stdafx.h"
#include "patch/golomb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum {
	NAL_NULL=0,
	NAL_SLICE,
	NAL_DPA,
	NAL_DPB,
	NAL_DPC,
	NAL_IDR_SLICE,
	NAL_SEI,
	NAL_SPS,
	NAL_PPS,
	NAL_AUD,
	NAL_END_SEQUENCE,
	NAL_END_STREAM,
	NAL_FILLER_DATA,
	NAL_SPS_EXT,
	NAL_AUXILIARY_SLICE=19
};

enum AVColorPrimaries{
	AVCOL_PRI_BT709      =1, ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
	AVCOL_PRI_UNSPECIFIED=2,
	AVCOL_PRI_BT470M     =4,
	AVCOL_PRI_BT470BG    =5, ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
	AVCOL_PRI_SMPTE170M  =6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
	AVCOL_PRI_SMPTE240M  =7, ///< functionally identical to above
	AVCOL_PRI_FILM       =8,
	AVCOL_PRI_NB           , ///< Not part of ABI
};

enum AVColorTransferCharacteristic{
	AVCOL_TRC_BT709      =1, ///< also ITU-R BT1361
	AVCOL_TRC_UNSPECIFIED=2,
	AVCOL_TRC_GAMMA22    =4, ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
	AVCOL_TRC_GAMMA28    =5, ///< also ITU-R BT470BG
	AVCOL_TRC_NB           , ///< Not part of ABI
};

enum AVColorSpace{
	AVCOL_SPC_RGB        =0,
	AVCOL_SPC_BT709      =1, ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
	AVCOL_SPC_UNSPECIFIED=2,
	AVCOL_SPC_FCC        =4,
	AVCOL_SPC_BT470BG    =5, ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
	AVCOL_SPC_SMPTE170M  =6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
	AVCOL_SPC_SMPTE240M  =7,
	AVCOL_SPC_NB           , ///< Not part of ABI
};


#define MAX_SPS_COUNT  32
#define MAX_PICTURE_COUNT 32

#define EXTENDED_SAR  255


typedef struct AVRational{
	int num; ///< numerator
	int den; ///< denominator
} AVRational;

typedef struct SPS{

	int profile_idc;
	int level_idc;
	int chroma_format_idc;
	int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
	int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
	int poc_type;                      ///< pic_order_cnt_type
	int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
	int delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
	int ref_frame_count;               ///< num_ref_frames
	int gaps_in_frame_num_allowed_flag;
	int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
	int mb_height;                     ///< pic_height_in_map_units_minus1 + 1
	int frame_mbs_only_flag;
	int mb_aff;                        ///<mb_adaptive_frame_field_flag
	int direct_8x8_inference_flag;
	int crop;                   ///< frame_cropping_flag
	unsigned int crop_left;            ///< frame_cropping_rect_left_offset
	unsigned int crop_right;           ///< frame_cropping_rect_right_offset
	unsigned int crop_top;             ///< frame_cropping_rect_top_offset
	unsigned int crop_bottom;          ///< frame_cropping_rect_bottom_offset
	int vui_parameters_present_flag;
	AVRational sar;
	int video_signal_type_present_flag;
	int full_range;
	int colour_description_present_flag;
	enum AVColorPrimaries color_primaries;
	enum AVColorTransferCharacteristic color_trc;
	enum AVColorSpace colorspace;
	int timing_info_present_flag;
	uint32_t num_units_in_tick;
	uint32_t time_scale;
	int fixed_frame_rate_flag;
	short offset_for_ref_frame[256]; //FIXME dyn aloc?
	int bitstream_restriction_flag;
	int num_reorder_frames;
	int scaling_matrix_present;
	uint8_t scaling_matrix4[6][16];
	uint8_t scaling_matrix8[2][64];
	int nal_hrd_parameters_present_flag;
	int vcl_hrd_parameters_present_flag;
	int pic_struct_present_flag;
	int time_offset_length;
	int cpb_cnt;                       ///< See H.264 E.1.2
	int initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 +1
	int cpb_removal_delay_length;      ///< cpb_removal_delay_length_minus1 + 1
	int dpb_output_delay_length;       ///< dpb_output_delay_length_minus1 + 1
	int bit_depth_luma;                ///< bit_depth_luma_minus8 + 8
	int bit_depth_chroma;              ///< bit_depth_chroma_minus8 + 8
	int residual_color_transform_flag; ///< residual_colour_transform_flag
}SPS;

#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define LogH264 printf

static const AVRational pixel_aspect[17]={
	{0, 1},
	{1, 1},
	{12, 11},
	{10, 11},
	{16, 11},
	{40, 33},
	{24, 11},
	{20, 11},
	{32, 11},
	{80, 33},
	{18, 11},
	{15, 11},
	{64, 33},
	{160,99},
	{4, 3},
	{3, 2},
	{2, 1},
};

static int FindNAL(unsigned char *pData, unsigned int nEsLen, unsigned char *byState,
						 unsigned int *nStartPos, unsigned int *nEndPos )
{
	unsigned int nPos = 0;
	unsigned char *pEnd = pData + nEsLen;
	unsigned char byFlag = 0;

	while(pData + nPos + 6 < pEnd)
	{
		if((pData[nPos + 0] == 0x00) && (pData[nPos + 1] == 0x00) &&
			(pData[nPos + 2] == 0x00) && (pData[nPos + 3] == 0x01))
		{
			byFlag ++;

			if(1 == byFlag)
			{
				*byState = pData[nPos + 4] & 0x1f;
				*nStartPos = nPos + 5;
			}
			else if(2 == byFlag)
			{
				*nEndPos = nPos;
				return 1;
			}
		}
		nPos++;
	}

	if(1 == byFlag)
	{
		*nEndPos = nEsLen;
		return 1;
	}

	return 0;
}

static int decode_hrd_parameters(GetBitContext *gb, SPS *sps)
{
	int cpb_count, i;
	cpb_count = get_ue_golomb_31(gb) + 1;

	if(cpb_count > 32U){
		LogH264("cpb_count %d invalid\n", cpb_count);
		return -1;
	}

	get_bits(gb, 4); /* bit_rate_scale */
	get_bits(gb, 4); /* cpb_size_scale */
	for(i=0; i<cpb_count; i++){
		get_ue_golomb(gb); /* bit_rate_value_minus1 */
		get_ue_golomb(gb); /* cpb_size_value_minus1 */
		get_bits1(gb);     /* cbr_flag */
	}
	sps->initial_cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->dpb_output_delay_length = get_bits(gb, 5) + 1;
	sps->time_offset_length = get_bits(gb, 5);
	sps->cpb_cnt = cpb_count;
	return 0;
}

static int decode_vui_parameters(GetBitContext *gb, SPS *sps)
{
	int aspect_ratio_info_present_flag;
	unsigned int aspect_ratio_idc;

	aspect_ratio_info_present_flag= get_bits1(gb);

	if( aspect_ratio_info_present_flag ) {
		aspect_ratio_idc= get_bits(gb, 8);
		if( aspect_ratio_idc == EXTENDED_SAR ) {
			sps->sar.num= get_bits(gb, 16);
			sps->sar.den= get_bits(gb, 16);
		}else if(aspect_ratio_idc < FF_ARRAY_ELEMS(pixel_aspect)){
			sps->sar=  pixel_aspect[aspect_ratio_idc];
		}else{
			LogH264("illegal aspect ratio\n");
			return -1;
		}
	}else{
		sps->sar.num=
			sps->sar.den= 0;
	}
	//            s->avctx->aspect_ratio= sar_width*s->width / (float)(s->height*sar_height);

	if(get_bits1(gb)){      /* overscan_info_present_flag */
		get_bits1(gb);      /* overscan_appropriate_flag */
	}

	sps->video_signal_type_present_flag = get_bits1(gb);
	if(sps->video_signal_type_present_flag){
		get_bits(gb, 3);    /* video_format */
		sps->full_range = get_bits1(gb); /* video_full_range_flag */

		sps->colour_description_present_flag = get_bits1(gb);
		if(sps->colour_description_present_flag)
		{
			sps->color_primaries = get_bits(gb, 8); /* colour_primaries */
			sps->color_trc       = get_bits(gb, 8); /* transfer_characteristics */
			sps->colorspace      = get_bits(gb, 8); /* matrix_coefficients */
			if (sps->color_primaries >= AVCOL_PRI_NB)
				sps->color_primaries  = AVCOL_PRI_UNSPECIFIED;
			if (sps->color_trc >= AVCOL_TRC_NB)
				sps->color_trc  = AVCOL_TRC_UNSPECIFIED;
			if (sps->colorspace >= AVCOL_SPC_NB)
				sps->colorspace  = AVCOL_SPC_UNSPECIFIED;
		}
	}

	if(get_bits1(gb)){      /* chroma_location_info_present_flag */
		unsigned int nChroma_sample_location = get_ue_golomb(gb)+1;  /* chroma_sample_location_type_top_field */
		get_ue_golomb(gb);  /* chroma_sample_location_type_bottom_field */
	}

	sps->timing_info_present_flag = get_bits1(gb);
	if(sps->timing_info_present_flag){
		sps->num_units_in_tick = get_bits_long(gb, 32);
		sps->time_scale = get_bits_long(gb, 32);
		if(!sps->num_units_in_tick || !sps->time_scale){
			LogH264("time_scale/num_units_in_tick invalid or unsupported (%d/%d)\n", sps->time_scale, sps->num_units_in_tick);
			return -1;
		}
		sps->fixed_frame_rate_flag = get_bits1(gb);
	}

	sps->nal_hrd_parameters_present_flag = get_bits1(gb);
	if(sps->nal_hrd_parameters_present_flag)
		if(decode_hrd_parameters(gb, sps) < 0)
			return -1;
	sps->vcl_hrd_parameters_present_flag = get_bits1(gb);
	if(sps->vcl_hrd_parameters_present_flag)
		if(decode_hrd_parameters(gb, sps) < 0)
			return -1;
	if(sps->nal_hrd_parameters_present_flag || sps->vcl_hrd_parameters_present_flag)
		get_bits1(gb);     /* low_delay_hrd_flag */
	sps->pic_struct_present_flag = get_bits1(gb);

	//
	// Start modifying max_dec_frame_buffering.
	// First let's backup gb in case of no bitstream_restriction_flag.
	GetBitContext gbk = *gb;
	sps->bitstream_restriction_flag = get_bits1(gb);

	if(!sps->bitstream_restriction_flag) // No bitstream_restriction_flag, we simply add one.
	{
		// Restore gb.
		*gb = gbk;

		// Prepare PutBitContext.
		PutBitContext pb;
		init_put_bits(&pb, (uint8_t*)gb->buffer, gb->buffer_end - gb->buffer);
		pb.buf_ptr = pb.buf + (gb->index >> 3);
		pb.bit_left = 32 - (gb->index & 7);
		pb.bit_buf = *pb.buf_ptr;

		put_bits(&pb, 1, 1); /* bitstream_restriction_flag */
		put_bits(&pb, 1, 0); /* motion_vectors_over_pic_boundaries_flag */
		set_ue_golomb(&pb, 0); /* max_bytes_per_pic_denom */
		set_ue_golomb(&pb, 0); /* max_bits_per_mb_denom */
		set_ue_golomb(&pb, 16); /* log2_max_mv_length_horizontal */
		set_ue_golomb(&pb, 16); /* log2_max_mv_length_vertical */
		set_ue_golomb(&pb, 0); /* num_reorder_frames */
		set_ue_golomb(&pb, 1); /* max_dec_frame_buffering */
		flush_put_bits(&pb);

		// Let gb catch up.
		//printf("bitstream_restriction_flag:%d\n", get_bits1(gb));     /* motion_vectors_over_pic_boundaries_flag */
		//printf("motion_vectors_flag:%d\n", get_bits1(gb));     /* motion_vectors_over_pic_boundaries_flag */
		//printf("max_bytes_per_pic_denom:%d\n", get_ue_golomb(gb)); /* max_bytes_per_pic_denom */
		//printf("max_bits_per_mb_denom:%d\n", get_ue_golomb(gb)); /* max_bits_per_mb_denom */
		//printf("log2_max_mv_length_horizontal:%d\n", get_ue_golomb(gb)); /* log2_max_mv_length_horizontal */
		//printf("log2_max_mv_length_vertical:%d\n", get_ue_golomb(gb)); /* log2_max_mv_length_vertical */
		//printf("num_reorder_frames:%d\n", get_ue_golomb(gb)); /* num_reorder_frames */
		get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
		get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
		get_ue_golomb(gb); /* max_bytes_per_pic_denom */
		get_ue_golomb(gb); /* max_bits_per_mb_denom */
		get_ue_golomb(gb); /* log2_max_mv_length_horizontal */
		get_ue_golomb(gb); /* log2_max_mv_length_vertical */
		get_ue_golomb(gb); /* num_reorder_frames */
		//get_ue_golomb(gb); /*max_dec_frame_buffering*/
	}
	else // Modify existing max_dec_frame_buffering.
	{
		get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
		get_ue_golomb(gb); /* max_bytes_per_pic_denom */
		get_ue_golomb(gb); /* max_bits_per_mb_denom */
		get_ue_golomb(gb); /* log2_max_mv_length_horizontal */
		get_ue_golomb(gb); /* log2_max_mv_length_vertical */
		sps->num_reorder_frames= get_ue_golomb(gb);
		//
		// Change max_dec_frame_buffering in-place.
		// Since we are to set max_dec_frame_buffering to 1,
		// we can assure the buffer is sufficient.
		PutBitContext pb;
		init_put_bits(&pb, (uint8_t*)gb->buffer, gb->buffer_end - gb->buffer);
		pb.buf_ptr = pb.buf + (gb->index >> 3);
		pb.bit_left = 32 - (gb->index & 7);
		pb.bit_buf = *pb.buf_ptr;
		set_ue_golomb(&pb, 1);
		flush_put_bits(&pb);
	}

	int max_dec_frame_buffering = get_ue_golomb(gb); /*max_dec_frame_buffering*/
//printf("MDFB: %d\n", max_dec_frame_buffering);

	if(gb->size_in_bits < get_bits_count(gb)){
		LogH264("Overread VUI by %d bits\n", get_bits_count(gb) - gb->size_in_bits);
		sps->num_reorder_frames=0;
		sps->bitstream_restriction_flag= 0;
	}

	if(sps->num_reorder_frames > 16U /*max_dec_frame_buffering || max_dec_frame_buffering > 16*/){
		LogH264("illegal num_reorder_frames %d\n", sps->num_reorder_frames);
		return -1;
	}

	return 0;
}

static unsigned int DecSPS(unsigned char *pData, unsigned int nLen, unsigned int *nRealLen)
{
	int profile_idc, level_idc;
	unsigned int sps_id;
	GetBitContext gb;
	PutBitContext pb;

	SPS *sps;
	int i;

	*nRealLen = 0;

	init_get_bits(&gb, pData, nLen * 8);

	profile_idc = get_bits(&gb, 8);
	//put_bits(&pb, 8, profile_idc);
	get_bits1(&gb);   //constraint_set0_flag
	get_bits1(&gb);   //constraint_set1_flag
	get_bits1(&gb);   //constraint_set2_flag
	get_bits1(&gb);   //constraint_set3_flag
	get_bits(&gb, 4); // reserved
	level_idc= get_bits(&gb, 8);

	sps_id= get_ue_golomb_31(&gb);

	if(sps_id >= MAX_SPS_COUNT) {
		//		av_log(h->s.avctx, AV_LOG_ERROR, "sps_id (%d) out of range\n", sps_id);
		return -1;
	}

	sps = (SPS *)malloc(sizeof(SPS));
	if(sps == NULL)
	{
		return -1;
	}

	memset(sps, 0x00, sizeof(SPS));

	sps->profile_idc= profile_idc;
	sps->level_idc= level_idc;

	memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
	memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
	sps->scaling_matrix_present = 0;

	if(sps->profile_idc >= 100){ //high profile
		sps->chroma_format_idc= get_ue_golomb_31(&gb);
		if(sps->chroma_format_idc == 3)
			sps->residual_color_transform_flag = get_bits1(&gb);
		sps->bit_depth_luma   = get_ue_golomb(&gb) + 8;
		sps->bit_depth_chroma = get_ue_golomb(&gb) + 8;
		sps->transform_bypass = get_bits1(&gb);
		//		decode_scaling_matrices(h, sps, NULL, 1, sps->scaling_matrix4, sps->scaling_matrix8);
	}else{
		sps->chroma_format_idc= 1;
		sps->bit_depth_luma   = 8;
		sps->bit_depth_chroma = 8;
	}

	sps->log2_max_frame_num= get_ue_golomb(&gb) + 4;
	sps->poc_type= get_ue_golomb_31(&gb);

	if(sps->poc_type == 0){ //FIXME #define
		sps->log2_max_poc_lsb= get_ue_golomb(&gb) + 4;
	} else if(sps->poc_type == 1){//FIXME #define
		sps->delta_pic_order_always_zero_flag= get_bits1(&gb);
		sps->offset_for_non_ref_pic= get_se_golomb(&gb);
		sps->offset_for_top_to_bottom_field= get_se_golomb(&gb);
		sps->poc_cycle_length                = get_ue_golomb(&gb);

		if((unsigned)sps->poc_cycle_length >= FF_ARRAY_ELEMS(sps->offset_for_ref_frame)){
			LogH264("poc_cycle_length overflow %u\n", sps->poc_cycle_length);
			return -1;
		}

		for(i=0; i<sps->poc_cycle_length; i++)
			sps->offset_for_ref_frame[i]= get_se_golomb(&gb);
	}else if(sps->poc_type != 2){
		LogH264("illegal POC type %d\n", sps->poc_type);
		return -1;
	}

	sps->ref_frame_count= get_ue_golomb_31(&gb);
	if(sps->ref_frame_count > MAX_PICTURE_COUNT - 2 || sps->ref_frame_count >= 32U){
		LogH264("too many reference frames\n");
		return -1;
	}
	sps->gaps_in_frame_num_allowed_flag= get_bits1(&gb);
	sps->mb_width = get_ue_golomb(&gb) + 1;
	sps->mb_height= get_ue_golomb(&gb) + 1;
	if((unsigned)sps->mb_width >= INT_MAX/16 || (unsigned)sps->mb_height >= INT_MAX/16)
	{
		LogH264("mb_width/height overflow\n");
		return -1;
	}

	sps->frame_mbs_only_flag= get_bits1(&gb);
	if(!sps->frame_mbs_only_flag)
		sps->mb_aff= get_bits1(&gb);
	else
		sps->mb_aff= 0;

	sps->direct_8x8_inference_flag= get_bits1(&gb);
	if(!sps->frame_mbs_only_flag && !sps->direct_8x8_inference_flag){
		LogH264("This stream was generated by a broken encoder, invalid 8x8 inference\n");
		return -1;
	}

	sps->crop= get_bits1(&gb);
	if(sps->crop){
		sps->crop_left  = get_ue_golomb(&gb);
		sps->crop_right = get_ue_golomb(&gb);
		sps->crop_top   = get_ue_golomb(&gb);
		sps->crop_bottom= get_ue_golomb(&gb);
		if(sps->crop_left || sps->crop_top){
			LogH264("insane cropping not completely supported, this could look slightly wrong ...\n");
		}
		if(sps->crop_right >= 8 || sps->crop_bottom >= (8>> !sps->frame_mbs_only_flag)){
			LogH264("brainfart cropping not supported, this could look slightly wrong ...\n");
		}
	}else{
		sps->crop_left  =
			sps->crop_right =
			sps->crop_top   =
			sps->crop_bottom= 0;
	}

	sps->vui_parameters_present_flag= get_bits1(&gb);
	if( sps->vui_parameters_present_flag )
	{
		decode_vui_parameters(&gb, sps); 
	}

	// Output data length.
	*nRealLen = (gb.index >> 3);
	if (gb.index & 7)
	{
		*nRealLen ++;
	}

	return 1;
}

unsigned char* EsPatch(unsigned char *pData, unsigned int nEsLen, unsigned int *nOutLen)
{
	unsigned char byType;
	unsigned int nStartPos, nEndPos;
	unsigned char* pMove = pData;
	int nLeftLen = nEsLen;

	// FIXME: enough?
	unsigned char *pOut = (unsigned char*) malloc(nEsLen + 1024);
    *nOutLen = 0;
	int nLen = 0;

	nStartPos = nEndPos = 0;
	while(nLeftLen > 0)
	{
		int bRet;
		bRet = FindNAL(pMove, nLeftLen, &byType, &nStartPos, &nEndPos);

		if(!bRet)
		{
			break;
		}

		switch(byType)
		{
		case NAL_IDR_SLICE:	//IDR图像的片
			//DecSlice(pMove + nStartPos, nEndPos - nStartPos);
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		case NAL_SLICE:		//非IDR图像的片
			//DecSlice(pMove + nStartPos, nEndPos - nStartPos);
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		case NAL_SPS:		//序列参数集
			{
				unsigned int nRealLen;
				memcpy(pOut + nLen, pMove, nEndPos);
				// FIXME: use a more reliable buffer length or use realloc.
				DecSPS(pOut + nLen + nStartPos , nEndPos - nStartPos + 16, &nRealLen);
				nLen += (nStartPos + nRealLen);
			}
			break;
		case NAL_PPS:		//图像参数集
			//DecPPS(pMove + nStartPos, nEndPos - nStartPos);
			//VideoEs2Ts((char *)pData, nPts, nEsLen);
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		case NAL_SEI:		//补充增强信息单元
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		case NAL_AUD:
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		default:
			memcpy(pOut + nLen, pMove, nEndPos);
			nLen += nEndPos;
			break;
		}

		pMove += nEndPos;
		nLeftLen -= nEndPos;

	}

    *nOutLen = nLen;
	return pOut;
}


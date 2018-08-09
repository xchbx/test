#include "xavs.h"
#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_dsp_mmx.h"


const DECLARE_ALIGNED(8,uint64_t,gs_N96) = 0x0060006000600060ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N42) = 0x002A002A002A002AULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N7)  = 0x0007000700070007ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N16) = 0x0010001000100010ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N32) = 0x0020002000200020ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N5)  = 0x0005000500050005ULL;

const DECLARE_ALIGNED(8,uint64_t,gs_N4)  = 0x0004000400040004ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N64) = 0x0040004000400040ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N1)    = 0x0001000100010001ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_N2)    = 0x0002000200020002ULL;
const DECLARE_ALIGNED(8,uint64_t,gs_D_N512) = 0x0000020000000200ULL;

DECLARE_ALIGNED(8, uint8_t, chroma_mc_xy_b_table)[256] =
{
	0x40,0x00,0x00,0x00,
	0x38,0x08,0x00,0x00,
	0x30,0x10,0x00,0x00,
	0x28,0x18,0x00,0x00,
	0x20,0x20,0x00,0x00,
	0x18,0x28,0x00,0x00,
	0x10,0x30,0x00,0x00,
	0x08,0x38,0x00,0x00,
	0x38,0x00,0x08,0x00,
	0x31,0x07,0x07,0x01,
	0x2A,0x0E,0x06,0x02,
	0x23,0x15,0x05,0x03,
	0x1C,0x1C,0x04,0x04,
	0x15,0x23,0x03,0x05,
	0x0E,0x2A,0x02,0x06,
	0x07,0x31,0x01,0x07,
	0x30,0x00,0x10,0x00,
	0x2A,0x06,0x0E,0x02,
	0x24,0x0C,0x0C,0x04,
	0x1E,0x12,0x0A,0x06,
	0x18,0x18,0x08,0x08,
	0x12,0x1E,0x06,0x0A,
	0x0C,0x24,0x04,0x0C,
	0x06,0x2A,0x02,0x0E,
	0x28,0x00,0x18,0x00,
	0x23,0x05,0x15,0x03,
	0x1E,0x0A,0x12,0x06,
	0x19,0x0F,0x0F,0x09,
	0x14,0x14,0x0C,0x0C,
	0x0F,0x19,0x09,0x0F,
	0x0A,0x1E,0x06,0x12,
	0x05,0x23,0x03,0x15,
	0x20,0x00,0x20,0x00,
	0x1C,0x04,0x1C,0x04,
	0x18,0x08,0x18,0x08,
	0x14,0x0C,0x14,0x0C,
	0x10,0x10,0x10,0x10,
	0x0C,0x14,0x0C,0x14,
	0x08,0x18,0x08,0x18,
	0x04,0x1C,0x04,0x1C,
	0x18,0x00,0x28,0x00,
	0x15,0x03,0x23,0x05,
	0x12,0x06,0x1E,0x0A,
	0x0F,0x09,0x19,0x0F,
	0x0C,0x0C,0x14,0x14,
	0x09,0x0F,0x0F,0x19,
	0x06,0x12,0x0A,0x1E,
	0x03,0x15,0x05,0x23,
	0x10,0x00,0x30,0x00,
	0x0E,0x02,0x2A,0x06,
	0x0C,0x04,0x24,0x0C,
	0x0A,0x06,0x1E,0x12,
	0x08,0x08,0x18,0x18,
	0x06,0x0A,0x12,0x1E,
	0x04,0x0C,0x0C,0x24,
	0x02,0x0E,0x06,0x2A,
	0x08,0x00,0x38,0x00,
	0x07,0x01,0x31,0x07,
	0x06,0x02,0x2A,0x0E,
	0x05,0x03,0x23,0x15,
	0x04,0x04,0x1C,0x1C,
	0x03,0x05,0x15,0x23,
	0x02,0x06,0x0E,0x2A,
	0x01,0x07,0x07,0x31
};

#include <mmintrin.h> //mmx
#include <xmmintrin.h> //sse
#include <emmintrin.h> //sse2
#include <pmmintrin.h> //sse3
#include <smmintrin.h> //sse4.1
#include <nmmintrin.h> //sse4.2
//#include <ammintrin.h> //sse4a
//#include <x86intrin.h> //avx, avx2 etc
#ifdef __CONFIG_MMX
const __m64 __m_0 = _mm_setzero_si64();
const __m64 __m_w_16 = _mm_set1_pi16(16);
const __m64 __m_w_64 = _mm_set1_pi16(64);
const __m64 __m_w_32 = _mm_set1_pi16(32);

const __m64 __m_w_3 = _mm_set1_pi16(3);

const __m64 __m_w_1 = _mm_set1_pi16(1);
const __m64 __m_w_2 = _mm_set1_pi16(2);
const __m64 __m_w_96 = _mm_set1_pi16(96);
const __m64 __m_w_42 = _mm_set1_pi16(42);
const __m64 __m_w_7 = _mm_set1_pi16(7);

const __m64 __m_w_5 = _mm_set1_pi16(5);
const __m64 __m_w_4 = _mm_set1_pi16(4);
const __m64 __m_w_0x8000 = _mm_set1_pi16((short)0x8000);
const __m64 __m_w_0x7FFF = _mm_set1_pi16((short)0x7FFF);
const __m64 __m_q_F16 = {0xFFFFFFFFFFFFFFFF};

const __m64 __m_d_512 = _mm_set1_pi32(512);

#endif

#include "ccpuid.h"
#include "sse2/xavs_dsp_idct_sse2.hpp"
#include "sse2/xavs_dsp_intra_pred_sse2.hpp"
#ifdef __CONFIG_MMX
#include "mmx/xavs_dsp_idct_mmx.hpp"
#include "mmx/xavs_dsp_intra_pred_mmx.hpp"

#define __LOOP_FILTER_H
//#include "sse2/xavs_dsp_loop_filter_sse2.hpp"
#include "mmx/xavs_dsp_loop_filter_mmx.hpp"
#undef __LOOP_FILTER_H

#define __LOOP_FILTER_V
//#include "sse2/xavs_dsp_loop_filter_sse2.hpp"
#include "mmx/xavs_dsp_loop_filter_mmx.hpp"
#undef __LOOP_FILTER_V

#endif

void init_dsp_mmx()
{

#ifdef __CONFIG_MMX
	xavs_filter_lv = xavs_filter_lv_mmx;
    xavs_filter_lh = xavs_filter_lh_mmx;
    xavs_filter_cv = xavs_filter_cv_mmx;
    xavs_filter_ch = xavs_filter_ch_mmx;
	
	xavs_idct8_add = xavs_idct8_add_mmx;

	xavs_intra_pred_lum[INTRA_L_VERT]=xavs_intra_pred_vertical_mmx;
	xavs_intra_pred_lum[INTRA_L_HORIZ]=xavs_intra_pred_horizontal_mmx;
	xavs_intra_pred_lum[INTRA_L_LP]=xavs_intra_pred_dc_lp_mmx;
	xavs_intra_pred_lum[INTRA_L_DOWN_LEFT]=xavs_intra_pred_down_left_mmx;
	xavs_intra_pred_lum[INTRA_L_DOWN_RIGHT]=xavs_intra_pred_down_right_mmx;
	xavs_intra_pred_lum[INTRA_L_LP_LEFT]=xavs_intra_pred_dc_lp_left_mmx;
	xavs_intra_pred_lum[NTRA_L_LP_TOP]=xavs_intra_pred_dc_lp_top_mmx;
	xavs_intra_pred_lum[INTRA_L_DC_128]=xavs_intra_pred_dc_128_mmx;
	xavs_intra_pred_chroma[INTRA_C_LP]=xavs_intra_pred_dc_lp_mmx;
	xavs_intra_pred_chroma[INTRA_C_HORIZ]=xavs_intra_pred_horizontal_mmx;
	xavs_intra_pred_chroma[INTRA_C_VERT]=xavs_intra_pred_vertical_mmx;
	xavs_intra_pred_chroma[INTRA_C_PLANE]=xavs_intra_pred_plane_mmx;
	xavs_intra_pred_chroma[INTRA_C_LP_LEFT]=xavs_intra_pred_dc_lp_left_mmx;
	xavs_intra_pred_chroma[INTRA_C_LP_TOP]=xavs_intra_pred_dc_lp_top_mmx;
	xavs_intra_pred_chroma[INTRA_C_DC_128]=xavs_intra_pred_dc_128_mmx;
#endif
	init_dsp_mc_mmx();
	init_dsp_mc_w_mmx();
	
#if 1
	int	nhwsse = 0;	// Ó²¼þÖ§³ÖSSE.
	int nsse = simd_sse_level(&nhwsse);
	if(nhwsse)
	{
		printf("********** CPU support SSE; level = %d **********\n", nsse);
	}
	else
	{
		printf("********** CPU not support SSE **********\n");
	}

	if(nhwsse && nsse >= SIMD_SSE_41)
	{
		init_dsp_sse2();
	}
#else
	init_dsp_sse2();
#endif
}


void init_dsp_sse2()
{

	
/*	xavs_filter_lv = xavs_filter_lv_mmx;
    xavs_filter_lh = xavs_filter_lh_mmx;
    xavs_filter_cv = xavs_filter_cv_mmx;
    xavs_filter_ch = xavs_filter_ch_mmx;
*/	
	xavs_idct8_add = xavs_idct8_add_sse2;

	xavs_intra_pred_lum[INTRA_L_VERT]=xavs_intra_pred_vertical_sse2;
	xavs_intra_pred_lum[INTRA_L_HORIZ]=xavs_intra_pred_horizontal_sse2;
	xavs_intra_pred_lum[INTRA_L_LP]=xavs_intra_pred_dc_lp_sse2;
	xavs_intra_pred_lum[INTRA_L_DOWN_LEFT]=xavs_intra_pred_down_left_sse2;
	xavs_intra_pred_lum[INTRA_L_DOWN_RIGHT]=xavs_intra_pred_down_right_sse2;
	xavs_intra_pred_lum[INTRA_L_LP_LEFT]=xavs_intra_pred_dc_lp_left_sse2;
	xavs_intra_pred_lum[NTRA_L_LP_TOP]=xavs_intra_pred_dc_lp_top_sse2;
	xavs_intra_pred_lum[INTRA_L_DC_128]=xavs_intra_pred_dc_128_sse2;
	xavs_intra_pred_chroma[INTRA_C_LP]=xavs_intra_pred_dc_lp_sse2;
	xavs_intra_pred_chroma[INTRA_C_HORIZ]=xavs_intra_pred_horizontal_sse2;
	xavs_intra_pred_chroma[INTRA_C_VERT]=xavs_intra_pred_vertical_sse2;
	xavs_intra_pred_chroma[INTRA_C_PLANE]=xavs_intra_pred_plane_sse2;
	xavs_intra_pred_chroma[INTRA_C_LP_LEFT]=xavs_intra_pred_dc_lp_left_sse2;
	xavs_intra_pred_chroma[INTRA_C_LP_TOP]=xavs_intra_pred_dc_lp_top_sse2;
	xavs_intra_pred_chroma[INTRA_C_DC_128]=xavs_intra_pred_dc_128_sse2;

	init_dsp_mc_sse2();
	init_dsp_mc_w_sse2();
}

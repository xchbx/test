#include "xavs.h"
#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_dsp_mmx.h"
extern DECLARE_ALIGNED(8, uint8_t, chroma_mc_xy_b_table)[256];

#define __MC_CHROMA_FUN_W
#ifdef __CONFIG_MMX
#include "mmx/xavs_dsp_mc_chroma_mmx.hpp"
#endif
#include "sse2/xavs_dsp_mc_chroma_sse2.hpp"
#undef __MC_CHROMA_FUN_W

#define __MC0_FUN_W
#ifdef __CONFIG_MMX
#include "mmx/xavs_dsp_mc_luma_0_mmx.hpp"
#endif
#include "sse2/xavs_dsp_mc_luma_0_sse2.hpp"
#undef __MC0_FUN_W

#define __MC_PUT_FUN_W
#ifdef __CONFIG_MMX
#include "mmx/xavs_dsp_mc_luma_mmx.hpp"
#endif
#include "sse2/xavs_dsp_mc_luma_sse2.hpp"
#undef __MC_PUT_FUN_W

#define __MC_AVG_FUN_W
#ifdef __CONFIG_MMX
#include "mmx/xavs_dsp_mc_luma_mmx.hpp"
#endif
#include "sse2/xavs_dsp_mc_luma_sse2.hpp"
#undef __MC_AVG_FUN_W



void init_dsp_mc_w_mmx()
{
#ifdef __CONFIG_MMX
	xavs_chroma_put_w[0]=xavs_chroma_mc8_put_mmx;
	xavs_chroma_put_w[1]=xavs_chroma_mc4_put_mmx;
	//xavs_chroma_put_w[2]=xavs_chroma_mc2_put_mmx;
    xavs_chroma_avg_w[0]=xavs_chroma_mc8_avg_mmx;
	xavs_chroma_avg_w[1]=xavs_chroma_mc4_avg_mmx;
	//xavs_chroma_avg_w[2]=xavs_chroma_mc2_avg_mmx;


	 xavs_luma_put_w[0][ 0] =  xavs_qpel16_put_mc00_mmx;
	 xavs_luma_put_w[0][ 1] =  xavs_qpel16_put_mc10_mmx;
	 xavs_luma_put_w[0][ 2] =  xavs_qpel16_put_mc20_mmx;
	 xavs_luma_put_w[0][ 3] =  xavs_qpel16_put_mc30_mmx;
	 xavs_luma_put_w[0][ 4] =  xavs_qpel16_put_mc01_mmx;
	 xavs_luma_put_w[0][ 5] =  xavs_qpel16_put_mc11_mmx;
	 xavs_luma_put_w[0][ 6] =  xavs_qpel16_put_mc21_mmx;
	 xavs_luma_put_w[0][ 7] =  xavs_qpel16_put_mc31_mmx;
	 xavs_luma_put_w[0][ 8] =  xavs_qpel16_put_mc02_mmx;
	 xavs_luma_put_w[0][ 9] =  xavs_qpel16_put_mc12_mmx;
	 xavs_luma_put_w[0][10] =  xavs_qpel16_put_mc22_mmx;
	 xavs_luma_put_w[0][11] =  xavs_qpel16_put_mc32_mmx;
	 xavs_luma_put_w[0][12] =  xavs_qpel16_put_mc03_mmx;
	 xavs_luma_put_w[0][13] =  xavs_qpel16_put_mc13_mmx;
	 xavs_luma_put_w[0][14] =  xavs_qpel16_put_mc23_mmx;
	 xavs_luma_put_w[0][15] =  xavs_qpel16_put_mc33_mmx;

	 xavs_luma_put_w[1][ 0] =  xavs_qpel8_put_mc00_mmx;
	 xavs_luma_put_w[1][ 1] =  xavs_qpel8_put_mc10_mmx;
	 xavs_luma_put_w[1][ 2] =  xavs_qpel8_put_mc20_mmx;
	 xavs_luma_put_w[1][ 3] =  xavs_qpel8_put_mc30_mmx;
	 xavs_luma_put_w[1][ 4] =  xavs_qpel8_put_mc01_mmx;
	 xavs_luma_put_w[1][ 5] =  xavs_qpel8_put_mc11_mmx;
	 xavs_luma_put_w[1][ 6] =  xavs_qpel8_put_mc21_mmx;
	 xavs_luma_put_w[1][ 7] =  xavs_qpel8_put_mc31_mmx;
	 xavs_luma_put_w[1][ 8] =  xavs_qpel8_put_mc02_mmx;
	 xavs_luma_put_w[1][ 9] =  xavs_qpel8_put_mc12_mmx;
	 xavs_luma_put_w[1][10] =  xavs_qpel8_put_mc22_mmx;
	 xavs_luma_put_w[1][11] =  xavs_qpel8_put_mc32_mmx;
	 xavs_luma_put_w[1][12] =  xavs_qpel8_put_mc03_mmx;
	 xavs_luma_put_w[1][13] =  xavs_qpel8_put_mc13_mmx;
	 xavs_luma_put_w[1][14] =  xavs_qpel8_put_mc23_mmx;
	 xavs_luma_put_w[1][15] =  xavs_qpel8_put_mc33_mmx;

	 xavs_luma_avg_w[0][ 0] =  xavs_qpel16_avg_mc00_mmx;
	 xavs_luma_avg_w[0][ 1] =  xavs_qpel16_avg_mc10_mmx;
	 xavs_luma_avg_w[0][ 2] =  xavs_qpel16_avg_mc20_mmx;
	 xavs_luma_avg_w[0][ 3] =  xavs_qpel16_avg_mc30_mmx;
	 xavs_luma_avg_w[0][ 4] =  xavs_qpel16_avg_mc01_mmx;
	 xavs_luma_avg_w[0][ 5] =  xavs_qpel16_avg_mc11_mmx;
	 xavs_luma_avg_w[0][ 6] =  xavs_qpel16_avg_mc21_mmx;
	 xavs_luma_avg_w[0][ 7] =  xavs_qpel16_avg_mc31_mmx;
	 xavs_luma_avg_w[0][ 8] =  xavs_qpel16_avg_mc02_mmx;
	 xavs_luma_avg_w[0][ 9] =  xavs_qpel16_avg_mc12_mmx;
	 xavs_luma_avg_w[0][10] =  xavs_qpel16_avg_mc22_mmx;
	 xavs_luma_avg_w[0][11] =  xavs_qpel16_avg_mc32_mmx;
	 xavs_luma_avg_w[0][12] =  xavs_qpel16_avg_mc03_mmx;
	 xavs_luma_avg_w[0][13] =  xavs_qpel16_avg_mc13_mmx;
	 xavs_luma_avg_w[0][14] =  xavs_qpel16_avg_mc23_mmx;
	 xavs_luma_avg_w[0][15] =  xavs_qpel16_avg_mc33_mmx;

	 xavs_luma_avg_w[1][ 0] =  xavs_qpel8_avg_mc00_mmx;
	 xavs_luma_avg_w[1][ 1] =  xavs_qpel8_avg_mc10_mmx;
	 xavs_luma_avg_w[1][ 2] =  xavs_qpel8_avg_mc20_mmx;
	 xavs_luma_avg_w[1][ 3] =  xavs_qpel8_avg_mc30_mmx;
	 xavs_luma_avg_w[1][ 4] =  xavs_qpel8_avg_mc01_mmx;
	 xavs_luma_avg_w[1][ 5] =  xavs_qpel8_avg_mc11_mmx;
	 xavs_luma_avg_w[1][ 6] =  xavs_qpel8_avg_mc21_mmx;
	 xavs_luma_avg_w[1][ 7] =  xavs_qpel8_avg_mc31_mmx;
	 xavs_luma_avg_w[1][ 8] =  xavs_qpel8_avg_mc02_mmx;
	 xavs_luma_avg_w[1][ 9] =  xavs_qpel8_avg_mc12_mmx;
	 xavs_luma_avg_w[1][10] =  xavs_qpel8_avg_mc22_mmx;
	 xavs_luma_avg_w[1][11] =  xavs_qpel8_avg_mc32_mmx;
	 xavs_luma_avg_w[1][12] =  xavs_qpel8_avg_mc03_mmx;
	 xavs_luma_avg_w[1][13] =  xavs_qpel8_avg_mc13_mmx;
	 xavs_luma_avg_w[1][14] =  xavs_qpel8_avg_mc23_mmx;
	 xavs_luma_avg_w[1][15] =  xavs_qpel8_avg_mc33_mmx;
#endif	
}


void init_dsp_mc_w_sse2()
{
	xavs_chroma_put_w[0]=xavs_chroma_mc8_put_sse2;
    xavs_chroma_avg_w[0]=xavs_chroma_mc8_avg_sse2;

	
	
#ifdef __CONFIG_MMX
	xavs_chroma_put_w[1]=xavs_chroma_mc4_put_sse2;
	xavs_chroma_avg_w[1]=xavs_chroma_mc4_avg_sse2;
#endif

	 xavs_luma_put_w[0][ 0] =  xavs_qpel16_put_mc00_sse2;
	 xavs_luma_put_w[0][ 1] =  xavs_qpel16_put_mc10_sse2;
	 xavs_luma_put_w[0][ 2] =  xavs_qpel16_put_mc20_sse2;
	 xavs_luma_put_w[0][ 3] =  xavs_qpel16_put_mc30_sse2;
	 xavs_luma_put_w[0][ 4] =  xavs_qpel16_put_mc01_sse2;
	 xavs_luma_put_w[0][ 5] =  xavs_qpel16_put_mc11_sse2;
	 xavs_luma_put_w[0][ 6] =  xavs_qpel16_put_mc21_sse2;
	 xavs_luma_put_w[0][ 7] =  xavs_qpel16_put_mc31_sse2;
	 xavs_luma_put_w[0][ 8] =  xavs_qpel16_put_mc02_sse2;
	 xavs_luma_put_w[0][ 9] =  xavs_qpel16_put_mc12_sse2;
	 xavs_luma_put_w[0][10] =  xavs_qpel16_put_mc22_sse2;
	 xavs_luma_put_w[0][11] =  xavs_qpel16_put_mc32_sse2;
	 xavs_luma_put_w[0][12] =  xavs_qpel16_put_mc03_sse2;
	 xavs_luma_put_w[0][13] =  xavs_qpel16_put_mc13_sse2;
	 xavs_luma_put_w[0][14] =  xavs_qpel16_put_mc23_sse2;
	 xavs_luma_put_w[0][15] =  xavs_qpel16_put_mc33_sse2;

	 xavs_luma_put_w[1][ 0] =  xavs_qpel8_put_mc00_sse2;
	 xavs_luma_put_w[1][ 1] =  xavs_qpel8_put_mc10_sse2;
	 xavs_luma_put_w[1][ 2] =  xavs_qpel8_put_mc20_sse2;
	 xavs_luma_put_w[1][ 3] =  xavs_qpel8_put_mc30_sse2;
	 xavs_luma_put_w[1][ 4] =  xavs_qpel8_put_mc01_sse2;
	 xavs_luma_put_w[1][ 5] =  xavs_qpel8_put_mc11_sse2;
	 xavs_luma_put_w[1][ 6] =  xavs_qpel8_put_mc21_sse2;
	 xavs_luma_put_w[1][ 7] =  xavs_qpel8_put_mc31_sse2;
	 xavs_luma_put_w[1][ 8] =  xavs_qpel8_put_mc02_sse2;
	 xavs_luma_put_w[1][ 9] =  xavs_qpel8_put_mc12_sse2;
	 xavs_luma_put_w[1][10] =  xavs_qpel8_put_mc22_sse2;
	 xavs_luma_put_w[1][11] =  xavs_qpel8_put_mc32_sse2;
	 xavs_luma_put_w[1][12] =  xavs_qpel8_put_mc03_sse2;
	 xavs_luma_put_w[1][13] =  xavs_qpel8_put_mc13_sse2;
	 xavs_luma_put_w[1][14] =  xavs_qpel8_put_mc23_sse2;
	 xavs_luma_put_w[1][15] =  xavs_qpel8_put_mc33_sse2;

	 xavs_luma_avg_w[0][ 0] =  xavs_qpel16_avg_mc00_sse2;
	 xavs_luma_avg_w[0][ 1] =  xavs_qpel16_avg_mc10_sse2;
	 xavs_luma_avg_w[0][ 2] =  xavs_qpel16_avg_mc20_sse2;
	 xavs_luma_avg_w[0][ 3] =  xavs_qpel16_avg_mc30_sse2;
	 xavs_luma_avg_w[0][ 4] =  xavs_qpel16_avg_mc01_sse2;
	 xavs_luma_avg_w[0][ 5] =  xavs_qpel16_avg_mc11_sse2;
	 xavs_luma_avg_w[0][ 6] =  xavs_qpel16_avg_mc21_sse2;
	 xavs_luma_avg_w[0][ 7] =  xavs_qpel16_avg_mc31_sse2;
	 xavs_luma_avg_w[0][ 8] =  xavs_qpel16_avg_mc02_sse2;
	 xavs_luma_avg_w[0][ 9] =  xavs_qpel16_avg_mc12_sse2;
	 xavs_luma_avg_w[0][10] =  xavs_qpel16_avg_mc22_sse2;
	 xavs_luma_avg_w[0][11] =  xavs_qpel16_avg_mc32_sse2;
	 xavs_luma_avg_w[0][12] =  xavs_qpel16_avg_mc03_sse2;
	 xavs_luma_avg_w[0][13] =  xavs_qpel16_avg_mc13_sse2;
	 xavs_luma_avg_w[0][14] =  xavs_qpel16_avg_mc23_sse2;
	 xavs_luma_avg_w[0][15] =  xavs_qpel16_avg_mc33_sse2;

	 xavs_luma_avg_w[1][ 0] =  xavs_qpel8_avg_mc00_sse2;
	 xavs_luma_avg_w[1][ 1] =  xavs_qpel8_avg_mc10_sse2;
	 xavs_luma_avg_w[1][ 2] =  xavs_qpel8_avg_mc20_sse2;
	 xavs_luma_avg_w[1][ 3] =  xavs_qpel8_avg_mc30_sse2;
	 xavs_luma_avg_w[1][ 4] =  xavs_qpel8_avg_mc01_sse2;
	 xavs_luma_avg_w[1][ 5] =  xavs_qpel8_avg_mc11_sse2;
	 xavs_luma_avg_w[1][ 6] =  xavs_qpel8_avg_mc21_sse2;
	 xavs_luma_avg_w[1][ 7] =  xavs_qpel8_avg_mc31_sse2;
	 xavs_luma_avg_w[1][ 8] =  xavs_qpel8_avg_mc02_sse2;
	 xavs_luma_avg_w[1][ 9] =  xavs_qpel8_avg_mc12_sse2;
	 xavs_luma_avg_w[1][10] =  xavs_qpel8_avg_mc22_sse2;
	 xavs_luma_avg_w[1][11] =  xavs_qpel8_avg_mc32_sse2;
	 xavs_luma_avg_w[1][12] =  xavs_qpel8_avg_mc03_sse2;
	 xavs_luma_avg_w[1][13] =  xavs_qpel8_avg_mc13_sse2;
	 xavs_luma_avg_w[1][14] =  xavs_qpel8_avg_mc23_sse2;
	 xavs_luma_avg_w[1][15] =  xavs_qpel8_avg_mc33_sse2;
	
}
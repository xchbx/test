#include "xavs.h"
#include "xavs_globe.h"
#include "xavs_dsp.h"

xavs_idct8_add_func          xavs_idct8_add;
xavs_intra_pred_lum_func     xavs_intra_pred_lum[8];
xavs_intra_pred_chroma_func  xavs_intra_pred_chroma[7];
xavs_filter_lv_func          xavs_filter_lv;
xavs_filter_lh_func          xavs_filter_lh;
xavs_filter_cv_func          xavs_filter_cv;
xavs_filter_ch_func          xavs_filter_ch;

xavs_luma_mc_func xavs_luma_put[2][16];
xavs_luma_mc_func xavs_luma_avg[2][16];
xavs_chroma_mc_func xavs_chroma_put[3];
xavs_chroma_mc_func xavs_chroma_avg[3];


xavs_luma_mc_w_func xavs_luma_put_w[2][16];
xavs_luma_mc_w_func xavs_luma_avg_w[2][16];
xavs_chroma_mc_w_func xavs_chroma_put_w[3];
xavs_chroma_mc_w_func xavs_chroma_avg_w[3];


#define LOWPASS(ARRAY,INDEX)                                            \
    (( ARRAY[(INDEX)-1] + 2*ARRAY[(INDEX)] + ARRAY[(INDEX)+1] + 2) >> 2)

/****************************************************************************
 * * xavs_intra_pred_dc_128:帧内预测模式之直流,顶块和左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_128(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
 {

    int y;
    uint64_t i_value = 0x8080808080808080;
    for(y=0;y<8;y++)
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp:帧内预测模式之直流,顶块和左块都能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            p_dest[y*i_stride+x] = (LOWPASS(p_top,x+1) + LOWPASS(p_left,y+1)) >> 1;
}

/****************************************************************************
 * * xavs_intra_pred_dc_lp_left:帧内预测模式之直流,顶块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_left(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            p_dest[y*i_stride+x] = LOWPASS(p_left,y+1);
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp_top:帧内预测模式之直流,左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_top(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            p_dest[y*i_stride+x] = LOWPASS(p_top,x+1);
}
/****************************************************************************
 * * xavs_intra_pred_horizontal:帧内预测模式之水平
 ****************************************************************************/
static void xavs_intra_pred_horizontal(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int y;
    uint64_t i_value;
    for(y=0;y<8;y++) 
	{
		//0x0101010101010101ULL为什么编译不通过64位常量后面应该跟什么呢？
        i_value = p_left[y+1] * 0x0101010101010101;
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
    }
}
/****************************************************************************
 * * xavs_intra_pred_vertical:帧内预测模式之垂直
 ****************************************************************************/
static void xavs_intra_pred_vertical(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int y;
    uint64_t i_value = unaligned64(&p_top[1]);
    for(y=0;y<8;y++) 
	{
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
    }
}
/****************************************************************************
 * * xavs_intra_pred_down_left:帧内预测模式之左下
 ****************************************************************************/
static void xavs_intra_pred_down_left(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            p_dest[y*i_stride+x] = (LOWPASS(p_top,x+y+2) + LOWPASS(p_left,x+y+2)) >> 1;
}
/****************************************************************************
 * * xavs_intra_pred_down_left:帧内预测模式之右下
 ****************************************************************************/
static void xavs_intra_pred_down_right(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            if(x==y)
                p_dest[y*i_stride+x] = (p_left[1]+2*p_top[0]+p_top[1]+2)>>2;
            else if(x>y)
                p_dest[y*i_stride+x] = LOWPASS(p_top,x-y);
            else
                p_dest[y*i_stride+x] = LOWPASS(p_left,y-x);
}

/****************************************************************************
 * * xavs_intra_pred_plane:帧内预测模式之色度平面
 ****************************************************************************/
static void xavs_intra_pred_plane(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y,i_value;
    int i_h = 0;
    int i_v = 0;
    uint8_t *p_crop = crop_table + MAX_NEG_CROP;

    for(x=0; x<4; x++)
	{
        i_h += (x+1)*(p_top[5+x]-p_top[3-x]);
        i_v += (x+1)*(p_left[5+x]-p_left[3-x]);
    }
    i_value = (p_top[8]+p_left[8])<<4;
    i_h = (17*i_h+16)>>5;
    i_v = (17*i_v+16)>>5;
    for(y=0; y<8; y++)
	{
        for(x=0; x<8; x++)
		{
            p_dest[y*i_stride+x] = p_crop[(i_value+(x-3)*i_h+(y-3)*i_v+16)>>5];
		}
	}
}
#undef LOWPASS
static void xavs_idct8_add_c(uint8_t *dst, DCTELEM *block, int stride) 
{
     int i;
     DCTELEM (*src)[8] = (DCTELEM(*)[8])block;
     uint8_t *cm = crop_table + MAX_NEG_CROP;

	

     src[0][0] += 8;

    for( i = 0; i < 8; i++ ) {
         const int a0 =  3*src[i][1] - (src[i][7]<<1);
        const int a1 =  3*src[i][3] + (src[i][5]<<1);
        const int a2 =  (src[i][3]<<1) - 3*src[i][5];
        const int a3 =  (src[i][1]<<1) + 3*src[i][7];

       const int b4 = ((a0 + a1 + a3)<<1) + a1;
         const int b5 = ((a0 - a1 + a2)<<1) + a0;
         const int b6 = ((a3 - a2 - a1)<<1) + a3;
         const int b7 = ((a0 - a2 - a3)<<1) - a2;
 
         const int a7 = (src[i][2]<<2) - 10*src[i][6];
         const int a6 = (src[i][6]<<2) + 10*src[i][2];
        const int a5 = ((src[i][0] - src[i][4]) << 3) + 4;
         const int a4 = ((src[i][0] + src[i][4]) << 3) + 4;

        const int b0 = a4 + a6;
         const int b1 = a5 + a7;
         const int b2 = a5 - a7;
        const int b3 = a4 - a6;
 
         src[i][0] = (b0 + b4) >> 3;
         src[i][1] = (b1 + b5) >> 3;
         src[i][2] = (b2 + b6) >> 3;
         src[i][3] = (b3 + b7) >> 3;
         src[i][4] = (b3 - b7) >> 3;
         src[i][5] = (b2 - b6) >> 3;
         src[i][6] = (b1 - b5) >> 3;
         src[i][7] = (b0 - b4) >> 3;
     }
     for( i = 0; i < 8; i++ ) {
         const int a0 =  3*src[1][i] - (src[7][i]<<1);
         const int a1 =  3*src[3][i] + (src[5][i]<<1);
         const int a2 =  (src[3][i]<<1) - 3*src[5][i];
         const int a3 =  (src[1][i]<<1) + 3*src[7][i];

         const int b4 = ((a0 + a1 + a3)<<1) + a1;
         const int b5 = ((a0 - a1 + a2)<<1) + a0;
         const int b6 = ((a3 - a2 - a1)<<1) + a3;
         const int b7 = ((a0 - a2 - a3)<<1) - a2;

         const int a7 = (src[2][i]<<2) - 10*src[6][i];
         const int a6 = (src[6][i]<<2) + 10*src[2][i];
         const int a5 = (src[0][i] - src[4][i]) << 3;
         const int a4 = (src[0][i] + src[4][i]) << 3;
 
         const int b0 = a4 + a6;
         const int b1 = a5 + a7;
         const int b2 = a5 - a7;
         const int b3 = a4 - a6;

         dst[i + 0*stride] = cm[ dst[i + 0*stride] + ((b0 + b4) >> 7)];
         dst[i + 1*stride] = cm[ dst[i + 1*stride] + ((b1 + b5) >> 7)];
         dst[i + 2*stride] = cm[ dst[i + 2*stride] + ((b2 + b6) >> 7)];
         dst[i + 3*stride] = cm[ dst[i + 3*stride] + ((b3 + b7) >> 7)];
         dst[i + 4*stride] = cm[ dst[i + 4*stride] + ((b3 - b7) >> 7)];
         dst[i + 5*stride] = cm[ dst[i + 5*stride] + ((b2 - b6) >> 7)];
         dst[i + 6*stride] = cm[ dst[i + 6*stride] + ((b1 - b5) >> 7)];
         dst[i + 7*stride] = cm[ dst[i + 7*stride] + ((b0 - b4) >> 7)];
     }
     memset(block,0,64*sizeof(DCTELEM));
}

#define P2 p0_p[-3*stride]
#define P1 p0_p[-2*stride]
#define P0 p0_p[-1*stride]
#define Q0 p0_p[ 0*stride]
#define Q1 p0_p[ 1*stride]
#define Q2 p0_p[ 2*stride]

static inline void loop_filter_l2(uint8_t *p0_p,int stride,int alpha, int beta) 
{
	
	int p0 = P0;
	int q0 = Q0;
    if(abs(p0-q0)<alpha && abs(P1-p0)<beta && abs(Q1-q0)<beta) 
	{
		int s = p0 + q0 + 2;
		alpha = (alpha>>2) + 2;
		if(abs(P2-p0) < beta && abs(p0-q0) < alpha) 
		{
			P0 = (P1 + p0 + s) >> 2;
			P1 = (2*P1 + s) >> 2;
		} 
		else
		{
			P0 = (2*P1 + s) >> 2;
		}
		if(abs(Q2-q0) < beta && abs(q0-p0) < alpha) 
		{
			Q0 = (Q1 + q0 + s) >> 2;
			Q1 = (2*Q1 + s) >> 2;
		}
		else
		{
			Q0 = (2*Q1 + s) >> 2;
		}
	}
}
 
static inline void loop_filter_l1(uint8_t *p0_p, int stride, int alpha, int beta, int tc) 
{
	
     int p0 = P0;
     int q0 = Q0;
 
     if(abs(p0-q0)<alpha && abs(P1-p0)<beta && abs(Q1-q0)<beta) 
	 {
         int delta = xavs_clip(((q0-p0)*3+P1-Q1+4)>>3,-tc, tc);
         P0 = xavs_clip_uint8(p0+delta);
         Q0 = xavs_clip_uint8(q0-delta);
         if(abs(P2-p0)<beta) 
		 {
            delta = xavs_clip(((P0-P1)*3+P2-Q0+4)>>3, -tc, tc);
             P1 = xavs_clip_uint8(P1+delta);
         }
         if(abs(Q2-q0)<beta) 
		 {
             delta = xavs_clip(((Q1-Q0)*3+P0-Q2+4)>>3, -tc, tc);
             Q1 = xavs_clip_uint8(Q1-delta);
        }
    }
 }
 
static inline void loop_filter_c2(uint8_t *p0_p,int stride,int alpha, int beta) 
{
	
    int p0 = P0;
    int q0 = Q0;
 
	if(abs(p0-q0)<alpha && abs(P1-p0)<beta && abs(Q1-q0)<beta) 
	{
		int s = p0 + q0 + 2;
		alpha = (alpha>>2) + 2;
		if(abs(P2-p0) < beta && abs(p0-q0) < alpha) 
		{
			P0 = (P1 + p0 + s) >> 2;
        }
		else
		{
			P0 = (2*P1 + s) >> 2;
		}
        if(abs(Q2-q0) < beta && abs(q0-p0) < alpha) 
		{
            Q0 = (Q1 + q0 + s) >> 2;
		}
		else
		{
			Q0 = (2*Q1 + s) >> 2;
		}
	}
 }

static inline void loop_filter_c1(uint8_t *p0_p,int stride,int alpha, int beta,int tc) 
{
	
    if(abs(P0-Q0)<alpha && abs(P1-P0)<beta && abs(Q1-Q0)<beta) 
	{
         int delta = xavs_clip(((Q0-P0)*3+P1-Q1+4)>>3, -tc, tc);
         P0 = xavs_clip_uint8(P0+delta);
         Q0 = xavs_clip_uint8(Q0-delta);
     }
 }

#undef P0
#undef P1
#undef P2
#undef Q0
#undef Q1
#undef Q2

static void xavs_filter_lv_c(uint8_t *d, int stride, int alpha, int beta, int tc,
                          int bs1, int bs2) {
   int i;
    if(bs1==2)
        for(i=0;i<16;i++)
             loop_filter_l2(d + i*stride,1,alpha,beta);
    else 
	{
        if(bs1)
            for(i=0;i<8;i++)
               loop_filter_l1(d + i*stride,1,alpha,beta,tc);
        if (bs2)
           for(i=8;i<16;i++)
               loop_filter_l1(d + i*stride,1,alpha,beta,tc);
    }
}
 
static void xavs_filter_lh_c(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) 
{
   int i;
   if(bs1==2)
        for(i=0;i<16;i++)
             loop_filter_l2(d + i,stride,alpha,beta);
    else {
         if(bs1)
            for(i=0;i<8;i++)
                 loop_filter_l1(d + i,stride,alpha,beta,tc);
         if (bs2)
            for(i=8;i<16;i++)
                 loop_filter_l1(d + i,stride,alpha,beta,tc);
     }
}
 
static void xavs_filter_cv_c(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) 
{
     int i;
     if(bs1==2)
        for(i=0;i<8;i++)
             loop_filter_c2(d + i*stride,1,alpha,beta);
     else 
	 {
         if(bs1)
            for(i=0;i<4;i++)
                 loop_filter_c1(d + i*stride,1,alpha,beta,tc);
         if (bs2)
             for(i=4;i<8;i++)
                 loop_filter_c1(d + i*stride,1,alpha,beta,tc);
    }
}

static void xavs_filter_ch_c(uint8_t *d, int stride, int alpha, int beta, int tc, int bs1, int bs2) 
{
    int i;
    if(bs1==2)
        for(i=0;i<8;i++)
             loop_filter_c2(d + i,stride,alpha,beta);
    else 
	{
         if(bs1)
            for(i=0;i<4;i++)
                loop_filter_c1(d + i,stride,alpha,beta,tc);
       if (bs2)
            for(i=4;i<8;i++)
                 loop_filter_c1(d + i,stride,alpha,beta,tc);
     }
}

void init_dsp()
{
	xavs_intra_pred_lum[INTRA_L_VERT]=xavs_intra_pred_vertical;
	xavs_intra_pred_lum[INTRA_L_HORIZ]=xavs_intra_pred_horizontal;
	xavs_intra_pred_lum[INTRA_L_LP]=xavs_intra_pred_dc_lp;
	xavs_intra_pred_lum[INTRA_L_DOWN_LEFT]=xavs_intra_pred_down_left;
	xavs_intra_pred_lum[INTRA_L_DOWN_RIGHT]=xavs_intra_pred_down_right;
	xavs_intra_pred_lum[INTRA_L_LP_LEFT]=xavs_intra_pred_dc_lp_left;
	xavs_intra_pred_lum[NTRA_L_LP_TOP]=xavs_intra_pred_dc_lp_top;
	xavs_intra_pred_lum[INTRA_L_DC_128]=xavs_intra_pred_dc_128;
	xavs_intra_pred_chroma[INTRA_C_LP]=xavs_intra_pred_dc_lp;
	xavs_intra_pred_chroma[INTRA_C_HORIZ]=xavs_intra_pred_horizontal;
	xavs_intra_pred_chroma[INTRA_C_VERT]=xavs_intra_pred_vertical;
	xavs_intra_pred_chroma[INTRA_C_PLANE]=xavs_intra_pred_plane;
	xavs_intra_pred_chroma[INTRA_C_LP_LEFT]=xavs_intra_pred_dc_lp_left;
	xavs_intra_pred_chroma[INTRA_C_LP_TOP]=xavs_intra_pred_dc_lp_top;
	xavs_intra_pred_chroma[INTRA_C_DC_128]=xavs_intra_pred_dc_128;
	xavs_filter_lv = xavs_filter_lv_c;
    xavs_filter_lh = xavs_filter_lh_c;
    xavs_filter_cv = xavs_filter_cv_c;
    xavs_filter_ch = xavs_filter_ch_c;
	xavs_idct8_add=xavs_idct8_add_c;
	

	init_dsp_mc(); 
	init_dsp_mc_w(); 

#ifndef __CONFIG_ANDROID
	init_dsp_mmx();
#endif
}

#ifndef _XAVS_DSP_H
#define _XAVS_DSP_H


typedef void (*xavs_idct8_add_func)(uint8_t *dst, DCTELEM *block, int stride);
typedef void (*xavs_intra_pred_lum_func)(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride);
typedef void (*xavs_intra_pred_chroma_func)(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride);
typedef void (*xavs_filter_lv_func)(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) ;
typedef void (*xavs_filter_lh_func)(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) ;
typedef void (*xavs_filter_cv_func)(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) ;
typedef void (*xavs_filter_ch_func)(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) ;
extern xavs_idct8_add_func          xavs_idct8_add;
extern xavs_intra_pred_lum_func     xavs_intra_pred_lum[8];
extern xavs_intra_pred_chroma_func  xavs_intra_pred_chroma[7];
extern xavs_filter_lv_func          xavs_filter_lv;
extern xavs_filter_lh_func          xavs_filter_lh;
extern xavs_filter_cv_func          xavs_filter_cv;
extern xavs_filter_ch_func          xavs_filter_ch;



typedef void (*xavs_luma_mc_func)(uint8_t *dst/*align width (8 or 16)*/, uint8_t *src/*align 1*/, int stride);
typedef void (*xavs_chroma_mc_func)(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int srcStride, int h, int x, int y);
typedef void (*xavs_luma_mc_w_func)(uint8_t *dst/*align width (8 or 16)*/, uint8_t *src/*align 1*/, int stride , int scale, int shift);
typedef void (*xavs_chroma_mc_w_func)(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int srcStride, int h, int x, int y, int scale, int shift);

extern xavs_luma_mc_func xavs_luma_put[2][16];
extern xavs_luma_mc_func xavs_luma_avg[2][16];
extern xavs_chroma_mc_func xavs_chroma_put[3];
extern xavs_chroma_mc_func xavs_chroma_avg[3];


extern xavs_luma_mc_w_func xavs_luma_put_w[2][16];
extern xavs_luma_mc_w_func xavs_luma_avg_w[2][16];
extern xavs_chroma_mc_w_func xavs_chroma_put_w[3];
extern xavs_chroma_mc_w_func xavs_chroma_avg_w[3];


void init_dsp();
void init_dsp_mc();
void init_dsp_mc_w();

void init_dsp_mmx();
void init_dsp_mc_mmx();
void init_dsp_mc_w_mmx();


void init_dsp_sse2();
void init_dsp_mc_sse2();
void init_dsp_mc_w_sse2();


#endif
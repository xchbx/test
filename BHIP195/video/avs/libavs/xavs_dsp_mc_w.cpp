#include "xavs.h"
#include "xavs_globe.h"
#include "xavs_dsp.h"
#define AH -1
#define BH -2
#define CH 96
#define DH 42
#define EH -7
#define FH 0
#define AV 0 
#define BV -1
#define CV 5
#define DV 5
#define EV -1
#define FV 0
#define MC_W 8
#define MC_H 8

#define __opt_w(__b) cm[(((__b) * scale + 16) >> 5) + shift]


//#define put(a, b)  a = cm[((b)+512)>>10]
#define put(a, b)  a = __opt_w(cm[((b)+512)>>10])
static void xavs_put_filt8_hv_ik(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	//已知src值对于临时半象素5个点 d对于临时1/4象素
	//从标准可以知道
	//要获得a' c'的值需要获得b',ee',D',E'
	//b'  = (-1)*src[ -1] +   5*src[ 0] + 5*src[ 1] + (-1)*src[ 2]
	//ee' = (-1)*src[ -2] +   5*src[-1] + 5*src[ 0] + (-1)*src[ 1]
	//D'  =    8*src[  0]
	//E'  =    8*src[  1]
	//a'  =  1*ee'+7*D'+7*b'+1*E'
	//    = (-1)*src[ -2] +   5*src[-1] + 5*src[ 0] + (-1)*src[ 1]
	//    +   56*src[  0]
	//    + (-7)*src[ -1] +  35*src[ 0] +35*src[ 1] + (-7)*src[ 2]
	//    +    8*src[  1]
	//    = (-1)*src[ -2] +(-2)*src[-1] +96*src[ 0] +   42**src[ 1] + (-7)*src[ 2]
	//这就是为什么是-1,-2,96,42,-7系数的来源
//对于1/2部分的i的计算i'=gg'+7h''+7j'+m''
//i=-xx-2bb'+96h'+42m'-7cc'
//是对半个象素的水平方向上5个点获得
//由于xx'，bb'，h'，m'，cc'可以通过垂直F1滤波获得，这里xx'是bb'外一个垂直1/2象素值
//所以最终i可以通过计算水平1/4象素再通过垂直F1滤波获得
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], AH*tmp[-2+i] + BH*tmp[-1+i] + CH*tmp[0+i] + DH*tmp[1+i] + EH*tmp[2+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
     
}
#undef put

//#define put(a, b)  a = cm[((b)+512)>>10]
#define put(a, b)  a = __opt_w(cm[((b)+512)>>10])

static void xavs_put_filt8_hv_ki(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], EH*tmp[-1+i] + DH*tmp[0+i] + CH*tmp[1+i] + BH*tmp[2+i] + AH*tmp[3+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
     
}
#undef put



//#define put(a, b)  a = cm[((b)+512)>>10]
#define put(a, b)  a = __opt_w(cm[((b)+512)>>10])

static void xavs_put_filt8_hv_fq(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	//f=-xx-2aa'+96b'+42s'-7dd'
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp;
	 src1-=2*srcStride;
	 for(j=-2;j<MC_H+3;j++)
	 {
		 for(i=0; i<MC_W; i++)
		 {
			 tmp[i]= BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i];			 
		 }
		 tmp+=8;
		 src1+=srcStride;
	 }
     tmp = temp+2*8;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], AH*tmp[-2*8+i] + BH*tmp[-1*8+i] + CH*tmp[0+i] + DH*tmp[1*8+i] + EH*tmp[2*8+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=8;                                                        
	 }                                                               
     
}
#undef put

//#define put(a, b)  a = cm[((b)+512)>>10]
#define put(a, b)  a = __opt_w(cm[((b)+512)>>10])
static void xavs_put_filt8_hv_qf(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	//f=-xx-2aa'+96b'+42s'-7dd'
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp;
	 src1-=2*srcStride;
	 for(j=-2;j<MC_H+3;j++)
	 {
		 for(i=0; i<MC_W; i++)
		 {
			 tmp[i]= BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i];			 
		 }
		 tmp+=8;
		 src1+=srcStride;
	 }
     tmp = temp+2*8;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], EH*tmp[-1*8+i] + DH*tmp[0+i] + CH*tmp[1*8+i] + BH*tmp[2*8+i] + AH*tmp[3*8+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=8;                                                        
	 }                                                               
     
}
#undef put


//#define put(a, b)  a = cm[((b)+32)>>6]
#define put(a, b)  a = __opt_w(cm[((b)+32)>>6])
static void xavs_put_filt8_hv_j(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	
      int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i],  BV*tmp[-1+i] + CV*tmp[0+i] + DV*tmp[1+i] + EV*tmp[2+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
}
#undef put



//#define put(a, b)  a = cm[((b)+64)>>7]
#define put(a, b)  a = __opt_w(cm[((b)+64)>>7])
static void xavs_put_filt8_h_ac(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
	//i=-xx-2bb'+96h'+42m'-7cc'
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], AH*src1[-2+i] + BH*src1[-1+i] + CH*src1[0+i] + DH*src1[1+i] + EH*src1[2+i] ); 	
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef put


//#define put(a, b)  a = cm[((b)+64)>>7]
#define put(a, b)  a = __opt_w(cm[((b)+64)>>7])
static void xavs_put_filt8_h_ca(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
	
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], EH*src1[-1+i] + DH*src1[0+i] + CH*src1[1+i] + BH*src1[2+i] + AH*src1[3+i] ); 	
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef put


//#define put(a, b)  a = cm[((b)+4)>>3]
#define put(a, b)  a = __opt_w(cm[((b)+4)>>3])
static void xavs_put_filt8_h_b(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i]);			 		
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef put


//#define put(a, b)  a = cm[((b)+4)>>3]
#define put(a, b)  a = __opt_w(cm[((b)+4)>>3])
static void xavs_put_filt8_v_h(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i]);			 		
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef put


//#define put(a, b)  a = cm[((b)+64)>>7]
#define put(a, b)  a = __opt_w(cm[((b)+64)>>7])
static void xavs_put_filt8_v_dn(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], AH*src1[-2*srcStride+i] + BH*src1[-1*srcStride+i] + CH*src1[0+i] + DH*src1[1*srcStride+i] + EH*src1[2*srcStride+i] ); 
			
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               

}
#undef put


//#define put(a, b)  a = cm[((b)+64)>>7]
#define put(a, b)  a = __opt_w(cm[((b)+64)>>7])
static void xavs_put_filt8_v_nd(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], EH*src1[-1*srcStride+i] + DH*src1[0+i] + CH*src1[1*srcStride+i] + BH*src1[2*srcStride+i] + AH*src1[3*srcStride+i] ); 
			
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               

}
#undef put


//#define put(a, b)  a = cm[((b)+64)>>7]
#define put(a, b)  a = __opt_w(cm[((b)+64)>>7])
static void xavs_put_filt8_hv_egpr(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 put(dst[i], BV*tmp[-1+i] + CV*tmp[0+i] + DV*tmp[1+i] + EV*tmp[2+i]+64*src2[0+i]); 
			
		 }
		 src2+=srcStride;
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }                                                               
}
#undef put

 


//#define avg(a, b)  a = ((a)+cm[((b)+512)>>10]+1)>>1

#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+512)>>10])+1)>>1

static void xavs_avg_filt8_hv_ik(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], AH*tmp[-2+i] + BH*tmp[-1+i] + CH*tmp[0+i] + DH*tmp[1+i] + EH*tmp[2+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
     
}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+512)>>10]+1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+512)>>10])+1)>>1
static void xavs_avg_filt8_hv_ki(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], EH*tmp[-1+i] + DH*tmp[0+i] + CH*tmp[1+i] + BH*tmp[2+i] + AH*tmp[3+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
     
}
#undef avg




//#define avg(a, b)  a = ((a)+cm[((b)+512)>>10]+1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+512)>>10])+1)>>1
static void xavs_avg_filt8_hv_fq(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	//f=-xx-2aa'+96b'+42s'-7dd'
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp;
	 src1-=2*srcStride;
	 for(j=-2;j<MC_H+3;j++)
	 {
		 for(i=0; i<MC_W; i++)
		 {
			 tmp[i]= BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i];			 
		 }
		 tmp+=8;
		 src1+=srcStride;
	 }
     tmp = temp+2*8;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], AH*tmp[-2*8+i] + BH*tmp[-1*8+i] + CH*tmp[0+i] + DH*tmp[1*8+i] + EH*tmp[2*8+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=8;                                                        
	 }                                                               
     
}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+512)>>10]+1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+512)>>10])+1)>>1
static void xavs_avg_filt8_hv_qf(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	//f=-xx-2aa'+96b'+42s'-7dd'
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp;
	 src1-=2*srcStride;
	 for(j=-2;j<MC_H+3;j++)
	 {
		 for(i=0; i<MC_W; i++)
		 {
			 tmp[i]= BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i];			 
		 }
		 tmp+=8;
		 src1+=srcStride;
	 }
     tmp = temp+2*8;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], EH*tmp[-1*8+i] + DH*tmp[0+i] + CH*tmp[1*8+i] + BH*tmp[2*8+i] + AH*tmp[3*8+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=8;                                                        
	 }                                                               
     
}
#undef avg




//#define avg(a, b)  a = ((a)+cm[((b)+32)>>6]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+32)>>6])+1)>>1
static void xavs_avg_filt8_hv_j(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	
      int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i],  BV*tmp[-1+i] + CV*tmp[0+i] + DV*tmp[1+i] + EV*tmp[2+i] ); 
			
		 }
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }
     
}
#undef avg



//#define avg(a, b)  a = ((a)+cm[((b)+64)>>7]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+64)>>7])+1)>>1
static void xavs_avg_filt8_h_ac(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], AH*src1[-2+i] + BH*src1[-1+i] + CH*src1[0+i] + DH*src1[1+i] + EH*src1[2+i] ); 
			
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+64)>>7]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+64)>>7])+1)>>1
static void xavs_avg_filt8_h_ca(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], EH*src1[-1+i] + DH*src1[0+i] + CH*src1[1+i] + BH*src1[2+i] + AH*src1[3+i] ); 
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+4)>>3]   +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+4)>>3])+1)>>1
static void xavs_avg_filt8_h_b(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], BV*src1[-1+i] + CV*src1[0+i] + DV*src1[1+i] + EV*src1[2+i]);			 		
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef avg



//#define avg(a, b)  a = ((a)+cm[((b)+4)>>3]   +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+4)>>3])+1)>>1
static void xavs_avg_filt8_v_h(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{ 
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i]);			 		
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               
}
#undef avg



//#define avg(a, b)  a = ((a)+cm[((b)+64)>>7]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+64)>>7])+1)>>1
static void xavs_avg_filt8_v_dn(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], AH*src1[-2*srcStride+i] + BH*src1[-1*srcStride+i] + CH*src1[0+i] + DH*src1[1*srcStride+i] + EH*src1[2*srcStride+i] ); 
			
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               

}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+64)>>7]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+64)>>7])+1)>>1
static void xavs_avg_filt8_v_nd(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], EH*src1[-1*srcStride+i] + DH*src1[0+i] + CH*src1[1*srcStride+i] + BH*src1[2*srcStride+i] + AH*src1[3*srcStride+i] ); 
			
		 }
		 src1+=srcStride;
		 dst+=dstStride;
	 }                                                               

}
#undef avg


//#define avg(a, b)  a = ((a)+cm[((b)+64)>>7]  +1)>>1
#define avg(a, b)  a = ((a)+__opt_w(cm[((b)+64)>>7])+1)>>1
static void xavs_avg_filt8_hv_egpr(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{

     
     int32_t temp[8*(8+5)];
     int32_t *tmp = temp;
     uint8_t *cm = crop_table + MAX_NEG_CROP;
     int i,j;
	 tmp = temp+2;
	 for(j=0;j<MC_H;j++)
	 {
		 for(i=-2; i<MC_W+3; i++)
		 {
			 tmp[i]= BV*src1[-1*srcStride+i] + CV*src1[0+i] + DV*src1[1*srcStride+i] + EV*src1[2*srcStride+i];			 
		 }
		 tmp+=13;
		 src1+=srcStride;
	 }
     tmp = temp+2;                         
	 for(j=0; j<MC_H; j++)                        
	 {
		 for(i=0;i<MC_W;i++)
		 {
			 avg(dst[i], BV*tmp[-1+i] + CV*tmp[0+i] + DV*tmp[1+i] + EV*tmp[2+i]+64*src2[0+i]); 
			
		 }
		 src2+=srcStride;
		 dst+=dstStride;
		 tmp+=13;                                                        
	 }                                                               
}
#undef avg

#undef AH 
#undef BH 
#undef CH 
#undef DH 
#undef EH 
#undef FH 
#undef AV  
#undef BV 
#undef CV 
#undef DV 
#undef EV 
#undef FV 
#undef MC_W 
#undef MC_H 

static void xavs_put_filt16_hv_fq(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_fq(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_fq(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_fq(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_fq(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_put_filt16_hv_qf(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_qf(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_qf(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_qf(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_qf(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_put_filt16_v_dn(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_dn(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_dn(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_dn(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_dn(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_v_nd(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_nd(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_nd(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_nd(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_nd(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_put_filt16_hv_egpr(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_egpr(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_egpr(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_egpr(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_hv_j(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_j(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_j(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_j(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_j(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}



static void xavs_put_filt16_h_b(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_b(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_b(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_b(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_b(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_v_h(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_h(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_h(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_h(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_v_h(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_h_ac(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ac(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_ac(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ac(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_ac(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_h_ca(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ca(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_ca(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ca(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_h_ca(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_put_filt16_hv_ki(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ki(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_ki(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ki(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_ki(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_put_filt16_hv_ik(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ik(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_ik(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ik(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_put_filt8_hv_ik(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_qpel16_put_mc10_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ac(dst,src,src+stride+1,stride,stride, scale, shift);
}

static void xavs_qpel16_put_mc20_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_b(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc30_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ca(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc01_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_dn(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc11_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc21_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_fq(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc31_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc02_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_h(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel16_put_mc12_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ik(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc22_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_j(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel16_put_mc32_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ki(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc03_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_nd(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc13_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr(dst,src,src+stride,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc23_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_qf(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_put_mc33_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc10_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ac(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc20_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_b(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc30_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ca(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc01_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_dn(dst,src,src,stride,stride, scale, shift);
}

static void xavs_qpel8_put_mc11_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc21_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_fq(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc31_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc02_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_h(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel8_put_mc12_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ik(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc22_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_j(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel8_put_mc32_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ki(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc03_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_nd(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc13_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr(dst,src,src+stride,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc23_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_qf(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_put_mc33_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr(dst,src,src+stride+1,stride,stride, scale, shift);

}

#define PUT_PIX(OPNAME,OP) \
static void OPNAME ## _pixels8_c(uint8_t *block, const uint8_t *pixels, int line_size, int h, int scale, int shift){\
    int i;\
	uint8_t *cm = crop_table + MAX_NEG_CROP;\
    for(i=0; i<h; i++){\
        OP(block[0], pixels[0]);\
        OP(block[1], pixels[1]);\
		OP(block[2], pixels[2]);\
		OP(block[3], pixels[3]);\
		OP(block[4], pixels[4]);\
		OP(block[5], pixels[5]);\
		OP(block[6], pixels[6]);\
		OP(block[7], pixels[7]);\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static void OPNAME ## _pixels16_c(uint8_t *block, const uint8_t *pixels, int line_size, int h, int scale, int shift){\
    OPNAME ## _pixels8_c(block  , pixels  , line_size, h, scale, shift);\
    OPNAME ## _pixels8_c(block+8, pixels+8, line_size, h, scale, shift);\
}

#define op_avg(a, b) a = (( (a) + __opt_w(b) + 1) >> 1)
#define op_put(a, b) a = __opt_w(b)
PUT_PIX(put,op_put);
PUT_PIX(avg,op_avg);

#undef op_avg
#undef op_put



static void xavs_qpel8_put_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    put_pixels8_c(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_put_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    put_pixels16_c(dst, src, stride, 16, scale, shift);
}

static void xavs_qpel8_avg_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels8_c(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_avg_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels16_c(dst, src, stride, 16, scale, shift);
}

static void xavs_avg_filt16_hv_fq(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_fq(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_fq(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_fq(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_avg_filt16_hv_qf(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_qf(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_qf(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_qf(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_v_dn(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_dn(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_dn(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_dn(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_dn(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_v_nd(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_nd(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_nd(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_nd(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_nd(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_hv_egpr(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_egpr(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_egpr(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_egpr(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_hv_j(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_j(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_j(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_j(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_j(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}



static void xavs_avg_filt16_h_b(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_b(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_b(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_b(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_b(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_v_h(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_h(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_h(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_h(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_v_h(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_h_ac(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ac(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_ac(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ac(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_ac(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_h_ca(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ca(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_ca(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ca(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_h_ca(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_avg_filt16_hv_ik(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_ik(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ik(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_ik(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}

static void xavs_avg_filt16_hv_ki(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_ki(dst +8 , src1+8,   src2+8  , dstStride, srcStride, scale, shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ki(dst  , src1,   src2  , dstStride, srcStride, scale, shift);
	xavs_avg_filt8_hv_ki(dst +8, src1+8,   src2+8  , dstStride, srcStride, scale, shift);
}


static void xavs_qpel16_avg_mc10_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ac(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc20_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_b(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc30_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ca(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc01_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_dn(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc11_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc21_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_fq(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc31_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc02_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_h(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel16_avg_mc12_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ik(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc22_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_j(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel16_avg_mc32_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ki(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc03_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_nd(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc13_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr(dst,src,src+stride,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc23_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_qf(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel16_avg_mc33_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc10_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ac(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc20_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_b(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc30_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ca(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc01_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_dn(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc11_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc21_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq(dst,src,src,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc31_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc02_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_h(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel8_avg_mc12_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc22_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_j(dst,src,src+1,stride,stride, scale, shift);
}

static void xavs_qpel8_avg_mc32_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki(dst,src,src+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc03_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_nd(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc13_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr(dst,src,src+stride,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc23_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf(dst,src,src+stride+1,stride,stride, scale, shift);

}

static void xavs_qpel8_avg_mc33_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr(dst,src,src+stride+1,stride,stride, scale, shift);

}


#define XAVS_CHROMA_MC(OPNAME, OP)\
static void  xavs_chroma_mc2_## OPNAME ## _c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
	uint8_t *cm = crop_table + MAX_NEG_CROP;\
    \
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        dst+= stride;\
        src+= stride;\
    }\
}\
\
static void xavs_chroma_mc4_## OPNAME ## _c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
	uint8_t *cm = crop_table + MAX_NEG_CROP;\
    \
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        OP(dst[2], (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3]));\
        OP(dst[3], (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4]));\
        dst+= stride;\
        src+= stride;\
    }\
}\
\
static void xavs_chroma_mc8_## OPNAME ##_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
	uint8_t *cm = crop_table + MAX_NEG_CROP;\
    \
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        OP(dst[2], (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3]));\
        OP(dst[3], (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4]));\
        OP(dst[4], (A*src[4] + B*src[5] + C*src[stride+4] + D*src[stride+5]));\
        OP(dst[5], (A*src[5] + B*src[6] + C*src[stride+5] + D*src[stride+6]));\
        OP(dst[6], (A*src[6] + B*src[7] + C*src[stride+6] + D*src[stride+7]));\
        OP(dst[7], (A*src[7] + B*src[8] + C*src[stride+7] + D*src[stride+8]));\
        dst+= stride;\
        src+= stride;\
    }\
}



//#define op_avg(a, b) a = (((a)+(((b) + 32)>>6)+1)>>1)
//#define op_put(a, b) a = (((b) + 32)>>6)

#define op_avg(a, b) a = (((a)+__opt_w(((b) + 32)>>6)+1)>>1)
#define op_put(a, b) a = __opt_w(((b) + 32)>>6)


XAVS_CHROMA_MC(put       , op_put)
XAVS_CHROMA_MC(avg       , op_avg)
#undef op_avg
#undef op_put








static void put_xavs_qpel8_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
	put_pixels8_c(dst, src, stride, 8, scale, shift);
}

static void avg_xavs_qpel8_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels8_c(dst, src, stride, 8, scale, shift);
}

static void put_xavs_qpel16_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    put_pixels16_c(dst, src, stride, 16, scale, shift);
}

static void avg_xavs_qpel16_mc00_c(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels16_c(dst, src, stride, 16, scale, shift);
}




void init_dsp_mc_w()
{
	xavs_chroma_put_w[0]=xavs_chroma_mc8_put_c;
	xavs_chroma_put_w[1]=xavs_chroma_mc4_put_c;
	xavs_chroma_put_w[2]=xavs_chroma_mc2_put_c;
    xavs_chroma_avg_w[0]=xavs_chroma_mc8_avg_c;
	xavs_chroma_avg_w[1]=xavs_chroma_mc4_avg_c;
	xavs_chroma_avg_w[2]=xavs_chroma_mc2_avg_c;


	 xavs_luma_put_w[0][ 0] =  xavs_qpel16_put_mc00_c;
	 xavs_luma_put_w[0][ 1] =  xavs_qpel16_put_mc10_c;
	 xavs_luma_put_w[0][ 2] =  xavs_qpel16_put_mc20_c;
	 xavs_luma_put_w[0][ 3] =  xavs_qpel16_put_mc30_c;
	 xavs_luma_put_w[0][ 4] =  xavs_qpel16_put_mc01_c;
	 xavs_luma_put_w[0][ 5] =  xavs_qpel16_put_mc11_c;
	 xavs_luma_put_w[0][ 6] =  xavs_qpel16_put_mc21_c;
	 xavs_luma_put_w[0][ 7] =  xavs_qpel16_put_mc31_c;
	 xavs_luma_put_w[0][ 8] =  xavs_qpel16_put_mc02_c;
	 xavs_luma_put_w[0][ 9] =  xavs_qpel16_put_mc12_c;
	 xavs_luma_put_w[0][10] =  xavs_qpel16_put_mc22_c;
	 xavs_luma_put_w[0][11] =  xavs_qpel16_put_mc32_c;
	 xavs_luma_put_w[0][12] =  xavs_qpel16_put_mc03_c;
	 xavs_luma_put_w[0][13] =  xavs_qpel16_put_mc13_c;
	 xavs_luma_put_w[0][14] =  xavs_qpel16_put_mc23_c;
	 xavs_luma_put_w[0][15] =  xavs_qpel16_put_mc33_c;

	 xavs_luma_put_w[1][ 0] =  xavs_qpel8_put_mc00_c;
	 xavs_luma_put_w[1][ 1] =  xavs_qpel8_put_mc10_c;
	 xavs_luma_put_w[1][ 2] =  xavs_qpel8_put_mc20_c;
	 xavs_luma_put_w[1][ 3] =  xavs_qpel8_put_mc30_c;
	 xavs_luma_put_w[1][ 4] =  xavs_qpel8_put_mc01_c;
	 xavs_luma_put_w[1][ 5] =  xavs_qpel8_put_mc11_c;
	 xavs_luma_put_w[1][ 6] =  xavs_qpel8_put_mc21_c;
	 xavs_luma_put_w[1][ 7] =  xavs_qpel8_put_mc31_c;
	 xavs_luma_put_w[1][ 8] =  xavs_qpel8_put_mc02_c;
	 xavs_luma_put_w[1][ 9] =  xavs_qpel8_put_mc12_c;
	 xavs_luma_put_w[1][10] =  xavs_qpel8_put_mc22_c;
	 xavs_luma_put_w[1][11] =  xavs_qpel8_put_mc32_c;
	 xavs_luma_put_w[1][12] =  xavs_qpel8_put_mc03_c;
	 xavs_luma_put_w[1][13] =  xavs_qpel8_put_mc13_c;
	 xavs_luma_put_w[1][14] =  xavs_qpel8_put_mc23_c;
	 xavs_luma_put_w[1][15] =  xavs_qpel8_put_mc33_c;

	 xavs_luma_avg_w[0][ 0] =  xavs_qpel16_avg_mc00_c;
	 xavs_luma_avg_w[0][ 1] =  xavs_qpel16_avg_mc10_c;
	 xavs_luma_avg_w[0][ 2] =  xavs_qpel16_avg_mc20_c;
	 xavs_luma_avg_w[0][ 3] =  xavs_qpel16_avg_mc30_c;
	 xavs_luma_avg_w[0][ 4] =  xavs_qpel16_avg_mc01_c;
	 xavs_luma_avg_w[0][ 5] =  xavs_qpel16_avg_mc11_c;
	 xavs_luma_avg_w[0][ 6] =  xavs_qpel16_avg_mc21_c;
	 xavs_luma_avg_w[0][ 7] =  xavs_qpel16_avg_mc31_c;
	 xavs_luma_avg_w[0][ 8] =  xavs_qpel16_avg_mc02_c;
	 xavs_luma_avg_w[0][ 9] =  xavs_qpel16_avg_mc12_c;
	 xavs_luma_avg_w[0][10] =  xavs_qpel16_avg_mc22_c;
	 xavs_luma_avg_w[0][11] =  xavs_qpel16_avg_mc32_c;
	 xavs_luma_avg_w[0][12] =  xavs_qpel16_avg_mc03_c;
	 xavs_luma_avg_w[0][13] =  xavs_qpel16_avg_mc13_c;
	 xavs_luma_avg_w[0][14] =  xavs_qpel16_avg_mc23_c;
	 xavs_luma_avg_w[0][15] =  xavs_qpel16_avg_mc33_c;

	 xavs_luma_avg_w[1][ 0] =  xavs_qpel8_avg_mc00_c;
	 xavs_luma_avg_w[1][ 1] =  xavs_qpel8_avg_mc10_c;
	 xavs_luma_avg_w[1][ 2] =  xavs_qpel8_avg_mc20_c;
	 xavs_luma_avg_w[1][ 3] =  xavs_qpel8_avg_mc30_c;
	 xavs_luma_avg_w[1][ 4] =  xavs_qpel8_avg_mc01_c;
	 xavs_luma_avg_w[1][ 5] =  xavs_qpel8_avg_mc11_c;
	 xavs_luma_avg_w[1][ 6] =  xavs_qpel8_avg_mc21_c;
	 xavs_luma_avg_w[1][ 7] =  xavs_qpel8_avg_mc31_c;
	 xavs_luma_avg_w[1][ 8] =  xavs_qpel8_avg_mc02_c;
	 xavs_luma_avg_w[1][ 9] =  xavs_qpel8_avg_mc12_c;
	 xavs_luma_avg_w[1][10] =  xavs_qpel8_avg_mc22_c;
	 xavs_luma_avg_w[1][11] =  xavs_qpel8_avg_mc32_c;
	 xavs_luma_avg_w[1][12] =  xavs_qpel8_avg_mc03_c;
	 xavs_luma_avg_w[1][13] =  xavs_qpel8_avg_mc13_c;
	 xavs_luma_avg_w[1][14] =  xavs_qpel8_avg_mc23_c;
	 xavs_luma_avg_w[1][15] =  xavs_qpel8_avg_mc33_c;
	
}
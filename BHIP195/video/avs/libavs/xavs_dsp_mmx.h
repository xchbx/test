#ifndef _XAVS_DSP_MMX_H
#define _XAVS_DSP_MMX_H

#ifdef __CONFIG_LINUX
#define DECLARE_ALIGNED(n,t,v)      t __attribute__ ((aligned (n))) v
#else

#define DECLARE_ALIGNED(n,t,v)      __declspec(align(n)) t v
#endif
//#define __MISMATCH_ENABLE 

extern const DECLARE_ALIGNED(8,uint64_t,gs_N96);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N42);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N7);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N16);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N32);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N5);

extern const DECLARE_ALIGNED(8,uint64_t,gs_N4);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N64);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N1);
extern const DECLARE_ALIGNED(8,uint64_t,gs_N2);
extern const DECLARE_ALIGNED(8,uint64_t,gs_D_N512);

#ifndef __x86_64__
#ifndef _M_AMD64 
#define __CONFIG_MMX
#endif
#endif
#include <mmintrin.h> //mmx
#include <xmmintrin.h> //sse
#include <emmintrin.h> //sse2
#include <pmmintrin.h> //sse3
#include <smmintrin.h> //sse4.1
#include <nmmintrin.h> //sse4.2
//#include <ammintrin.h> //sse4a
//#include <x86intrin.h> //avx, avx2 etc
#ifdef __CONFIG_MMX
extern const __m64 __m_0;
extern const __m64 __m_w_16;
extern const __m64 __m_w_64;
extern const __m64 __m_w_32;

extern const __m64 __m_w_3;

extern const __m64 __m_w_1;
extern const __m64 __m_w_2;
extern const __m64 __m_w_96;
extern const __m64 __m_w_42;
extern const __m64 __m_w_7;

extern const __m64 __m_w_5;
extern const __m64 __m_w_4;
extern const __m64 __m_w_0x8000;
extern const __m64 __m_w_0x7FFF;
extern const __m64 __m_q_F16;

extern const __m64 __m_d_512;



#define __m_AH __m_w_1
#define __m_BH __m_w_2
#define __m_CH __m_w_96
#define __m_DH __m_w_42
#define __m_EH __m_w_7
#define __m_FH __m_0

#define __m_AV __m_0
#define __m_BV __m_w_1
#define __m_CV __m_w_5
#define __m_DV __m_w_5
#define __m_EV __m_w_1
#define __m_FV __m_0
#endif
#define scale_shift_init() \
	__m64 scale64 = _mm_set1_pi16(scale); \
	__m64 shift64 = _mm_set1_pi16(shift);

#define scale_shift(r) \
	r = _m_pmullw (r, scale64); \
	r = _m_paddw (r, __m_w_16); \
	r = _m_psrlwi (r, 5);\
	r = _m_paddw (r, shift64); 
    
#define scale_shift_0(s, r0, r1, d) \
	r0 = _m_punpcklbw(s, __m_0);\
	scale_shift(r0) \
	r1 = _m_punpckhbw(s, __m_0); \
	scale_shift(r1) \
	d = _m_packuswb( r0, r1);


#define mc_mul_r(s, r0, r1, r2,  r, op) \
	r2 = _m_punpcklbw(s, __m_0) ;\
	r2 = _m_pmullw (r2, r); \
	r0 = op(reg0,r2); \
	r2 = _m_punpckhbw(s, __m_0); \
	r2 = _m_pmullw (r2, r); \
	r1 = op(r1,r2);


#define mc_mul_2(s, r0, r1, r2,  r, op) \
	r2 = _m_punpcklbw(s, __m_0) ;\
	r2 = _m_psllwi (r2, 1); \
	r0 = op(r0,r2); \
	r2 = _m_punpckhbw(s, __m_0); \
	r2 = _m_psllwi (r2, 1); \
	r1 = op(r1,r2);

#define mc_mul_1(s, r0, r1, r2,  r, op) \
	r2 = _m_punpcklbw(s, __m_0) ;\
	r0 = op(r0,r2); \
	r2 = _m_punpckhbw(s, __m_0); \
	r1 = op(r1,r2);

#define mc_crop_w(r0, r1, round, shift,d ) \
	r0 = _m_paddw(r0,round); \
	r1 = _m_paddw(r1,round); \
	r0 = _m_psrlwi(r0,shift); \
	r1 = _m_psrlwi(r1,shift);
	


#define mc_mul_wd_r(s,r,r0,r1,r2,r3,r4,r5,op) \
			 r2 = _m_pmullw(s,r); \
			 r3 = _m_pmulhw(s,r); \
			 r4 = _m_punpcklwd(r2,r3); \
			 r5 = _m_punpckhwd(r2,r3); \
			 r0 = op(r0,r4);\
			 r1 = op(r1,r5);

#define __mc_chroma_abcd() \
	uint8_t *table = chroma_mc_xy_b_table + (((y << 3) + x) << 2); \
	__m64 __m_C_A =  _mm_set1_pi16(table[0]); \
	__m64 __m_C_B =  _mm_set1_pi16(table[1]); \
	__m64 __m_C_C =  _mm_set1_pi16(table[2]); \
	__m64 __m_C_D =  _mm_set1_pi16(table[3]);


#define __mc_chroma_mul_4(__s, __c) \
		reg1 = _m_from_int(*((uint32_t *)(__s))); \
		reg1 = _m_punpcklbw(reg1,__m_0); \
		reg1 = _m_pmullw(reg1, __c); \
		reg0 = _m_paddusw(reg0, reg1);

#define __mc_chroma_mul_8(__s, __c) \
		reg2 = _m_punpcklbw(__s, __m_0); \
		reg2 = _m_pmullw(reg2, __c); \
		reg3 = _m_punpckhbw(__s, __m_0); \
		reg3 = _m_pmullw(reg3, __c); \
		reg0 = _m_paddusw(reg0, reg2); \
		reg1 = _m_paddusw(reg1, reg3);

#define __MC_TEMP_STRIDE  16


#define __sum_sub_ba( a, b) \
	a = _m_paddw(a, b); \
	b = _m_paddw(b, b); \
	b = _m_psubw(b, a);

#define __butterfly(a,b,t,n,m) \
	t = _m_punpckh##n(a, b);  /* cgdh */ \
	a = _m_punpckl##n(a, b);  /* aebf */


#define __transpose4(a,b,c,d,t) \
    __butterfly(a,b,t,wd,q) /* a=aebf t=cgdh */\
    __butterfly(c,d,b,wd,q) /* c=imjn b=kolp */\
    __butterfly(a,c,d,dq,q) /* a=aeim d=bfjn */\
    __butterfly(t,b,c,dq,q) /* t=cgko c=dhlp */



#define __diff_gt_0(r1, r2, r3, r4, r5) \
	r5 = _m_psubusb(r2, r1); \
	r4 = _m_psubusb(r1, r2); \
	r4 = _m_por(r4, r5); \
	r4 = _m_psubusb(r4, r3);
		

#define __diff_gt_1(r1,r2,r3,r4,r5) \
    r5 = _m_psubusb(r2, r1); \
	r4 = _m_psubusb(r1, r2); \
    r5 = _m_psubusb(r5, r3); \
    r4 = _m_psubusb(r4, r3); \
    r4 = _m_pcmpeqb(r4, r5);

#define __load_mask_0() \
	__diff_gt_0(q1 ,q0, beta1_64 , mask0, reg0);/*abs(q1-q0)<beta1: out is mask0, temp is reg0*/ \
	__diff_gt_0(p1 ,p0, beta1_64 , reg1, reg0);/*abs(p1-p0)<beta1: out is reg1, temp is reg0*/ \
	mask0 = _m_por(mask0, reg1); \
	__diff_gt_0(q0, p0, alpha1_64, reg1, reg0); /*abs(p0-q0)<alpha1*/  \
	mask0 = _m_por(mask0, reg1); \
	mask0 = _m_pcmpeqb(mask0, __m_0);/*abs(p0-q0)<alpha1 && abs(p1-p0)<beta1 && abs(q1-q0)<beta1*/ 


#define __load_mask_1_2() \
	__diff_gt_0(p2, p0, beta1_64, mask1, reg0);/*abs(p2-p0) < beta1*/ \
	__diff_gt_0(p0, q0, alpha2_64, reg1, reg0);/*abs(p0-q0) < alpha2*/ \
	mask1  = _m_por(mask1, reg1); \
	mask1 = _m_pcmpeqb(mask1, __m_0);/*abs(p2-p0) < beta1 && abs(p0-q0) < alpha2*/ \
	 \
	__diff_gt_0(q2, q0,beta1_64 ,mask2, reg0);/*|q2-q0| > beta1*/ \
	mask2 = _m_por(mask2, reg1); \
	mask2 = _m_pcmpeqb(mask2, __m_0);/*abs(q2-q0) < beta1 && abs(q0-p0) < alpha2*/ 




#define __transpose6x8_mem(src,ss,dst,ds,tmp) \
		reg0 = *((__m64 *)src); src += ss;\
		reg1 = *((__m64 *)src); src += ss;\
		reg2 = *((__m64 *)src); src += ss;\
		reg3 = *((__m64 *)src); src += ss;\
		reg4 = *((__m64 *)src); src += ss;\
		reg5 = *((__m64 *)src); src += ss;\
		reg6 = *((__m64 *)src); src += ss;\
		__butterfly(reg0, reg1, reg7, bw, q); \
		__butterfly(reg2, reg3, reg1, bw, q); \
		__butterfly(reg4, reg5, reg3, bw, q); \
		tmp = reg1; \
		reg1 = *((__m64 *)src); \
		reg5 = _m_punpckhbw(reg6, reg1); \
		reg6 = _m_punpcklbw(reg6, reg1); \
		__butterfly(reg0, reg2, reg1, wd, q);  \
		__butterfly(reg4, reg6, reg2, wd, q); \
	    reg0 = _m_punpckhdq(reg0, reg4); \
		*((__m64 *)dst) = reg0; dst += ds; \
		reg6 = _m_punpcklwd(reg7, tmp); \
		reg7 = _m_punpckhwd(reg7, tmp); \
		__butterfly(reg1, reg2, reg0, dq, q); \
		__butterfly(reg3, reg5, reg2, wd, q); \
		__butterfly(reg6, reg3, reg5, dq, q); \
		reg7 = _m_punpckldq(reg7, reg2); \
		\
		*((__m64 *)dst) = reg1; dst += ds; \
		*((__m64 *)dst) = reg0; dst += ds; \
		*((__m64 *)dst) = reg6; dst += ds; \
		*((__m64 *)dst) = reg5; dst += ds; \
		*((__m64 *)dst) = reg7; 


#define __transpose8x4_store_core(s0,s1,s2,s3,dst, ds) \
		reg4 = _m_punpckhdq (s0, s0); \
		reg5 = _m_punpckhdq (s1, s1); \
		reg6 = _m_punpckhdq (s2, s2); \
		s0 = _m_punpcklbw(s0, s1); \
		s2 = _m_punpcklbw(s2, s3); \
		s1 = _m_punpcklwd(s0, s2); \
		s0 = _m_punpckhwd(s0, s2); \
		*((int *)dst) = _m_to_int(s1); dst += ds;\
		s1 = _m_punpckhdq(s1, s1); \
		*((int *)dst) = _m_to_int(s1);  dst += ds;\
		*((int *)dst) = _m_to_int(s0);  dst += ds;\
		s0 = _m_punpckhdq(s0, s0); \
		*((int *)dst) = _m_to_int(s0);  dst += ds;\
		\
		s3 = _m_punpckhdq(s3, s3); \
		reg4 = _m_punpcklbw(reg4, reg5); \
		reg6 = _m_punpcklbw(reg6, s3); \
		reg5 = _m_punpcklwd(reg4, reg6); \
		reg4 = _m_punpckhwd(reg4, reg6); \
		*((int *)dst) = _m_to_int(reg5);  dst += ds;\
		reg5 = _m_punpckhdq(reg5, reg5); \
		*((int *)dst) = _m_to_int(reg5);  dst += ds;\
		*((int *)dst) = _m_to_int(reg4);  dst += ds;\
		reg4 = _m_punpckhdq(reg4, reg4); \
		*((int *)dst) = _m_to_int(reg4); 


#define __transpose8x4_store_buf(src,ss, dst, ds) \
		reg0 = *((__m64 *)src); src += ss; \
		reg1 = *((__m64 *)src); src += ss; \
		reg2 = *((__m64 *)src); src += ss; \
		reg3 = *((__m64 *)src);\
		__transpose8x4_store_core(reg0,reg1,reg2,reg3,dst,ds);
	

#define __get_delta(r0,r1,r2,r3,_out, op) \
	reg0 = op(r0,__m_0); \
	reg1 = op(r1,__m_0); \
	reg1 = _m_psubw(reg0,reg1);/*(q0-p0)*/ \
	reg0 = reg1;\
	reg0 = _m_psllwi(reg0, 1);/*(q0-p0)*2*/ \
	reg0 = _m_paddw(reg0, reg1);/*(q0-p0)*3*/ \
	reg1 = op(r2,__m_0); \
	reg0 = _m_paddw(reg0, reg1); /*(q0-p0)*3 + p1*/ \
	reg1 = op(r3,__m_0); \
	reg0 = _m_psubw(reg0, reg1); /*(q0-p0)*3 + p1 - q1*/ \
	reg0 = _m_paddw(reg0, __m_w_4); /*(q0-p0)*3 + p1 - q1 + 4*/ \
	_out = _m_psrawi(reg0, 3);/*((q0-p0)*3 + p1 - q1 + 4) >> 3*/ 

#define __clip_delta() \
	delta_low = _m_pminsw(delta_low, tc_64); \
	delta_low = _m_pmaxsw(delta_low, _tc_64); \
	delta_high = _m_pminsw(delta_high, tc_64); \
	delta_high = _m_pmaxsw(delta_high, _tc_64);

#define __add_delta(r0,op,_out) \
	reg0 = _m_punpcklbw(r0,__m_0); \
	reg0 = op(reg0, delta_low);\
	reg1 = _m_punpckhbw(r0,__m_0); \
	reg1 = op(reg1, delta_high);\
	_out = _m_packuswb(reg0,reg1);

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sse2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __sum_sub_ba_sse2( a, b) \
	a = _mm_add_epi16(a, b); \
	b = _mm_add_epi16(b, b); \
	b = _mm_sub_epi16(b, a);


#define __butterfly_sse2(a,b,t,n) \
	t = _mm_unpackhi_epi##n(a, b); \
	a = _mm_unpacklo_epi##n(a, b);  


#define __transpose8_sse2(r0,r1,r2,r3,r4,r5,r6,r7,t) \
	__butterfly_sse2(r0, r1, t,  16); \
	__butterfly_sse2(r2, r3, r1, 16); \
	__butterfly_sse2(r4, r5, r3, 16); \
	__butterfly_sse2(r6, r7, r5, 16); \
	__butterfly_sse2(r0, r2, r7, 32); \
	__butterfly_sse2(t,  r1, r2, 32); \
	__butterfly_sse2(r3, r5, r1, 32); \
	__butterfly_sse2(r4, r6, r5, 32); \
	__butterfly_sse2(t,  r3, r6, 64); \
	__butterfly_sse2(r0, r4, r3, 64); \
	__butterfly_sse2(r7, r5, r4, 64); \
	__butterfly_sse2(r2, r1, r5, 64); 


#define __mc_chroma_abcd_sse2() \
	uint8_t *table = chroma_mc_xy_b_table + (((y << 3) + x) << 2); \
	__m128i __m_C_A =  _mm_set1_epi16(table[0]); \
	__m128i __m_C_B =  _mm_set1_epi16(table[1]); \
	__m128i __m_C_C =  _mm_set1_epi16(table[2]); \
	__m128i __m_C_D =  _mm_set1_epi16(table[3]);

#define __mc_chroma_mul_8_sse2(__s, __c) \
		reg1 = _mm_unpacklo_epi8(__s, _mm_setzero_si128()); \
		reg1 = _mm_mullo_epi16(reg1, __c); \
		reg0 = _mm_add_epi16(reg0, reg1); 

#define scale_shift_init_sse2() \
	__m128i scale128 = _mm_set1_epi16(scale); \
	__m128i shift128 = _mm_set1_epi16(shift);

#define scale_shift_sse2(r) \
	r = _mm_mullo_epi16 (r, scale128); \
	r = _mm_add_epi16 (r, _mm_set1_epi16(16)); \
	r = _mm_srli_epi16 (r, 5);\
	r = _mm_add_epi16 (r, shift128); 
    
#define scale_shift_0_8_sse2(s, r0, d) \
	r0 = _mm_unpacklo_epi8(s, _mm_setzero_si128());\
	scale_shift_sse2(r0) \
	d = _mm_packus_epi16( r0, r0); 
	// _mm_storel_epi64((__m128i *)d, r0);
		
#define scale_shift_0_16_sse2(s, r0, r1, d) \
	r0 = _mm_unpacklo_epi8(s, _mm_setzero_si128());\
	scale_shift_sse2(r0) \
	r1 = _mm_unpackhi_epi8(s, _mm_setzero_si128()); \
	scale_shift_sse2(r1) \
	d = _mm_packus_epi16( r0, r1);



#define mc_mul_r_sse2(s, r0, r, op) \
	s = _mm_unpacklo_epi8(s, _mm_setzero_si128()) ;\
	s = _mm_mullo_epi16 (s, _mm_set1_epi16(r)); \
	r0 = op(r0,s); 


#define mc_mul_2_sse2(s, r0, r, op) \
	s = _mm_unpacklo_epi8(s,  _mm_setzero_si128()) ; \
	s = _mm_slli_epi16 (s, 1); \
	r0 = op(r0,s); 
	

#define mc_mul_1_sse2(s, r0, r, op) \
	s = _mm_unpacklo_epi8(s, _mm_setzero_si128()) ;\
	r0 = op(r0,s); 

#define mc_mulhi_1_sse2(s, r0, r, op) \
	s = _mm_unpackhi_epi8(s, _mm_setzero_si128()) ;\
	r0 = op(r0,s); 


#define mc_crop_w_sse2(r0, round, shift) \
	r0 = _mm_add_epi16(r0, _mm_set1_epi16(round)); \
	r0 = _mm_srli_epi16(r0, shift); 
	
#define mc_mul_wd_r_sse2(s, r, op0, op1) \
		reg1 = _mm_mullo_epi16(s, _mm_set1_epi16(r));\
		reg2 = _mm_mulhi_epi16(s, _mm_set1_epi16(r));\
		reg1 = op0(reg1, reg2);\
		reg0 = op1(reg0, reg1);


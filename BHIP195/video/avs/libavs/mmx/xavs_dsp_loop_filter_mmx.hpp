
#ifdef __LOOP_FILTER_H
#define loop_filter_l2_mmx loop_filter_l2_h_mmx
#define loop_filter_l1_mmx loop_filter_l1_h_mmx
#define loop_filter_c2_mmx loop_filter_c2_h_mmx
#define loop_filter_c1_mmx loop_filter_c1_h_mmx
#define xavs_filter_l_mmx xavs_filter_lh_mmx
#define xavs_filter_c_mmx xavs_filter_ch_mmx

#define _transpos_begin() \
	__m64 reg0, reg1,reg2, reg3; \
	int stride2 = stride << 1; \
	int stride3 = stride2 + stride; \
	__m64 *P2 = (__m64 *)(_q0 - stride3); \
	__m64 *P1 = (__m64 *)(_q0 - stride2); \
	__m64 *P0 = (__m64 *)(_q0 - stride); \
	__m64 *Q0 = (__m64 *)(_q0 - 0); \
	__m64 *Q1 = (__m64 *)(_q0 + stride); \
	__m64 *Q2 = (__m64 *)(_q0 + stride2);

#define _transpos_end() 

#endif

#ifdef __LOOP_FILTER_V
#define loop_filter_l2_mmx loop_filter_l2_v_mmx
#define loop_filter_l1_mmx loop_filter_l1_v_mmx
#define loop_filter_c2_mmx loop_filter_c2_v_mmx
#define loop_filter_c1_mmx loop_filter_c1_v_mmx
#define xavs_filter_l_mmx xavs_filter_lv_mmx
#define xavs_filter_c_mmx xavs_filter_cv_mmx

#define _transpos_begin() \
	__m64 reg0, reg1,reg2, reg3,reg4,reg5,reg6,reg7,reg8;\
	DECLARE_ALIGNED(16,uint8_t,data)[6 * 16]; \
	__m64 *P2 = (__m64 *)(data + 0); \
	__m64 *P1 = (__m64 *)(data + 16); \
	__m64 *P0 = (__m64 *)(data + 32); \
	__m64 *Q0 = (__m64 *)(data + 48); \
	__m64 *Q1 = (__m64 *)(data + 64); \
	__m64 *Q2 = (__m64 *)(data + 80); \
	uint8_t *_src = _q0 - 4; \
	uint8_t *_dst = _q0 - 2; \
	uint8_t *_data = data; \
	__transpose6x8_mem(_src, stride,_data,16,reg8); \
	_data = data + 16;\


#define _transpos_end() \
	__transpose8x4_store_buf(_data, 16, _dst,stride)

#endif



static inline void loop_filter_l2_mmx(uint8_t *_q0, int stride, int alpha, int beta, int tc)
{
	__m64 alpha1_64 = _mm_set1_pi8(alpha - 1);
	__m64 beta1_64 = _mm_set1_pi8(beta - 1);
	__m64 alpha2_64 = _mm_set1_pi8((alpha>>2) + 1);
	
	__m64 mask0, mask1,mask2;
	__m64 low_q0_p0_2, high_q0_p0_2, low_sum,high_sum;
	__m64 low_sum_0_s2, high_sum_0_s2,value_sum_0_s2, value_sum_1_s2;
	
	_transpos_begin();
	__m64 q1 = *Q1, q0 = *Q0, p1 = *P1, p0 = *P0, q2 = *Q2, p2 = *P2;

	__load_mask_0();
	__load_mask_1_2();
///////////////////////////////////////////////////////////////////////////
//Get P0,P1
///////////////////////////////////////////////////////////////////////////
	reg0 = _m_punpcklbw(q0, __m_0);//q0
	reg1 = _m_punpcklbw(p0, __m_0);//p0
	low_q0_p0_2 =_m_paddw(reg1, reg0);//p0 + q0
	low_q0_p0_2 =_m_paddw(low_q0_p0_2, __m_w_2);//p0 + q0 + 2

	reg2 = _m_punpcklbw(p1, __m_0);//p1 low
	low_sum =_m_paddw(low_q0_p0_2, reg2);// (p0 + q0 +  2) + p1
	low_sum_0_s2 = _m_paddw(low_sum,reg1); //((p0 + q0 + 2) + p1 ) + p0 
	low_sum_0_s2 = _m_psrlwi(low_sum_0_s2, 2); //(((p0 + q0 + 2) + p1 ) + p0 ) >> 2

	reg0 = _m_punpckhbw(q0, __m_0);//q0
	reg1 = _m_punpckhbw(p0, __m_0);//p0
	high_q0_p0_2 =_m_paddw(reg1, reg0);//p0 + q0
	high_q0_p0_2 =_m_paddw(high_q0_p0_2, __m_w_2);//p0 + q0 + 2

	reg3 = _m_punpckhbw(p1, __m_0);//p1  high
	high_sum =_m_paddw(high_q0_p0_2, reg3);//(p0 + q0 + 2) + p1
	high_sum_0_s2 = _m_paddw(high_sum,reg1); //((p0 + q0 + 2) + p1 ) + p0 
	high_sum_0_s2 = _m_psrlwi(high_sum_0_s2, 2);//(((p0 + q0 + 2) + p1 ) + p0 ) >> 2
	value_sum_0_s2 =  _m_packuswb(low_sum_0_s2,high_sum_0_s2);

	//reg2 = p1 low  ,reg3 = p1 high
	reg0 = _m_paddw(reg2,low_sum);///((p0 + q0 + 2) + p1) + p1 low
	reg0 = _m_psrlwi(reg0, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 low
	reg1 = _m_paddw(reg3,high_sum);//((p0 + q0 + 2) + p1) + p1 high
	reg1 = _m_psrlwi(reg1, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 high
	value_sum_1_s2 =  _m_packuswb(reg0,reg1); //(((p0 + q0 + 2) + p1) + p1) >> 2

	//abs(p0-q0)<alpha && abs(p1-p0)<beta && abs(q1-q0)<beta
	//abs(p2-p0) < beta && abs(p0-q0) < alpha
	reg0 = _m_pand(mask1,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	reg2 = _m_pand(reg1, p1);
	reg3 = _m_pand(reg0,value_sum_1_s2);
	P1[0] = _m_paddb(reg2, reg3);//get P1

	reg2 = _m_pxor(mask0, __m_q_F16);
	reg2 = _m_pand(reg2, p0);
	reg3 = _m_pand(reg0, value_sum_0_s2);//P0 when (abs(p2-p0) < beta && abs(p0-q0) < alpha) == false
	reg2 = _m_paddb(reg2, reg3);
	reg1 = _m_pand(reg1, mask0);
	reg3 = _m_pand(reg1, value_sum_1_s2);//P0 when (abs(p2-p0) < beta && abs(p0-q0) < alpha) == true
	P0[0] = _m_paddb(reg2, reg3);//get P0

///////////////////////////////////////////////////////////////////////////
//Get Q0,Q1
///////////////////////////////////////////////////////////////////////////
	reg1 = _m_punpcklbw(q0, __m_0);//q0 low
	reg2 = _m_punpcklbw(q1, __m_0);//q1 low
	low_sum =_m_paddw(low_q0_p0_2, reg2);// (p0 + q0 +  2) + q1
	low_sum_0_s2 = _m_paddw(low_sum,reg1); //((p0 + q0 + 2) + q1 ) + q0 
	low_sum_0_s2 = _m_psrlwi(low_sum_0_s2, 2); //(((p0 + q0 + 2) + q1 ) + q0 ) >> 2

	reg1 = _m_punpckhbw(q0, __m_0);//q0  high
	reg3 = _m_punpckhbw(q1, __m_0);//q1  high
	high_sum =_m_paddw(high_q0_p0_2, reg3);//(p0 + q0 + 2) + q1
	high_sum_0_s2 = _m_paddw(high_sum,reg1); //((p0 + q0 + 2) + q1 ) + q0 
	high_sum_0_s2 = _m_psrlwi(high_sum_0_s2, 2);//(((p0 + q0 + 2) + q1 ) + q0 ) >> 2
	value_sum_0_s2 =  _m_packuswb(low_sum_0_s2,high_sum_0_s2);

	//reg2 = q1 low  ,reg3 = q1 high
	reg0 = _m_paddw(reg2,low_sum);///((p0 + q0 + 2) + q1) + q1 low
	reg0 = _m_psrlwi(reg0, 2);//(((p0 + q0 + 2) + q1) + q1) >> 2 low
	reg1 = _m_paddw(reg3,high_sum);//((p0 + q0 + 2) + q1) + q1 high
	reg1 = _m_psrlwi(reg1, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 high
	value_sum_1_s2 =  _m_packuswb(reg0,reg1); //(((p0 + q0 + 2) + q1) + q1) >> 2

	//abs(p0-q0)<alpha && abs(p1-p0)<beta && abs(q1-q0)<beta
	//abs(q2-q0) < beta && abs(p0-q0) < alpha
	reg0 = _m_pand(mask2,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	reg2 = _m_pand(reg1, q1);
	reg3 = _m_pand(reg0,value_sum_1_s2);
	Q1[0] = _m_paddb(reg2, reg3);//get Q1

	reg2 = _m_pxor(mask0, __m_q_F16);
	reg2 = _m_pand(reg2, q0);
	reg3 = _m_pand(reg0, value_sum_0_s2);//P0 when (abs(q2-p0) < beta && abs(p0-q0) < alpha) == false
	reg2 = _m_paddb(reg2, reg3);
	reg1 = _m_pand(reg1, mask0);
	reg3 = _m_pand(reg1, value_sum_1_s2);//P0 when (abs(q2-p0) < beta && abs(p0-q0) < alpha) == true
	Q0[0] = _m_paddb(reg2, reg3);//get Q0
	_transpos_end();
	_mm_empty();
}




		
		
static inline void loop_filter_l1_mmx(uint8_t *_q0, int stride, int alpha, int beta, int tc) 
{
	
	__m64 alpha1_64 = _mm_set1_pi8(alpha - 1);
	__m64 beta1_64 = _mm_set1_pi8(beta - 1);
	__m64 tc_64 = _mm_set1_pi16(tc);
	__m64 _tc_64 = _m_psubw(__m_0, tc_64);
	__m64 mask0, mask1,mask2;
	__m64 delta_low,delta_high;

	_transpos_begin();
	__m64 q1 = *Q1, q0 = *Q0, p1 = *P1, p0 = *P0, q2 = *Q2, p2 = *P2;

	
	
	__load_mask_0();
	__diff_gt_1(p2, p0, beta1_64, mask1, reg0) //|p2-p0| > beta-1
	__diff_gt_1(q2, q0, beta1_64, mask2, reg0) //|q2-q0| > beta-1	

	//delta = xavs_clip(((q0-p0)*3+P1-Q1+4)>>3,-tc, tc);
	__get_delta(q0,p0,p1,q1,delta_low,_m_punpcklbw);
	__get_delta(q0,p0,p1,q1,delta_high,_m_punpckhbw);
	__clip_delta();

	
	reg3 = _m_pxor(mask0, __m_q_F16);
	__add_delta(p0,_m_paddw,reg2);//P0 = xavs_clip_uint8(p0+delta);
	reg2 = _m_pand(mask0,reg2);
	p0 = _m_pand(reg3, p0);
	p0 = _m_paddb(reg2, p0);//get P0

	__add_delta(q0,_m_psubw,reg2);//Q0 = xavs_clip_uint8(q0-delta);
	reg2 = _m_pand(mask0,reg2);
	q0 = _m_pand(reg3, q0);
	q0 = _m_paddb(reg2, q0);//get Q0
	Q0[0] = q0; P0[0] = p0;

	//delta = xavs_clip(((P0-P1)*3+P2-Q0+4)>>3, -tc, tc);
	__get_delta(p0,p1,p2,q0,delta_low,_m_punpcklbw);
	__get_delta(p0,p1,p2,q0,delta_high,_m_punpckhbw);
	__clip_delta();
	//P1 = xavs_clip_uint8(P1+delta);
	__add_delta(p1,_m_paddw,reg2);
	reg0 = _m_pand(mask1,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	reg2 = _m_pand(reg0,reg2);
	reg3 = _m_pand(reg1, p1);
	P1[0] = _m_paddb(reg2, reg3);//get P1

	//delta = xavs_clip(((Q1-Q0)*3+P0-Q2+4)>>3, -tc, tc);
	__get_delta(q1,q0,p0,q2,delta_low,_m_punpcklbw);
	__get_delta(q1,q0,p0,q2,delta_high,_m_punpckhbw);
	__clip_delta();
	//Q1 = xavs_clip_uint8(Q1-delta);
	__add_delta(q1,_m_psubw,reg2);
	reg0 = _m_pand(mask2,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	reg2 = _m_pand(reg0,reg2);
	reg3 = _m_pand(reg1, q1);
	Q1[0] = _m_paddb(reg2, reg3);//get Q1
	_transpos_end();
	_mm_empty();	
 }
 


static inline void loop_filter_c2_mmx(uint8_t *_q0,int stride,int alpha, int beta) 
{
	
	__m64 alpha1_64 = _mm_set1_pi8(alpha - 1);
	__m64 beta1_64 = _mm_set1_pi8(beta - 1);
	__m64 alpha2_64 = _mm_set1_pi8((alpha>>2) + 1);
	
	__m64 mask0, mask1,mask2;
	__m64 low_q0_p0_2, high_q0_p0_2, low_sum,high_sum;
	__m64 low_sum_0_s2, high_sum_0_s2,value_sum_0_s2, value_sum_1_s2;
	
	_transpos_begin();
	__m64 q1 = *Q1, q0 = *Q0, p1 = *P1, p0 = *P0, q2 = *Q2, p2 = *P2;
	
	
	__load_mask_0();
	__load_mask_1_2();

	reg0 = _m_punpcklbw(q0, __m_0);//q0
	reg1 = _m_punpcklbw(p0, __m_0);//p0
	low_q0_p0_2 =_m_paddw(reg1, reg0);//p0 + q0
	low_q0_p0_2 =_m_paddw(low_q0_p0_2, __m_w_2);//p0 + q0 + 2

	reg2 = _m_punpcklbw(p1, __m_0);//p1 low
	low_sum =_m_paddw(low_q0_p0_2, reg2);// (p0 + q0 +  2) + p1
	low_sum_0_s2 = _m_paddw(low_sum,reg1); //((p0 + q0 + 2) + p1 ) + p0 
	low_sum_0_s2 = _m_psrlwi(low_sum_0_s2, 2); //(((p0 + q0 + 2) + p1 ) + p0 ) >> 2

	reg0 = _m_punpckhbw(q0, __m_0);//q0
	reg1 = _m_punpckhbw(p0, __m_0);//p0
	high_q0_p0_2 =_m_paddw(reg1, reg0);//p0 + q0
	high_q0_p0_2 =_m_paddw(high_q0_p0_2, __m_w_2);//p0 + q0 + 2

	reg3 = _m_punpckhbw(p1, __m_0);//p1  high
	high_sum =_m_paddw(high_q0_p0_2, reg3);//(p0 + q0 + 2) + p1
	high_sum_0_s2 = _m_paddw(high_sum,reg1); //((p0 + q0 + 2) + p1 ) + p0 
	high_sum_0_s2 = _m_psrlwi(high_sum_0_s2, 2);//(((p0 + q0 + 2) + p1 ) + p0 ) >> 2
	value_sum_0_s2 =  _m_packuswb(low_sum_0_s2,high_sum_0_s2);

	//reg2 = p1 low  ,reg3 = p1 high
	reg0 = _m_paddw(reg2,low_sum);///((p0 + q0 + 2) + p1) + p1 low
	reg0 = _m_psrlwi(reg0, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 low
	reg1 = _m_paddw(reg3,high_sum);//((p0 + q0 + 2) + p1) + p1 high
	reg1 = _m_psrlwi(reg1, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 high
	value_sum_1_s2 =  _m_packuswb(reg0,reg1); //(((p0 + q0 + 2) + p1) + p1) >> 2

	//abs(p0-q0)<alpha && abs(p1-p0)<beta && abs(q1-q0)<beta
	//abs(p2-p0) < beta && abs(p0-q0) < alpha
	reg0 = _m_pand(mask1,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	//reg2 = _m_pand(reg1, p1);
	//reg3 = _m_pand(reg0,value_sum_1_s2);
	//P1[0] = _m_paddb(reg2, reg3);//get P1

	reg2 = _m_pxor(mask0, __m_q_F16);
	reg2 = _m_pand(reg2, p0);
	reg3 = _m_pand(reg0, value_sum_0_s2);//P0 when (abs(p2-p0) < beta && abs(p0-q0) < alpha) == false
	reg2 = _m_paddb(reg2, reg3);
	reg1 = _m_pand(reg1, mask0);
	reg3 = _m_pand(reg1, value_sum_1_s2);//P0 when (abs(p2-p0) < beta && abs(p0-q0) < alpha) == true
	P0[0] = _m_paddb(reg2, reg3);//get P0

///////////////////////////////////////////////////////////////////////////
//Get Q0,Q1
///////////////////////////////////////////////////////////////////////////
	reg1 = _m_punpcklbw(q0, __m_0);//q0 low
	reg2 = _m_punpcklbw(q1, __m_0);//q1 low
	low_sum =_m_paddw(low_q0_p0_2, reg2);// (p0 + q0 +  2) + q1
	low_sum_0_s2 = _m_paddw(low_sum,reg1); //((p0 + q0 + 2) + q1 ) + q0 
	low_sum_0_s2 = _m_psrlwi(low_sum_0_s2, 2); //(((p0 + q0 + 2) + q1 ) + q0 ) >> 2

	reg1 = _m_punpckhbw(q0, __m_0);//q0  high
	reg3 = _m_punpckhbw(q1, __m_0);//q1  high
	high_sum =_m_paddw(high_q0_p0_2, reg3);//(p0 + q0 + 2) + q1
	high_sum_0_s2 = _m_paddw(high_sum,reg1); //((p0 + q0 + 2) + q1 ) + q0 
	high_sum_0_s2 = _m_psrlwi(high_sum_0_s2, 2);//(((p0 + q0 + 2) + q1 ) + q0 ) >> 2
	value_sum_0_s2 =  _m_packuswb(low_sum_0_s2,high_sum_0_s2);

	//reg2 = q1 low  ,reg3 = q1 high
	reg0 = _m_paddw(reg2,low_sum);///((p0 + q0 + 2) + q1) + q1 low
	reg0 = _m_psrlwi(reg0, 2);//(((p0 + q0 + 2) + q1) + q1) >> 2 low
	reg1 = _m_paddw(reg3,high_sum);//((p0 + q0 + 2) + q1) + q1 high
	reg1 = _m_psrlwi(reg1, 2);//(((p0 + q0 + 2) + p1) + p1) >> 2 high
	value_sum_1_s2 =  _m_packuswb(reg0,reg1); //(((p0 + q0 + 2) + q1) + q1) >> 2

	//abs(p0-q0)<alpha && abs(p1-p0)<beta && abs(q1-q0)<beta
	//abs(q2-q0) < beta && abs(p0-q0) < alpha
	reg0 = _m_pand(mask2,  mask0);
	reg1 = _m_pxor(reg0, __m_q_F16);
	//reg2 = _m_pand(reg1, q1);
	//reg3 = _m_pand(reg0,value_sum_1_s2);
	//Q1[0] = _m_paddb(reg2, reg3);//get Q1

	reg2 = _m_pxor(mask0, __m_q_F16);
	reg2 = _m_pand(reg2, q0);
	reg3 = _m_pand(reg0, value_sum_0_s2);//P0 when (abs(q2-p0) < beta && abs(p0-q0) < alpha) == false
	reg2 = _m_paddb(reg2, reg3);
	reg1 = _m_pand(reg1, mask0);
	reg3 = _m_pand(reg1, value_sum_1_s2);//P0 when (abs(q2-p0) < beta && abs(p0-q0) < alpha) == true
	Q0[0] = _m_paddb(reg2, reg3);//get Q0
	_transpos_end();
	_mm_empty();
 }

 static inline void loop_filter_c1_mmx(uint8_t *_q0,int stride,int alpha, int beta,int tc,uint64_t mask) 
{
	__m64 alpha1_64 = _mm_set1_pi8(alpha - 1);
	__m64 beta1_64 = _mm_set1_pi8(beta - 1);
	__m64 tc_64 = _mm_set1_pi16(tc);
	__m64 _tc_64 = _m_psubw(__m_0, tc_64);
	__m64 mask0, mask1 ={mask};
	__m64 delta_low,delta_high;


	_transpos_begin();
	__m64 q1 = *Q1, q0 = *Q0, p1 = *P1, p0 = *P0;


	__load_mask_0();
	mask0 = _m_pand(mask0, mask1);

	//delta = xavs_clip(((q0-p0)*3+P1-Q1+4)>>3,-tc, tc);
	__get_delta(q0,p0,p1,q1,delta_low,_m_punpcklbw);
	__get_delta(q0,p0,p1,q1,delta_high,_m_punpckhbw);
	__clip_delta();

	reg3 = _m_pxor(mask0, __m_q_F16);
	__add_delta(p0,_m_paddw,reg2);//P0 = xavs_clip_uint8(p0+delta);
	reg2 = _m_pand(mask0,reg2);
	p0 = _m_pand(reg3, p0);
	P0[0] = _m_paddb(reg2, p0);//get P0

	__add_delta(q0,_m_psubw,reg2);//Q0 = xavs_clip_uint8(q0-delta);
	reg2 = _m_pand(mask0,reg2);
	q0 = _m_pand(reg3, q0);
	Q0[0] = _m_paddb(reg2, q0);//get Q0
	_transpos_end();
	_mm_empty();

 }


 static void xavs_filter_l_mmx(uint8_t *d, int stride, int alpha, int beta, int tc,int bs1, int bs2) 
{
   if(bs1==2)
   {
	   loop_filter_l2_mmx(d,stride,alpha,beta,tc);
#ifdef __LOOP_FILTER_H
	   loop_filter_l2_mmx(d + 8, stride,alpha,beta,tc);
#endif
#ifdef __LOOP_FILTER_V
	   loop_filter_l2_mmx(d + 8 * stride, stride,alpha,beta,tc);
#endif
   }
    else 
	{
         if(bs1)
		 {
			 loop_filter_l1_mmx(d,stride,alpha,beta,tc);
		 }
         if (bs2)
		 {
#ifdef __LOOP_FILTER_H
			loop_filter_l1_mmx(d + 8,stride,alpha,beta,tc);           
#endif
#ifdef __LOOP_FILTER_V
			loop_filter_l1_mmx(d + 8 * stride,stride,alpha,beta,tc);           
#endif
		 }
     }
  
}
 
 static void xavs_filter_c_mmx(uint8_t *d, int stride, int alpha, int beta, int tc, int bs1, int bs2) 
 {
    if(bs1==2)
	{
		loop_filter_c2_mmx(d,stride,alpha,beta);
	}
    else 
	{
		 uint64_t mask = 0;
         if(bs1)
		 {
			 mask |= 0xFFFFFFFFULL;
		 }
         if(bs2) 
		 {
			 mask |= 0xFFFFFFFF00000000ULL;
		 }
		 loop_filter_c1_mmx(d,stride,alpha,beta,tc,mask);
		
     }
}

#undef loop_filter_l2_mmx
#undef loop_filter_l1_mmx
#undef loop_filter_c2_mmx
#undef loop_filter_c1_mmx
#undef xavs_filter_l_mmx
#undef xavs_filter_c_mmx
#undef _transpos_begin
#undef _transpos_end
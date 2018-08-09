
#define AH 1
#define BH 2
#define CH 96
#define DH 42
#define EH 7
#define FH 0
#define AV 0 
#define BV 1
#define CV 5
#define DV 5
#define EV 1
#define FV 0
#define MC_W 8
#define MC_H 8


#ifdef __MC_PUT_FUN_W
#define filt8_hv_ik_fun() \
	xavs_put_filt8_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_ki_fun() \
	xavs_put_filt8_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_fq_fun() \
	xavs_put_filt8_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_qf_fun() \
	xavs_put_filt8_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_egpr_fun() \
	xavs_put_filt8_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_j_fun() \
	xavs_put_filt8_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_nd_fun() \
	xavs_put_filt8_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_dn_fun() \
	xavs_put_filt8_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_h_fun() \
	xavs_put_filt8_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_b_fun() \
	xavs_put_filt8_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ca_fun() \
	xavs_put_filt8_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ac_fun() \
	xavs_put_filt8_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define __MC_OUT0_SSE2() \
	reg0 = _mm_packus_epi16(reg0, reg0);\
	scale_shift_0_8_sse2(reg0, reg0, reg0); \
	_mm_storel_epi64((__m128i*)dst, reg0);

#define __MC_OUT1_SSE2(r) \
	r = _mm_packus_epi16(r, r); \
	scale_shift_0_8_sse2(r, r, r); 

#define __MC_OUT2_SSE2() \
	reg0 = _m_paddw(reg0,reg1); \
	scale_shift_0_8_sse2(r, r, r); 
	
#define SACLE_SHIFT_INIT_SSE2() scale_shift_init_sse2()
#endif


#ifdef __MC_AVG_FUN_W
#define filt8_hv_ik_fun() \
	xavs_avg_filt8_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_ki_fun() \
	xavs_avg_filt8_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_fq_fun() \
	xavs_avg_filt8_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_qf_fun() \
	xavs_avg_filt8_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_egpr_fun() \
	xavs_avg_filt8_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_j_fun() \
	xavs_avg_filt8_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_nd_fun() \
	xavs_avg_filt8_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_dn_fun() \
	xavs_avg_filt8_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_h_fun() \
	xavs_avg_filt8_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_b_fun() \
	xavs_avg_filt8_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ca_fun() \
	xavs_avg_filt8_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ac_fun() \
	xavs_avg_filt8_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define __MC_OUT0_SSE2() \
	reg0 = _mm_packus_epi16(reg0, reg0);\
	scale_shift_0_8_sse2(reg0, reg0, reg0); \
	reg0 = _mm_avg_epu8(reg0, _mm_loadl_epi64((__m128i*)dst)); \
	_mm_storel_epi64((__m128i*)dst, reg0);

#define __MC_OUT1_SSE2() \
	reg0 = _m_packuswb(reg6, reg7); \
	scale_shift_0(reg0,reg1,reg2,reg0); \
	_dst[0] = _m_pavgb(_dst[0], reg0); 
	

#define __MC_OUT2_SSE2() \
	reg0 = _m_paddw(reg0,reg1); \
	scale_shift_0(reg0,reg1,reg2,reg0); \
	_dst[0] = _m_pavgb(_dst[0], reg0); 

	

#define SACLE_SHIFT_INIT_SSE2() scale_shift_init_sse2()
#endif

#ifdef __MC_PUT_FUN
#define filt8_hv_ik_fun() \
	xavs_put_filt8_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_ki_fun() \
	xavs_put_filt8_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_fq_fun() \
	xavs_put_filt8_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_qf_fun() \
	xavs_put_filt8_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_egpr_fun() \
	xavs_put_filt8_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_j_fun() \
	xavs_put_filt8_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_nd_fun() \
	xavs_put_filt8_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_dn_fun() \
	xavs_put_filt8_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_h_fun() \
	xavs_put_filt8_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_b_fun() \
	xavs_put_filt8_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ca_fun() \
	xavs_put_filt8_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ac_fun() \
	xavs_put_filt8_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define __MC_OUT0_SSE2() \
	reg0 = _mm_packus_epi16(reg0, reg0); \
	_mm_storel_epi64((__m128i*)dst, reg0);

#define __MC_OUT1_SSE2() \
	r = _mm_packus_epi16(r, r);

#define __MC_OUT2_SSE2() \
	reg0 = _mm_add_epi16(reg0, reg0);

#define SACLE_SHIFT_INIT_SSE2() 
#endif


#ifdef __MC_AVG_FUN
#define filt8_hv_ik_fun() \
	xavs_avg_filt8_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_ki_fun() \
	xavs_avg_filt8_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_fq_fun() \
	xavs_avg_filt8_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_qf_fun() \
	xavs_avg_filt8_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_egpr_fun() \
	xavs_avg_filt8_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_j_fun() \
	xavs_avg_filt8_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_nd_fun() \
	xavs_avg_filt8_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_dn_fun() \
	xavs_avg_filt8_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_h_fun() \
	xavs_avg_filt8_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_b_fun() \
	xavs_avg_filt8_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ca_fun() \
	xavs_avg_filt8_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ac_fun() \
	xavs_avg_filt8_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define __MC_OUT0_SSE2() \
	reg0 = _mm_packus_epi16(reg0, reg0); \
	reg0 = _mm_avg_epu8(reg0,  _mm_loadl_epi64((__m128i*)dst)); \
	_mm_storel_epi64((__m128i*)dst, reg0);

#define __MC_OUT1_SSE2() \
	reg0 = _m_packuswb(reg6, reg7); \
	_dst[0] = _m_pavgb(_dst[0], reg0);

#define __MC_OUT2_SSE2() \
	reg0 = _m_paddw(reg0,reg1); \
	_dst[0] = _m_pavgb(_dst[0], reg0);

#define SACLE_SHIFT_INIT_SSE2() 
#endif

static inline void filt8_hv_ik_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1, reg2, reg3;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	for(int i = 0; i < MC_H; i++) 
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(src1 + srcStride2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(src1 + srcStride));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(src1 - srcStride));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		reg1 = _srcBV;
		mc_mul_1_sse2(reg1, reg0, BV, _mm_sub_epi16);
		reg1 = _srcEV;
		mc_mul_1_sse2(reg1, reg0, EV, _mm_sub_epi16);
		_mm_store_si128((__m128i *)tmp, reg0);


		reg0 = _mm_unpackhi_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpackhi_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mulhi_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mulhi_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);

		_mm_store_si128((__m128i *)(tmp + 8), reg0);

		src1 += srcStride;
		tmp += __MC_TEMP_STRIDE;

	}

	tmp = temp ;
	for(int i = 0; i < MC_H; i++)  
	{
		__m128i _srcEH = _mm_loadu_si128((__m128i *)(tmp + 4));
		__m128i _srcDH = _mm_loadu_si128((__m128i *)(tmp + 3));
		__m128i _srcCH = _mm_loadu_si128((__m128i *)(tmp + 2));
		__m128i _srcBH = _mm_loadu_si128((__m128i *)(tmp + 1));
		__m128i _srcAH = _mm_loadu_si128((__m128i *)(tmp + 0));

		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpacklo_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpacklo_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg3 = _mm_srai_epi32(reg0, 10);
///////////////////////////////////////////////////////
		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpackhi_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpackhi_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg0 = _mm_srai_epi32(reg0, 10);

		reg0 = _mm_packus_epi32(reg3, reg0);
		__MC_OUT0_SSE2();
		dst+=dstStride;
		tmp += __MC_TEMP_STRIDE;  
	}
	
	/*SACLE_SHIFT_INIT();
	int16_t temp[8 * __MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	__m64 reg0, reg1, reg2,reg3;
	for(int i = 0; i < MC_H; i++)                        
	{
		{
			__m64* _dst = (__m64*)(tmp + 0);
			const __m64* _srcEV = (const __m64*)(src1 + srcStride2 + 0);
			const __m64* _srcDV = (const __m64*)(src1 + srcStride + 0);
			const __m64* _srcCV = (const __m64*)(src1 + 0);
			const __m64* _srcBV = (const __m64*)(src1 - srcStride + 0);

			reg0 = _m_punpcklbw(_srcCV[0], __m_0);
			reg1 = _m_punpckhbw(_srcCV[0], __m_0);
			reg2 = _m_punpcklbw(_srcDV[0], __m_0);
			reg3 = _m_punpckhbw(_srcDV[0], __m_0);
			reg0 = _m_paddw(reg0, reg2);
			reg1 = _m_paddw(reg1, reg3);
			reg0 = _m_pmullw(reg0,__m_CV);
			reg1 = _m_pmullw(reg1,__m_CV);
			mc_mul_1(_srcEV[0],    reg0,    reg1, reg2, __m_EV,_m_psubsw)
			reg2 = _m_punpcklbw(_srcBV[0], __m_0) ;
			_dst[0] = _m_psubsw(reg0,reg2); 
			reg2 = _m_punpckhbw(_srcBV[0], __m_0) ;
			_dst[1] = _m_psubsw(reg1,reg2); 
		}

		{
			__m64* _dst = (__m64*)(tmp + 8);
			const __m64* _srcEV = (const __m64*)(src1 + srcStride2 + 8);
			const __m64* _srcDV = (const __m64*)(src1 + srcStride + 8);
			const __m64* _srcCV = (const __m64*)(src1 + 8);
			const __m64* _srcBV = (const __m64*)(src1 - srcStride + 8);
			
			reg0 = _m_punpcklbw(_srcCV[0], __m_0);
			reg2 = _m_punpcklbw(_srcDV[0], __m_0);
			reg0 = _m_paddw(reg0, reg2);
			reg0 = _m_pmullw(reg0,__m_CV);

			reg2 = _m_punpcklbw(_srcEV[0], __m_0);
			reg0 = _m_psubsw(reg0, reg2);
			reg2 = _m_punpcklbw(_srcBV[0], __m_0) ;
			_dst[0] = _m_psubsw(reg0,reg2); 
		}
		

		src1+=srcStride;
		tmp += __MC_TEMP_STRIDE;
	}
	
	__m64 reg4, reg5,reg6,reg7;
	tmp = temp  ;
	for(int j=0; j<MC_H; j++)  
	{
		__m64* _dst = (__m64*)(dst);
		for(int i = 0; i < 8; i += 4)
		{
			const __m64* _srcEH = (const __m64*)(tmp + 4  + i);
			const __m64* _srcDH = (const __m64*)(tmp + 3  + i);
			const __m64* _srcCH = (const __m64*)(tmp + 2 + i);
			const __m64* _srcBH = (const __m64*)(tmp + 1 + i);
			const __m64* _srcAH = (const __m64*)(tmp + 0 + i);

			reg2 = _m_pmullw(_srcCH[0],__m_CH);
			reg3 = _m_pmulhw(_srcCH[0],__m_CH);
			reg0 = _m_punpcklwd(reg2,reg3);
			reg1 = _m_punpckhwd(reg2,reg3);

			mc_mul_wd_r(_srcDH[0],__m_DH, reg0,reg1,reg2,reg3,reg4,reg5,_m_paddd);
			mc_mul_wd_r(_srcEH[0],__m_EH, reg0,reg1,reg2,reg3,reg4,reg5,_m_psubd);
			mc_mul_wd_r(_srcBH[0],__m_BH, reg0,reg1,reg2,reg3,reg4,reg5,_m_psubd);
			mc_mul_wd_r(_srcAH[0],__m_AH, reg0,reg1,reg2,reg3,reg4,reg5,_m_psubd);

			reg0 = _m_paddd(reg0,__m_d_512);
			reg1 = _m_paddd(reg1,__m_d_512);
			reg0 = _m_psradi(reg0,10);
			reg1 = _m_psradi(reg1,10);
			if(i == 0)
			{
				reg6 = _m_packssdw(reg0, reg1);
			}
			else
			{
				reg7 = _m_packssdw(reg0, reg1);
			}
		}
		__MC_OUT1();

		dst+=dstStride;
		tmp += __MC_TEMP_STRIDE;  
	}
	_mm_empty(); */   
}


static inline void filt8_hv_ki_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1, reg2, reg3;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 1; 
	for(int i = 0; i < MC_H; i++) 
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(src1 + srcStride2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(src1 + srcStride));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(src1 - srcStride));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		reg1 = _srcBV;
		mc_mul_1_sse2(reg1, reg0, BV, _mm_sub_epi16);
		reg1 = _srcEV;
		mc_mul_1_sse2(reg1, reg0, EV, _mm_sub_epi16);
		_mm_store_si128((__m128i *)tmp, reg0);


		reg0 = _mm_unpackhi_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpackhi_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mulhi_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mulhi_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);

		_mm_store_si128((__m128i *)(tmp + 8), reg0);

		src1 += srcStride;
		tmp += __MC_TEMP_STRIDE;

	}

	tmp = temp ;
	for(int i = 0; i < MC_H; i++)  
	{
		__m128i _srcEH = _mm_loadu_si128((__m128i *)(tmp + 0));
		__m128i _srcDH = _mm_loadu_si128((__m128i *)(tmp + 1));
		__m128i _srcCH = _mm_loadu_si128((__m128i *)(tmp + 2));
		__m128i _srcBH = _mm_loadu_si128((__m128i *)(tmp + 3));
		__m128i _srcAH = _mm_loadu_si128((__m128i *)(tmp + 4));

		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpacklo_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpacklo_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg3 = _mm_srai_epi32(reg0, 10);
///////////////////////////////////////////////////////
		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpackhi_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpackhi_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg0 = _mm_srai_epi32(reg0, 10);

		reg0 = _mm_packus_epi32(reg3, reg0);
		__MC_OUT0_SSE2();
		dst+=dstStride;
		tmp += __MC_TEMP_STRIDE;  
	}
}

static inline void filt8_hv_fq_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1, reg2, reg3;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	src1 -= 2 * srcStride;
	for(int i = 0; i < 12; i++)      
	{
		__m128i _srcEV = _mm_loadl_epi64((__m128i *)(src1 + 2));
		__m128i _srcDV = _mm_loadl_epi64((__m128i *)(src1 + 1));
		__m128i _srcCV = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadl_epi64((__m128i *)(src1 - 1));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mul_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mul_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);		
		_mm_store_si128((__m128i *)tmp, reg0);

		src1 += srcStride;
		tmp += 8;
	}

	tmp = temp ;
	for(int i = 0; i < MC_H; i++)  
	{
		__m128i _srcEH = _mm_load_si128((__m128i *)(tmp + 32));
		__m128i _srcDH = _mm_load_si128((__m128i *)(tmp + 24));
		__m128i _srcCH = _mm_load_si128((__m128i *)(tmp + 16));
		__m128i _srcBH = _mm_load_si128((__m128i *)(tmp + 8));
		__m128i _srcAH = _mm_load_si128((__m128i *)(tmp + 0));

		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpacklo_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpacklo_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg3 = _mm_srai_epi32(reg0, 10);
///////////////////////////////////////////////////////
		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpackhi_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpackhi_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg0 = _mm_srai_epi32(reg0, 10);

		reg0 = _mm_packus_epi32(reg3, reg0);
		__MC_OUT0_SSE2();
		dst += dstStride;
		tmp += 8;  
	}
}


static inline void filt8_hv_qf_fun()
{

	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1, reg2, reg3;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	src1 -= srcStride;
	for(int i = 0; i < 12; i++)      
	{
		__m128i _srcEV = _mm_loadl_epi64((__m128i *)(src1 + 2));
		__m128i _srcDV = _mm_loadl_epi64((__m128i *)(src1 + 1));
		__m128i _srcCV = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadl_epi64((__m128i *)(src1 - 1));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mul_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mul_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);		
		_mm_store_si128((__m128i *)tmp, reg0);

		src1 += srcStride;
		tmp += 8;
	}

	tmp = temp ;
	for(int i = 0; i < MC_H; i++)  
	{
		__m128i _srcEH = _mm_load_si128((__m128i *)(tmp + 0));
		__m128i _srcDH = _mm_load_si128((__m128i *)(tmp + 8));
		__m128i _srcCH = _mm_load_si128((__m128i *)(tmp + 16));
		__m128i _srcBH = _mm_load_si128((__m128i *)(tmp + 24));
		__m128i _srcAH = _mm_load_si128((__m128i *)(tmp + 32));

		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpacklo_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpacklo_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpacklo_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg3 = _mm_srai_epi32(reg0, 10);
///////////////////////////////////////////////////////
		reg1 = _mm_mullo_epi16(_srcCH, _mm_set1_epi16(CH));
		reg2 = _mm_mulhi_epi16(_srcCH, _mm_set1_epi16(CH));
		reg0 = _mm_unpackhi_epi16(reg1, reg2);
		mc_mul_wd_r_sse2(_srcAH, AH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcBH, BH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcEH, EH, _mm_unpackhi_epi16, _mm_sub_epi32);
		mc_mul_wd_r_sse2(_srcDH, DH, _mm_unpackhi_epi16, _mm_add_epi32);
		reg0 = _mm_add_epi32(reg0, _mm_set1_epi32(512));
		reg0 = _mm_srai_epi32(reg0, 10);

		reg0 = _mm_packus_epi32(reg3, reg0);
		__MC_OUT0_SSE2();
		dst += dstStride;
		tmp += 8;  
	}
}

static inline void filt8_hv_egpr_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(src1 + srcStride2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(src1 + srcStride));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(src1 - srcStride));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		reg1 = _srcBV;
		mc_mul_1_sse2(reg1, reg0, BV, _mm_sub_epi16);
		reg1 = _srcEV;
		mc_mul_1_sse2(reg1, reg0, EV, _mm_sub_epi16);
		_mm_store_si128((__m128i *)tmp, reg0);


		reg0 = _mm_unpackhi_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpackhi_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mulhi_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mulhi_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);

		_mm_store_si128((__m128i *)(tmp + 8), reg0);

		src1 += srcStride;
		tmp += __MC_TEMP_STRIDE;
	}
#if 1
	tmp = temp+2;    
	for(int i = 0; i < MC_H; i++)                        
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(tmp + 2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(tmp + 1));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(tmp + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(tmp - 1));
		__m128i _src2 =  _mm_loadl_epi64((__m128i *)(src2+ 0));

		reg0 = _mm_add_epi16(_srcCV, _srcDV);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));
		reg0 = _mm_sub_epi16(reg0, _srcBV);
		reg0 = _mm_sub_epi16(reg0, _srcEV);
		reg0 = _mm_add_epi16(reg0, _mm_set1_epi16(64));

		reg1 = _mm_unpacklo_epi8(_src2,_mm_setzero_si128());
		reg1 = _mm_slli_epi16(reg1, 6);
	
		reg0 = _mm_adds_epi16(reg0, reg1);
		

		
		reg0 = _mm_srai_epi16(reg0, 7);
		__MC_OUT0_SSE2();

		src2 += srcStride;
		dst += dstStride;
		tmp += __MC_TEMP_STRIDE;   
	}
#else
#define put(a, b)  a = cm[((b)+64)>>7]
	 uint8_t *cm = crop_table + MAX_NEG_CROP;
	 tmp = temp+2;                         
	 for(int j=0; j<MC_H; j++)                        
	 {
		 for(int i=0;i<MC_W;i++)
		 {
			 put(dst[i], -BV*tmp[-1+i] + CV*tmp[0+i] + DV*tmp[1+i] - EV*tmp[2+i]+64*src2[0+i]); 
			
		 }
		 src2+=srcStride;
		 dst+=dstStride;
		 tmp+=__MC_TEMP_STRIDE;                                                        
	 }                                                               
#undef put
#endif

}


static inline void filt8_hv_j_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1;
	DECLARE_ALIGNED(16,int16_t, temp[8*__MC_TEMP_STRIDE]);
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(src1 + srcStride2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(src1 + srcStride));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(src1 - srcStride));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		reg1 = _srcBV;
		mc_mul_1_sse2(reg1, reg0, BV, _mm_sub_epi16);
		reg1 = _srcEV;
		mc_mul_1_sse2(reg1, reg0, EV, _mm_sub_epi16);
		
		_mm_store_si128((__m128i *)tmp, reg0);


		reg0 = _mm_unpackhi_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpackhi_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));

		mc_mulhi_1_sse2(_srcBV, reg0, BV, _mm_sub_epi16);
		mc_mulhi_1_sse2(_srcEV, reg0, EV, _mm_sub_epi16);

		_mm_store_si128((__m128i *)(tmp + 8), reg0);

		src1 += srcStride;
		tmp += __MC_TEMP_STRIDE;
	}

	tmp = temp + 2;
	for(int i = 0; i < MC_H; i++)                        
	{
		__m128i _srcEV = _mm_loadu_si128((__m128i *)(tmp + 2));
		__m128i _srcDV = _mm_loadu_si128((__m128i *)(tmp + 1));
		__m128i _srcCV = _mm_loadu_si128((__m128i *)(tmp + 0));
		__m128i _srcBV = _mm_loadu_si128((__m128i *)(tmp - 1));
		reg0 = _mm_add_epi16(_srcCV, _srcDV);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));
		reg0 = _mm_sub_epi16(reg0, _srcBV);
		reg0 = _mm_sub_epi16(reg0, _srcEV);

		reg0 = _mm_add_epi16(reg0, _mm_set1_epi16(32));
		reg0 = _mm_srai_epi16(reg0, 6);
		__MC_OUT0_SSE2();

		dst += dstStride;
		tmp += __MC_TEMP_STRIDE;   
	}
}

static inline void filt8_h_ac_fun()
{ 
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0;
	int srcStride2 = (srcStride << 1);
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEH = _mm_loadl_epi64((__m128i *)(src1 + 2));
		__m128i _srcDH = _mm_loadl_epi64((__m128i *)(src1 + 1));
		__m128i _srcCH = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBH = _mm_loadl_epi64((__m128i *)(src1 - 1));
		__m128i _srcAH = _mm_loadl_epi64((__m128i *)(src1 - 2));

		reg0 = _mm_unpacklo_epi8(_srcCH, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CH));
		mc_mul_r_sse2(_srcDH, reg0, DH, _mm_add_epi16);
		mc_mul_r_sse2(_srcEH, reg0, EH, _mm_subs_epu16);
		mc_mul_2_sse2(_srcBH, reg0, BH, _mm_subs_epu16);
		mc_mul_1_sse2(_srcAH, reg0, AH, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 64, 7);

		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}                                     
}



static inline void filt8_h_ca_fun()
{
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0;
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEH = _mm_loadl_epi64((__m128i *)(src1 - 1));
		__m128i _srcDH = _mm_loadl_epi64((__m128i *)(src1 - 0));
		__m128i _srcCH = _mm_loadl_epi64((__m128i *)(src1 + 1));
		__m128i _srcBH = _mm_loadl_epi64((__m128i *)(src1 + 2));
		__m128i _srcAH = _mm_loadl_epi64((__m128i *)(src1 + 3));

		reg0 = _mm_unpacklo_epi8(_srcCH, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CH));
		mc_mul_r_sse2(_srcDH, reg0, DH, _mm_add_epi16);
		mc_mul_r_sse2(_srcEH, reg0, EH, _mm_subs_epu16);
		mc_mul_2_sse2(_srcBH, reg0, BH, _mm_subs_epu16);
		mc_mul_1_sse2(_srcAH, reg0, AH, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 64, 7);

		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}
	
}

static inline void filt8_h_b_fun()
{ 

	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1;
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEV = _mm_loadl_epi64((__m128i *)(src1 + 2));
		__m128i _srcDV = _mm_loadl_epi64((__m128i *)(src1 + 1));
		__m128i _srcCV = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadl_epi64((__m128i *)(src1 - 1));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));		
		mc_mul_1_sse2(_srcBV, reg0, BV, _mm_subs_epu16);
		mc_mul_1_sse2(_srcEV, reg0, EV, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 4, 3);
		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}
}

static inline void filt8_v_h_fun()
{ 

	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0, reg1;
	int srcStride2 = (srcStride << 1);
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEV = _mm_loadl_epi64((__m128i *)(src1 + srcStride2));
		__m128i _srcDV = _mm_loadl_epi64((__m128i *)(src1 + srcStride));
		__m128i _srcCV = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBV = _mm_loadl_epi64((__m128i *)(src1 - srcStride));
		
		reg0 = _mm_unpacklo_epi8(_srcCV, _mm_setzero_si128());
		reg1 = _mm_unpacklo_epi8(_srcDV, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CV));
		mc_mul_1_sse2(_srcBV, reg0, BV, _mm_subs_epu16);
		mc_mul_1_sse2(_srcEV, reg0, EV, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 4, 3);
		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}
	

}



static inline void filt8_v_dn_fun()
{

	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0;
	int srcStride2 = (srcStride << 1);
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEH = _mm_loadl_epi64((__m128i *)(src1 + srcStride2));
		__m128i _srcDH = _mm_loadl_epi64((__m128i *)(src1 + srcStride));
		__m128i _srcCH = _mm_loadl_epi64((__m128i *)(src1 + 0));
		__m128i _srcBH = _mm_loadl_epi64((__m128i *)(src1 - srcStride));
		__m128i _srcAH = _mm_loadl_epi64((__m128i *)(src1 - srcStride2));

		reg0 = _mm_unpacklo_epi8(_srcCH, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CH));
		mc_mul_r_sse2(_srcDH, reg0, DH, _mm_add_epi16);
		mc_mul_r_sse2(_srcEH, reg0, EH, _mm_subs_epu16);
		mc_mul_2_sse2(_srcBH, reg0, BH, _mm_subs_epu16);
		mc_mul_1_sse2(_srcAH, reg0, AH, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 64, 7);

		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}
}



static inline void filt8_v_nd_fun()
{
	
	SACLE_SHIFT_INIT_SSE2();
	__m128i reg0;
	int srcStride2 = (srcStride << 1);
	for(int i = 0; i < MC_H; i++)      
	{
		__m128i _srcEH = _mm_loadl_epi64((__m128i *)(src1 - srcStride));
		__m128i _srcDH = _mm_loadl_epi64((__m128i *)(src1 - 0));
		__m128i _srcCH = _mm_loadl_epi64((__m128i *)(src1 + srcStride));
		__m128i _srcBH = _mm_loadl_epi64((__m128i *)(src1 + srcStride2));
		__m128i _srcAH = _mm_loadl_epi64((__m128i *)(src1 + srcStride2 + srcStride));

		reg0 = _mm_unpacklo_epi8(_srcCH, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, _mm_set1_epi16(CH));
		mc_mul_r_sse2(_srcDH, reg0, DH, _mm_add_epi16);
		mc_mul_r_sse2(_srcEH, reg0, EH, _mm_subs_epu16);
		mc_mul_2_sse2(_srcBH, reg0, BH, _mm_subs_epu16);
		mc_mul_1_sse2(_srcAH, reg0, AH, _mm_subs_epu16);
		mc_crop_w_sse2(reg0, 64, 7);

		__MC_OUT0_SSE2();
		src1 += srcStride;
		dst += dstStride;
	}
}


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


#undef  filt8_hv_ik_fun
#undef  filt8_hv_ki_fun
#undef  filt8_hv_fq_fun
#undef  filt8_hv_qf_fun
#undef  filt8_hv_egpr_fun
#undef  filt8_hv_j_fun
#undef  filt8_v_nd_fun
#undef  filt8_v_dn_fun
#undef  filt8_v_h_fun
#undef  filt8_h_b_fun
#undef  filt8_h_ca_fun
#undef  filt8_h_ac_fun
#undef  __MC_OUT0_SSE2
#undef  __MC_OUT1_SSE2
#undef  __MC_OUT2_SSE2


#ifdef __MC_PUT_FUN
static inline void xavs_put_filt16_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_fq_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_fq_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_qf_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_qf_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_dn_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_dn_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_nd_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_nd_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_egpr_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_egpr_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_j_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_j_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}



static inline void xavs_put_filt16_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_b_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_b_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_h_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_h_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ac_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ac_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ca_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ca_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ki_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ki_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ik_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ik_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static void xavs_qpel16_put_mc10_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_ac_sse2(dst,src,src+stride+1,stride,stride);
}

static void xavs_qpel16_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_b_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc30_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_ca_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_dn_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc11_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc21_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_fq_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc31_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_h_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_put_mc12_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_ik_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc22_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_j_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_put_mc32_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_ki_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc13_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+stride,stride,stride);

}

static void xavs_qpel16_put_mc23_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_qf_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc33_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc10_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_ac_sse2(dst,src,src+stride+1,stride,stride);

}
/*
static void xavs_qpel8_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride);

}
*/
static void xavs_qpel8_put_mc30_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_ca_sse2(dst,src,src+stride+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_sse2(dst,src,src,stride,stride);
}*/

static void xavs_qpel8_put_mc11_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel8_put_mc21_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_fq_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel8_put_mc31_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_sse2(dst,src,src+1,stride,stride);
}
*/
static void xavs_qpel8_put_mc12_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_ik_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc22_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_j_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_put_mc32_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_ki_sse2(dst,src,src+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}*/

static void xavs_qpel8_put_mc13_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+stride,stride,stride);

}

static void xavs_qpel8_put_mc23_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_qf_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc33_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride);

}


static void xavs_qpel8_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_sse2(dst,src,src,stride,stride);
}

static void xavs_qpel8_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_sse2(dst,src,src+1,stride,stride);
}


static void xavs_qpel8_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}
#endif

#ifdef __MC_AVG_FUN

static inline void xavs_avg_filt16_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_fq_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_fq_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_avg_filt16_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_qf_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_qf_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_dn_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_dn_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_nd_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_nd_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_egpr_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_egpr_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_j_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_j_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}



static inline void xavs_avg_filt16_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_b_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_b_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_h_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_h_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ac_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ac_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ca_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ca_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_avg_filt16_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ik_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ik_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ki_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ki_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static void xavs_qpel16_avg_mc10_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_ac_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_b_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc30_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_ca_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_dn_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc11_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc21_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_fq_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc31_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_avg_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_h_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_avg_mc12_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_ik_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc22_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_j_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_avg_mc32_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_ki_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_avg_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc13_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+stride,stride,stride);

}

static void xavs_qpel16_avg_mc23_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_qf_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc33_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc10_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_ac_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc30_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_ca_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_dn_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc11_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc21_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_fq_sse2(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc31_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel8_avg_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_h_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_avg_mc12_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_ik_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc22_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_j_sse2(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_avg_mc32_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_ki_sse2(dst,src,src+1,stride,stride);

}

static void xavs_qpel8_avg_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc13_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+stride,stride,stride);

}

static void xavs_qpel8_avg_mc23_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_qf_sse2(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc33_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+stride+1,stride,stride);

}


#endif

#ifdef __MC_PUT_FUN_W
static inline void xavs_put_filt16_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_fq_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_fq_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_qf_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_qf_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_dn_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_dn_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_nd_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_nd_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_egpr_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_egpr_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_j_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_j_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}



static inline void xavs_put_filt16_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_b_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_b_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_h_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_h_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ac_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ac_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ca_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ca_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ki_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ki_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ik_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ik_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static void xavs_qpel16_put_mc10_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ac_sse2(dst,src,src+stride+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_b_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc30_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ca_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_dn_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc11_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc21_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_fq_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc31_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_h_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc12_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ik_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc22_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_j_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc32_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ki_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_nd_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc13_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc23_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_qf_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc33_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc10_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ac_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}
/*
static void xavs_qpel8_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride);

}
*/
static void xavs_qpel8_put_mc30_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ca_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_sse2(dst,src,src,stride,stride);
}*/

static void xavs_qpel8_put_mc11_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc21_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_fq_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc31_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_sse2(dst,src,src+1,stride,stride);
}
*/
static void xavs_qpel8_put_mc12_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ik_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc22_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_j_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_put_mc32_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ki_sse2(dst,src,src+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride);

}*/

static void xavs_qpel8_put_mc13_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc23_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_qf_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc33_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}




static void xavs_qpel8_put_mc20_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}


static void xavs_qpel8_put_mc01_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_dn_sse2(dst,src,src,stride,stride,scale,shift);
}

static void xavs_qpel8_put_mc02_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_h_sse2(dst,src,src+1,stride,stride,scale,shift);
}


static void xavs_qpel8_put_mc03_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

#endif

#ifdef __MC_AVG_FUN_W
static inline void xavs_avg_filt16_hv_fq_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_fq_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_fq_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_fq_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_avg_filt16_hv_qf_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_qf_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_qf_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_qf_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_dn_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_dn_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_dn_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_dn_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_nd_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_nd_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_nd_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_nd_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_egpr_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_egpr_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_egpr_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_egpr_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_j_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_j_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_j_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_j_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}



static inline void xavs_avg_filt16_h_b_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_b_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_b_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_b_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_h_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_h_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_h_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_h_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_h_ac_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ac_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ac_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ac_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_h_ca_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ca_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ca_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ca_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_avg_filt16_hv_ik_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ik_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ik_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ik_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_ki_sse2(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ki_sse2(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ki_sse2(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ki_sse2(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static void xavs_qpel16_avg_mc10_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ac_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc20_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_b_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc30_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ca_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc01_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_dn_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc11_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc21_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_fq_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc31_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc02_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_h_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_avg_mc12_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ik_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc22_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_j_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_avg_mc32_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ki_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc03_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_nd_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc13_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc23_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_qf_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc33_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc10_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ac_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc20_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_b_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc30_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ca_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc01_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_dn_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc11_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc21_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq_sse2(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc31_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc02_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_h_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_avg_mc12_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc22_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_j_sse2(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_avg_mc32_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki_sse2(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc03_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_nd_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc13_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc23_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc33_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_sse2(dst,src,src+stride+1,stride,stride,scale,shift);

}


#endif
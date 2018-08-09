
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


#ifdef __MC_PUT_FUN_W
#define filt8_hv_ik_fun() \
	xavs_put_filt8_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_ki_fun() \
	xavs_put_filt8_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_fq_fun() \
	xavs_put_filt8_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_qf_fun() \
	xavs_put_filt8_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_egpr_fun() \
	xavs_put_filt8_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_j_fun() \
	xavs_put_filt8_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_nd_fun() \
	xavs_put_filt8_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_dn_fun() \
	xavs_put_filt8_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_h_fun() \
	xavs_put_filt8_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_b_fun() \
	xavs_put_filt8_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ca_fun() \
	xavs_put_filt8_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ac_fun() \
	xavs_put_filt8_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define __MC_OUT0() \
	reg0 = _m_packuswb(reg0,reg1);\
	scale_shift_0(reg0,reg1,reg2,_dst[0]); 

#define __MC_OUT1() \
	reg0 = _m_packuswb(reg6, reg7); \
	scale_shift_0(reg0,reg1,reg2,_dst[0]); 

#define __MC_OUT2() \
	reg0 = _m_paddw(reg0,reg1); \
	scale_shift_0(reg0,reg1,reg2,_dst[0]); 
	
#define SACLE_SHIFT_INIT() scale_shift_init()
#endif


#ifdef __MC_AVG_FUN_W
#define filt8_hv_ik_fun() \
	xavs_avg_filt8_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_ki_fun() \
	xavs_avg_filt8_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_fq_fun() \
	xavs_avg_filt8_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_qf_fun() \
	xavs_avg_filt8_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_egpr_fun() \
	xavs_avg_filt8_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_hv_j_fun() \
	xavs_avg_filt8_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_nd_fun() \
	xavs_avg_filt8_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_dn_fun() \
	xavs_avg_filt8_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_v_h_fun() \
	xavs_avg_filt8_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_b_fun() \
	xavs_avg_filt8_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ca_fun() \
	xavs_avg_filt8_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define filt8_h_ac_fun() \
	xavs_avg_filt8_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)

#define __MC_OUT0() \
	reg0 = _m_packuswb(reg0,reg1); \
	scale_shift_0(reg0,reg1,reg2, reg0); \
	_dst[0] = _m_pavgb(_dst[0], reg0); 
	

#define __MC_OUT1() \
	reg0 = _m_packuswb(reg6, reg7); \
	scale_shift_0(reg0,reg1,reg2,reg0); \
	_dst[0] = _m_pavgb(_dst[0], reg0); 
	

#define __MC_OUT2() \
	reg0 = _m_paddw(reg0,reg1); \
	scale_shift_0(reg0,reg1,reg2,reg0); \
	_dst[0] = _m_pavgb(_dst[0], reg0); 

	

#define SACLE_SHIFT_INIT() scale_shift_init()
#endif

#ifdef __MC_PUT_FUN
#define filt8_hv_ik_fun() \
	xavs_put_filt8_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_ki_fun() \
	xavs_put_filt8_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_fq_fun() \
	xavs_put_filt8_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_qf_fun() \
	xavs_put_filt8_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_egpr_fun() \
	xavs_put_filt8_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_j_fun() \
	xavs_put_filt8_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_nd_fun() \
	xavs_put_filt8_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_dn_fun() \
	xavs_put_filt8_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_h_fun() \
	xavs_put_filt8_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_b_fun() \
	xavs_put_filt8_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ca_fun() \
	xavs_put_filt8_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ac_fun() \
	xavs_put_filt8_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define __MC_OUT0() \
	_dst[0] = _m_packuswb(reg0,reg1);

#define __MC_OUT1() \
	_dst[0] = _m_packuswb(reg6, reg7);

#define __MC_OUT2() \
	_dst[0] = _m_paddw(reg0,reg1);

#define SACLE_SHIFT_INIT() 
#endif


#ifdef __MC_AVG_FUN
#define filt8_hv_ik_fun() \
	xavs_avg_filt8_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_ki_fun() \
	xavs_avg_filt8_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_fq_fun() \
	xavs_avg_filt8_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_qf_fun() \
	xavs_avg_filt8_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_egpr_fun() \
	xavs_avg_filt8_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_hv_j_fun() \
	xavs_avg_filt8_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_nd_fun() \
	xavs_avg_filt8_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_dn_fun() \
	xavs_avg_filt8_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_v_h_fun() \
	xavs_avg_filt8_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_b_fun() \
	xavs_avg_filt8_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ca_fun() \
	xavs_avg_filt8_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define filt8_h_ac_fun() \
	xavs_avg_filt8_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)

#define __MC_OUT0() \
	reg0 = _m_packuswb(reg0,reg1); \
	_dst[0] = _m_pavgb(_dst[0], reg0);

#define __MC_OUT1() \
	reg0 = _m_packuswb(reg6, reg7); \
	_dst[0] = _m_pavgb(_dst[0], reg0);

#define __MC_OUT2() \
	reg0 = _m_paddw(reg0,reg1); \
	_dst[0] = _m_pavgb(_dst[0], reg0);

#define SACLE_SHIFT_INIT() 
#endif

static inline void filt8_hv_ik_fun()
{
	SACLE_SHIFT_INIT();
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
	_mm_empty();    
}


static inline void filt8_hv_ki_fun()
{
	SACLE_SHIFT_INIT();
	int16_t temp[8 * __MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	int srcStride2 = (srcStride << 1);
	src1 -= 1; 
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
			const __m64* _srcEH = (const __m64*)(tmp + 0  + i);
			const __m64* _srcDH = (const __m64*)(tmp + 1  + i);
			const __m64* _srcCH = (const __m64*)(tmp + 2 + i);
			const __m64* _srcBH = (const __m64*)(tmp + 3 + i);
			const __m64* _srcAH = (const __m64*)(tmp + 4 + i);

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
	_mm_empty(); 
}

static inline void filt8_hv_fq_fun()
{
	SACLE_SHIFT_INIT();
	int j;
	int16_t temp[8 * __MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	src1 -= 2 * srcStride;
	__m64 reg0, reg1, reg2,reg3;
	for(j= 0;j < 12;j++)
	{

		__m64* _dst = (__m64*)(tmp);
		const __m64* _srcEV = (const __m64*)(src1 + 2);
		const __m64* _srcDV = (const __m64*)(src1 + 1);
		const __m64* _srcCV = (const __m64*)(src1 );
		const __m64* _srcBV = (const __m64*)(src1 - 1);

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

		src1+=srcStride;
		tmp += 8;
	}
	__m64 reg4, reg5,reg6,reg7;
	tmp = temp  ;
	for(j=0; j<MC_H; j++)  
	{
		__m64* _dst = (__m64*)(dst);
		for(int i = 0; i < 8; i += 4)
		{
			const __m64* _srcEH = (const __m64*)(tmp + 32  + i);
			const __m64* _srcDH = (const __m64*)(tmp + 24  + i);
			const __m64* _srcCH = (const __m64*)(tmp + 16 + i);
			const __m64* _srcBH = (const __m64*)(tmp + 8 + i);
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
		tmp+=8;  
	}
	_mm_empty();
}



static inline void filt8_hv_qf_fun()
{
	SACLE_SHIFT_INIT();
	int j;
	int16_t temp[8 * __MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	src1 -= srcStride;
	__m64 reg0, reg1, reg2,reg3;
	for(j = 0;j < 12 ;j++)
	{

		__m64* _dst = (__m64*)(tmp);
		const __m64* _srcEV = (const __m64*)(src1 + 2);
		const __m64* _srcDV = (const __m64*)(src1 + 1);
		const __m64* _srcCV = (const __m64*)(src1 );
		const __m64* _srcBV = (const __m64*)(src1 - 1);

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

		src1+=srcStride;
		tmp += 8;
	}
	__m64 reg4, reg5,reg6,reg7;
	tmp = temp ;
	for(j=0; j<MC_H; j++)  
	{
		__m64* _dst = (__m64*)(dst);
		for(int i = 0; i < 8; i += 4)
		{
			const __m64* _srcEH = (const __m64*)(tmp + 0  + i);
			const __m64* _srcDH = (const __m64*)(tmp + 8  + i);
			const __m64* _srcCH = (const __m64*)(tmp + 16 + i);
			const __m64* _srcBH = (const __m64*)(tmp + 24 + i);
			const __m64* _srcAH = (const __m64*)(tmp + 32 + i);

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
		tmp+=8;  
	}
	_mm_empty();

}

static inline void filt8_hv_egpr_fun()
{
	SACLE_SHIFT_INIT();
	int i;
	int16_t temp[8*__MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	__m64 reg0, reg1, reg2,reg3;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	for(i = 0; i < MC_H; i++)                        
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
	
	__m64 mask0,mask1, reg4;
	tmp = temp + 2;
	for(i = 0; i < MC_H; i++)                        
	{
		__m64* _dst = (__m64*)(dst);
		const __m64* _srcEV = (const __m64*)(tmp + 2);
		const __m64* _srcDV = (const __m64*)(tmp + 1);
		const __m64* _srcCV = (const __m64*)(tmp);
		const __m64* _srcBV = (const __m64*)(tmp - 1);
		const __m64* _src2 =  (const __m64*)(src2);

		

		reg0 = _m_paddw(_srcCV[0], _srcDV[0]);
		reg1 = _m_paddw(_srcCV[1], _srcDV[1]);
		reg0 = _m_pmullw(reg0,__m_CV);
		reg1 = _m_pmullw(reg1,__m_CV);

	
		reg0 = _m_psubw(reg0, _srcBV[0]);
		reg1 = _m_psubw(reg1, _srcBV[1]);
		reg0 = _m_psubw(reg0, _srcEV[0]);
		reg1 = _m_psubw(reg1, _srcEV[1]);
		
		reg0 = _m_paddw(reg0, __m_w_64);
		reg1 = _m_paddw(reg1, __m_w_64);

		reg2 = _m_punpcklbw(_src2[0], __m_0);
		reg3 = _m_punpckhbw(_src2[0], __m_0);
		reg2 = _m_pmullw(reg2,__m_w_64);
		reg3 = _m_pmullw(reg3,__m_w_64);
		
		mask0 = _m_pcmpgtw(reg0, __m_0);
		mask0 = _m_packsswb(mask0,mask0);
		mask1 = _m_pcmpgtw(reg1, __m_0);
		mask1 = _m_packsswb(mask1,mask1);
		mask0 = _m_punpckldq(mask0,mask1);
		mask1 = _m_pxor(mask0,__m_q_F16);

		reg0 = _m_paddw(reg0, reg2);
		reg1 = _m_paddw(reg1, reg3);

		reg2 = _m_psrawi( reg0, 7);
		reg3 = _m_psrawi( reg1, 7);
		reg4 = _m_packuswb(reg2,reg3);
		
		reg2 = _m_psrlwi( reg0, 7);
		reg3 = _m_psrlwi( reg1, 7);
		reg0 = _m_packuswb(reg2,reg3);

		reg1 = _m_pand(reg4,mask1);
		reg0 = _m_pand(reg0,mask0);

		__MC_OUT2();
		

		src2 += srcStride;
		dst += dstStride;
		tmp += __MC_TEMP_STRIDE;   
	}
	 _mm_empty();
}


static inline void filt8_hv_j_fun()
{
	SACLE_SHIFT_INIT();
	int i;
	int16_t temp[8*__MC_TEMP_STRIDE];
	int16_t *tmp = temp;
	__m64 reg0, reg1, reg2,reg3;
	int srcStride2 = (srcStride << 1);
	src1 -= 2; 
	for(i = 0; i < MC_H; i++)                        
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
		tmp+=__MC_TEMP_STRIDE;
	}
	
	tmp = temp + 2;
	for(i = 0; i < MC_H; i++)                        
	{
		__m64* _dst = (__m64*)(dst);
		const __m64* _srcEV = (const __m64*)(tmp + 2);
		const __m64* _srcDV = (const __m64*)(tmp + 1);
		const __m64* _srcCV = (const __m64*)(tmp);
		const __m64* _srcBV = (const __m64*)(tmp - 1);
		reg0 = _m_paddw(_srcCV[0], _srcDV[0]);
		reg1 = _m_paddw(_srcCV[1], _srcDV[1]);
		reg0 = _m_pmullw(reg0,__m_CV);
		reg1 = _m_pmullw(reg1,__m_CV);
		reg0 = _m_psubw(reg0, _srcBV[0]);
		reg1 = _m_psubw(reg1, _srcBV[1]);
		reg0 = _m_psubw(reg0, _srcEV[0]);
		reg1 = _m_psubw(reg1, _srcEV[1]);
		
		reg0 = _m_paddw(reg0, __m_w_32);
		reg1 = _m_paddw(reg1, __m_w_32);
		reg0 = _m_psrawi( reg0, 6);
		reg1 = _m_psrawi( reg1, 6);

		__MC_OUT0();
		

		dst += dstStride;
		tmp += __MC_TEMP_STRIDE;   
	}

	 _mm_empty();
  
}


static inline void filt8_h_ac_fun()
{ 
 
	SACLE_SHIFT_INIT();
	int i;
	__m64 reg0, reg1, reg2;
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEH = (const __m64*)(src1 + 2);
		 const __m64* _srcDH = (const __m64*)(src1 + 1);
		 const __m64* _srcCH = (const __m64*)(src1);
		 const __m64* _srcBH = (const __m64*)(src1 - 1);
		 const __m64* _srcAH = (const __m64*)(src1 - 2);
		 reg0 = _m_punpcklbw(_srcCH[0], __m_0);
		 reg0 = _m_pmullw (reg0, __m_CH);
		 reg1 = _m_punpckhbw(_srcCH[0], __m_0);
		 reg1 = _m_pmullw (reg1, __m_CH);

		 mc_mul_r(_srcDH[0], reg0,reg1,reg2, __m_DH, _m_paddw)
		 mc_mul_r(_srcEH[0], reg0,reg1,reg2, __m_EH, _m_psubusw) 
		 mc_mul_2(_srcBH[0], reg0,reg1,reg2, __m_BH,_m_psubusw)
		 mc_mul_1(_srcAH[0], reg0,reg1,reg2, __m_AH,_m_psubusw)

		 mc_crop_w(reg0, reg1, __m_w_64, 7,_dst[0]) 
		__MC_OUT0();

		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();
                                         
}



static inline void filt8_h_ca_fun()
{
	SACLE_SHIFT_INIT();
	 int i;
	__m64 reg0, reg1, reg2;
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEH = (const __m64*)(src1 - 1);
		 const __m64* _srcDH = (const __m64*)(src1);
		 const __m64* _srcCH = (const __m64*)(src1 + 1);
		 const __m64* _srcBH = (const __m64*)(src1 + 2);
		 const __m64* _srcAH = (const __m64*)(src1 + 3);
		 reg0 = _m_punpcklbw(_srcCH[0], __m_0);
		 reg0 = _m_pmullw (reg0, __m_CH);
		 reg1 = _m_punpckhbw(_srcCH[0], __m_0);
		 reg1 = _m_pmullw (reg1, __m_CH);

		 mc_mul_r(_srcDH[0], reg0,reg1,reg2, __m_DH, _m_paddw)
		 mc_mul_r(_srcEH[0], reg0,reg1,reg2, __m_EH, _m_psubusw) 
		 mc_mul_2(_srcBH[0], reg0,reg1,reg2, __m_BH,_m_psubusw)
		 mc_mul_1(_srcAH[0], reg0,reg1,reg2, __m_AH,_m_psubusw)
		 mc_crop_w(reg0, reg1, __m_w_64, 7,_dst[0]) 
		 __MC_OUT0();
		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();
}

static inline void filt8_h_b_fun()
{ 
	SACLE_SHIFT_INIT();
	int i;
	__m64 reg0, reg1, reg2,reg3;
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEV = (const __m64*)(src1 + 2);
		 const __m64* _srcDV = (const __m64*)(src1 + 1);
		 const __m64* _srcCV = (const __m64*)(src1);
		 const __m64* _srcBV = (const __m64*)(src1 - 1);
		 reg0 = _m_punpcklbw(_srcCV[0], __m_0);
		 reg1 = _m_punpckhbw(_srcCV[0], __m_0);
		 reg2 = _m_punpcklbw(_srcDV[0], __m_0);
		 reg3 = _m_punpckhbw(_srcDV[0], __m_0);
		 reg0 = _m_paddw(reg0, reg2);
		 reg1 = _m_paddw(reg1, reg3);
		 reg0 = _m_pmullw(reg0,__m_CV);
		 reg1 = _m_pmullw(reg1,__m_CV);
		 mc_mul_1(_srcEV[0], reg0,reg1,reg2, __m_EV,_m_psubusw)
		 mc_mul_1(_srcBV[0], reg0,reg1,reg2, __m_BV,_m_psubusw)

		 mc_crop_w(reg0, reg1, __m_w_4, 3,_dst[0]) 
		 __MC_OUT0();
		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();

}

static inline void filt8_v_h_fun()
{ 
	SACLE_SHIFT_INIT();
	int i;
	__m64 reg0, reg1, reg2,reg3;
	int srcStride2 = (srcStride << 1);
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEV = (const __m64*)(src1 + srcStride2);
		 const __m64* _srcDV = (const __m64*)(src1 + srcStride);
		 const __m64* _srcCV = (const __m64*)(src1);
		 const __m64* _srcBV = (const __m64*)(src1 - srcStride);
		 reg0 = _m_punpcklbw(_srcCV[0], __m_0);
		 reg1 = _m_punpckhbw(_srcCV[0], __m_0);
		 reg2 = _m_punpcklbw(_srcDV[0], __m_0);
		 reg3 = _m_punpckhbw(_srcDV[0], __m_0);
		 reg0 = _m_paddw(reg0, reg2);
		 reg1 = _m_paddw(reg1, reg3);
		 reg0 = _m_pmullw(reg0,__m_CV);
		 reg1 = _m_pmullw(reg1,__m_CV);
		 mc_mul_1(_srcEV[0], reg0,reg1,reg2, __m_EV,_m_psubusw)
		 mc_mul_1(_srcBV[0], reg0,reg1,reg2, __m_BV,_m_psubusw)

		 mc_crop_w(reg0, reg1, __m_w_4, 3,_dst[0]) 
		 __MC_OUT0();
		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();

}



static inline void filt8_v_dn_fun()
{
	SACLE_SHIFT_INIT();
	int i;
	__m64 reg0, reg1, reg2;
	int srcStride2 = (srcStride << 1);
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEH = (const __m64*)(src1 + srcStride2);
		 const __m64* _srcDH = (const __m64*)(src1 + srcStride);
		 const __m64* _srcCH = (const __m64*)(src1);
		 const __m64* _srcBH = (const __m64*)(src1 - srcStride);
		 const __m64* _srcAH = (const __m64*)(src1 - srcStride2);
		 reg0 = _m_punpcklbw(_srcCH[0], __m_0);
		 reg0 = _m_pmullw (reg0, __m_CH);
		 reg1 = _m_punpckhbw(_srcCH[0], __m_0);
		 reg1 = _m_pmullw (reg1, __m_CH);

		 mc_mul_r(_srcDH[0], reg0,reg1,reg2, __m_DH, _m_paddw)
		 mc_mul_r(_srcEH[0], reg0,reg1,reg2, __m_EH, _m_psubusw) 
		 mc_mul_2(_srcBH[0], reg0,reg1,reg2, __m_BH,_m_psubusw)
		 mc_mul_1(_srcAH[0], reg0,reg1,reg2, __m_AH,_m_psubusw)
		 mc_crop_w(reg0, reg1, __m_w_64, 7,_dst[0]) 
		 __MC_OUT0();
		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();
}



static inline void filt8_v_nd_fun()
{
	SACLE_SHIFT_INIT();
	 int i;
	__m64 reg0, reg1, reg2;
	int srcStride2 = (srcStride << 1);
	 for(i = 0; i < MC_H; i++)                        
	 {
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcEH = (const __m64*)(src1 - srcStride);
		 const __m64* _srcDH = (const __m64*)(src1);
		 const __m64* _srcCH = (const __m64*)(src1 + srcStride);
		 const __m64* _srcBH = (const __m64*)(src1 + srcStride2);
		 const __m64* _srcAH = (const __m64*)(src1 + srcStride2 + srcStride);
		 reg0 = _m_punpcklbw(_srcCH[0], __m_0);
		 reg0 = _m_pmullw (reg0, __m_CH);
		 reg1 = _m_punpckhbw(_srcCH[0], __m_0);
		 reg1 = _m_pmullw (reg1, __m_CH);

		 mc_mul_r(_srcDH[0], reg0,reg1,reg2, __m_DH, _m_paddw)
		 mc_mul_r(_srcEH[0], reg0,reg1,reg2, __m_EH, _m_psubusw) 
		 mc_mul_2(_srcBH[0], reg0,reg1,reg2, __m_BH,_m_psubusw)
		 mc_mul_1(_srcAH[0], reg0,reg1,reg2, __m_AH,_m_psubusw)
		 mc_crop_w(reg0, reg1, __m_w_64, 7,_dst[0]) 
		 __MC_OUT0();
		 src1+=srcStride;
		 dst+=dstStride;
	 }
	 _mm_empty();

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
#undef  __MC_OUT0
#undef  __MC_OUT1
#undef  __MC_OUT2


#ifdef __MC_PUT_FUN
static inline void xavs_put_filt16_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_fq_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_fq_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_qf_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_qf_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_dn_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_dn_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_nd_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_nd_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_egpr_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_egpr_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_j_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_j_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}



static inline void xavs_put_filt16_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_b_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_b_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_h_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_v_h_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ac_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ac_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ca_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_h_ca_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_put_filt16_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ki_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ki_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_put_filt16_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_put_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ik_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_put_filt8_hv_ik_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static void xavs_qpel16_put_mc10_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_ac_mmx(dst,src,src+stride+1,stride,stride);
}

static void xavs_qpel16_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_b_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc30_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_h_ca_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_dn_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc11_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc21_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_fq_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_put_mc31_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_h_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_put_mc12_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_ik_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc22_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_j_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_put_mc32_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_ki_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc13_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+stride,stride,stride);

}

static void xavs_qpel16_put_mc23_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_qf_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_put_mc33_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc10_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_ac_mmx(dst,src,src+stride+1,stride,stride);

}
/*
static void xavs_qpel8_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride);

}
*/
static void xavs_qpel8_put_mc30_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_ca_mmx(dst,src,src+stride+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_mmx(dst,src,src,stride,stride);
}*/

static void xavs_qpel8_put_mc11_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel8_put_mc21_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_fq_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel8_put_mc31_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_mmx(dst,src,src+1,stride,stride);
}
*/
static void xavs_qpel8_put_mc12_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_ik_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc22_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_j_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_put_mc32_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_ki_mmx(dst,src,src+1,stride,stride);

}

/*
static void xavs_qpel8_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}*/

static void xavs_qpel8_put_mc13_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+stride,stride,stride);

}

static void xavs_qpel8_put_mc23_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_qf_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc33_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride);

}


static void xavs_qpel8_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_mmx(dst,src,src,stride,stride);
}

static void xavs_qpel8_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_mmx(dst,src,src+1,stride,stride);
}


static void xavs_qpel8_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}
#endif

#ifdef __MC_AVG_FUN

static inline void xavs_avg_filt16_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_fq_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_fq_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_avg_filt16_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_qf_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_qf_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_dn_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_dn_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_nd_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_nd_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_egpr_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_egpr_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_j_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_j_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}



static inline void xavs_avg_filt16_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_b_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_b_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_h_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_v_h_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ac_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ac_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ca_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_h_ca_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static inline void xavs_avg_filt16_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ik_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ik_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}

static inline void xavs_avg_filt16_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride)
{
	xavs_avg_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ki_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride);
	xavs_avg_filt8_hv_ki_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride);
}


static void xavs_qpel16_avg_mc10_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_ac_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_b_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc30_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_h_ca_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_dn_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc11_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc21_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_fq_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel16_avg_mc31_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_avg_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_h_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_avg_mc12_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_ik_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc22_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_j_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel16_avg_mc32_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_ki_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel16_avg_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc13_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+stride,stride,stride);

}

static void xavs_qpel16_avg_mc23_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_qf_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel16_avg_mc33_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc10_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_ac_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc30_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_h_ca_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_dn_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc11_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc21_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_fq_mmx(dst,src,src,stride,stride);

}

static void xavs_qpel8_avg_mc31_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel8_avg_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_h_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_avg_mc12_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_ik_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc22_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_j_mmx(dst,src,src+1,stride,stride);
}

static void xavs_qpel8_avg_mc32_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_ki_mmx(dst,src,src+1,stride,stride);

}

static void xavs_qpel8_avg_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc13_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+stride,stride,stride);

}

static void xavs_qpel8_avg_mc23_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_qf_mmx(dst,src,src+stride+1,stride,stride);

}

static void xavs_qpel8_avg_mc33_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+stride+1,stride,stride);

}


#endif

#ifdef __MC_PUT_FUN_W
static inline void xavs_put_filt16_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_fq_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_fq_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_qf_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_qf_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_dn_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_dn_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_nd_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_nd_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_egpr_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_egpr_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_j_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_j_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}



static inline void xavs_put_filt16_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_b_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_b_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_h_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_v_h_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ac_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ac_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ca_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_h_ca_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_put_filt16_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ki_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ki_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_put_filt16_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_put_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ik_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_put_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_put_filt8_hv_ik_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static void xavs_qpel16_put_mc10_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ac_mmx(dst,src,src+stride+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_b_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc30_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_h_ca_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_dn_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc11_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc21_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_fq_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc31_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_h_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc12_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ik_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc22_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_j_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_put_mc32_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_ki_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_v_nd_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc13_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc23_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_qf_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_put_mc33_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt16_hv_egpr_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc10_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ac_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}
/*
static void xavs_qpel8_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride);

}
*/
static void xavs_qpel8_put_mc30_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_ca_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_dn_mmx(dst,src,src,stride,stride);
}*/

static void xavs_qpel8_put_mc11_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc21_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_fq_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc31_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_h_mmx(dst,src,src+1,stride,stride);
}
*/
static void xavs_qpel8_put_mc12_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ik_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc22_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_j_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_put_mc32_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_ki_mmx(dst,src,src+1,stride,stride,scale,shift);

}

/*
static void xavs_qpel8_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride)
{
	xavs_put_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride);

}*/

static void xavs_qpel8_put_mc13_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc23_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_qf_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_put_mc33_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_hv_egpr_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}




static void xavs_qpel8_put_mc20_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}


static void xavs_qpel8_put_mc01_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_dn_mmx(dst,src,src,stride,stride,scale,shift);
}

static void xavs_qpel8_put_mc02_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_h_mmx(dst,src,src+1,stride,stride,scale,shift);
}


static void xavs_qpel8_put_mc03_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_put_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

#endif

#ifdef __MC_AVG_FUN_W
static inline void xavs_avg_filt16_hv_fq_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_fq_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_fq_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_fq_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_avg_filt16_hv_qf_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_qf_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_qf_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_qf_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_dn_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_dn_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_dn_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_dn_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_nd_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_nd_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_nd_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_nd_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_egpr_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_egpr_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_egpr_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_egpr_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_j_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_j_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_j_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_j_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}



static inline void xavs_avg_filt16_h_b_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_b_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_b_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_b_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_v_h_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_h_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_v_h_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_v_h_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_h_ac_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ac_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ac_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ac_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_h_ca_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ca_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_h_ca_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_h_ca_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static inline void xavs_avg_filt16_hv_ik_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ik_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ik_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ik_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}

static inline void xavs_avg_filt16_hv_ki_mmx(uint8_t *dst, uint8_t *src1, uint8_t *src2, int dstStride, int srcStride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ki_mmx(dst +8 , src1+8,   src2+8  , dstStride, srcStride,scale,shift);
	src1 += 8*srcStride;
    src2 += 8*srcStride;
    dst += 8*dstStride;
	xavs_avg_filt8_hv_ki_mmx(dst  , src1,   src2  , dstStride, srcStride,scale,shift);
	xavs_avg_filt8_hv_ki_mmx(dst +8, src1+8,   src2+8  , dstStride, srcStride,scale,shift);
}


static void xavs_qpel16_avg_mc10_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ac_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc20_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_b_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc30_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_h_ca_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc01_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_dn_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc11_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc21_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_fq_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc31_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc02_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_h_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_avg_mc12_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ik_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc22_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_j_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel16_avg_mc32_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_ki_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc03_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_v_nd_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc13_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc23_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_qf_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel16_avg_mc33_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt16_hv_egpr_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc10_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ac_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc20_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_b_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc30_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_h_ca_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc01_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_dn_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc11_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc21_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_fq_mmx(dst,src,src,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc31_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc02_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_h_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_avg_mc12_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ik_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc22_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_j_mmx(dst,src,src+1,stride,stride,scale,shift);
}

static void xavs_qpel8_avg_mc32_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_ki_mmx(dst,src,src+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc03_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_v_nd_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc13_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+stride,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc23_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_qf_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}

static void xavs_qpel8_avg_mc33_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift)
{
	xavs_avg_filt8_hv_egpr_mmx(dst,src,src+stride+1,stride,stride,scale,shift);

}


#endif
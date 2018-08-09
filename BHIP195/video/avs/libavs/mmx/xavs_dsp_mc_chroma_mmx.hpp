
#ifdef __MC_CHROMA_FUN
#define chroma_mc4_put() \
	xavs_chroma_mc4_put_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)

#define chroma_mc8_put() \
	xavs_chroma_mc8_put_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)


#define chroma_mc4_avg() \
	xavs_chroma_mc4_avg_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)

#define chroma_mc8_avg() \
	xavs_chroma_mc8_avg_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)
#define SACLE_SHIFT_INIT() 
#define SCAL_SHIFT() 
#define SCAL_SHIFT1() _dst[0] = _m_packuswb(reg0,reg1);
#endif
#ifdef __MC_CHROMA_FUN_W

#define chroma_mc4_put() \
	xavs_chroma_mc4_put_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define chroma_mc8_put() \
	xavs_chroma_mc8_put_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)


#define chroma_mc4_avg() \
	xavs_chroma_mc4_avg_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define chroma_mc8_avg() \
	xavs_chroma_mc8_avg_mmx(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define SACLE_SHIFT_INIT() scale_shift_init()

#define SCAL_SHIFT() scale_shift_0(reg0,reg1,reg2,reg0); 
#define SCAL_SHIFT1() reg0 = _m_packuswb(reg0,reg1);scale_shift_0(reg0,reg1,reg2,_dst[0]); 
#endif
static void chroma_mc4_put()
{
	SACLE_SHIFT_INIT();
	__mc_chroma_abcd();
#ifdef __MC_CHROMA_FUN_W
	__m64 reg0, reg1,reg2;
#else
	__m64 reg0, reg1;
#endif
	for(int i = 0; i < h; i++)
	{
		reg0 = _m_from_int(*((uint32_t *)(src + 0)));
		reg0 = _m_punpcklbw(reg0, __m_0);
		reg0 = _m_pmullw(reg0, __m_C_A);
		__mc_chroma_mul_4(src + 1,__m_C_B);
		__mc_chroma_mul_4(src + stride + 0, __m_C_C);
		__mc_chroma_mul_4(src + stride + 1, __m_C_D);
		reg0 = _m_paddusw(reg0,__m_w_32);
		reg0 = _m_psrlwi(reg0,6);
		reg0 = _m_packuswb(reg0,reg0);

		SCAL_SHIFT();
		*((uint32_t *)(dst)) = _m_to_int(reg0);

		src += stride;
		dst += stride;
	}
	_mm_empty();
}

static void chroma_mc4_avg()
{
	SACLE_SHIFT_INIT();
	__mc_chroma_abcd();
#ifdef __MC_CHROMA_FUN_W
	__m64 reg0, reg1,reg2;
#else
	__m64 reg0, reg1;
#endif

	for(int i = 0; i < h; i++)
	{
		reg0 = _m_from_int(*((uint32_t *)(src + 0)));
		reg0 = _m_punpcklbw(reg0, __m_0);
		reg0 = _m_pmullw(reg0, __m_C_A);
		__mc_chroma_mul_4(src + 1,__m_C_B);
		__mc_chroma_mul_4(src + stride + 0, __m_C_C);
		__mc_chroma_mul_4(src + stride + 1, __m_C_D);
		reg0 = _m_paddusw(reg0,__m_w_32);
		reg0 = _m_psrlwi(reg0,6);
		reg0 = _m_packuswb(reg0,reg0);

		

		SCAL_SHIFT();
		reg1 = _m_from_int(*((uint32_t *)(dst)));
		reg0 = _m_pavgb(reg0,reg1);
		*((uint32_t *)(dst)) = _m_to_int(reg0);

		src += stride;
		dst += stride;
	}
	_mm_empty();
}

static void chroma_mc8_put()
{
	SACLE_SHIFT_INIT();
	__mc_chroma_abcd();
	__m64 reg0, reg1,reg2, reg3;
	for(int i = 0; i < h; i++)
	{
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcA = (const __m64*)(src + 0);
		 const __m64* _srcB = (const __m64*)(src + 1);
		 const __m64* _srcC = (const __m64*)(src + stride + 0);
		 const __m64* _srcD = (const __m64*)(src + stride + 1);
		
		reg0 = _m_punpcklbw(_srcA[0], __m_0);
		reg0 = _m_pmullw(reg0, __m_C_A);
		reg1 = _m_punpckhbw(_srcA[0], __m_0);
		reg1 = _m_pmullw(reg1, __m_C_A);

		__mc_chroma_mul_8(_srcB[0],__m_C_B);	
		__mc_chroma_mul_8(_srcC[0],__m_C_C);
		__mc_chroma_mul_8(_srcD[0],__m_C_D);

		reg0 = _m_paddusw(reg0,__m_w_32);
		reg0 = _m_psrlwi(reg0,6);
		reg1 = _m_paddusw(reg1,__m_w_32);
		reg1 = _m_psrlwi(reg1,6);

		SCAL_SHIFT1();
		
		
		

		src += stride;
		dst += stride;
	}
	_mm_empty();
}

static void chroma_mc8_avg()
{
	SACLE_SHIFT_INIT();
	__mc_chroma_abcd();
	__m64 reg0, reg1,reg2, reg3;
	for(int i = 0; i < h; i++)
	{
		 __m64* _dst = (__m64*)dst;
		 const __m64* _srcA = (const __m64*)(src + 0);
		 const __m64* _srcB = (const __m64*)(src + 1);
		 const __m64* _srcC = (const __m64*)(src + stride + 0);
		 const __m64* _srcD = (const __m64*)(src + stride + 1);
		
		reg0 = _m_punpcklbw(_srcA[0], __m_0);
		reg0 = _m_pmullw(reg0, __m_C_A);
		reg1 = _m_punpckhbw(_srcA[0], __m_0);
		reg1 = _m_pmullw(reg1, __m_C_A);

		__mc_chroma_mul_8(_srcB[0],__m_C_B);	
		__mc_chroma_mul_8(_srcC[0],__m_C_C);
		__mc_chroma_mul_8(_srcD[0],__m_C_D);

		reg0 = _m_paddusw(reg0,__m_w_32);
		reg0 = _m_psrlwi(reg0,6);
		reg1 = _m_paddusw(reg1,__m_w_32);
		reg1 = _m_psrlwi(reg1,6);

		
		reg0 = _m_packuswb(reg0,reg1);

		SCAL_SHIFT();
		_dst[0]  = _m_pavgb(reg0, _dst[0]);
		
		

		src += stride;
		dst += stride;
	}
	_mm_empty();
}


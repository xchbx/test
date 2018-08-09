
#ifdef __MC_CHROMA_FUN
#define chroma_mc4_put_sse2() \
	xavs_chroma_mc4_put_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)

#define chroma_mc8_put_sse2() \
	xavs_chroma_mc8_put_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)


#define chroma_mc4_avg_sse2() \
	xavs_chroma_mc4_avg_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)

#define chroma_mc8_avg_sse2() \
	xavs_chroma_mc8_avg_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y)
#define SACLE_SHIFT_INIT_SSE2() 
#define SCAL_SHIFT_SSE2() 
#define SCAL_SHIFT1_SSE2() reg0 = _mm_packus_epi16(reg0,reg1); _mm_storel_epi64((__m128i *)dst, reg0);
#endif
#ifdef __MC_CHROMA_FUN_W

#define chroma_mc4_put_sse2() \
	xavs_chroma_mc4_put_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define chroma_mc8_put_sse2() \
	xavs_chroma_mc8_put_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)


#define chroma_mc4_avg_sse2() \
	xavs_chroma_mc4_avg_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define chroma_mc8_avg_sse2() \
	xavs_chroma_mc8_avg_sse2(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y, int scale, int shift)

#define SACLE_SHIFT_INIT_SSE2() scale_shift_init_sse2()

#define SCAL_SHIFT_SSE2() scale_shift_0_8_sse2(reg0,reg1,reg0); 
#define SCAL_SHIFT1_SSE2() reg0 = _mm_packus_epi16(reg0,reg1); scale_shift_0_8_sse2(reg0,reg1, reg0); 
#endif
#ifdef __CONFIG_MMX
static void chroma_mc4_put_sse2()
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

static void chroma_mc4_avg_sse2()
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
#endif
static void chroma_mc8_put_sse2()
{
	SACLE_SHIFT_INIT_SSE2();
	__mc_chroma_abcd_sse2();
	__m128i reg0, reg1;
	
	for(int i = 0; i < h; i++)
	{
		__m128i _srcA = _mm_loadl_epi64((__m128i*)(src + 0));
		__m128i _srcB = _mm_loadl_epi64((__m128i*)(src + 1));
		src += stride;
		__m128i _srcC = _mm_loadl_epi64((__m128i*)(src + 0));
		__m128i _srcD = _mm_loadl_epi64((__m128i*)(src + 1));

		reg0 = _mm_unpacklo_epi8(_srcA, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, __m_C_A);

		__mc_chroma_mul_8_sse2(_srcB,__m_C_B);
		__mc_chroma_mul_8_sse2(_srcC,__m_C_C);
		__mc_chroma_mul_8_sse2(_srcD,__m_C_D);

		reg0 = _mm_add_epi16(reg0,_mm_set1_epi16(32));
		reg0 = _mm_srli_epi16(reg0, 6);

		SCAL_SHIFT1_SSE2();
		 _mm_storel_epi64((__m128i *)dst, reg0);
		dst += stride;
	}
}

static void chroma_mc8_avg_sse2()
{

	SACLE_SHIFT_INIT_SSE2();
	__mc_chroma_abcd_sse2();
	__m128i reg0, reg1;
	
	for(int i = 0; i < h; i++)
	{
		__m128i _srcA = _mm_loadl_epi64((__m128i*)(src + 0));
		__m128i _srcB = _mm_loadl_epi64((__m128i*)(src + 1));
		src += stride;
		__m128i _srcC = _mm_loadl_epi64((__m128i*)(src + 0));
		__m128i _srcD = _mm_loadl_epi64((__m128i*)(src + 1));

		reg0 = _mm_unpacklo_epi8(_srcA, _mm_setzero_si128());
		reg0 = _mm_mullo_epi16(reg0, __m_C_A);

		__mc_chroma_mul_8_sse2(_srcB,__m_C_B);
		__mc_chroma_mul_8_sse2(_srcC,__m_C_C);
		__mc_chroma_mul_8_sse2(_srcD,__m_C_D);

		reg0 = _mm_add_epi16(reg0,_mm_set1_epi16(32));
		reg0 = _mm_srli_epi16(reg0, 6);

		reg0 = _mm_packus_epi16(reg0, reg0);

		SCAL_SHIFT_SSE2();

		reg1 = _mm_loadl_epi64((__m128i*)(dst + 0));
		reg0  = _mm_avg_epu8(reg0, reg1);
		_mm_storel_epi64((__m128i*)dst, reg0);
		dst += stride;
	}
	
}


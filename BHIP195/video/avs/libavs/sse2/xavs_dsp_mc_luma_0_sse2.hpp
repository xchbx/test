#ifdef __MC0_FUN_W
static inline void avg_pixels16_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{
	__m128i reg0, reg1,reg2;
	scale_shift_init_sse2();
	for(int i = 0; i < h; i++)
	{
		reg0 =  _mm_load_si128((__m128i*)src);
		scale_shift_0_16_sse2(reg0, reg1, reg2, reg0);
		reg1 =  _mm_load_si128((__m128i*)dst);
		reg0 = _mm_avg_epu8(reg0,reg1);
		_mm_store_si128((__m128i*)dst, reg0);
		dst += line_size;
		src += line_size;

	}
}


static inline void avg_pixels8_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{
	/*const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	__m64 reg0, reg1, reg2;
	scale_shift_init();
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		scale_shift_0(_src[0],reg0, reg1,reg2);
		_dst[0] = _m_pavgb(_dst[0], reg2);
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();*/

	__m128i reg0, reg1,reg2;
	scale_shift_init_sse2();
	for(int i = 0; i < h; i++)
	{
		reg0 =  _mm_loadl_epi64((__m128i*)src);
		scale_shift_0_16_sse2(reg0, reg1, reg2, reg0);
		reg1 =  _mm_loadl_epi64((__m128i*)dst);
		reg0 = _mm_avg_epu8(reg0,reg1);
		_mm_storel_epi64((__m128i*)dst, reg0);
		dst += line_size;
		src += line_size;

	}

}
static inline void put_pixels8_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{

	/*const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	__m64 reg0, reg1;
	scale_shift_init();
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		scale_shift_0(_src[0],reg0, reg1,_dst[0]);
		_dst += line_size;
		_src += line_size;
	}
	_mm_empty();*/

	__m128i reg0, reg1,reg2;
	scale_shift_init_sse2();
	for(int i = 0; i < h; i++)
	{
		reg0 =  _mm_loadl_epi64((__m128i*)src);
		scale_shift_0_16_sse2(reg0, reg1, reg2, reg0);
		_mm_storel_epi64((__m128i*)dst, reg0);
		dst += line_size;
		src += line_size;

	}

}


static inline void put_pixels16_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{


	__m128i reg0, reg1,reg2;
	scale_shift_init_sse2();
	for(int i = 0; i < h; i++)
	{
		reg0 =  _mm_load_si128((__m128i*)src);
		scale_shift_0_16_sse2(reg0, reg1, reg2, reg0);
		_mm_store_si128((__m128i*)dst, reg0);
		dst += line_size;
		src += line_size;

	}


}

static void xavs_qpel8_put_mc00_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
	put_pixels8_sse2(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_put_mc00_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    put_pixels16_sse2(dst, src, stride, 16, scale, shift);
}


static void xavs_qpel8_avg_mc00_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels8_sse2(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_avg_mc00_sse2(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels16_sse2(dst, src, stride, 16, scale, shift);
}
#endif
#ifdef __MC0_FUN
static inline void avg_pixels16_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	__m128i reg0, reg1;
	for(int i = 0; i < h; i++)
	{
		reg0 = _mm_loadu_si128((__m128i*)src);
		reg1 = _mm_loadu_si128((__m128i*)dst);
		reg0 = _mm_avg_epu8(reg0,reg1);
		_mm_storeu_si128((__m128i*)dst, reg0);
		dst += line_size; src += line_size;
	}

}


static inline void avg_pixels8_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	/*const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _m_pavgb(_dst[0], _src[0]);
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();*/

	__m128i reg0, reg1;
	for(int i = 0; i < h; i++)
	{
		reg0 = _mm_loadl_epi64((__m128i*)src);
		reg1 = _mm_loadl_epi64((__m128i*)dst);
		reg0 = _mm_avg_epu8(reg0,reg1);
		_mm_storel_epi64((__m128i*)dst, reg0);
		dst += line_size; src += line_size;
	}

}
static inline void put_pixels8_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	/*const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _src[0];
		_dst += line_size;
		_src += line_size;
	}
	_mm_empty();*/
	for(int i = 0; i < h; i++)
	{
		_mm_storel_epi64((__m128i*)dst, _mm_loadl_epi64((__m128i*)src));
		dst += line_size; src += line_size;
	}


}


static inline void put_pixels16_sse2(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	for(int i = 0; i < h; i++)
	{
		_mm_storeu_si128((__m128i*)dst, _mm_loadu_si128((__m128i*)src));
		dst += line_size; src += line_size;
	}
}

static void xavs_qpel8_put_mc00_sse2(uint8_t *dst, uint8_t *src, int stride) 
{
	put_pixels8_sse2(dst, src, stride, 8);
}

static void xavs_qpel16_put_mc00_sse2(uint8_t *dst, uint8_t *src, int stride) 
{
    put_pixels16_sse2(dst, src, stride, 16);
}


static void xavs_qpel8_avg_mc00_sse2(uint8_t *dst, uint8_t *src, int stride) 
{
    avg_pixels8_sse2(dst, src, stride, 8);
}

static void xavs_qpel16_avg_mc00_sse2(uint8_t *dst, uint8_t *src, int stride) 
{
    avg_pixels16_sse2(dst, src, stride, 16);
}

#endif


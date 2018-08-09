#ifdef __MC0_FUN_W
static inline void avg_pixels16_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	__m64 reg0, reg1, reg2;
	scale_shift_init();
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		scale_shift_0(_src[0],reg0, reg1,reg2);
		_dst[0] = _m_pavgb(_dst[0], reg2);
		scale_shift_0(_src[1],reg0, reg1,reg2);
		_dst[1] = _m_pavgb(_dst[1], reg2);
		_dst += line_size;
		_src += line_size;
	}
	_mm_empty();
}


static inline void avg_pixels8_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{
	const __m64* _src = (const __m64*)src;
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
	_mm_empty();

}
static inline void put_pixels8_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{

	const __m64* _src = (const __m64*)src;
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
	_mm_empty();

}


static inline void put_pixels16_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h, int scale, int shift)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	__m64 reg0, reg1;
	scale_shift_init();
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		scale_shift_0(_src[0],reg0, reg1,_dst[0]);
		scale_shift_0(_src[1],reg0, reg1,_dst[1]);
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();
}

static void xavs_qpel8_put_mc00_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
	put_pixels8_mmx(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_put_mc00_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    put_pixels16_mmx(dst, src, stride, 16, scale, shift);
}


static void xavs_qpel8_avg_mc00_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels8_mmx(dst, src, stride, 8, scale, shift);
}

static void xavs_qpel16_avg_mc00_mmx(uint8_t *dst, uint8_t *src, int stride, int scale, int shift) 
{
    avg_pixels16_mmx(dst, src, stride, 16, scale, shift);
}


#endif
#ifdef __MC0_FUN
static inline void avg_pixels16_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _m_pavgb(_dst[0], _src[0]);
		_dst[1] = _m_pavgb(_dst[1], _src[1]);
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();
}


static inline void avg_pixels8_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _m_pavgb(_dst[0], _src[0]);
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();

}
static inline void put_pixels8_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _src[0];
		_dst += line_size;
		_src += line_size;
	}
	_mm_empty();
}


static inline void put_pixels16_mmx(uint8_t *dst, const uint8_t *src, int line_size, int h)
{
	const __m64* _src = (const __m64*)src;
	__m64* _dst = (__m64*)dst;
	line_size = line_size >> 3;
	for(int i = 0; i < h; i++)
	{
		_dst[0] = _src[0];
		_dst[1] = _src[1];
		_dst += line_size;
		_src += line_size;

	}
	_mm_empty();
}

static void xavs_qpel8_put_mc00_mmx(uint8_t *dst, uint8_t *src, int stride) 
{
	put_pixels8_mmx(dst, src, stride, 8);
}

static void xavs_qpel16_put_mc00_mmx(uint8_t *dst, uint8_t *src, int stride) 
{
    put_pixels16_mmx(dst, src, stride, 16);
}


static void xavs_qpel8_avg_mc00_mmx(uint8_t *dst, uint8_t *src, int stride) 
{
    avg_pixels8_mmx(dst, src, stride, 8);
}

static void xavs_qpel16_avg_mc00_mmx(uint8_t *dst, uint8_t *src, int stride) 
{
    avg_pixels16_mmx(dst, src, stride, 16);
}

#endif
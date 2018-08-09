


static inline void xavs_idct8_1d_sse2(uint8_t *src, uint8_t*dst, short bias, short scale)
{
	__m128i reg0, reg1,reg2, reg3,reg4, reg5,reg6, reg7, tmp;
	__m128i bias128 = _mm_set1_epi16(bias);

	reg0 = reg4 = _mm_load_si128((__m128i*)(src + 112));
	reg3 = reg5 = _mm_load_si128((__m128i*)(src + 16));
	reg6 = reg2 = _mm_load_si128((__m128i*)(src + 80));
	reg1 = reg7 = _mm_load_si128((__m128i*)(src + 48));

	reg4 = _mm_slli_epi16(reg4, 1);
	reg3 = _mm_slli_epi16(reg3, 1);
	reg6 = _mm_slli_epi16(reg6, 1);
	reg1 = _mm_slli_epi16(reg1, 1);

	reg0 = _mm_add_epi16(reg0, reg4); //reg0 = 3*src7 
	reg5 = _mm_add_epi16(reg5, reg3); //reg5 = 3*src1 
	reg2 = _mm_add_epi16(reg2, reg6); //reg2 = 3*src5 
	reg7 = _mm_add_epi16(reg7, reg1); //reg7 = 3*src3 

	reg5 = _mm_sub_epi16(reg5, reg4); // reg5 = 3*src1 - 2*src7 = a0 
	reg7 = _mm_add_epi16(reg7, reg6); // reg7 = 3*src3 + 2*src5 = a1 
	reg1 = _mm_sub_epi16(reg1, reg2); // reg1 = 2*src3 - 3*src5 = a2 
	reg3 = _mm_add_epi16(reg3, reg0); // reg3 = 2*src1 + 3*src7 = a3 

	reg4 = reg5; // mm4 = a0 
	reg6 = reg7; // mm6 = a1 
	reg2 = reg1; // mm2 = a3 
	reg0 = reg3; // mm0 = a2 

	__sum_sub_ba_sse2(reg7, reg5);
	reg7 = _mm_add_epi16(reg7, reg3); // reg7 = a0 + a1 + a3 
	reg5 = _mm_add_epi16(reg5, reg1); // reg5 = a0 - a1 + a2 
	reg7 = _mm_add_epi16(reg7, reg7);
	reg5 = _mm_add_epi16(reg5, reg5);
	reg7 = _mm_add_epi16(reg7, reg6); // reg7 = b4 
	reg5 = _mm_add_epi16(reg5, reg4); // reg5 = b5

	__sum_sub_ba_sse2( reg1, reg3 )   // reg1 = a3 + a2  reg3 = a3 - a2 
	reg4 = _mm_sub_epi16(reg4, reg1); //reg4 = a0 - a2 - a3 
	reg1 = reg4; // reg1 = a0 - a2 - a3 
	reg3 = _mm_sub_epi16(reg3, reg6); // reg3 = a3 - a2 - a1 
	reg1 = _mm_add_epi16(reg1, reg1);
	reg3 = _mm_add_epi16(reg3, reg3);
	reg1 = _mm_sub_epi16(reg1, reg2); // reg1 = b7 
	reg3 = _mm_add_epi16(reg3, reg0); // reg3 = b6 


	reg4 = reg2 =  _mm_load_si128((__m128i*)(src + 32)); // reg2 = src2 
	reg0 = reg6 =  _mm_load_si128((__m128i*)(src + 96)); // reg6 = src6 

	reg4 = _mm_slli_epi16(reg4, 2); // reg4 = 4*src2 
	reg6 = _mm_slli_epi16(reg6, 2); // reg6 = 4*src6 

	reg2 = _mm_add_epi16(reg2, reg4); // reg2 = 5*src2
	reg0 = _mm_add_epi16(reg0, reg6); // reg0 = 5*src6
	reg2 = _mm_add_epi16(reg2, reg2);
	reg0 = _mm_add_epi16(reg0, reg0);
	reg4 = _mm_sub_epi16(reg4, reg0); // reg4 = 4*src2 - 10*src6 = a7
	reg6 = _mm_add_epi16(reg6, reg2); // reg6 = 4*src6 + 10*src2 = a6

	reg2 = _mm_load_si128((__m128i*)(src + 0));  // reg2 = src0 
	reg0 = _mm_load_si128((__m128i*)(src + 64)); // reg0 = src4 
	__sum_sub_ba_sse2(reg0, reg2 )     // reg0 = src0+src4  reg2 = src0-src4
	reg0 = _mm_slli_epi16(reg0, 3);
	reg2 = _mm_slli_epi16(reg2, 3);
	reg0 = _mm_add_epi16(reg0, bias128);
	reg2 = _mm_add_epi16(reg2, bias128);

	__sum_sub_ba_sse2( reg6, reg0 )   // reg6 = a4 + a6  reg0 = a4 - a6 
	__sum_sub_ba_sse2( reg4, reg2 )   // reg4 = a5 + a7  reg2 = a5 - a7 
	__sum_sub_ba_sse2( reg7, reg6 )   // reg7 = dst0  reg6 = dst7 
	__sum_sub_ba_sse2( reg5, reg4 )   // reg5 = dst1  reg4 = dst6 
	__sum_sub_ba_sse2( reg3, reg2 )   // reg3 = dst2  reg2 = dst5 
	__sum_sub_ba_sse2( reg1, reg0 )   // reg1 = dst3  reg0 = dst4 

	reg0 = _mm_srai_epi16(reg0, scale);
	reg1 = _mm_srai_epi16(reg1, scale);
	reg2 = _mm_srai_epi16(reg2, scale);
	reg3 = _mm_srai_epi16(reg3, scale);
	reg4 = _mm_srai_epi16(reg4, scale);
	reg5 = _mm_srai_epi16(reg5, scale);
	reg6 = _mm_srai_epi16(reg6, scale);
	tmp = _mm_srai_epi16(reg7, scale);
	//////////////////r0     r1    r2    r3    r4   r5    r6   r7    tmp
	__transpose8_sse2( tmp, reg5, reg3, reg1,reg0, reg2, reg4, reg6,reg7);

	_mm_store_si128((__m128i*)(dst + 0)  ,tmp);
	_mm_store_si128((__m128i*)(dst + 16) ,reg1);
	_mm_store_si128((__m128i*)(dst + 32) ,reg6);
	_mm_store_si128((__m128i*)(dst + 48) ,reg0);

	_mm_store_si128((__m128i*)(dst + 64) ,reg7);
	_mm_store_si128((__m128i*)(dst + 80) ,reg4);
	_mm_store_si128((__m128i*)(dst + 96) ,reg3);
	_mm_store_si128((__m128i*)(dst + 112),reg2);
	
}

static inline void  xavs_add_pixels_clamped_zero_sse2(const int16_t *block, uint8_t *pixels, int line_size)
{
	__m128i reg0, reg1;
	for(int i = 0; i < 8; i++)
	{
		reg0 = _mm_loadl_epi64((__m128i *)pixels);
		reg1 = _mm_load_si128((__m128i *)block);
		reg0 = _mm_unpacklo_epi8(reg0, _mm_setzero_si128());
		reg0 = _mm_add_epi16(reg0, reg1);
		reg0 = _mm_packus_epi16(reg0, reg1);
		_mm_storel_epi64((__m128i *)pixels, reg0);
		_mm_store_si128((__m128i *)block,_mm_setzero_si128());
		block += 8;
		pixels += line_size;
	}
	
	
}
static void xavs_idct8_add_sse2(uint8_t *dst, DCTELEM *src, int stride) 
{
	DECLARE_ALIGNED(32, DCTELEM, block)[64]; 
	xavs_idct8_1d_sse2((uint8_t *)src, (uint8_t *)block, 4, 3);
	xavs_idct8_1d_sse2((uint8_t *)block, (uint8_t *)src, 64, 7);
	xavs_add_pixels_clamped_zero_sse2(src, dst, stride);
	
}




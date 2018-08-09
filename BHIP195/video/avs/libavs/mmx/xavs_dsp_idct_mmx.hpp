

static inline void xavs_idct8_1d_mmx(uint8_t *src, uint8_t*dst, short bias, short scale)
{
	__m64 reg0, reg1,reg2, reg3,reg4, reg5,reg6, reg7, tmp;
	__m64 bias64 = _mm_set1_pi16(bias);
	for(int i = 0; i < 2; i++)
	{
		const __m64* _src = (const __m64*)(src);
		__m64* _dst = (__m64*)(dst);
		
		reg0 = reg4 = _src[14]; //reg3 = src7
		reg3 = reg5 = _src[2];  //reg5 = src1
		reg6 = reg2 = _src[10];  //reg2 = src5
		reg1 = reg7 = _src[6]; //reg7 = src3

		reg4 = _m_psllwi(reg4, 1);
		reg3 = _m_psllwi(reg3, 1);
		reg6 = _m_psllwi(reg6, 1);
		reg1 = _m_psllwi(reg1, 1);

		reg0 = _m_paddw(reg0, reg4); //reg0 = 3*src7 
		reg5 = _m_paddw(reg5, reg3); //reg5 = 3*src1 
		reg2 = _m_paddw(reg2, reg6); //reg2 = 3*src5 
		reg7 = _m_paddw(reg7, reg1); //reg7 = 3*src3 


		reg5 = _m_psubw(reg5, reg4); // reg5 = 3*src1 - 2*src7 = a0 
		reg7 = _m_paddw(reg7, reg6); // reg7 = 3*src3 + 2*src5 = a1 
		reg1 = _m_psubw(reg1, reg2); // reg1 = 2*src3 - 3*src5 = a2 
		reg3 = _m_paddw(reg3, reg0); // reg3 = 2*src1 + 3*src7 = a3 

		reg4 = reg5; // mm4 = a0 
		reg6 = reg7; // mm6 = a1 
		reg2 = reg1; // mm2 = a3 
		reg0 = reg3; // mm0 = a2 
		

		__sum_sub_ba(reg7, reg5);
		reg7 = _m_paddw(reg7, reg3); // reg7 = a0 + a1 + a3 
		reg5 = _m_paddw(reg5, reg1); // reg5 = a0 - a1 + a2 
		reg7 = _m_paddw(reg7, reg7);
		reg5 = _m_paddw(reg5, reg5);
		reg7 = _m_paddw(reg7, reg6); // reg7 = b4 
		reg5 = _m_paddw(reg5, reg4); // reg5 = b5

		__sum_sub_ba( reg1, reg3 )   // reg1 = a3 + a2  reg3 = a3 - a2 
		reg4 = _m_psubw(reg4, reg1); //reg4 = a0 - a2 - a3 
		reg1 = reg4; // reg1 = a0 - a2 - a3 
		reg3 = _m_psubw(reg3, reg6); // reg3 = a3 - a2 - a1 
		reg1 = _m_paddw(reg1, reg1);
		reg3 = _m_paddw(reg3, reg3);
		reg1 = _m_psubw(reg1, reg2); // reg1 = b7 
		reg3 = _m_paddw(reg3, reg0); // reg3 = b6 

		reg4 = reg2 = _src[4]; // reg2 = src2 
		reg0 = reg6 = _src[12]; // reg6 = src6 
		
		reg4 = _m_psllwi(reg4, 2); // reg4 = 4*src2 
		reg6 = _m_psllwi(reg6, 2); // reg6 = 4*src6 

		reg2 = _m_paddw(reg2, reg4); // reg2 = 5*src2
		reg0 = _m_paddw(reg0, reg6); // reg0 = 5*src6
		reg2 = _m_paddw(reg2, reg2);
		reg0 = _m_paddw(reg0, reg0);
		reg4 = _m_psubw(reg4, reg0); // reg4 = 4*src2 - 10*src6 = a7
		reg6 = _m_paddw(reg6, reg2); // reg6 = 4*src6 + 10*src2 = a6

		reg2 =  _src[0];  // reg2 = src0 
		reg0 =  _src[8]; // reg0 = src4 
		__sum_sub_ba(reg0, reg2 )     // reg0 = src0+src4  reg2 = src0-src4
		reg0 = _m_psllwi(reg0, 3);
		reg2 = _m_psllwi(reg2, 3);
		reg0 = _m_paddw(reg0, bias64);
		reg2 = _m_paddw(reg2, bias64);

		__sum_sub_ba( reg6, reg0 )   // reg6 = a4 + a6  reg0 = a4 - a6 
		__sum_sub_ba( reg4, reg2 )   // reg4 = a5 + a7  reg2 = a5 - a7 
		__sum_sub_ba( reg7, reg6 )   // reg7 = dst0  reg6 = dst7 
		__sum_sub_ba( reg5, reg4 )   // reg5 = dst1  reg4 = dst6 
		__sum_sub_ba( reg3, reg2 )   // reg3 = dst2  reg2 = dst5 
		__sum_sub_ba( reg1, reg0 )   // reg1 = dst3  reg0 = dst4 

		reg0 = _m_psrawi(reg0, scale);
		reg1 = _m_psrawi(reg1, scale);
		reg2 = _m_psrawi(reg2, scale);
		reg3 = _m_psrawi(reg3, scale);
		reg4 = _m_psrawi(reg4, scale);
		reg5 = _m_psrawi(reg5, scale);
		reg6 = _m_psrawi(reg6, scale);
		tmp = _m_psrawi(reg7, scale);

		//tmp = reg7;
		__transpose4(reg0, reg2, reg4, reg6, reg7 );
		_dst[1] = reg0; 
		_dst[3] = reg6;
		_dst[5] = reg7;
		_dst[7] = reg4;

		reg7 = tmp; 
		__transpose4( reg7, reg5, reg3, reg1, reg0 )
		_dst[0] = reg7; 
		_dst[2] = reg1;
		_dst[4] = reg0;
		_dst[6] = reg3;

		src += 8;
		dst += 64;
		
	}
	_mm_empty();
}

static inline void  xavs_add_pixels_clamped_zero_mmx(const int16_t *block, uint8_t *pixels, int line_size)
{
	__m64 reg0, reg1;
	for(int i = 0; i < 8; i++)
	{
		reg0 = _m_punpcklbw(*(__m64 *)(pixels),__m_0);
		reg1 = _m_punpckhbw(*(__m64 *)(pixels),__m_0);
		reg0 = _m_paddw(reg0,*(__m64 *)(block +  0));
		reg1 = _m_paddw(reg1,*(__m64 *)(block +  4));
		*(__m64 *)(pixels) = _m_packuswb(reg0, reg1);
		*(__m64 *)(block +  0) = __m_0;
		*(__m64 *)(block +  4) = __m_0;
		block += 8;
		pixels += line_size;
	}
	_mm_empty();
	
}

static void xavs_idct8_add_mmx(uint8_t *dst, DCTELEM *src, int stride) 
{
	DECLARE_ALIGNED(8, DCTELEM, block)[64]; 
	
	xavs_idct8_1d_mmx((uint8_t *)src, (uint8_t *)block, 4, 3);
	xavs_idct8_1d_mmx((uint8_t *)block, (uint8_t *)src, 64, 7);
	//xavs_idct8_1d_mmx((uint8_t *)src, (uint8_t *)block, 0x0004000400040004ULL, 3ULL);
	//xavs_idct8_1d_mmx((uint8_t *)block, (uint8_t *)src, 0x0040004000400040ULL, 7ULL);
	xavs_add_pixels_clamped_zero_mmx(src, dst, stride);		
}

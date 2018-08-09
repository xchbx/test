
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/


#define LOWPASS(ARRAY,INDEX)                                            \
    (( ARRAY[(INDEX)-1] + 2*ARRAY[(INDEX)] + ARRAY[(INDEX)+1] + 2) >> 2)

#define LOWPASS_MMX(val, r0,r1, r2,r3) \
{ \
		const __m64* t0 = (const __m64*)(val + 0); \
		const __m64* t1 = (const __m64*)(val + 1); \
		const __m64* t2 = (const __m64*)(val + 2); \
		r0 = _m_punpcklbw(t0[0], __m_0); \
		r1 = _m_punpckhbw(t0[0], __m_0); \
		r2 = _m_punpcklbw(t1[0], __m_0); \
		r3 = _m_punpckhbw(t1[0], __m_0); \
		r2 = _m_psllwi(r2, 1); \
		r3 = _m_psllwi(r3, 1); \
		r0 = _m_paddw(r0, r2); \
		r1 = _m_paddw(r1, r3); \
		r2 = _m_punpcklbw(t2[0], __m_0); \
		r3 = _m_punpckhbw(t2[0], __m_0); \
		r0 = _m_paddw(r0, r2); \
		r1 = _m_paddw(r1, r3); \
		r0 = _m_paddw(r0, __m_w_2); \
		r1 = _m_paddw(r1, __m_w_2); \
		r0 = _m_psrlwi(r0, 2);	\
		r1 = _m_psrlwi(r1, 2);	\
}


#define LOWPASS_SSE2(val, r0, r1) \
{ \
		__m128i t0 = _mm_loadl_epi64((__m128i*)(val + 0)); \
		__m128i t1 = _mm_loadl_epi64((__m128i*)(val + 1)); \
		__m128i t2 = _mm_loadl_epi64((__m128i*)(val + 2)); \
		r0 = _mm_unpacklo_epi8(t0, _mm_setzero_si128()); \
		r1 = _mm_unpacklo_epi8(t1, _mm_setzero_si128()); \
		r0 = _mm_add_epi16(r0, r1); \
		r0 = _mm_add_epi16(r0, r1); \
		r1 = _mm_unpacklo_epi8(t2, _mm_setzero_si128()); \
		r0 = _mm_add_epi16(r0, r1); \
		r0 = _mm_add_epi16(r0, _mm_set1_epi16(2)); \
		r0 = _mm_srli_epi16(r0, 2);	\
}

/****************************************************************************
 * * xavs_intra_pred_dc_128:帧内预测模式之直流,顶块和左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_128_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
 {
    int y;
    uint64_t i_value = 0x8080808080808080;
    for(y=0;y<8;y++)
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp:帧内预测模式之直流,顶块和左块都能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m128i reg0, reg1,  reg2;
	LOWPASS_SSE2(p_top, reg0, reg1);
	LOWPASS_SSE2(p_left, reg1, reg2);

	DECLARE_ALIGNED(16, uint16_t, temp)[8]; 
	_mm_store_si128((__m128i*)temp  ,reg1);

	for(int i = 0; i < 8; i++)
	{
		//reg2 = _mm_set1_epi16(reg1.m128i_u16[i]);
		reg2 = _mm_set1_epi16(temp[i]);
		reg2 = _mm_add_epi16(reg0, reg2);
		reg2 = _mm_srli_epi16(reg2, 1);
		reg2 = _mm_packus_epi16(reg2, reg2);
		_mm_storel_epi64((__m128i*)p_dest, reg2);
		p_dest += i_stride;
	}
}


/****************************************************************************
 * * xavs_intra_pred_dc_lp_left:帧内预测模式之直流,顶块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_left_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m128i reg0, reg1,  reg2;
	LOWPASS_SSE2(p_left, reg0, reg1);
	reg0 = _mm_packus_epi16(reg0, reg0);

	DECLARE_ALIGNED(16, uint8_t, temp)[16]; 
	_mm_store_si128((__m128i*)temp  ,reg0);
	for(int i = 0; i < 8; i++)
	{
		//reg2 = _mm_set1_epi8(reg0.m128i_u8[i]);
		reg2 = _mm_set1_epi8(temp[i]);
		_mm_storel_epi64((__m128i*)p_dest, reg2);
		p_dest += i_stride;
	}
	
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp_top:帧内预测模式之直流,左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_top_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m128i reg0, reg1;
	LOWPASS_SSE2(p_top, reg0, reg1);
	reg0 = _mm_packus_epi16(reg0, reg0);
	for(int i = 0; i < 8; i++)
	{
		_mm_storel_epi64((__m128i*)p_dest, reg0);
		p_dest += i_stride;
	}
	
}
/****************************************************************************
 * * xavs_intra_pred_horizontal:帧内预测模式之水平
 ****************************************************************************/
static void xavs_intra_pred_horizontal_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int y;
    uint64_t i_value;
    for(y=0;y<8;y++) 
	{
		//0x0101010101010101ULL为什么编译不通过64位常量后面应该跟什么呢？
        i_value = p_left[y+1] * 0x0101010101010101;
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
    }
}
/****************************************************************************
 * * xavs_intra_pred_vertical:帧内预测模式之垂直
 ****************************************************************************/
static void xavs_intra_pred_vertical_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int y;
    uint64_t i_value = unaligned64(&p_top[1]);
    for(y=0;y<8;y++) 
	{
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
    }
}
/****************************************************************************
 * * xavs_intra_pred_down_left:帧内预测模式之左下
 ****************************************************************************/
static void xavs_intra_pred_down_left_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	/*int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            p_dest[y*i_stride+x] = (LOWPASS(p_top,x+y+2) + LOWPASS(p_left,x+y+2)) >> 1;*/
	__m128i reg0, reg1,  reg2;
	p_left++; p_top++;
	for(int i = 0; i < 8; i++)
	{
		LOWPASS_SSE2(p_top, reg0, reg1);
		LOWPASS_SSE2(p_left, reg1, reg2);
		reg2 = _mm_add_epi16(reg0, reg1);
		reg2 = _mm_srli_epi16(reg2, 1);
		reg2 = _mm_packus_epi16(reg2, reg2);
		_mm_storel_epi64((__m128i*)p_dest, reg2);

		p_left++; p_top++;
		p_dest += i_stride;

	}
}
/****************************************************************************
 * * xavs_intra_pred_down_left:帧内预测模式之右下
 ****************************************************************************/
static void xavs_intra_pred_down_right_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
    int x,y;
    for(y=0; y<8; y++)
        for(x=0; x<8; x++)
            if(x==y)
                p_dest[y*i_stride+x] = (p_left[1]+2*p_top[0]+p_top[1]+2)>>2;
            else if(x>y)
                p_dest[y*i_stride+x] = LOWPASS(p_top,x-y);
            else
                p_dest[y*i_stride+x] = LOWPASS(p_left,y-x);
}

/****************************************************************************
 * * xavs_intra_pred_plane:帧内预测模式之色度平面
 ****************************************************************************/
static void xavs_intra_pred_plane_sse2(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	  /*  int x,y,i_value;
    int i_h = 0;
    int i_v = 0;
    uint8_t *p_crop = crop_table + MAX_NEG_CROP;

    for(x=0; x<4; x++)
	{
        i_h += (x+1)*(p_top[5+x]-p_top[3-x]);
        i_v += (x+1)*(p_left[5+x]-p_left[3-x]);
    }
    i_value = (p_top[8]+p_left[8])<<4;
    i_h = (17*i_h+16)>>5;
    i_v = (17*i_v+16)>>5;
    for(y=0; y<8; y++)
	{
        for(x=0; x<8; x++)
		{
            p_dest[y*i_stride+x] = p_crop[(i_value+(x-3)*i_h+(y-3)*i_v+16)>>5];
		}
	}*/
	
	int i_value = ((p_top[8]+p_left[8]) << 4) + 16;
	int i_h = 0;
    int i_v = 0;
    for(int x=0; x < 4; x++)
	{
        i_h += (x+1)*(p_top[5+x]-p_top[3-x]);
        i_v += (x+1)*(p_left[5+x]-p_left[3-x]);
    }
    i_value = ((p_top[8]+p_left[8])<<4) + 16;
    i_h = (17*i_h+16)>>5;
    i_v = (17*i_v+16)>>5;
	static DECLARE_ALIGNED(16, int16_t, K)[8] = {-3,-2,-1,0,1,2,3,4};
	__m128i reg3 = _mm_load_si128((__m128i *)K);
	__m128i reg2 = _mm_set1_epi16(i_h);
	__m128i reg1 = _mm_mullo_epi16(reg2, reg3);//(x-3)*i_h
	reg2 = _mm_set1_epi16(i_v);
	reg3 = _mm_mullo_epi16(reg2, reg3);//(y - 3) * i_v 
	reg2 =  _mm_set1_epi16( i_value);
	reg3 =  _mm_add_epi16(reg2, reg3);

	DECLARE_ALIGNED(16, int16_t, temp)[8]; 
	_mm_store_si128((__m128i*)temp  ,reg3);
	for(int  y = 0; y < 8; y++)
	{
		//reg2 = _mm_set1_epi16( reg3.m128i_i16[y]);
		 reg2 = _mm_set1_epi16( temp[y]);
		
		reg2 = _mm_add_epi16(reg1, reg2);
		reg2 = _mm_srai_epi16(reg2, 5);
		reg2 = _mm_packus_epi16(reg2, reg2);
		_mm_storel_epi64((__m128i*)p_dest, reg2);
		p_dest += i_stride;
	}
	
}
#undef LOWPASS

#undef LOWPASS_SSE2

#undef LOWPASS_MMX

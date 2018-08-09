
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/

#define LOWPASS(ARRAY, INDEX)                                           \
    ((ARRAY[(INDEX) - 1] + 2 * ARRAY[(INDEX)] + ARRAY[(INDEX) + 1] + 2) >> 2)

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

/****************************************************************************
 * * xavs_intra_pred_dc_128:帧内预测模式之直流,顶块和左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_128_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
 {
    int y;
    uint64_t i_value = 0x8080808080808080;
    for(y=0;y<8;y++)
        *((uint64_t *)(p_dest+y*i_stride)) = i_value;
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp:帧内预测模式之直流,顶块和左块都能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m64 reg0, reg1,reg2, reg3, reg4,reg5;
	LOWPASS_MMX(p_left,reg0,reg1,reg2,reg3);
	reg0 = _m_packuswb(reg0, reg1);
	for(int i = 0; i < 8; i++)
	{
		__m64* _dst = (__m64*)p_dest;
		LOWPASS_MMX(p_top,reg4,reg5,reg2,reg3);
		reg1 = reg0;
		reg1 = _m_punpcklbw(reg1,reg1);
		reg1 = _m_punpcklwd(reg1,reg1);
		reg1 = _m_punpcklbw(reg1,__m_0);
	
		reg4 = _m_paddw(reg1, reg4);
		reg5 = _m_paddw(reg1, reg5);		
		reg4 = _m_psrlwi( reg4,1);
		reg5 = _m_psrlwi( reg5,1);
		_dst[0] = _m_packuswb(reg4, reg5);
		reg0 = _m_psrlqi(reg0,8);

		p_dest += i_stride;
	}
	_mm_empty();
}

/****************************************************************************
 * * xavs_intra_pred_dc_lp_left:帧内预测模式之直流,顶块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_left_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m64 reg0, reg1,reg2, reg3;
	LOWPASS_MMX(p_left,reg0,reg1,reg2,reg3);
	reg0 = _m_packuswb(reg0, reg1);
	for(int i = 0; i < 8; i++)
	{
		__m64* _dst = (__m64*)p_dest;
		reg1 = reg0;
		reg1 = _m_punpcklbw(reg1,reg1);
		reg1 = _m_punpcklwd(reg1,reg1);
		_dst[0] = _m_punpckldq(reg1,reg1);

		reg0 = _m_psrlqi(reg0,8);
		p_dest += i_stride;
	}
	_mm_empty();
}
/****************************************************************************
 * * xavs_intra_pred_dc_lp_top:帧内预测模式之直流,左块不能得到的情况下处理
 ****************************************************************************/
static void xavs_intra_pred_dc_lp_top_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m64 reg0, reg1,reg2, reg3;
	for(int i = 0; i < 8; i++)
	{
		__m64* _dst = (__m64*)p_dest;
		LOWPASS_MMX(p_top,reg0,reg1,reg2,reg3);
		_dst[0] = _m_packuswb(reg0, reg1);
		p_dest += i_stride;
	}
	_mm_empty();
	
}
/****************************************************************************
 * * xavs_intra_pred_horizontal:帧内预测模式之水平
 ****************************************************************************/
static void xavs_intra_pred_horizontal_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
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
static void xavs_intra_pred_vertical_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
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
static void xavs_intra_pred_down_left_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
{
	__m64 reg0, reg1,reg2, reg3, reg4,reg5;
	p_left++; p_top++;
	for(int i = 0; i < 8; i++)
	{
		__m64* _dst = (__m64*)p_dest;
		LOWPASS_MMX(p_left,reg0,reg1,reg2,reg3);
		LOWPASS_MMX(p_top,reg2,reg3,reg4,reg5);
		reg0 = _m_paddw(reg0, reg2);
		reg1 = _m_paddw(reg1, reg3);		
		reg0 = _m_psrlwi( reg0,1);
		reg1 = _m_psrlwi( reg1,1);
		_dst[0] = _m_packuswb(reg0, reg1);
		p_left++; p_top++;
		p_dest += i_stride;
	}
	_mm_empty();
}
/****************************************************************************
 * * xavs_intra_pred_down_left:帧内预测模式之右下
 ****************************************************************************/
static void xavs_intra_pred_down_right_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
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
static void xavs_intra_pred_plane_mmx(uint8_t *p_dest,uint8_t *p_top,uint8_t *p_left,int i_stride)
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
	static const __m64 K0 = {0x0000FFFFFFFEFFFDULL};//-3 -2 -1 0
	static const __m64 K1 = {0x0004000300020001ULL};//1,2,3,4
	__m64 reg0, reg1,reg2,reg3,reg4;
	reg2 = _mm_set1_pi16(i_h);
	reg0 = _m_pmullw(reg2,K0);
	reg1 = _m_pmullw(reg2,K1);
	for(int  y = 0; y < 8; y++)
	{
		__m64* _dst = (__m64*)p_dest;
		reg2 = _mm_set1_pi16( (y - 3) * i_v + i_value);
		reg3 = _m_paddw(reg0,reg2);
		reg4 = _m_paddw(reg1,reg2);
		reg3 = _m_psrawi(reg3, 5);
		reg4 = _m_psrawi(reg4, 5);
		_dst[0] = _m_packuswb(reg3, reg4);
		p_dest += i_stride;
	}
	_mm_empty();
}
#undef LOWPASS_MMX
#undef LOWPASS




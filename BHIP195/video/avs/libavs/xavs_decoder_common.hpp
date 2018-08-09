

static inline void adapt_pred_mode(int i_num, int *p_mode) 
{
	static const uint32_t adapt_tale[4][8] =
	{
		{ 0,-1, 6,-1,-1, 7, 6, 7},
		{-1, 1, 5,-1,-1, 5, 7, 7},
		{ 5,-1, 2,-1, 6, 5, 6,-1},
		{ 4, 1,-1,-1, 4, 6, 6,-1},
	};
    *p_mode = adapt_tale[i_num][*p_mode];
    if(*p_mode  < 0) 
	{
       
        *p_mode  = 0;
    }
}

static inline int get_cbp(int  i_cbp_code,int i_inter)
{
	static const uint8_t cbp_tab[64][2] = 
	{
		{63, 0},{15,15},{31,63},{47,31},{ 0,16},{14,32},{13,47},{11,13},
		{ 7,14},{ 5,11},{10,12},{ 8, 5},{12,10},{61, 7},{ 4,48},{55, 3},
		{ 1, 2},{ 2, 8},{59, 4},{ 3, 1},{62,61},{ 9,55},{ 6,59},{29,62},
		{45,29},{51,27},{23,23},{39,19},{27,30},{46,28},{53, 9},{30, 6},
		{43,60},{37,21},{60,44},{16,26},{21,51},{28,35},{19,18},{35,20},
		{42,24},{26,53},{44,17},{32,37},{58,39},{24,45},{20,58},{17,43},
		{18,42},{48,46},{22,36},{33,33},{25,34},{49,40},{40,52},{36,49},
		{34,50},{50,56},{52,25},{54,22},{41,54},{56,57},{38,41},{57,38}
	};
	return cbp_tab[i_cbp_code][i_inter];
}




static inline void load_intra_pred_chroma(xavs_decoder *p) 
{
	int i_offset=p->i_mb_x*10;
	p->i_left_border_cb[9] = p->i_left_border_cb[8];
    p->i_left_border_cr[9] = p->i_left_border_cr[8];
	if(p->i_mb_x==p->i_mb_width-1)
	{
		p->p_top_border_cb[i_offset+9] = p->p_top_border_cb[i_offset+8];
        p->p_top_border_cr[i_offset+9] = p->p_top_border_cr[i_offset+8];
        
	}
	else
	{
		p->p_top_border_cb[i_offset+9] = p->p_top_border_cb[i_offset+11];
        p->p_top_border_cr[i_offset+9] = p->p_top_border_cr[i_offset+11];
	}

	if(p->i_mb_x && p->i_mb_y&&p->b_first_line==0) 
	{
	    p->p_top_border_cb[i_offset] = p->i_left_border_cb[0] = p->i_topleft_border_cb;
        p->p_top_border_cr[i_offset] = p->i_left_border_cr[0] = p->i_topleft_border_cr;
    } 
	else
	{
        p->i_left_border_cb[0] = p->i_left_border_cb[1];
        p->i_left_border_cr[0] = p->i_left_border_cr[1];
        p->p_top_border_cb[i_offset] = p->p_top_border_cb[i_offset+1];
        p->p_top_border_cr[i_offset] = p->p_top_border_cr[i_offset+1];

    }
}
static inline void load_intra_pred_luma(xavs_decoder *p, uint8_t *p_top,uint8_t **pp_left, int block) 
{
    int i;
    switch(block) 
	{
    case 0:
        *pp_left = p->i_left_border_y;
        p->i_left_border_y[0] = p->i_left_border_y[1];
        memset(&p->i_left_border_y[17],p->i_left_border_y[16],9);
        memcpy(&p_top[1],&p->p_top_border_y[p->i_mb_x*XAVS_MB_SIZE],16);
        p_top[17] = p_top[16];
        p_top[0] = p_top[1];
        if((p->i_mb_flags & A_AVAIL) && (p->i_mb_flags & B_AVAIL))
            p->i_left_border_y[0] = p_top[0] = p->i_topleft_border_y;
        break;
    case 1:
        *pp_left = p->i_internal_border_y;
        for(i=0;i<8;i++)
		{
            p->i_internal_border_y[i+1] = *(p->p_y + 7 + i*p->cur.i_stride[0]);
		}
//由于下边块（索引为3）未解码所以9以后的象素直接用象素8的值
//当解码1，3块的时候由于左边块c[i]只有1~8是可用的，其他的还未解码，所以不可用
//所以不可能采用Intra_8x8_Down_Left,Intra_8x8_Down_Right模式，所以不可能采用17位的象素
        memset(&p->i_internal_border_y[9],p->i_internal_border_y[8],9);//但是17以后呢？？
        p->i_internal_border_y[0] = p->i_internal_border_y[1];
        memcpy(&p_top[1],&p->p_top_border_y[p->i_mb_x*XAVS_MB_SIZE+8],8);
        if(p->i_mb_flags & C_AVAIL)
            memcpy(&p_top[9],&p->p_top_border_y[(p->i_mb_x + 1)*XAVS_MB_SIZE],8);
        else
            memset(&p_top[9],p_top[8],9);
        p_top[17] =p_top[16];
        p_top[0] = p_top[1];
        if(p->i_mb_flags & B_AVAIL)
            p->i_internal_border_y[0] = p_top[0] = p->p_top_border_y[p->i_mb_x*XAVS_MB_SIZE+7];
        break;
    case 2:
        *pp_left = &p->i_left_border_y[8];
        memcpy(&p_top[1],p->p_y + 7*p->cur.i_stride[0],16);
        p_top[17] = p_top[16];
        p_top[0] = p_top[1];
        if(p->i_mb_flags & A_AVAIL)
            p_top[0] = p->i_left_border_y[8];
        break;
    case 3:
        *pp_left = &p->i_internal_border_y[8];
        for(i=0;i<8;i++)
		{
            p->i_internal_border_y[i+9] = *(p->p_y + 7 + (i+8)*p->cur.i_stride[0]);
		}
        memset(&p->i_internal_border_y[17],p->i_internal_border_y[16],9);
        memcpy(&p_top[0],p->p_y + 7 + 7*p->cur.i_stride[0],9);
        memset(&p_top[9],p_top[8],9);
        break;
    }
}



static inline void mv_pred_sym(xavs_decoder *p, xavs_vector *src, enum xavs_block_size size,int i_ref) 
{
    xavs_vector *dst = src + MV_BWD_OFFS;

    /* backward mv is the scaled and negated forward mv */
	if(p->ph.b_picture_structure==0)
	{
		int  iBlockDistanceBw,iBlockDistanceFw;
		dst->ref = 1-i_ref;
		iBlockDistanceFw = p->cur.i_distance_index - p->ref[i_ref + 2].i_distance_index;
		iBlockDistanceBw = p->ref[dst->ref].i_distance_index - p->cur.i_distance_index;
		iBlockDistanceFw = (iBlockDistanceFw+512) % 512;
		iBlockDistanceBw = (iBlockDistanceBw+512) % 512;
		dst->x = -((src->x * iBlockDistanceBw*(512/iBlockDistanceFw) + 256) >> 9);
		dst->y = -((src->y * iBlockDistanceBw*(512/iBlockDistanceFw) + 256) >> 9);
		//dst->y = -((src->y * p->i_sym_factor + 256) >> 9);

	}
	else
	{
		dst->ref = i_ref;
		dst->x = -((src->x * p->i_sym_factor + 256) >> 9);
		dst->y = -((src->y * p->i_sym_factor + 256) >> 9);
		
	}

   // dst->dist = p->i_ref_distance[0];
    copy_mvs(dst, size);
}

static inline void scale_mv(xavs_decoder *p, int *d_x, int *d_y, xavs_vector *src, int distp) 
{
    int den = p->i_scale_den[src->ref];

    *d_x = (src->x*distp*den + 256 + (src->x>>31)) >> 9;
    *d_y = (src->y*distp*den + 256 + (src->y>>31)) >> 9;
}

static inline int mid_pred(int a, int b, int c)
{
#if 0
    int t= (a-b)&((a-b)>>31);
    a-=t;
    b+=t;
    b-= (b-c)&((b-c)>>31);
    b+= (a-b)&((a-b)>>31);

    return b;
#else
    if(a>b){
        if(c>b){
            if(c>a) b=a;
            else    b=c;
        }
    }else{
        if(b>c){
            if(c>a) b=c;
            else    b=a;
        }
    }
    return b;
#endif
}


static inline void mv_pred_median(xavs_decoder *p, xavs_vector *mvP, xavs_vector *mvA, xavs_vector *mvB, xavs_vector *mvC)
{
    int ax, ay, bx, by, cx, cy;
    int len_ab, len_bc, len_ca, len_mid;

    /* scale candidates according to their temporal span */
    scale_mv(p, &ax, &ay, mvA, mvP->dist);
    scale_mv(p, &bx, &by, mvB, mvP->dist);
    scale_mv(p, &cx, &cy, mvC, mvP->dist);
    /* find the geometrical median of the three candidates */
    len_ab = abs(ax - bx) + abs(ay - by);
    len_bc = abs(bx - cx) + abs(by - cy);
    len_ca = abs(cx - ax) + abs(cy - ay);
    len_mid = mid_pred(len_ab, len_bc, len_ca);
    if(len_mid == len_ab) 
	{
        mvP->x = cx;
        mvP->y = cy;
    }
	else if(len_mid == len_bc) 
	{
        mvP->x = ax;
        mvP->y = ay;
    }
	else 
	{
        mvP->x = bx;
        mvP->y = by;
    }
}





static void inline xavs_emulated_edge_mc(uint8_t *buf, uint8_t *src, int linesize, int block_w, int block_h, 
                                    int src_x, int src_y, int w, int h)
{
    int x, y;
    int start_y, start_x, end_y, end_x;
    
    if(src_y>= h){
        src+= (h-1-src_y)*linesize;
        src_y=h-1;
    }else if(src_y<=-block_h){
        src+= (1-block_h-src_y)*linesize;
        src_y=1-block_h;
    }
    if(src_x>= w){
        src+= (w-1-src_x);
        src_x=w-1;
    }else if(src_x<=-block_w){
        src+= (1-block_w-src_x);
        src_x=1-block_w;
    }

    start_y= XAVS_MAX(0, -src_y);
    start_x= XAVS_MAX(0, -src_x);
    end_y= XAVS_MIN(block_h, h-src_y);
    end_x= XAVS_MIN(block_w, w-src_x);

    // copy existing part
    for(y=start_y; y<end_y; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*linesize]= src[x + y*linesize];
        }
    }

    //top
    for(y=0; y<start_y; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*linesize]= buf[x + start_y*linesize];
        }
    }

    //bottom
    for(y=end_y; y<block_h; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*linesize]= buf[x + (end_y-1)*linesize];
        }
    }
                                    
    for(y=0; y<block_h; y++){
       //left
        for(x=0; x<start_x; x++){
            buf[x + y*linesize]= buf[start_x + y*linesize];
        }
       
       //right
        for(x=end_x; x<block_w; x++){
            buf[x + y*linesize]= buf[end_x - 1 + y*linesize];
        }
    }
}
//weighting prediction

static inline void mc_dir_part_w(xavs_decoder *p,xavs_image *ref,
                        int chroma_height,int forward,uint8_t *dest_y,
                        uint8_t *dest_cb,uint8_t *dest_cr,int src_x_offset,
                        int src_y_offset,xavs_luma_mc_w_func *luma,
                        xavs_chroma_mc_w_func chroma,xavs_vector *mv, int b_mb)
{
    
    const int mx= mv->x + src_x_offset*8;
    const int my= mv->y + src_y_offset*8;
    const int luma_xy= (mx&3) + ((my&3)<<2);
    uint8_t * src_y = ref->p_data[0] + (mx>>2) + (my>>2)*ref->i_stride[0];
    uint8_t * src_cb= ref->p_data[1] + (mx>>3) + (my>>3)*ref->i_stride[1];
    uint8_t * src_cr= ref->p_data[2] + (mx>>3) + (my>>3)*ref->i_stride[2];
    int extra_width= 0; //(s->flags&CODEC_FLAG_EMU_EDGE) ? 0 : 16;
    int extra_height= extra_width;
    int emu=0;
    const int full_mx= mx>>2;
    const int full_my= my>>2;
    const int pic_width  = 16*p->i_mb_width;
    const int pic_height =16*( p->ph.b_picture_structure==0?(p->i_mb_height>>1):p->i_mb_height);
    if(!ref->p_data[0])
        return;
    if(mx&7) extra_width -= 3;
    if(my&7) extra_height -= 3;

    if(   full_mx < 0-extra_width
          || full_my < 0-extra_height
          || full_mx + 16/*FIXME*/ > pic_width + extra_width
          || full_my + 16/*FIXME*/ > pic_height + extra_height)
	{
        xavs_emulated_edge_mc(p->p_edge, src_y - 2 - 2*ref->i_stride[0], ref->i_stride[0],
                            16+5, 16+5/*FIXME*/, full_mx-2, full_my-2, pic_width, pic_height);
        src_y= p->p_edge + 2 + 2*ref->i_stride[0];
        emu=1;
    }
	//int b_mb = 0;
	//if(p->)
	if(b_mb)
	{
		if(forward == 1)
		{
			luma[luma_xy](dest_y, src_y, ref->i_stride[0],
				p->sh.i_luma_scale[(mv->ref - 2) * 2],p->sh.i_luma_shift[(mv->ref - 2) * 2]); //FIXME try variable height perhaps?
		}
		else
		{
			luma[luma_xy](dest_y, src_y, ref->i_stride[0],
				p->sh.i_luma_scale[mv->ref * 2 + 1 ],p->sh.i_luma_shift[mv->ref * 2 + 1]); //FIXME try variable height perhaps?
		}
	}
	else
	{
		luma[luma_xy](dest_y, src_y, ref->i_stride[0],
			p->sh.i_luma_scale[mv->ref],p->sh.i_luma_shift[mv->ref]); //FIXME try variable height perhaps?
	}
    

    if(emu)
	{
        xavs_emulated_edge_mc(p->p_edge, src_cb, ref->i_stride[1],
                            9, 9/*FIXME*/, (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
        src_cb= p->p_edge;
    }
	if(b_mb)
	{
		if(forward == 1)
		{
			chroma(dest_cb, src_cb, ref->i_stride[1], chroma_height, mx&7, my&7,
				p->sh.i_chroma_scale[(mv->ref - 2) * 2 ],p->sh.i_chroma_shift[(mv->ref - 2) * 2]);
		}
		else
		{
			chroma(dest_cb, src_cb, ref->i_stride[1], chroma_height, mx&7, my&7,
				p->sh.i_chroma_scale[mv->ref * 2 + 1 ],p->sh.i_chroma_shift[mv->ref * 2 + 1]);
		}
		
	}
	else
	{
		chroma(dest_cb, src_cb, ref->i_stride[1], chroma_height, mx&7, my&7,
			p->sh.i_chroma_scale[mv->ref],p->sh.i_chroma_shift[mv->ref]);
		
	}
   // chroma(dest_cb, src_cb, ref->i_stride[1], chroma_height, mx&7, my&7);

    if(emu)
	{
        xavs_emulated_edge_mc(p->p_edge, src_cr, ref->i_stride[2],
                            9, 9/*FIXME*/, (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
        src_cr= p->p_edge;
    }
	if(b_mb)
	{

		if(forward == 1)
		{
			chroma(dest_cr, src_cr, ref->i_stride[2], chroma_height, mx&7, my&7,
				p->sh.i_chroma_scale[(mv->ref - 2) * 2 ],p->sh.i_chroma_shift[(mv->ref - 2) * 2]);
		}
		else
		{
			chroma(dest_cr, src_cr, ref->i_stride[2], chroma_height, mx&7, my&7,
				p->sh.i_chroma_scale[mv->ref * 2 + 1 ],p->sh.i_chroma_shift[mv->ref * 2 + 1]);
		}
		
	}
	else
	{
		chroma(dest_cr, src_cr, ref->i_stride[2], chroma_height, mx&7, my&7,
			p->sh.i_chroma_scale[mv->ref],p->sh.i_chroma_shift[mv->ref]);
		
	}
  //  
	//chroma(dest_cr, src_cr, ref->i_stride[2], chroma_height, mx&7, my&7);
}

static inline void mc_part_std_w(xavs_decoder *p,int chroma_height,
                       uint8_t *dest_y,uint8_t *dest_cb,uint8_t *dest_cr,
					   int x_offset, int y_offset,xavs_vector *mv,int b_mb,
					   xavs_luma_mc_w_func *luma_put,xavs_chroma_mc_w_func chroma_put,
					   xavs_luma_mc_w_func *luma_avg,xavs_chroma_mc_w_func chroma_avg)
{
//	int i_ref_offset=(p->ph.b_picture_structure==0&&p->ph.i_picture_coding_type==XAVS_B_PICTURE)?2:0;
	xavs_luma_mc_w_func *lum=  luma_put;
    xavs_chroma_mc_w_func chroma= chroma_put;

	dest_y  += 2*x_offset + 2*y_offset*p->cur.i_stride[0];
    dest_cb +=   x_offset +   y_offset*p->cur.i_stride[1];
    dest_cr +=   x_offset +   y_offset*p->cur.i_stride[2];
    x_offset += 8*p->i_mb_x;
    y_offset += 8*(p->i_mb_y-p->i_mb_offset);
	if(mv->ref >= 0)
	{

        xavs_image *ref= &p->ref[mv->ref];
		
        mc_dir_part_w(p, ref,chroma_height,1,dest_y, dest_cb, dest_cr, x_offset, y_offset,
                    lum, chroma, mv,b_mb);

        lum=  luma_avg;
        chroma= chroma_avg;
    }

    if((mv+MV_BWD_OFFS)->ref >= 0)
	{
		xavs_image *ref= &p->ref[(mv+MV_BWD_OFFS)->ref];
        mc_dir_part_w(p, ref,  chroma_height,  0,
                    dest_y, dest_cb, dest_cr, x_offset, y_offset,
                     lum, chroma, mv+MV_BWD_OFFS, b_mb);
    }

}

static inline void inter_pred_w(xavs_decoder *p, int i_mb_type, int b_mb) 
{
	if(partition_flags[i_mb_type] == 0)
	{ // 16x16
        mc_part_std_w(p, 8, p->p_y, p->p_cb, p->p_cr,
			0, 0,&p->mv[MV_FWD_X0],b_mb,
			xavs_luma_put_w[0],xavs_chroma_put_w[0],
			xavs_luma_avg_w[0],xavs_chroma_avg_w[0]);
    }
	else
	{
        mc_part_std_w(p, 4, p->p_y, p->p_cb, p->p_cr, 
			0, 0,&p->mv[MV_FWD_X0],b_mb,
			xavs_luma_put_w[1],xavs_chroma_put_w[1],
			xavs_luma_avg_w[1],xavs_chroma_avg_w[1]);
		mc_part_std_w(p, 4, p->p_y, p->p_cb, p->p_cr,
			4, 0,&p->mv[MV_FWD_X1],b_mb,
			xavs_luma_put_w[1],xavs_chroma_put_w[1],
			xavs_luma_avg_w[1],xavs_chroma_avg_w[1]);
		mc_part_std_w(p, 4, p->p_y, p->p_cb, p->p_cr,
			0, 4,&p->mv[MV_FWD_X2],b_mb,
			xavs_luma_put_w[1],xavs_chroma_put_w[1],
			xavs_luma_avg_w[1],xavs_chroma_avg_w[1]);
		mc_part_std_w(p, 4, p->p_y, p->p_cb, p->p_cr,
			4, 4,&p->mv[MV_FWD_X3],b_mb,
			xavs_luma_put_w[1],xavs_chroma_put_w[1],
			xavs_luma_avg_w[1],xavs_chroma_avg_w[1]);
        
    }
    /* set intra prediction modes to default values */
    //p->i_intra_pred_mode_y[3] =  p->i_intra_pred_mode_y[6] = INTRA_L_LP;
	//p->p_top_intra_pred_mode_y[p->i_mb_x*2+0] = p->p_top_intra_pred_mode_y[p->i_mb_x*2+1] = INTRA_L_LP;
	p->i_intra_pred_mode_y[3] =  p->i_intra_pred_mode_y[6] = NOT_AVAIL;
	p->p_top_intra_pred_mode_y[p->i_mb_x*2+0] = p->p_top_intra_pred_mode_y[p->i_mb_x*2+1] = NOT_AVAIL;
}



static inline void mc_dir_part(xavs_decoder *p,xavs_image *ref,
                        int chroma_height,int list,uint8_t *dest_y,
                        uint8_t *dest_cb,uint8_t *dest_cr,int src_x_offset,
                        int src_y_offset,xavs_luma_mc_func *luma,
                        xavs_chroma_mc_func chroma,xavs_vector *mv)
{
    
    const int mx= mv->x + src_x_offset*8;
    const int my= mv->y + src_y_offset*8;
    const int luma_xy= (mx&3) + ((my&3)<<2);
    uint8_t * src_y = ref->p_data[0] + (mx>>2) + (my>>2)*ref->i_stride[0];
    uint8_t * src_cb= ref->p_data[1] + (mx>>3) + (my>>3)*ref->i_stride[1];
    uint8_t * src_cr= ref->p_data[2] + (mx>>3) + (my>>3)*ref->i_stride[2];
    int extra_width= 0; //(s->flags&CODEC_FLAG_EMU_EDGE) ? 0 : 16;
    int extra_height= extra_width;
    int emu=0;
    const int full_mx= mx>>2;
    const int full_my= my>>2;
    const int pic_width  = 16*p->i_mb_width;
    const int pic_height =16*( p->ph.b_picture_structure==0?(p->i_mb_height>>1):p->i_mb_height);
    if(!ref->p_data[0])
        return;
    if(mx&7) extra_width -= 3;
    if(my&7) extra_height -= 3;

    if(   full_mx < 0-extra_width
          || full_my < 0-extra_height
          || full_mx + 16/*FIXME*/ > pic_width + extra_width
          || full_my + 16/*FIXME*/ > pic_height + extra_height)
	{
        xavs_emulated_edge_mc(p->p_edge, src_y - 2 - 2*ref->i_stride[0], ref->i_stride[0],
                            16+5, 16+5/*FIXME*/, full_mx-2, full_my-2, pic_width, pic_height);
        src_y= p->p_edge + 2 + 2*ref->i_stride[0];
        emu=1;
    }
    luma[luma_xy](dest_y, src_y, ref->i_stride[0]); //FIXME try variable height perhaps?
    


    if(emu)
	{
        xavs_emulated_edge_mc(p->p_edge, src_cb, ref->i_stride[1],
                            9, 9/*FIXME*/, (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
        src_cb= p->p_edge;
    }
    chroma(dest_cb, src_cb, ref->i_stride[1], chroma_height, mx&7, my&7);

	
	
    if(emu)
	{
        xavs_emulated_edge_mc(p->p_edge, src_cr, ref->i_stride[2],
                            9, 9/*FIXME*/, (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
        src_cr= p->p_edge;
    }
    chroma(dest_cr, src_cr, ref->i_stride[2], chroma_height, mx&7, my&7);

	

	
}

static inline void mc_part_std(xavs_decoder *p,int chroma_height,
                       uint8_t *dest_y,uint8_t *dest_cb,uint8_t *dest_cr,
					   int x_offset, int y_offset,xavs_vector *mv,
					   xavs_luma_mc_func *luma_put,xavs_chroma_mc_func chroma_put,
					   xavs_luma_mc_func *luma_avg,xavs_chroma_mc_func chroma_avg)
{


	xavs_luma_mc_func *lum=  luma_put;
    xavs_chroma_mc_func chroma= chroma_put;

	dest_y  += 2*x_offset + 2*y_offset*p->cur.i_stride[0];
    dest_cb +=   x_offset +   y_offset*p->cur.i_stride[1];
    dest_cr +=   x_offset +   y_offset*p->cur.i_stride[2];
    x_offset += 8*p->i_mb_x;
    y_offset += 8*(p->i_mb_y-p->i_mb_offset);
	if(mv->ref >= 0)
	{

		
        xavs_image *ref= &p->ref[mv->ref];
		
        mc_dir_part(p, ref,chroma_height,0,dest_y, dest_cb, dest_cr, x_offset, y_offset,
                     lum, chroma, mv);

		
        lum=  luma_avg;
        chroma= chroma_avg;
    }

    if((mv+MV_BWD_OFFS)->ref >= 0)
	{
		xavs_image *ref= &p->ref[(mv+MV_BWD_OFFS)->ref];
        mc_dir_part(p, ref,  chroma_height,  1,
                    dest_y, dest_cb, dest_cr, x_offset, y_offset,
                     lum, chroma, mv+MV_BWD_OFFS);
		

    }

	
}

static inline void inter_pred(xavs_decoder *p, int i_mb_type) 
{
	
	
    if(partition_flags[i_mb_type] == 0)
	{ // 16x16
        mc_part_std(p, 8, p->p_y, p->p_cb, p->p_cr,
			0, 0,&p->mv[MV_FWD_X0],
			xavs_luma_put[0],xavs_chroma_put[0],
			xavs_luma_avg[0],xavs_chroma_avg[0]);
		
    }
	else
	{
		
        mc_part_std(p, 4, p->p_y, p->p_cb, p->p_cr, 
			0, 0,&p->mv[MV_FWD_X0],
			xavs_luma_put[1],xavs_chroma_put[1],
			xavs_luma_avg[1],xavs_chroma_avg[1]);
		mc_part_std(p, 4, p->p_y, p->p_cb, p->p_cr,
			4, 0,&p->mv[MV_FWD_X1],
			xavs_luma_put[1],xavs_chroma_put[1],
			xavs_luma_avg[1],xavs_chroma_avg[1]);
		mc_part_std(p, 4, p->p_y, p->p_cb, p->p_cr,
			0, 4,&p->mv[MV_FWD_X2],
			xavs_luma_put[1],xavs_chroma_put[1],
			xavs_luma_avg[1],xavs_chroma_avg[1]);
		mc_part_std(p, 4, p->p_y, p->p_cb, p->p_cr,
			4, 4,&p->mv[MV_FWD_X3],
			xavs_luma_put[1],xavs_chroma_put[1],
			xavs_luma_avg[1],xavs_chroma_avg[1]);
        
    }
    /* set intra prediction modes to default values */
    //p->i_intra_pred_mode_y[3] =  p->i_intra_pred_mode_y[6] = INTRA_L_LP;
	//p->p_top_intra_pred_mode_y[p->i_mb_x*2+0] = p->p_top_intra_pred_mode_y[p->i_mb_x*2+1] = INTRA_L_LP;
	p->i_intra_pred_mode_y[3] =  p->i_intra_pred_mode_y[6] = NOT_AVAIL;
	p->p_top_intra_pred_mode_y[p->i_mb_x*2+0] = p->p_top_intra_pred_mode_y[p->i_mb_x*2+1] = NOT_AVAIL;
	
    
}


static void inline mv_pred_sub_direct(xavs_decoder *p,xavs_vector *mv, int i_Offset,uint8_t nP,uint8_t nC,
                    enum xavs_mv_pred mode, enum xavs_block_size size, int ref) 
{
    xavs_vector *mvP = &p->mv[nP];
    xavs_vector *mvA = &mv[MV_FWD_A1+i_Offset];
    xavs_vector *mvB = &mv[MV_FWD_B2+i_Offset];
    xavs_vector *mvC = &mv[MV_FWD_C2+i_Offset];
    const xavs_vector *mvP2 = NULL;

    mvP->ref = ref;
    mvP->dist = p->i_ref_distance[mvP->ref];
    if(mvC->ref == NOT_AVAIL)
        mvC = &mv[MV_FWD_D3+i_Offset]; // set to top-left (mvD)
   if((mvA->ref >= 0) && (mvB->ref < 0) && (mvC->ref < 0)) 
	{
        mvP2= mvA;
    }
	else if((mvA->ref < 0) && (mvB->ref >= 0) && (mvC->ref < 0)) 
	{
        mvP2= mvB;
    }
	else if((mvA->ref < 0) && (mvB->ref < 0) && (mvC->ref >= 0)) 
	{
        mvP2= mvC;
    } 
	else if(mode == MV_PRED_LEFT     && mvA->ref == ref)
	{
        mvP2= mvA;
    }
	else if(mode == MV_PRED_TOP      && mvB->ref == ref)
	{
        mvP2= mvB;
    }
	else if(mode == MV_PRED_TOPRIGHT && mvC->ref == ref)
	{
        mvP2= mvC;
    }
    if(mvP2)
	{
        mvP->x = mvP2->x;
        mvP->y = mvP2->y;
    }
	else
	{
        mv_pred_median(p, mvP, mvA, mvB, mvC);
	}
}

static inline void mv_pred_direct(xavs_decoder *p, xavs_vector *pmv_fw,xavs_vector *col_mv,
	int iDistatnceIndexCol,int iDistanceIndexRef,int iDistanceIndexFw,int iDistanceIndexBw,
	int delta1, int deltaFw, int deltaBw) 
{
	int den ;
	int iBlockDistanceRef;
	int iDistanceIndexCur = p->cur.i_distance_index;
    xavs_vector *pmv_bw = pmv_fw + MV_BWD_OFFS;	
	int m = col_mv->x >> 31;
	iBlockDistanceRef = (iDistatnceIndexCol - iDistanceIndexRef + 512) % 512;
	pmv_fw->dist = (iDistanceIndexCur - iDistanceIndexFw + 512) % 512;
	pmv_bw->dist = (iDistanceIndexBw - iDistanceIndexCur + 512) % 512;
	den = (iBlockDistanceRef == 0) ? 0 : 16384 / iBlockDistanceRef;
    
	if(m)
	{
		
	//	pmv_fw->x = -((den*(1-col_mv->x*pmv_fw->dist)-1)>>14);
	//	pmv_bw->x = ((den*(1-col_mv->x*pmv_bw->dist)-1)>>14);
		pmv_fw->x = -((den-den*col_mv->x*pmv_fw->dist-1)>>14);
		pmv_bw->x = (den-den*col_mv->x*pmv_bw->dist-1)>>14;
	}
	else
	{
	//	pmv_fw->x = ((den*(1+col_mv->x*pmv_fw->dist)-1)>>14);
	//	pmv_bw->x = -((den*(1+col_mv->x*pmv_bw->dist)-1)>>14);
		pmv_fw->x = ((den+den*col_mv->x*pmv_fw->dist-1)>>14);
		pmv_bw->x = -((den+den*col_mv->x*pmv_bw->dist-1)>>14);
	}
	int t = (col_mv->y + delta1);
	m = t >> 31;
	if(m)
	{//小于0
		//pmv_fw->y = -((den*(1-col_mv->y*pmv_fw->dist)-1)>>14);
		//pmv_bw->y = ((den*(1-col_mv->y*pmv_bw->dist)-1)>>14);
		pmv_fw->y = -((den - den * t * pmv_fw->dist - 1) >> 14) - deltaFw;
		pmv_bw->y =  ((den - den * t * pmv_bw->dist - 1) >> 14) - deltaBw;
	}
	else
	{
		//pmv_fw->y = ((den*(1+col_mv->y*pmv_fw->dist)-1)>>14);
		//pmv_bw->y = -((den*(1+col_mv->y*pmv_bw->dist)-1)>>14);
		pmv_fw->y =  ((den + den *  t * pmv_fw->dist - 1)>>14) - deltaFw;
		pmv_bw->y = -((den + den * t * pmv_bw->dist - 1 )>>14) - deltaBw;
	}
}


#include "xavs_decoder_b_special.hpp"

static inline void get_b_direct_skip_mb(xavs_decoder *p)
{
	xavs_vector mv[24];
	int block;
	uint32_t b_next_field=p->i_mb_index>=p->i_mb_num_half;
	int i_ref_offset=p->ph.b_picture_structure==0?2:1;
	uint8_t i_col_type;
	int     i_col_distance, i_col_top_field;
	xavs_vector *p_mv;

	
	mv[MV_FWD_A1]=p->mv[MV_FWD_A1];
	mv[MV_FWD_B2]=p->mv[MV_FWD_B2];
	mv[MV_FWD_C2]=p->mv[MV_FWD_C2];
	mv[MV_FWD_D3]=p->mv[MV_FWD_D3];
	mv[MV_BWD_A1]=p->mv[MV_BWD_A1];
	mv[MV_BWD_B2]=p->mv[MV_BWD_B2];
	mv[MV_BWD_C2]=p->mv[MV_BWD_C2];
	mv[MV_BWD_D3]=p->mv[MV_BWD_D3];
	for(block=0;block<4;block++)
	{
		get_col_info(p,&i_col_type,&p_mv,block,&i_col_distance, &i_col_top_field );
		if(!i_col_type) 
		{
			mv_pred_sub_direct(p,mv, 0,mv_scan[block], mv_scan[block]-3,
                            MV_PRED_BSKIP, BLK_8X8, i_ref_offset);
			mv_pred_sub_direct(p, mv,MV_BWD_OFFS,mv_scan[block]+MV_BWD_OFFS,
				mv_scan[block]-3+MV_BWD_OFFS,
				MV_PRED_BSKIP, BLK_8X8, 0);
			
		}
		else
		{
			get_b_direct_skip_sub_mb(p,block,p_mv,i_col_distance, i_col_top_field);
		}
	}
	
	
}


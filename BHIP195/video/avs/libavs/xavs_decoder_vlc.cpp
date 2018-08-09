

//#include "stdafx.h"
#include "xavs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_decoder.h"
#include "xavs_decoder_common.hpp"

static inline void mv_pred(xavs_decoder *p, uint8_t nP,uint8_t nC,
                    enum xavs_mv_pred mode, enum xavs_block_size size, int ref) 
{
    xavs_vector *mvP = &p->mv[nP];
    xavs_vector *mvA = &p->mv[nP-1];
    xavs_vector *mvB = &p->mv[nP-4];
    xavs_vector *mvC = &p->mv[nC];
    const xavs_vector *mvP2 = NULL;

	
    mvP->ref = ref;
    mvP->dist = p->i_ref_distance[mvP->ref];
    if(mvC->ref == NOT_AVAIL)
        mvC = &p->mv[nP-5]; // set to top-left (mvD)
    if((mode == MV_PRED_PSKIP) &&
       ((mvA->ref == NOT_AVAIL) || (mvB->ref == NOT_AVAIL) ||
           ((mvA->x | mvA->y ) == 0 &&  mvA->ref  == ref)  ||
           ((mvB->x | mvB->y ) == 0 &&  mvB->ref  == ref) )) 
	{
        mvP2 = &MV_NOT_AVAIL;
    /* if there is only one suitable candidate, take it */
    }
	else if((mvA->ref >= 0) && (mvB->ref < 0) && (mvC->ref < 0)) 
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
    }else
	{
        mv_pred_median(p, mvP, mvA, mvB, mvC);
	}
    
    if(mode < MV_PRED_PSKIP) 
	{
        mvP->x += xavs_bitstream_get_se(&p->s);
        mvP->y += xavs_bitstream_get_se(&p->s);
    }
	

	
    copy_mvs(mvP,size);
}

static int get_residual_block(xavs_decoder *p,const xavs_vlc *p_vlc_table, 
							  int i_escape_order,int i_qp, uint8_t *p_dest, int i_stride) 
{
 //  static const xavs_vlc *vlc_tale[3]={intra_2dvlc,inter_2dvlc,chroma_2dvlc};

    int i,pos = -1;
    int level_code, esc_code, level, run, mask;
    int level_buf[65];
    int run_buf[65];
    int dqm = dequant_mul[i_qp];
    int dqs = dequant_shift[i_qp];
    int dqa = 1 << (dqs - 1);
    const uint8_t *zigzag=p->ph.b_picture_structure==0?zigzag_field:zigzag_progressive ;
    DCTELEM *block = p->p_block;

    for(i=0;i<65;i++) 
	{
		
        level_code = xavs_bitstream_get_ue_k(&p->s,p_vlc_table->golomb_order);
        if(level_code >= ESCAPE_CODE) 
		{
            run = ((level_code - ESCAPE_CODE) >> 1) + 1;
			//码表里run是加了1的所以对应的也应该加1
            esc_code = xavs_bitstream_get_ue_k(&p->s,i_escape_order);
			
            level = esc_code + (run > p_vlc_table->max_run ? 1 : p_vlc_table->level_add[run]);
            while(level > p_vlc_table->inc_limit)
                p_vlc_table++;
            mask = -(level_code & 1);
            level = (level^mask) - mask;
        } 
		else
		{
            level = p_vlc_table->rltab[level_code][0];
            if(!level) //end of block signal
                break;
            run   = p_vlc_table->rltab[level_code][1];
            p_vlc_table += p_vlc_table->rltab[level_code][2];
        }
        level_buf[i] = level;
        run_buf[i] = run;
		
		
    }


	
	if(p->ph.b_weighting_quant_flag)
	{
		uint8_t *wq = p->p_wqM88;
		while(--i >= 0)
		{
			pos += run_buf[i];
			if(pos > 63) 
			{
				return -1;
			}

			block[zigzag[pos]] = ((((level_buf[i] * wq[zigzag[pos]] >> 3 ) * dqm) >> 4) + dqa) >> dqs;
		}


	}
	else
	{
		while(--i >= 0)
		{
			pos += run_buf[i];
			if(pos > 63) 
			{
				return -1;
			}

			block[zigzag[pos]] = (level_buf[i]*dqm + dqa) >> dqs;
		}
	}

	
	

    xavs_idct8_add(p_dest,block,i_stride);

	
    return 0;
}


static inline int get_residual_inter(xavs_decoder *p) 
{
    int block;
    /* get coded block pattern */
    int i_cbp_code= xavs_bitstream_get_ue(&p->s);
    if(i_cbp_code > 63)
	{
        return -1;
    }
    p->i_cbp = get_cbp(i_cbp_code,1);
    /* get quantizer */
    if(p->i_cbp && !p->b_fixed_qp)
        p->i_qp = (p->i_qp + xavs_bitstream_get_se(&p->s)) & 63;
    for(block=0;block<4;block++)
	{
        if(p->i_cbp & (1<<block))
		{
            get_residual_block(p,inter_2dvlc,0,p->i_qp,
                                  p->p_y + p->i_luma_offset[block], p->cur.i_stride[0]);
		}
		
		
	}
	if(p->i_cbp & (1<<4))
	{
        get_residual_block(p,chroma_2dvlc,0, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cb],
                              p->p_cb,p->cur.i_stride[1]);
	}
	
    if(p->i_cbp & (1<<5))
	{
        get_residual_block(p,chroma_2dvlc,0, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cr],
                              p->p_cr,p->cur.i_stride[2]);
	}
	



    return 0;
}










static inline void get_b_frame_ref(xavs_decoder *p, int *p_ref)
{
	
	if(p->ph.b_picture_structure==0&&p->ph.b_picture_reference_flag==0 )
	{
		*p_ref=xavs_bitstream_get_bits(&p->s,1);
	}
	else
	{
		
		*p_ref=0;
		
	}
}










static int get_mb_b(xavs_decoder *p,uint32_t i_mb_type)
{   
	
	int kk=0; 
	xavs_vector mv[24];
    int block;
	
    enum xavs_mb_sub_type sub_type[4];
    int flags;
	int ref[4];    
	int i_ref_offset=p->ph.b_picture_structure==0?2:1;
	uint8_t i_col_type;
	int     i_col_distance, i_col_top_field;
	xavs_vector *p_mv;
	xavs_mb_init(p);
	p->mv[MV_FWD_X0] = MV_REF_DIR;
    copy_mvs(&p->mv[MV_FWD_X0], BLK_16X16);
    p->mv[MV_BWD_X0] = MV_REF_DIR;
    copy_mvs(&p->mv[MV_BWD_X0], BLK_16X16);
	

	
    switch(i_mb_type) 
	{
    case B_SKIP:
    case B_DIRECT:
		get_b_direct_skip_mb(p);
		

        break;
    case B_FWD_16X16:
		get_b_frame_ref(p,&ref[0]);
        mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN, BLK_16X16, ref[0]+i_ref_offset);
        break;
    case B_SYM_16X16:
		get_b_frame_ref(p,&ref[0]);
        mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN, BLK_16X16, ref[0]+i_ref_offset);
        mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X16,ref[0]);
        break;
    case B_BWD_16X16:
		get_b_frame_ref(p,&ref[0]);
        mv_pred(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_MEDIAN, BLK_16X16,  ref[0]);
        break;
    case B_8X8:
	    mv[MV_FWD_A1]=p->mv[MV_FWD_A1];
		mv[MV_FWD_B2]=p->mv[MV_FWD_B2];
		mv[MV_FWD_C2]=p->mv[MV_FWD_C2];
		mv[MV_FWD_D3]=p->mv[MV_FWD_D3];
		mv[MV_BWD_A1]=p->mv[MV_BWD_A1];
		mv[MV_BWD_B2]=p->mv[MV_BWD_B2];
		mv[MV_BWD_C2]=p->mv[MV_BWD_C2];
		mv[MV_BWD_D3]=p->mv[MV_BWD_D3];
        for(block=0;block<4;block++)
            sub_type[block] = (xavs_mb_sub_type)xavs_bitstream_get_bits(&p->s,2);
		for(block=0;block<4;block++)
		{
			if(sub_type[block]==B_SUB_DIRECT||sub_type[block]==B_SUB_BWD)
				ref[block]=0;
		    else
				get_b_frame_ref(p,&ref[block]);	
		}
		for(block=0;block<4;block++)
		{
			if(sub_type[block]==B_SUB_BWD)
				get_b_frame_ref(p,&ref[block]);	
		}
        for(block=0;block<4;block++) 
		{
            switch(sub_type[block]) 
			{
            case B_SUB_DIRECT:
				
	            get_col_info(p,&i_col_type,&p_mv,block, &i_col_distance, &i_col_top_field);
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
					

					get_b_direct_skip_sub_mb(p,block,p_mv, i_col_distance, i_col_top_field);
				}
                break;
            case B_SUB_FWD:
                mv_pred(p, mv_scan[block], mv_scan[block]-3,
                        MV_PRED_MEDIAN, BLK_8X8, ref[block]+i_ref_offset);
                break;
            case B_SUB_SYM:
                mv_pred(p, mv_scan[block], mv_scan[block]-3,
                        MV_PRED_MEDIAN, BLK_8X8, ref[block]+i_ref_offset);
                mv_pred_sym(p, &p->mv[mv_scan[block]], BLK_8X8,ref[block]);
                break;
            }
        }
        for(block=0;block<4;block++)
		{
            if(sub_type[block] == B_SUB_BWD)
			{
                mv_pred(p, mv_scan[block]+MV_BWD_OFFS,
                        mv_scan[block]+MV_BWD_OFFS-3,
                        MV_PRED_MEDIAN, BLK_8X8, ref[block]);
			}
        }
		
		
        break;
    default:
//        assert((mb_type > B_SYM_16X16) && (mb_type < B_8X8));
        flags = partition_flags[i_mb_type];
        if(i_mb_type & 1) 
		{ /* 16x8 macroblock types */

			
			get_b_frame_ref(p,&ref[0]);
			get_b_frame_ref(p,&ref[1]);
            if(flags & FWD0)
                mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[kk++]+i_ref_offset);
            if(flags & SYM0)
                mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X8,ref[kk-1]);
            if(flags & FWD1)
                mv_pred(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[kk++]+i_ref_offset);
            if(flags & SYM1)
                mv_pred_sym(p, &p->mv[MV_FWD_X2], BLK_16X8,ref[kk-1]);
            if(flags & BWD0)
                mv_pred(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_TOP,  BLK_16X8, ref[kk++]);
            if(flags & BWD1)
                mv_pred(p, MV_BWD_X2, MV_BWD_A1, MV_PRED_LEFT, BLK_16X8, ref[kk++]);
        } 
		else 
		{          /* 8x16 macroblock types */
			get_b_frame_ref(p,&ref[0]);
			get_b_frame_ref(p,&ref[1]);
            if(flags & FWD0)
                mv_pred(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16, ref[kk++]+i_ref_offset);
            if(flags & SYM0)
                mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_8X16,ref[kk-1]);
            if(flags & FWD1)
                mv_pred(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[kk++]+i_ref_offset);
            if(flags & SYM1)
                mv_pred_sym(p, &p->mv[MV_FWD_X1], BLK_8X16,ref[kk-1]);
            if(flags & BWD0)
                mv_pred(p, MV_BWD_X0, MV_BWD_B3, MV_PRED_LEFT, BLK_8X16, ref[kk++]);
            if(flags & BWD1)
                mv_pred(p, MV_BWD_X1, MV_BWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[kk++]);
        }
    }

	
	
	int weighting_prediction = 0;
	if(p->sh.b_mb_weighting_flag && i_mb_type != I_8X8  && i_mb_type != B_SKIP)
	{
		weighting_prediction = xavs_bitstream_get_bit1(&p->s);
	}

	if(weighting_prediction == 0)
	{
		inter_pred(p, i_mb_type);
	}
	else
	{
		inter_pred_w(p, i_mb_type , 1);
	}

	
    

	
    if(i_mb_type != B_SKIP)
	{
        get_residual_inter(p);
		
	}
	
	

	
	
    xavs_mb_filter(p,i_mb_type);
	return xavs_mb_next(p);
}


static inline void get_p_frame_ref(xavs_decoder *p, int *p_ref)
{
	if(p->ph.b_picture_reference_flag )
	{
		*p_ref=0;
	}
	else
	{
		if(p->ph.b_picture_structure==0&&p->ph.i_picture_coding_type==XAVS_P_PICTURE)
		{
			*p_ref=xavs_bitstream_get_bits(&p->s,2);
		}
		else
		{
			*p_ref=xavs_bitstream_get_bit1(&p->s);	
		}
		
	}

}


static int get_mb_p(xavs_decoder *p,uint32_t i_mb_type)
{
/*     0:    D3  B2  B3  C2
       4:    A1  X0  X1   -
       8:    A3  X2  X3   -
*/
	int i_offset;
	int ref[4];    
	xavs_mb_init(p);
	
	switch(i_mb_type) 
	{
    case P_SKIP:
		//肯定是P帧
		//if()
		mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_PSKIP, BLK_16X16, 
			(p->ph.b_pb_field_enhanced_flag && p->ph.i_picture_coding_type == 1 &&p->ph.b_picture_structure == 0) ? 1 : 0);
        break;
    case P_16X16:
		get_p_frame_ref(p,&ref[0]);	
        mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN,   BLK_16X16,ref[0]);
        break;
    case P_16X8:
		get_p_frame_ref(p,&ref[0]);
		get_p_frame_ref(p,&ref[2]);
        mv_pred(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,      BLK_16X8, ref[0]);
		//对于X2,C块还没有解码所以用D块代替，这里的D块就是A1
        mv_pred(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT,     BLK_16X8, ref[2]);
        break;
    case P_8X16:
		get_p_frame_ref(p,&ref[0]);
		get_p_frame_ref(p,&ref[1]);
        
        mv_pred(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT,     BLK_8X16, ref[0]);
        mv_pred(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT, BLK_8X16, ref[1]);
        break;
    case P_8X8:
		get_p_frame_ref(p,&ref[0]);
		get_p_frame_ref(p,&ref[1]);
		get_p_frame_ref(p,&ref[2]);
		get_p_frame_ref(p,&ref[3]);
        mv_pred(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_MEDIAN,   BLK_8X8, ref[0]);
        mv_pred(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_MEDIAN,   BLK_8X8, ref[1]);
        mv_pred(p, MV_FWD_X2, MV_FWD_X1, MV_PRED_MEDIAN,   BLK_8X8, ref[2]);
        mv_pred(p, MV_FWD_X3, MV_FWD_X0, MV_PRED_MEDIAN,   BLK_8X8, ref[3]);
		break;
    }

	int weighting_prediction = 0;
	if(p->sh.b_mb_weighting_flag && i_mb_type != I_8X8  && i_mb_type != P_SKIP)
	{
		weighting_prediction = xavs_bitstream_get_bit1(&p->s);
	}
	if(weighting_prediction == 0)
	{
		inter_pred(p, i_mb_type);
	}
	else
	{
		inter_pred_w(p, i_mb_type, 0);
	}

	




	i_offset=(p->i_mb_y*p->i_mb_width + p->i_mb_x)*4;
	p->p_col_mv[i_offset+ 0] = p->mv[MV_FWD_X0];
    p->p_col_mv[i_offset + 1] = p->mv[MV_FWD_X1];
    p->p_col_mv[i_offset + 2] = p->mv[MV_FWD_X2];
    p->p_col_mv[i_offset + 3] = p->mv[MV_FWD_X3];
	if(i_mb_type != P_SKIP)
	{
		get_residual_inter(p);
	}

	

	
	
	
	

	xavs_mb_filter(p,i_mb_type);
	

	*p->p_col_type = i_mb_type;
	return xavs_mb_next(p);
}


static int get_mb_i(xavs_decoder *p,uint32_t i_cbp_code)
{
	uint8_t i_top[18];
    uint8_t *p_left = NULL;
    uint8_t *p_d;

	static const uint8_t scan3x3[4] = {4,5,7,8};
	int i,i_pred_mode_chroma;
	int i_offset=p->i_mb_x<<1;
	xavs_mb_init(p);
    
   
	for(i=0;i<4;i++) 
	{
        int i_mode_a,i_mode_b,i_pred_mode,i_rem_mode;
        int i_pos = scan3x3[i];
        i_mode_a = p->i_intra_pred_mode_y[i_pos-1];
        i_mode_b = p->i_intra_pred_mode_y[i_pos-3];
        i_pred_mode = XAVS_MIN(i_mode_a,i_mode_b);
        if(i_pred_mode == NOT_AVAIL)
		{
            i_pred_mode = INTRA_L_LP;
		}
        if(!xavs_bitstream_get_bit1(&p->s))
		{
            i_rem_mode= xavs_bitstream_get_bits(&p->s, 2);
            i_pred_mode = i_rem_mode + (i_rem_mode >= i_pred_mode);
			
        }
        p->i_intra_pred_mode_y[i_pos] = i_pred_mode;
		
		

			
    }
	i_pred_mode_chroma = xavs_bitstream_get_ue(&p->s);
    if(i_pred_mode_chroma > 6) 
	{
        return -1;
    }
    p->i_intra_pred_mode_y[INTRA_MODE_A1] =  p->i_intra_pred_mode_y[INTRA_MODE_X1];
    p->i_intra_pred_mode_y[INTRA_MODE_A3] =  p->i_intra_pred_mode_y[INTRA_MODE_X3];
    p->p_top_intra_pred_mode_y[i_offset+0] = p->i_intra_pred_mode_y[INTRA_MODE_X2];
    p->p_top_intra_pred_mode_y[i_offset+1] = p->i_intra_pred_mode_y[INTRA_MODE_X3];

	
	


	if(!(p->i_mb_flags & A_AVAIL)) 
	{
        adapt_pred_mode(LEFT_ADAPT_INDEX_L, &p->i_intra_pred_mode_y[INTRA_MODE_X0] );
        adapt_pred_mode(LEFT_ADAPT_INDEX_L, &p->i_intra_pred_mode_y[INTRA_MODE_X2] );
        adapt_pred_mode(LEFT_ADAPT_INDEX_C, &i_pred_mode_chroma );
    }
    if(!(p->i_mb_flags & B_AVAIL)) 
	{
        adapt_pred_mode(TOP_ADAPT_INDEX_L, &p->i_intra_pred_mode_y[INTRA_MODE_X0] );
        adapt_pred_mode(TOP_ADAPT_INDEX_L, &p->i_intra_pred_mode_y[INTRA_MODE_X1] );
        adapt_pred_mode(TOP_ADAPT_INDEX_C, &i_pred_mode_chroma );
    }
	
	if(!p->b_have_pred)
        i_cbp_code = xavs_bitstream_get_ue(&p->s);
    if(i_cbp_code > 63)
	{
		//error cbp code
        return -1;
    }
    p->i_cbp =get_cbp(i_cbp_code,0);
    if(p->i_cbp && !p->b_fixed_qp)
	{
		int kkk= xavs_bitstream_get_se(&p->s);
        p->i_qp = (p->i_qp + kkk) & 63; //qp_delta
	}
	
	
    for(i=0;i<4;i++)
	{
		int i_mode=p->i_intra_pred_mode_y[scan3x3[i]];
        p_d = p->p_y + p->i_luma_offset[i];
        load_intra_pred_luma(p, i_top, &p_left, i);
		xavs_intra_pred_lum[i_mode](p_d, i_top, p_left, p->cur.i_stride[0]);        

        if(p->i_cbp & (1<<i))
		{
            get_residual_block(p,intra_2dvlc,1,p->i_qp,p_d,p->cur.i_stride[0]);
		}
		
		
    }

	load_intra_pred_chroma(p);
	xavs_intra_pred_chroma[i_pred_mode_chroma](p->p_cb, &p->p_top_border_cb[p->i_mb_x*10],
                                  p->i_left_border_cb, p->cur.i_stride[1]);
	
	
    xavs_intra_pred_chroma[i_pred_mode_chroma](p->p_cr, &p->p_top_border_cr[p->i_mb_x*10],
                                  p->i_left_border_cr, p->cur.i_stride[2]);

	
	if(p->i_cbp & (1<<4))
	{
		
		get_residual_block(p,chroma_2dvlc,0, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cb],
                              p->p_cb,p->cur.i_stride[1]);
	}
	
		
    if(p->i_cbp & (1<<5))
	{
        get_residual_block(p,chroma_2dvlc,0, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cr],
                              p->p_cr,p->cur.i_stride[2]);
	}
	
	
	
  
	xavs_mb_filter(p,I_8X8);
	

    p->mv[MV_FWD_X0] = MV_INTRA;
    copy_mvs(&p->mv[MV_FWD_X0], BLK_16X16);
    p->mv[MV_BWD_X0] = MV_INTRA;
    copy_mvs(&p->mv[MV_BWD_X0], BLK_16X16);
	// B帧不需要保存这个值，因为不用于预测
    if(p->ph.i_picture_coding_type != XAVS_B_PICTURE)
        *p->p_col_type = I_8X8;

	

	return xavs_mb_next(p);

}

int xavs_get_all_mbs(xavs_decoder *p)
{
	int i_mb_type;
	int i_skip, i_base, i_limit, i_result, i_skip_count;
	int (*get_mb_pb)(xavs_decoder *p,uint32_t i_mb_type);
	i_skip = (p->ph.i_picture_coding_type < XAVS_B_PICTURE ? P_SKIP : B_SKIP);
	i_base = i_skip + p->ph.b_skip_mode_flag;
	i_limit = p->ph.i_picture_coding_type<XAVS_B_PICTURE?P_8X8:B_8X8;
	get_mb_pb = p->ph.i_picture_coding_type<XAVS_B_PICTURE?get_mb_p:get_mb_b;
	for(;;)
	{
		if(p->b_have_pred)
		{
			if(p->ph.b_skip_mode_flag)
			{
				i_skip_count = xavs_bitstream_get_ue(&p->s);
				while(i_skip_count--) 
				{
					i_result=get_mb_pb(p,i_skip);     
					if(i_result!=0)
					{
						goto end;
					}
					p->i_mb_index++;
					if(xavs_check_index(p))
					{
						i_result=0;
						goto end;
					}
					
				}
				if(xavs_check_stream_end(p))
				{
					i_result=0;
					goto end;
				}
			}

			i_mb_type=xavs_bitstream_get_ue(&p->s);

			i_mb_type += i_base;

			if(i_mb_type>i_limit)
			{
				i_result=get_mb_i(p, i_mb_type - i_limit - 1);			

			}
			else
			{
				i_result=get_mb_pb(p,i_mb_type);
			}

			if(i_result!=0)
			{
				goto end;
			}
		}
		else
		{
			i_result=get_mb_i(p,0);
			if(i_result!=0)
			{
				goto end;
			}
		}
		p->i_mb_index++;
		if(xavs_check_stream_end(p))
		{
			i_result=0;
			goto end;
		}	
	}
end:
	
	return i_result;

}


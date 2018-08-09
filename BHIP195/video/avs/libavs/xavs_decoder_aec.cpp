

//#include "stdafx.h"
#include "xavs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_decoder.h"

#include "xavs_decoder_aec_decode.hpp"
#include "xavs_decoder_aec_read.hpp"

#include "xavs_decoder_common.hpp"
static inline int get_residual_block_aec(xavs_decoder *p, int chroma, int i_qp, uint8_t *p_dest, int i_stride) 
{
 //  static const xavs_vlc *vlc_tale[3]={intra_2dvlc,inter_2dvlc,chroma_2dvlc};

	static const int ctx_index_offset_table[ 4 ] = { 190,256,58,124}; 
	int pos = 0;
	int  level, run, i, i_max = 0;
    int level_buf[65];
    int run_buf[65];
    int dqm = dequant_mul[i_qp];
    int dqs = dequant_shift[i_qp];
    int dqa = 1 << (dqs - 1);
    const uint8_t *zigzag=p->ph.b_picture_structure==0?zigzag_field:zigzag_progressive ;
    DCTELEM *block = p->p_block;
	int ctx_index_offset = ctx_index_offset_table[(p->ph.b_picture_structure << 1) + chroma];

	
	level = aec_read_trans_coefficent_level_max_0(p, ctx_index_offset) + 1;
	
	if(aec_read_trans_coefficent_sign(p))
	{
		level_buf[0] = -level;
	}
	else
	{
		level_buf[0] = level;
	}
	run = aec_read_trans_coefficent_run(p, i_max, ctx_index_offset, level) + 1;
	run_buf[0] = run ;
	pos += run;
	i_max = level ;
	
	for(i = 1;i < 65;i++) 
	{
		level = aec_read_trans_coefficent_level_max_other(p, i_max, ctx_index_offset, pos > 63 ? 63 : pos);
		if(level != 0)
		{
			
			if(aec_read_trans_coefficent_sign(p))
			{
				level_buf[i] = -level;
			}
			else
			{
				level_buf[i] = level;
			}
			run = aec_read_trans_coefficent_run(p, i_max, ctx_index_offset, level) + 1;
			run_buf[i] = run;
			pos += run;
			if(level > i_max  )
			{
				i_max = level;
			}
		}
		else
		{
			break;
		}
    }

	pos = -1;
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

static inline void mv_pred_aec(xavs_decoder *p, uint8_t nP,uint8_t nC,
                    enum xavs_mv_pred mode, enum xavs_block_size size, int ref, xavs_mvd *mvd, int dir) 
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
		
		if(p->ph.i_picture_coding_type == XAVS_B_PICTURE)
		{
			
			mvd->x = aec_read_mv(p, mvd, mvd->enable && (mvd->dir & dir), 36, abs(mvd->x));
			
			mvd->y = aec_read_mv(p, mvd, mvd->enable && (mvd->dir & dir), 42, abs(mvd->y));
		}
		else
		{
			mvd->x = aec_read_mv(p, mvd,  mvd->enable, 36, abs(mvd->x));
			mvd->y = aec_read_mv(p, mvd,  mvd->enable, 42, abs(mvd->y));
		}
		mvP->x += mvd->x;
        mvP->y += mvd->y;

		mvd->dir = dir;
		mvd->enable = 1;
       
    }
	
    copy_mvs(mvP,size);
}

static inline int get_residual_inter_aec(xavs_decoder *p) 
{
    int block;
	
    /* get coded block pattern */
    p->i_cbp = aec_read_cbp(p);
    if(p->i_cbp > 63)
	{
        return -1;
    }
	
    /* get quantizer */
    if(p->i_cbp && !p->b_fixed_qp)
	{
        p->i_qp = (p->i_qp + aec_read_qp_delta(p)) & 63;
	}
	else
	{
		p->i_last_qp_delta = 0;
	}

    for(block=0;block<4;block++)
	{
        if(p->i_cbp & (1<<block))
		{
            get_residual_block_aec(p, 0,p->i_qp,
                                  p->p_y + p->i_luma_offset[block], p->cur.i_stride[0]);
		}
		
		
	}
	if(p->i_cbp & (1<<4))
	{
        get_residual_block_aec(p, 1, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cb],
                              p->p_cb,p->cur.i_stride[1]);
	}
	
    if(p->i_cbp & (1<<5))
	{
        get_residual_block_aec(p, 1, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cr],
                              p->p_cr,p->cur.i_stride[2]);
	}
	



    return 0;
}



static inline void get_b_frame_ref_aec(xavs_decoder *p, int *p_ref, xavs_mvref *mvref, xavs_mvref *fmvref, xavs_block_size bsize, int dir)
{
	fmvref->type = 1;
	fmvref->dir = dir;
	if(p->ph.b_picture_structure==0&&p->ph.b_picture_reference_flag==0 )
	{
		*p_ref = aec_read_ref_b(p, fmvref);
	}
	else
	{
		*p_ref = 0;
		
	}
	mvref->type = 1;
	mvref->dir = dir;
	mvref->ref = *p_ref;
	copy_mvref(mvref, bsize);


	
	if(mvref != fmvref)
	{
		copy_mvref_dir_type(fmvref, bsize);
	}
	
}


static inline void get_p_frame_ref_aec(xavs_decoder *p, int *p_ref, xavs_mvref *mvref, xavs_block_size bsize)
{
	if(p->ph.b_picture_reference_flag )
	{
		*p_ref=0;
	}
	else
	{
		*p_ref = aec_read_ref_p(p, mvref);	
	}

	mvref->type = 1;
	mvref->ref = * p_ref;
	copy_mvref( mvref, bsize);
	 

}

static int get_mb_b_aec(xavs_decoder *p,uint32_t i_mb_type)
{   
	


	int kk=0; 
	xavs_vector mv[24];
    int block;
	int dir0, dir1;
    enum xavs_mb_sub_type sub_type[4];
    int flags;
	int ref[4];    
	int i_ref_offset=p->ph.b_picture_structure==0?2:1;
	uint8_t i_col_type;
	int     i_col_distance, i_col_top_field;
	xavs_vector *p_mv;
	
	p->i_mb_type_a = p->p_top_mb_type[p->i_mb_x] = i_mb_type;

	p->mv[MV_FWD_X0] = MV_REF_DIR;
    copy_mvs(&p->mv[MV_FWD_X0], BLK_16X16);
    p->mv[MV_BWD_X0] = MV_REF_DIR;
    copy_mvs(&p->mv[MV_BWD_X0], BLK_16X16);

	p->mvref[MV_FWD_X0] = MVREF_NOT_AVAIL;
    copy_mvref(&p->mvref[MV_FWD_X0], BLK_16X16);
    p->mvref[MV_BWD_X0] = MVREF_NOT_AVAIL;
    copy_mvref(&p->mvref[MV_BWD_X0], BLK_16X16);


	
    switch(i_mb_type) 
	{
    case B_SKIP:
    case B_DIRECT:
		get_b_direct_skip_mb(p);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;
		p->mvref[MV_FWD_X0].type = 0;
		p->mvref[MV_FWD_X1].type = 0;
		p->mvref[MV_FWD_X2].type = 0;
		p->mvref[MV_FWD_X3].type = 0;

        break;
    case B_FWD_16X16:
		
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0],&p->mvref[MV_FWD_X0], BLK_16X16,1);
			
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN, BLK_16X16, ref[0]+i_ref_offset, p->mvd_a, 1);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;

        break;
    case B_SYM_16X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0],&p->mvref[MV_FWD_X0], BLK_16X16,3);
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN, BLK_16X16, ref[0]+i_ref_offset, p->mvd_a, 3);
        mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X16,ref[0]);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;

		
		
        break;
    case B_BWD_16X16:
		
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_BWD_X0], &p->mvref[MV_FWD_X0],BLK_16X16,2);
	
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_MEDIAN, BLK_16X16,  ref[0], p->mvd_a + 2,  2);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
		p->mvd_a[3] = p->mvd_a[2];
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
		{
            sub_type[block] = (xavs_mb_sub_type)aec_read_mb_part_type(p);
		}
	
		if(sub_type[0] == B_SUB_BWD)
		{
			p->mvd_a[0].enable = 0;
		}

		if(sub_type[2] == B_SUB_BWD)
		{
			p->mvd_a[1].enable = 0;
		}
		
		//先前向，再后向
		for(block=0;block<4;block++)
		{
			switch(sub_type[block])
			{
			case B_SUB_DIRECT:
				ref[block] = 0;
				p->mvref[mv_scan[block]].type = 0;
				break;
			case B_SUB_BWD:
				ref[block] = 0;
				break;

			case B_SUB_FWD:
				get_b_frame_ref_aec(p,&ref[block],  &p->mvref[mv_scan[block]], &p->mvref[mv_scan[block]], BLK_8X8,1);	
				break;
			case B_SUB_SYM:
				get_b_frame_ref_aec(p,&ref[block],  &p->mvref[mv_scan[block]], &p->mvref[mv_scan[block]], BLK_8X8,3);	
				break;

			}
			
		}

	
		for(block=0;block<4;block++)
		{
			if(sub_type[block]==B_SUB_BWD)
			{
				get_b_frame_ref_aec(p,&ref[block],&p->mvref[mv_scan[block] + MV_BWD_OFFS],&p->mvref[mv_scan[block]],BLK_8X8, 2);	
			}
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
				

				p->mvd_a[block >> 1].enable = 0;
				
                break;
            case B_SUB_FWD:
                mv_pred_aec(p, mv_scan[block], mv_scan[block]-3,
                        MV_PRED_MEDIAN, BLK_8X8, ref[block]+i_ref_offset, &p->mvd_a[block >> 1], 1);
				
                break;
            case B_SUB_SYM:
                mv_pred_aec(p, mv_scan[block], mv_scan[block]-3,
                        MV_PRED_MEDIAN, BLK_8X8, ref[block]+i_ref_offset,&p->mvd_a[block >> 1], 3);
                mv_pred_sym(p, &p->mv[mv_scan[block]], BLK_8X8,ref[block]);
                break;
			default:
				p->mvd_a[block >> 1].enable = 0;
				break;
            }
        }
		
		for(block=0;block<4;block++)
		{
			if(sub_type[block] == B_SUB_BWD)
			{
				mv_pred_aec(p, mv_scan[block]+MV_BWD_OFFS,
					mv_scan[block]+MV_BWD_OFFS-3,
					MV_PRED_MEDIAN, BLK_8X8, ref[block],&p->mvd_a[(block >> 1) + 2], 2);
			}
			else
			{
				p->mvd_a[(block >> 1) + 2].enable = 0;

			}
		}


		break;
	case B_FWD_FWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,1);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 0, 1);
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1] + i_ref_offset, p->mvd_a + 1, 1);
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;
		break;
	case B_BWD_BWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,2);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,2);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_TOP,  BLK_16X8, ref[kk++], p->mvd_a + 2, 2);
		mv_pred_aec(p, MV_BWD_X2, MV_BWD_A1, MV_PRED_LEFT, BLK_16X8, ref[kk++], p->mvd_a + 3, 2);
		
		break;
	case B_FWD_BWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,2);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 0, 1);
		p->mvd_a[1].enable = 0;
		p->mvd_a[2].enable = 0;
		mv_pred_aec(p, MV_BWD_X2, MV_BWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1] + 0,            p->mvd_a + 3, 2);
		
		break;
	case B_BWD_FWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,2);	
		p->mvd_a[0].enable = 0;
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 1, 1);	
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_TOP,  BLK_16X8, ref[1] + 0,            p->mvd_a + 2, 2);
		p->mvd_a[3].enable = 0;
		break;
	case B_FWD_SYM_16X8:
		
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,3);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 0, 1);
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1] + i_ref_offset, p->mvd_a + 1, 3);
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;	
		mv_pred_sym(p, &p->mv[MV_FWD_X2], BLK_16X8,ref[1]);
	
		break;
	case B_BWD_SYM_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,2);
		p->mvd_a[0].enable = 0;
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 1, 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X2], BLK_16X8,ref[0]);
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_TOP,  BLK_16X8, ref[1] + 0,            p->mvd_a + 2, 2);
		p->mvd_a[3].enable = 0;
		break;
	case B_SYM_FWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,1);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0]+i_ref_offset, p->mvd_a + 0, 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X8,ref[0]);
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1]+i_ref_offset, p->mvd_a + 1, 1);
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;	
		break;
	case B_SYM_BWD_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,2);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0] + i_ref_offset, p->mvd_a + 0, 3);
		p->mvd_a[1].enable = 0;
		p->mvd_a[2].enable = 0;
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X8,ref[0]);
		mv_pred_aec(p, MV_BWD_X2, MV_BWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1] + 0,            p->mvd_a + 3, 2);
		break;
	case B_SYM_SYM_16X8:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,3);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[0]+i_ref_offset, p->mvd_a + 0, 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X8,ref[0]);
		mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[1]+i_ref_offset, p->mvd_a + 1, 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X2], BLK_16X8,ref[1]);		
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;
		break;
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
	case B_FWD_FWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,1);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0] + i_ref_offset, p->mvd_a , 1);
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1] + i_ref_offset, p->mvd_a , 1);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;	
		break;
	case B_BWD_BWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,2);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,2);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;	
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0], p->mvd_a + 2, 2);
		mv_pred_aec(p, MV_BWD_X1, MV_BWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1], p->mvd_a + 2, 2);
		p->mvd_a[3] = p->mvd_a[2];	
		break;
	case B_FWD_BWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,2);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0] + i_ref_offset, p->mvd_a + 0, 1);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;
		mv_pred_aec(p, MV_BWD_X1, MV_BWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1] + 0 ,           p->mvd_a + 2, 2);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
		p->mvd_a[3] = p->mvd_a[2];
		break;
	case B_BWD_FWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,2);

		p->mvd_a[0].enable = 0;
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[0] + i_ref_offset, p->mvd_a + 0 , 1);
		p->mvd_a[1] = p->mvd_a[0];
		
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[1] + 0,            p->mvd_a + 2,  2);
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;
		

		break;
	case B_FWD_SYM_8X16:
		
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,1);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,3);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0]+i_ref_offset, p->mvd_a , 1);
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1]+i_ref_offset, p->mvd_a , 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X1], BLK_8X16,ref[1]);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;		
		break;
	case B_BWD_SYM_8X16:
		
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,2);
		p->mvd_a[0].enable = 0;
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[0] + i_ref_offset, p->mvd_a + 0, 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X1], BLK_8X16,ref[0]);
		mv_pred_aec(p, MV_BWD_X0, MV_BWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[1] + 0 ,           p->mvd_a + 2, 2);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;	
		break;
	case B_SYM_FWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,1);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0]+i_ref_offset, p->mvd_a , 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_8X16,ref[0]);
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1]+i_ref_offset, p->mvd_a , 1);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;		
		break;
	case B_SYM_BWD_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_BWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,2);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16,    ref[0] + i_ref_offset, p->mvd_a + 0,    3);
		p->mvd_a[1] = p->mvd_a[0];

		p->mvd_a[2].enable = 0;
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_8X16,ref[0]);
		mv_pred_aec(p, MV_BWD_X1, MV_BWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1] + 0,            p->mvd_a + 2, 2);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
		p->mvd_a[3] = p->mvd_a[2];
		break;
	case B_SYM_SYM_8X16:
		get_b_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,3);
		get_b_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,3);
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16, ref[0]+i_ref_offset, p->mvd_a , 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_8X16,ref[0]);
		mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[1]+i_ref_offset, p->mvd_a , 3);
		mv_pred_sym(p, &p->mv[MV_FWD_X1], BLK_8X16,ref[1]);
		p->mvd_a[1] = p->mvd_a[0];
		p->mvd_a[2].enable = 0;
		p->mvd_a[3].enable = 0;			
		break;
    default:
//        assert((mb_type > B_SYM_16X16) && (mb_type < B_8X8));
		break;
		flags = partition_flags[i_mb_type];
		dir0 = 0;
		dir1 = 0;
		if(flags & FWD0)
		{
			dir0 = 1;
		}
		if((flags & BWD0) )
		{
			dir0 += 2;
		}
		if (flags & SYM0)
		{
			dir0 = 3;
		}

		if(flags & FWD1)
		{
			dir1 = 1;
		}
		if((flags & BWD1) )
		{
			dir1 += 2;
		}
		if (flags & SYM1)
		{
			dir1 = 3;
		}
		if(i_mb_type & 1) 
		{ /* 16x8 macroblock types */
			
			kk = 0;
			if(flags & FWD0)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,dir0);
			}
			
			if(flags & FWD1)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_FWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,dir1);
			}

			if(flags & BWD0)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_16X8,dir0);
			}
			if(flags & BWD1)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_BWD_X2] ,&p->mvref[MV_FWD_X2] ,BLK_16X8,dir1);
			}

			kk = 0;
			if(flags & FWD0)
			{
				mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,  BLK_16X8, ref[kk++]+i_ref_offset, p->mvd_a, dir0);
			}
			if(flags & SYM0)
			{
				mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_16X8,ref[kk-1]);
			}
			if(flags & FWD1)
			{
				mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT, BLK_16X8, ref[kk++]+i_ref_offset, p->mvd_a + 1, dir1);
			}
			if(flags & SYM1)
			{
				mv_pred_sym(p, &p->mv[MV_FWD_X2], BLK_16X8,ref[kk-1]);
			}
			if(flags & BWD0)
			{
				mv_pred_aec(p, MV_BWD_X0, MV_BWD_C2, MV_PRED_TOP,  BLK_16X8, ref[kk++], p->mvd_a + 2, dir0);
			}
			if(flags & BWD1)
			{
				mv_pred_aec(p, MV_BWD_X2, MV_BWD_A1, MV_PRED_LEFT, BLK_16X8, ref[kk++], p->mvd_a + 3, dir1);
			}
			
		} 
		else 
		{          /* 8x16 macroblock types */
			//get_b_frame_ref_aec(p,&ref[0]);
			//get_b_frame_ref_aec(p,&ref[1]);
			
			kk = 0;
			if(flags & FWD0)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_FWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,dir0);
			}
			if(flags & FWD1)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_FWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,dir1);
			}
			if(flags & BWD0)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_BWD_X0] ,&p->mvref[MV_FWD_X0] ,BLK_8X16,dir0);
			}
			if(flags & BWD1)
			{
				get_b_frame_ref_aec(p,&ref[kk++], &p->mvref[MV_BWD_X1] ,&p->mvref[MV_FWD_X1] ,BLK_8X16,dir1);
			}

			kk = 0;
			if(flags & FWD0)
			{
				mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT, BLK_8X16, ref[kk++]+i_ref_offset, p->mvd_a , dir0);
			}
			if(flags & SYM0)
			{
				mv_pred_sym(p, &p->mv[MV_FWD_X0], BLK_8X16,ref[kk-1]);
			}
			if(flags & FWD1)
			{
				mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[kk++]+i_ref_offset, p->mvd_a , dir1);
			}
			if(flags & SYM1)
			{
				mv_pred_sym(p, &p->mv[MV_FWD_X1], BLK_8X16,ref[kk-1]);
			}
			if(flags & BWD0)
			{
				mv_pred_aec(p, MV_BWD_X0, MV_BWD_B3, MV_PRED_LEFT, BLK_8X16, ref[kk++], p->mvd_a + 2, dir0);
			}
			if(flags & BWD1)
			{
				mv_pred_aec(p, MV_BWD_X1, MV_BWD_C2, MV_PRED_TOPRIGHT,BLK_8X16, ref[kk++], p->mvd_a + 2, dir1);
			}
			
			p->mvd_a[1] = p->mvd_a[0]; 
			p->mvd_a[3] = p->mvd_a[2];
		}

    }

	
	
	int weighting_prediction = 0;
	if(p->sh.b_mb_weighting_flag && i_mb_type != I_8X8  && i_mb_type != B_SKIP)
	{
		weighting_prediction = aec_read_weighting_prediction(p);
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
        get_residual_inter_aec(p);
		
	}
	else
	{
		p->i_last_qp_delta = 0;
		p->i_cbp = 0;
	}
	

	p->i_intra_pred_mode_chroma_a = 0;
	p->p_top_intra_pred_mode_chroma[p->i_mb_x] = 0;
	
	
    xavs_mb_filter(p,i_mb_type);
	return xavs_mb_next(p);
}


static int get_mb_p_aec(xavs_decoder *p,uint32_t i_mb_type)
{
/*     0:    D3  B2  B3  C2
       4:    A1  X0  X1   -
       8:    A3  X2  X3   -
*/
	
	
	int i_offset;
	int ref[4];    
	p->i_mb_type_a = p->p_top_mb_type[p->i_mb_x] = i_mb_type;
	
	switch(i_mb_type) 
	{
    case P_SKIP:
		//肯定是P帧
		//if()
		mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_PSKIP, BLK_16X16, 
			(p->ph.b_pb_field_enhanced_flag && p->ph.i_picture_coding_type == 1 &&p->ph.b_picture_structure == 0) ? 1 : 0,
			p->mvd_a, 1);
		p->mvd_a[0].enable = 0;
		p->mvd_a[1].enable = 0;
	
		p->mvref[MV_FWD_X0].type = 0;
		p->mvref[MV_FWD_X1].type = 0;
		p->mvref[MV_FWD_X2].type = 0;
		p->mvref[MV_FWD_X3].type = 0;

        break;
    case P_16X16:
		get_p_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0], BLK_16X16);
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_MEDIAN,   BLK_16X16,ref[0], p->mvd_a, 1);
		p->mvd_a[1] = p->mvd_a[0];
        break;
    case P_16X8:
		get_p_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0], BLK_16X8);
		
		get_p_frame_ref_aec(p,&ref[2], &p->mvref[MV_FWD_X2], BLK_16X8);
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_C2, MV_PRED_TOP,      BLK_16X8, ref[0], p->mvd_a, 1);
		//对于X2,C块还没有解码所以用D块代替，这里的D块就是A1
        mv_pred_aec(p, MV_FWD_X2, MV_FWD_A1, MV_PRED_LEFT,     BLK_16X8, ref[2], p->mvd_a + 1, 1);
		
		
        break;
    case P_8X16:
	
		get_p_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0], BLK_8X16);
		
		get_p_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1], BLK_8X16);
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_LEFT,     BLK_8X16, ref[0], p->mvd_a,  1);
        mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_TOPRIGHT, BLK_8X16, ref[1] ,p->mvd_a,  1);
		p->mvd_a[1] = p->mvd_a[0];
        break;
    case P_8X8:
		get_p_frame_ref_aec(p,&ref[0], &p->mvref[MV_FWD_X0], BLK_8X8);
		get_p_frame_ref_aec(p,&ref[1], &p->mvref[MV_FWD_X1], BLK_8X8);
		get_p_frame_ref_aec(p,&ref[2], &p->mvref[MV_FWD_X2], BLK_8X8);
		get_p_frame_ref_aec(p,&ref[3], &p->mvref[MV_FWD_X3], BLK_8X8);
        mv_pred_aec(p, MV_FWD_X0, MV_FWD_B3, MV_PRED_MEDIAN,   BLK_8X8, ref[0], p->mvd_a ,    1);
		
        mv_pred_aec(p, MV_FWD_X1, MV_FWD_C2, MV_PRED_MEDIAN,   BLK_8X8, ref[1], p->mvd_a,     1);

        mv_pred_aec(p, MV_FWD_X2, MV_FWD_X1, MV_PRED_MEDIAN,   BLK_8X8, ref[2], p->mvd_a + 1, 1);
		
        mv_pred_aec(p, MV_FWD_X3, MV_FWD_X0, MV_PRED_MEDIAN,   BLK_8X8, ref[3], p->mvd_a + 1, 1) ;
		break;
    }

	int weighting_prediction = 0;
	if(p->sh.b_mb_weighting_flag && i_mb_type != I_8X8  && i_mb_type != P_SKIP)
	{
		weighting_prediction = aec_read_weighting_prediction(p);
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
		get_residual_inter_aec(p);
	}
	else
	{
		p->i_last_qp_delta = 0;
		p->i_cbp = 0;
	}
	
	p->i_intra_pred_mode_chroma_a = 0;
	p->p_top_intra_pred_mode_chroma[p->i_mb_x] = 0;


	

	xavs_mb_filter(p,i_mb_type);
	*p->p_col_type = i_mb_type;
	return xavs_mb_next(p);
}

static int get_mb_i_aec(xavs_decoder *p,uint32_t i_cbp_code)
{
	
	
	uint8_t *p_d;
	uint8_t i_top[18];
    uint8_t *p_left = NULL;
	int i,i_pred_mode_chroma;
	int i_offset = p->i_mb_x << 1;
	static const uint8_t scan3x3[4] = {4,5,7,8};

		
	p->i_mb_type_a = p->p_top_mb_type[p->i_mb_x] = I_8X8;
	

	p->mvd_a[0].enable = 0;
	p->mvd_a[1].enable = 0;
	p->mvd_a[2].enable = 0;
	p->mvd_a[3].enable = 0;
	for(i = 0; i < 4; i++) 
	{
        int i_mode_a,i_mode_b,i_pred_mode,i_intra_luma_pred_mode;
        int i_pos = scan3x3[i];
        i_mode_a = p->i_intra_pred_mode_y[i_pos - 1];
        i_mode_b = p->i_intra_pred_mode_y[i_pos - 3];
        i_pred_mode = XAVS_MIN(i_mode_a,i_mode_b);
        if(i_pred_mode == NOT_AVAIL)
		{
            i_pred_mode = INTRA_L_LP;
		}
		i_intra_luma_pred_mode  = aec_read_intra_luma_pred_mode(p);
		if(i_intra_luma_pred_mode == 0)
		{
			p->i_intra_pred_mode_y[i_pos] = i_pred_mode;
		}
		else 
		{
			if(i_intra_luma_pred_mode == 4)
			{
				i_intra_luma_pred_mode = 0;
			}
			p->i_intra_pred_mode_y[i_pos] = i_intra_luma_pred_mode + (i_intra_luma_pred_mode >= i_pred_mode);
			
		}
    }

	
	
	i_pred_mode_chroma = aec_read_intra_chroma_pred_mode(p);
    if(i_pred_mode_chroma > 6) 
	{
        return -1;
    }

	
	
    p->i_intra_pred_mode_y[INTRA_MODE_A1] =  p->i_intra_pred_mode_y[INTRA_MODE_X1];
    p->i_intra_pred_mode_y[INTRA_MODE_A3] =  p->i_intra_pred_mode_y[INTRA_MODE_X3];
    p->p_top_intra_pred_mode_y[i_offset+0] = p->i_intra_pred_mode_y[INTRA_MODE_X2];
    p->p_top_intra_pred_mode_y[i_offset+1] = p->i_intra_pred_mode_y[INTRA_MODE_X3];

	p->i_intra_pred_mode_chroma_a = i_pred_mode_chroma;
	p->p_top_intra_pred_mode_chroma[p->i_mb_x] = i_pred_mode_chroma;
	


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
	
	
    p->i_cbp = aec_read_cbp(p);
    if(p->i_cbp > 63)
	{
		//error cbp code
        return -1;
    }
	
    if(p->i_cbp && !p->b_fixed_qp)
	{
        p->i_qp = (p->i_qp + aec_read_qp_delta(p)) & 63; //qp_delta
	}
	else
	{
		p->i_last_qp_delta = 0;
	}
	
    for(i=0;i<4;i++)
	{
		int i_mode=p->i_intra_pred_mode_y[scan3x3[i]];
        p_d = p->p_y + p->i_luma_offset[i];
        load_intra_pred_luma(p, i_top, &p_left, i);
		xavs_intra_pred_lum[i_mode](p_d, i_top, p_left, p->cur.i_stride[0]);        

        if(p->i_cbp & (1<<i))
		{
            get_residual_block_aec(p,0, p->i_qp,p_d, p->cur.i_stride[0]);
		}
		
		
    }

	load_intra_pred_chroma(p);
	xavs_intra_pred_chroma[i_pred_mode_chroma](p->p_cb, &p->p_top_border_cb[p->i_mb_x*10],
                                  p->i_left_border_cb, p->cur.i_stride[1]);
	
	
    xavs_intra_pred_chroma[i_pred_mode_chroma](p->p_cr, &p->p_top_border_cr[p->i_mb_x*10],
                                  p->i_left_border_cr, p->cur.i_stride[2]);


	
	if(p->i_cbp & (1<<4))
	{
		
		get_residual_block_aec(p ,1, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cb],
                              p->p_cb,p->cur.i_stride[1]);
	}
	
	

	
		
    if(p->i_cbp & (1<<5))
	{
        get_residual_block_aec(p, 1, chroma_qp[p->i_qp + p->ph.i_chroma_quant_param_delta_cr],
                              p->p_cr,p->cur.i_stride[2]);
	}
	
	
	
	xavs_mb_filter(p,I_8X8);
	



    p->mv[MV_FWD_X0] = MV_INTRA;
    copy_mvs(&p->mv[MV_FWD_X0], BLK_16X16);
    p->mv[MV_BWD_X0] = MV_INTRA;
    copy_mvs(&p->mv[MV_BWD_X0], BLK_16X16);

	 p->mvref[MV_FWD_X0] = MVREF_NOT_AVAIL;
    copy_mvref(&p->mvref[MV_FWD_X0], BLK_16X16);
    p->mvref[MV_BWD_X0] = MVREF_NOT_AVAIL;
    copy_mvref(&p->mvref[MV_BWD_X0], BLK_16X16);
	// B帧不需要保存这个值，因为不用于预测
    if(p->ph.i_picture_coding_type != XAVS_B_PICTURE)
        *p->p_col_type = I_8X8;

	

	return xavs_mb_next(p);

}

int xavs_aec_decoder_get_all_mbs(xavs_decoder *p)
{
	xavs_aec_decoder_init(p);

	int i_mb_type;
	int i_skip, i_limit, i_result, i_skip_count, i_base;
	i_skip = (p->ph.i_picture_coding_type < XAVS_B_PICTURE ? P_SKIP : B_SKIP);
	i_base = i_skip + (p->ph.i_picture_coding_type < XAVS_B_PICTURE ? 0: p->ph.b_skip_mode_flag);
	i_limit = p->ph.i_picture_coding_type<XAVS_B_PICTURE?P_8X8:B_8X8;
	for(;;)
	{
		if(p->b_have_pred)
		{
			if(p->ph.b_skip_mode_flag)
			{
				i_skip_count = aec_read_skip_run(p);
				if(i_skip_count != 0)
				{
					aec_read_stuffing_bit(p);
				}

				while(i_skip_count--) 
				{
					xavs_mb_init(p);
					if(p->ph.i_picture_coding_type<XAVS_B_PICTURE)
					{
						i_result = get_mb_p_aec(p,i_skip);     
					}
					else
					{
						i_result=get_mb_b_aec(p,i_skip);     
					}
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

			
			xavs_mb_init(p);
			if(p->ph.i_picture_coding_type<XAVS_B_PICTURE)
			{
				
				i_mb_type = aec_read_mb_type_p(p) ;
				if(i_mb_type != 0 )
				{
					i_mb_type += i_base;
					i_result = get_mb_p_aec(p, i_mb_type);
				}
				else
				{
					i_result = get_mb_i_aec(p, i_mb_type);
				}
				
			}
			else
			{
				
				i_mb_type = aec_read_mb_type_b(p);
				i_mb_type += i_base;
				if(i_mb_type >= 30)
				{
					i_result=get_mb_i_aec(p,0);
				}
				else
				{

					i_result = get_mb_b_aec(p, i_mb_type);

				}
			
				
			}
			if(i_result!=0)
			{
				goto end;
			}
			
		}
		else
		{
			xavs_mb_init(p);
			i_result=get_mb_i_aec(p,0);
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
		aec_read_stuffing_bit(p);
	}
end:
	
	return i_result;
	return 0;
}


void xavs_aec_decoder_destroy(xavs_decoder *p)
{
	if(p->aec_decoder == NULL)
	{
		return ;
	}
	 XAVS_SAFE_FREE(p->aec_decoder->ctxes);
	 XAVS_SAFE_FREE(p->aec_decoder);
}


int xavs_aec_decoder_init(xavs_decoder *p)
{
	if(p->aec_decoder == NULL)
	{
		p->aec_decoder = (xavs_aec_decoder *)xavs_malloc(sizeof(*p->aec_decoder));
		if(p->aec_decoder == NULL)
		{
			return -1;
		}
		p->aec_decoder->ctx_count = 323;
		p->aec_decoder->ctxes = (xavs_aec_ctx *)xavs_malloc(sizeof(xavs_aec_ctx) * p->aec_decoder->ctx_count);
		if(p->aec_decoder->ctxes == NULL)
		{
			return -1;
		}
		p->aec_decoder->stream = &p->s;
	}
	
	
	p->i_intra_pred_mode_chroma_a = 0;
	memset(p->p_top_intra_pred_mode_chroma, 0 , sizeof(p->p_top_intra_pred_mode_chroma[0]) * p->i_mb_width);
	for(int i = 0; i < p->aec_decoder->ctx_count; i++)
	{
		p->aec_decoder->ctxes[i].mps = 0;
		p->aec_decoder->ctxes[i].cycno = 0;

		p->aec_decoder->ctxes[i].lgPmps = 1023;
	}

	int clear = (p->s.i_left + p->s.i_race_count * 2) % 8;
	if(clear != 0)
	{
		xavs_bitstream_clear_bits(&p->s, clear);
	}
	p->aec_decoder->rS1 = 0;
	p->aec_decoder->rT1 = 0xFF;
	p->aec_decoder->valueS = 0;
	p->aec_decoder->valueT = xavs_bitstream_get_bits(&p->s, 9);

	while( !((p->aec_decoder->valueT >> 8) & 0x01))
	{
		p->aec_decoder->valueT = (p->aec_decoder->valueT << 1) | xavs_bitstream_get_bit1_fast(&p->s);
		p->aec_decoder->valueS++;
	}

	p->aec_decoder->valueT = p->aec_decoder->valueT&0xFF;


	return 0;
}
//如果后向参考图像为帧，则其分解为场后各场运动矢量参考的前向参考索引与帧参考索引的关系如下
//first_field_ref_index = frame_ref_index * 2
//second_field_ref_index = frame_ref_index * 2 + 1

//如果后向参考图像为场，则其合并为帧后运动矢量的前向参考索引与场参考索引的关系如下
//frame_ref_index = first_field_ref_index / 2

#ifdef XAVS_INIT_REF_DISTANCE
static inline void init_ref_distatnce(xavs_decoder *p, uint32_t b_next_field)
{
	
	if(p->ph.i_picture_coding_type == XAVS_B_PICTURE)
	{
		return ;
	}
	if(p->vsh.b_progressive_sequence)
	{//逐行序列，只有P帧才有前向参考帧
		if(p->ph.i_picture_coding_type == XAVS_I_PICTURE)
		{
			return ;
		}
		if(p->p_save[1])
		{
			p->p_save[0]->i_frame_ref_distance[0]  = p->p_save[1]->i_distance_index;	
		}
		if(p->p_save[2])
		{
			p->p_save[0]->i_frame_ref_distance[1]  = p->p_save[2]->i_distance_index;

		}
		return ;
	}

	if(p->ph.i_picture_coding_type == XAVS_I_PICTURE)
	{
		
		p->p_save[0]->i_next_field_ref_distance[0]  = p->p_save[0]->i_distance_index ;	
		p->p_save[0]->i_first_field_ref_distance[0] = p->p_save[0]->i_distance_index ;	
		p->p_save[0]->i_frame_ref_distance[0]       = p->p_save[0]->i_distance_index ;
	}
	else
	{ // P Image

		if(p->p_save[1])
		{
			p->p_save[0]->i_frame_ref_distance[0]       = p->p_save[1]->i_distance_index ;
		}
		if(p->p_save[2])
		{
			p->p_save[0]->i_frame_ref_distance[1]       = p->p_save[2]->i_distance_index ;
		}


		
		if(p->p_save[1])
		{
			p->p_save[0]->i_first_field_ref_distance[0] = p->p_save[1]->i_distance_index + 1;	
			p->p_save[0]->i_first_field_ref_distance[1] = p->p_save[1]->i_distance_index ;		
		}
		if(p->p_save[2])
		{
			p->p_save[0]->i_first_field_ref_distance[2] = p->p_save[2]->i_distance_index + 1;	
			p->p_save[0]->i_first_field_ref_distance[3] = p->p_save[2]->i_distance_index ;	
		}


			p->p_save[0]->i_next_field_ref_distance[0] = p->p_save[0]->i_distance_index ;	
		if(p->p_save[1])
		{
			p->p_save[0]->i_next_field_ref_distance[1] = p->p_save[1]->i_distance_index + 1;	
			p->p_save[0]->i_next_field_ref_distance[2] = p->p_save[1]->i_distance_index ;	
		}
		if(p->p_save[2])
		{
			p->p_save[0]->i_next_field_ref_distance[3] = p->p_save[2]->i_distance_index + 1;	

		}
	}
}
#else

//块的距离DistatnceIndex索引定义为
//如果块的所有像素都属于所在隔行扫描图像的第二场或者都属于逐行扫描的底场，则DistanceIndex为picture_distance * 2 + 1
//否则DistanceIndex为picture_distance * 2 

//首先当前块为待解码块，与当前块指定位置(一般是左上角相同或者错场位置)对应的后向帧里对应的块，以及该对应块运动矢量指向的块
//他们分别位于当前帧或场， 对应块的帧或场，指向块的帧或者场
//iDistanceIndexRef为指向块的距离索引
//iDistanceIndexCol为对应块的距离索引
//后向参考帧中对应块的位置一般有如下两种
//(x0, y0)
//(x0, y0 - 2* top_field_first + 1)
//该样本位置对应的样本所以在的编码块，理解!!!!

//对于当前图像为帧编码
//对于后向参考图像为帧编码，则对于位置的编码块肯定属于该帧，则DistanceIndexCol应该始终为偶数
//如果后向参考图像为场，则对应位置的编码块肯定属于某个场，则DistanceIndexCol可能为奇数也可能为偶数

//对于当前图像为场编码,对于b_pb_field_enhanced_flag标志为0
//如果后向参考是帧编码，其对应位置的编码块始终在帧内，所以DistanceIndexCol应该始终为偶数
//如果后向参考是场编码，其对应位置的编码块始终在场内，则DistanceIndexCol可能为奇数也可能为偶数

//对于当前图像为场编码,b_pb_field_enhanced_flag标志为1
//如果后向参考为帧编码，其对应位置的编码块始终在帧内，所以DistanceIndexCol应该始终为偶数
//如果后向参考为场编码，始终指向第一场，所以DistanceIndexCol应该始终为偶数


//如果当前为帧编码，则其指向的块肯定不分顶场和底场
//如果当前为场编码，即使后向参考参考为帧，也可以分顶场和底场
static inline void get_col_info(xavs_decoder *p,uint8_t *p_col_type,xavs_vector **pp_mv,int block,int *i_col_distance, int *i_col_top_field)
{
	
	int i_mb_offset = 0;

	*i_col_top_field = p->p_save[1]->b_top_field_first;
	*i_col_distance = p->p_save[1]->i_distance_index;
	//当前如果为场，其对应块也同属于帧编码中的某一场，所以即使该后向参考为帧编码，也分上下场
	/*if(p->p_save[1]->b_picture_structure)
	{
		*i_col_top_field = -1;
	}*/
	if(p->ph.b_picture_structure)
	{//当前帧为帧结构
		if(p->p_save[1]->b_picture_structure)
		{//后向参考也为帧
			i_mb_offset = p->i_mb_y  * p->i_mb_width + p->i_mb_x;
			*p_col_type = p->p_col_type_base[i_mb_offset];
			*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + block];
			*i_col_top_field = -1;
		}
		else
		{//后向参考为场结构
			//由于宏块左上角y坐标始终为偶数，所以对应的始终为顶场
			
			i_mb_offset = p->i_mb_y / 2 * p->i_mb_width + p->i_mb_x;
			if(p->p_save[1]->b_top_field_first == 0)
			{//顶场为第二场
				 i_mb_offset += p->i_mb_num_half;
				 *i_col_distance = p->p_save[1]->i_distance_index + 1;
			}
			*i_col_top_field = 1;
			
			//当前8*8左上角对应的块因为场的关系，
			//比如对于p->i_mb_y为偶数 举例 p->i_mb_y = 16 当前块2块的位置为 (x, 16 * 16  + 8) 
			//在后向参考帧中对应的顶场位置为(x, 8 * 16 + 4)，所以相应的位置的block还是在当前块0块对应的位置，
			//对于p->i_mb_y为奇数 p->i_mb_y = 17 当前块2块的位置为(x,  17 * 16 + 8),
			//在后向参考帧中对应的顶场位置为(x, 8 * 16 + 12) 所以相应的位置的block还是在当前块2块对应的位置，
			*p_col_type = p->p_col_type_base[i_mb_offset];
			*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + 2 * (p->i_mb_y % 2) + (block % 2)];
		}

	}
	else
	{
		
		if(p->ph.b_pb_field_enhanced_flag)
		{

			//有当前块位置(x0,y0)
			//对于p->i_mb_index >= p->i_mb_num_half 
			//根据对应块位置(x0,  y0 - 2 * top_field_first + 1)可以知道，
			//当top_field_first为0，由于当前为第二场，所以为顶场，则有对应块(x0, y0 + 1),则为底场 为第一场
			//当top_field_first为1，由于当前为第二场，所以为底场，则有对应块(x0, y0 - 1),则为顶场 为第一场
			//所以不管第二场为底场还是顶场 对应块始终在后向参考帧的第一场，即后向参考里0参考场
			//为错场参考

			//而对于p->i_mb_index < p->i_mb_num_half 则对应块位置也为(x0, y0)，即始终为第一场，与上面一致
			//这里不考虑当前图像的b_top_field_first与后向参考图像的b_top_field_first不同,无现实意义

			if(p->i_mb_index >= p->i_mb_num_half )
			{//当前为第二场
				
				if(p->p_save[1]->b_picture_structure)
				{//后向参考为帧,

					//i_mb_offset = (p->i_mb_y - p->i_mb_height / 2) * 2 * p->i_mb_width + p->i_mb_x  ;
					i_mb_offset = p->i_mb_y * 2 * p->i_mb_width - p->i_mb_num + p->i_mb_x  ;
					if(block > 1)
					{//第二行的块，要增加一行宏块,比如在 b_top_field_first为 0，
						//当前块左上角为y = 32，此时p->i_mb_y = p->i_mb_height / 2 + 1,对应块为y = 33,即在帧编码中为p->i_mb_y = 2,
						//而第二行块为y = 48;此时依然p->i_mb_y = p->i_mb_height / 2 + 1,对应块为y = 49,即在帧编码中为p->i_mb_y = 3,
						//所以宏块偏移应该加上 p->i_mb_width
						i_mb_offset += p->i_mb_width;
					}
					//始终参考第一场，所以如果p->p_save[1]->b_top_field_first为true,第一场也为顶场
					*i_col_top_field = p->p_save[1]->b_top_field_first;
					*p_col_type = p->p_col_type_base[i_mb_offset];
					//(block % 2)表示始终为参考帧对应宏块的第一行块
					*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + (block % 2)];
					
				}
				else
				{//后向参考为场 
					//这里计算有区别
					i_mb_offset = (p->i_mb_y  - p->i_mb_height / 2 ) * p->i_mb_width + p->i_mb_x;
					*p_col_type = p->p_col_type_base[i_mb_offset];	
					*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + block];
					*i_col_top_field = p->p_save[1]->b_top_field_first;
				}
			}
			else
			{//第一场 相应位置的也应该是第一场 
				if(p->p_save[1]->b_picture_structure)
				{//后向参考为帧
					i_mb_offset = p->i_mb_y * 2 * p->i_mb_width  + p->i_mb_x;
					if(block > 1)
					{
						i_mb_offset += p->i_mb_width;
					}
					*p_col_type = p->p_col_type_base[i_mb_offset];
					*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + (block % 2)];
					*i_col_top_field = p->p_save[1]->b_top_field_first;
				}
				else
				{//后向参考为场
				
					i_mb_offset = p->i_mb_y  * p->i_mb_width + p->i_mb_x;
					*p_col_type = p->p_col_type_base[i_mb_offset];
					*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + block ];
					*i_col_top_field = p->p_save[1]->b_top_field_first;
				}	
			}

		}
		else
		{//!p->ph.b_pb_field_enhanced_flag
			//有当前块位置(x0,y0) 则对应块位置也为(x0, y0)，
			//如果后向图像为帧
			if(p->p_save[1]->b_picture_structure)
			{
				i_mb_offset = p->i_mb_y * 2 * p->i_mb_width + p->i_mb_x;
				if(block > 1)
				{
					i_mb_offset += p->i_mb_width;
				}
				if(i_mb_offset >= (int)p->i_mb_num)
				{
					i_mb_offset -= p->i_mb_num;
				}
				
				*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + (block % 2)];

				*p_col_type = p->p_col_type_base[i_mb_offset];
				*i_col_top_field = p->p_save[1]->b_top_field_first;
				
				
			}
			else
			{//后向参考为场，
			//这里不考虑当前帧的b_top_field_first与后向参考帧的b_top_field_first不同,无现实意义
				i_mb_offset = p->i_mb_index;
				*p_col_type = p->p_col_type_base[i_mb_offset];
				*pp_mv      = &p->p_col_mv[i_mb_offset * 4 + block];

				*i_col_top_field = p->ph.b_top_field_first;
				
				if(p->i_mb_index >= p->i_mb_num_half)	
				{//第二场
					*i_col_distance = p->p_save[1]->i_distance_index + 1;
				}
			}
			

		}
	}
	

}




static inline void get_b_direct_skip_sub_mb(xavs_decoder *p,int block,xavs_vector *p_mv,int col_distance,int i_col_top_field)
{
	xavs_vector temp_mv = p_mv[0];
	int delta1 = 0, deltaFw = 0, deltaBw = 0;
	uint32_t b_next_field = p->i_mb_index >= p->i_mb_num_half;
	int iDistanceIndexFw,iDistanceIndexBw,iDistanceIndexRef,iDistatnceIndexCol;
	int iDistanceIndexFw0,iDistanceIndexFw1,iDistanceIndexBw0,iDistanceIndexBw1;

	xavs_vector *p_fw_mv = &p->mv[mv_scan[block]];
	xavs_vector *p_bw_mv = &p->mv[mv_scan[block] + MV_BWD_OFFS];

	iDistatnceIndexCol = col_distance;
	if(p->ph.b_picture_structure)
	{

		if(p->p_save[1]->b_picture_structure)
		{
			iDistanceIndexRef = p->p_save[1]->i_frame_ref_distance[temp_mv.ref];
		}
		else
		{
			iDistanceIndexRef = p->p_save[1]->i_frame_ref_distance[temp_mv.ref / 2];
		}
	}
	else
	{
		if(p->ph.b_pb_field_enhanced_flag)
		{

			if(p->p_save[1]->b_picture_structure)
			{
				iDistanceIndexRef = p->p_save[1]->i_first_field_ref_distance[temp_mv.ref * 2];
			}
			else
			{
				iDistanceIndexRef = p->p_save[1]->i_first_field_ref_distance[temp_mv.ref ];
			}
		}
		else
		{
			if(p->p_save[1]->b_picture_structure)
			{
				if(b_next_field)
				{
					iDistanceIndexRef = p->p_save[1]->i_next_field_ref_distance[temp_mv.ref * 2 + 1];
				}
				else
				{
					iDistanceIndexRef = p->p_save[1]->i_next_field_ref_distance[temp_mv.ref * 2];
				}
			}
			else
			{
				if(b_next_field)
				{
					iDistanceIndexRef = p->p_save[1]->i_next_field_ref_distance[temp_mv.ref];
				}
				else
				{
					iDistanceIndexRef = p->p_save[1]->i_first_field_ref_distance[temp_mv.ref];
				}
				//iDistanceIndexRef = p->p_save[1]->i_first_field_ref_distance[temp_mv.ref];
			}
		}
	}

	
	if(p->ph.b_picture_structure)
	{
		//参考图像为缺省参考，即前向0，后向为0，其DistatnceIndex应该是偶数，因为第一场或者帧
		iDistanceIndexFw = p->p_save[2]->i_distance_index;
		iDistanceIndexBw = p->p_save[1]->i_distance_index;
		if(p->p_save[1]->b_picture_structure == 0)
		{
		   temp_mv.y *= 2;
		}
		p_fw_mv->ref = 1;
		p_bw_mv->ref = 0;
		
	}	
	else 
	{
		if(p->p_save[1]->b_picture_structure == 1)
		{	
			temp_mv.y /= 2;
		}
		iDistanceIndexFw0 = p->p_save[2]->i_distance_index + 1;
		iDistanceIndexFw1 = p->p_save[2]->i_distance_index;
		iDistanceIndexBw0 = p->p_save[1]->i_distance_index;
		iDistanceIndexBw1 = p->p_save[1]->i_distance_index + 1;
		
		
		if(iDistanceIndexRef == iDistanceIndexFw0)
		{
			iDistanceIndexFw = iDistanceIndexFw0;
			p_fw_mv->ref = 2;
		}
		else
		{
			iDistanceIndexFw = iDistanceIndexFw1;
			p_fw_mv->ref = 3;
		}


		if(b_next_field == 0)
		{
			iDistanceIndexBw = iDistanceIndexBw0;
			p_bw_mv->ref = 0;
		}
		else
		{
			iDistanceIndexBw = iDistanceIndexBw1;
			p_bw_mv->ref = 1;
		}
			
		
		
		if(p->ph.b_pb_field_enhanced_flag)
		{
			
			if(p->p_save[1]->b_picture_structure)
			{
				//temp_mv.ref = 2 * temp_mv.ref + b_next_field;
				temp_mv.ref = 2 * temp_mv.ref;
			}
			if(b_next_field)
			{
				if ( temp_mv.ref % 2 == 0) // mv_ref in bot field
				{
					delta1 = 2;
				}
				deltaBw = -2;

				iDistanceIndexFw = iDistanceIndexFw0;
				p_fw_mv->ref = 2;
			}
			else
			{
				if ( temp_mv.ref % 2 == 0 ) // mv_ref in bot field
				{
					delta1 = 2;
				}
				deltaFw = iDistanceIndexFw == iDistanceIndexFw0 ? 2 : 0;

			
				
			}
			iDistanceIndexBw = iDistanceIndexBw0;
			p_bw_mv->ref = 0;
		}

		iDistatnceIndexCol = iDistanceIndexBw;
		
	}
	
	

	

	
	mv_pred_direct(p,&p->mv[mv_scan[block]],&temp_mv,
			iDistatnceIndexCol, iDistanceIndexRef,iDistanceIndexFw,iDistanceIndexBw,
			delta1, deltaFw, deltaBw);
	
}

#endif

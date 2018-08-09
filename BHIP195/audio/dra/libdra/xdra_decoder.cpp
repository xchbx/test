
#include <stdio.h>
#include <memory.h>
#include  <string.h>
#include  <stdlib.h>

#include "math.h"
#include "xdra_util.h"
#include "xdra_huffman.h"
#include "xdra_qstep.h"
#include "xdra_mdct.h"
#include "xdra_win_tables.h"
#include "xdra_freq_band.h"

typedef struct tagxdra_deocder_band
{
	int edge;
	int index;
	int sum_diff_on;
	int jic_step_index;
}xdra_deocder_band;


typedef struct tagxdra_deocder_cluster
{
	
	int block_num;
	int bin0;
	int num_band;
	xdra_deocder_band bands[XDRA_MAX_BAND_NUM];
	int max_act_cb;
	int qstep_index[XDRA_MAX_BAND_NUM];
	int sum_diff_all_off;
}xdra_deocder_cluster;

typedef struct tagxdra_deocder_block
{
	int win_type;
}xdra_deocder_block;

typedef struct tagxdra_deocder_normal_channel
{
	int win_type;
	int last_win_type;
	int num_cluster;
	xdra_deocder_block blocks[XDRA_MAX_BLOCK_NUM];
	xdra_deocder_cluster clusters[XDRA_MAX_CLUSTER_NUM];
	float overlay[XDRA_FACTOR_NUM];
	float samples[XDRA_FACTOR_NUM];

	float factor[XDRA_FACTOR_NUM];
	float deinterlace_factor[XDRA_FACTOR_NUM];

}xdra_deocder_normal_channel;
typedef struct tagxdra_decoder
{
	int frame_count;
	void *mdct_256;
	void *mdct_2048;
	xdra_bitstream bs;
	xdra_frame_header header;
	xdra_deocder_normal_channel channels[XDRA_MAX_CHANNEL_NUM];

	int bins[XDRA_FACTOR_NUM];
	float out[XDRA_FACTOR_NUM * 2];
	

	unsigned char *data_buf;//输入缓冲区拷贝
	int  data_buf_len;//缓冲区实际有效字节
	int  data_buf_size;//缓冲区分配的大小
	int  data_buf_pos;//缓冲区现在有效数据起始位置

}xdra_decoder;



int xdra_decoder_parse_win_sequence(xdra_decoder *p, int ch)
{
	
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	if(ch == 0 || (p->header.use_sum_diff == 0 && p->header.use_jic == 0))
	{
		channel->win_type =  xdra_bitstream_get_bits(&p->bs, 4);
		if(IS_SHORT_WIN(channel->win_type))
		{
			
			int num_cluster =  xdra_bitstream_get_bits(&p->bs, 2) + 1;
			if(num_cluster >= XDRA_MAX_CLUSTER_NUM)
			{
				//error
				return -1;
			}
			channel->num_cluster = num_cluster;

			int num = 0;
			int cl = 0;
			for(cl = 0; cl< num_cluster - 1; cl++)
			{
				//该瞬态段分多少个块(MDCT块)
				int k = xdra_huffman_decode(&p->bs, &g_huffman_codebook1);
				if(k < 0)
				{
					return -1;
				}
				k++;
				channel->clusters[cl].bin0 = num * 128;
				channel->clusters[cl].block_num = k;
				num += k;
			}
			if(num > p->header.num_block_per_frame - 1)
			{
				return -1;
			}
			channel->clusters[cl].bin0 = num * 128;
			channel->clusters[cl].block_num = p->header.num_block_per_frame - num;	
		}
		else
		{
			channel->num_cluster = 1;
			channel->clusters[0].bin0 = 0;
			channel->clusters[0].block_num = 1;
		}
	}
	else
	{
		channel->win_type = p->channels[0].win_type;
		channel->num_cluster = p->channels[0].num_cluster;
		for(int cl = 0; cl< channel->num_cluster; cl++)
		{
			channel->clusters[cl].bin0 = p->channels[0].clusters[cl].bin0;;
			channel->clusters[cl].block_num = p->channels[0].clusters[cl].block_num;
		}
	}

	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		cluster->sum_diff_all_off = 1;
	}
	return 0;

}

int xdra_decoder_parse_code_book(xdra_decoder *p, int ch)
{

	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_huffman_codebook *book = IS_SHORT_WIN(channel->win_type) ? &g_huffman_codebook3 : &g_huffman_codebook2;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		cluster->num_band =  xdra_bitstream_get_bits(&p->bs, 5);
		int last = 0;
		for(int ba = 0; ba < cluster->num_band; ba++)
		{
			
			int k = xdra_huffman_decode_recursive(&p->bs, book);
			if(k < 0)
			{
				return -1;
			}
			k +=  last + 1;
			cluster->bands[ba].edge = k;
			last = k;
		}
	}

	book  = IS_SHORT_WIN(channel->win_type) ? &g_huffman_codebook5 : &g_huffman_codebook4;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		if(cluster->num_band > 0)
		{
			int last =  xdra_bitstream_get_bits(&p->bs, 4);
			cluster->bands[0].index = last;
			for(int ba = 1; ba < cluster->num_band; ba++)
			{
				int k = xdra_huffman_decode(&p->bs, book);
				if(k < 0)
				{
					return -1;
				}
				k -= k > 8 ? 8 : 9;
				k += last;
				cluster->bands[ba].index = k;
				last = k;
				
			}
		}	
	}	
	return 0;
}



int xdra_decoder_parse_qindex(xdra_decoder *p, int ch)
{

	int huff_diff_index = 0;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_huffman_codebook **book_tables = IS_SHORT_WIN(channel->win_type)  ? g_huffman_codebook_s_tables : g_huffman_codebook_l_tables;
	xdra_huffman_codebook *quotient_book = IS_SHORT_WIN(channel->win_type)  ? &g_huffman_codebook9 : &g_huffman_codebook8;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		int start = cluster->bin0;
		for(int ba = 0; ba < cluster->num_band; ba++)
		{
			int end = cluster->bin0 + cluster->bands[ba].edge * 4;
			int select = cluster->bands[ba].index;
			if(select == 0)
			{
				
				for(int bin = start; bin < end; bin++)
				{
					p->bins[bin] = 0;
				}
			}
			else
			{
				select--;
				if(select < 0 || select > 8)
				{
					return -1;
				}
				xdra_huffman_codebook *book = book_tables[select];
				if(select == 8)
				{
					int max_index = book->num_codes - 1;
					int ctrl = 0;
					for(int bin = start; bin < end; bin++)
					{
						int k = xdra_huffman_decode(&p->bs, book);
						if(k < 0)
						{
							return -1;
						}
						
						if(k == max_index)
						{
							ctrl++;
						}
						p->bins[bin] = k;
					}
					
					if(ctrl > 0)
					{
						huff_diff_index = xdra_huffman_decode_diff(&p->bs, quotient_book, huff_diff_index);
						if(huff_diff_index < 0)
						{
							return -1;
						}
	
						int quotient_width = huff_diff_index + 1;
						for(int bin = start; bin < end; bin++)
						{
							if(p->bins[bin] == max_index)
							{	
								p->bins[bin] *= (xdra_bitstream_get_bits(&p->bs, quotient_width) + 1);
								int k = xdra_huffman_decode(&p->bs,book);
								if(k < 0)
								{
									return -1;
								}
								p->bins[bin] = p->bins[bin] + k;
							}
						}
					}
				}
				else
				{
					if(book->dim > 1)
					{
						for(int bin = start; bin < end; bin += book->dim )
						{
							 int qindex = xdra_huffman_decode(&p->bs,book);
							 if(qindex < 0)
							 {
								 return -1;
							 }
							 for(int k = 0; k < book->dim; k++)
							 {
								 p->bins[bin + k] = qindex % book->num_codes;
								 qindex = qindex / book->num_codes;
							 }
						}
					}
					else
					{
						for(int bin = start; bin < end; bin ++ )
						{
							int k = xdra_huffman_decode(&p->bs,book);
							if(k < 0)
							{
								return -1;
							}
							 p->bins[bin] = k;
						}
					}
				}
				if(book->size != 256)
				{
					int max_index2 = book->num_codes / 2;
					for(int bin = start; bin < end; bin ++ )
					{
						p->bins[bin] -= max_index2;
					}
				}
				else
				{
					for(int bin = start; bin < end; bin ++ )
					{
						if(p->bins[bin] != 0)
						{
							int sign = xdra_bitstream_get_bits(&p->bs,1);
							if(sign == 0)
							{
								p->bins[bin] = -p->bins[bin];
							}
						}
					}
				}

			}
			start = end;
		}
	}
	return 0;
}

int xdra_decoder_get_act_cb(xdra_decoder *p, int ch)
{
	int huff_index = 0;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_freq_band_edge_table *table =  &(IS_SHORT_WIN(channel->win_type) ? g_freq_band_s_edge_tables : g_freq_band_l_edge_tables)[p->header.sample_rate_index];
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		int max_band = cluster->num_band;
		if(cluster->num_band == 0)
		{
			cluster->max_act_cb = 0;
			continue;
		}
		int max_bin = cluster->bands[max_band - 1].edge * 4;
		int num = (max_bin + cluster->block_num - 1)/ cluster->block_num;
		int max_num = table->table[table->num - 1];
		if(num > max_num)
		{
			return -1;
		}

		int cb = 0;
		while(table->table[cb + 1] < num)
		{
			
			cb++;
		}
		
		cluster->max_act_cb = cb + 1;	
		
	}
	return 0;
}

int xdra_decoder_parse_qstep_index(xdra_decoder *p, int ch)
{
	
	int huff_diff_index = 0;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_huffman_codebook *book = IS_SHORT_WIN(channel->win_type) ? &g_huffman_codebook7 : &g_huffman_codebook6;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		for(int ba = 0; ba < cluster->max_act_cb; ba++)
		{
			huff_diff_index = xdra_huffman_decode_diff(&p->bs, book, huff_diff_index) ;
			if(huff_diff_index < 0)
			{
				return -1;
			}
			cluster->qstep_index[ba] = huff_diff_index;
		}
	}
	return 0;
}

int xdra_decoder_inverse_quant(xdra_decoder *p, int ch)
{
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_freq_band_edge_table *table =  &(IS_SHORT_WIN(channel->win_type) ? g_freq_band_s_edge_tables : g_freq_band_l_edge_tables)[p->header.sample_rate_index];
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		for(int ba = 0; ba < cluster->max_act_cb; ba++)
		{
			int start = cluster->bin0 + cluster->block_num * table->table[ba];
			int end = cluster->bin0 + cluster->block_num * table->table[ba + 1];
			if(start >= XDRA_FACTOR_NUM || end > XDRA_FACTOR_NUM)
			{
				return -1;
			}
			float step = g_xdra_qstep_table[cluster->qstep_index[ba]];
			for(int bin = start; bin < end; bin++)
			{
				channel->factor[bin] = (p->bins[bin] * step);
			}
		}
	}
	return 0;
}



int xdra_decoder_parse_sum_diff(xdra_decoder *p, int ch)
{	
	int huff_index = 0;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_huffman_codebook *book = IS_SHORT_WIN(channel->win_type) ? &g_huffman_codebook7 : &g_huffman_codebook6;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];

		int max_act_cb_sum = p->channels[ch & 2].clusters[cl].max_act_cb;
		int max_cb = XDRA_MAX(max_act_cb_sum, cluster->max_act_cb);
		if(p->header.jic_cb > 0)
		{
			max_cb = XDRA_MIN(max_cb, p->header.jic_cb);
		}
		if(max_cb > 0)
		{
			cluster->sum_diff_all_off = xdra_bitstream_get_bits(&p->bs,1);
			if(cluster->sum_diff_all_off == 0)
			{
				for(int ba = 0; ba < max_cb; ba++)
				{

					cluster->bands[ba].sum_diff_on = xdra_bitstream_get_bits(&p->bs,1);;
				}
			}
			else
			{
				for(int ba = 0; ba < max_cb; ba++)
				{
					cluster->bands[ba].sum_diff_on = 0;
				}
			}
		}
	}
	return 0;
}




int xdra_decoder_parse_jic_scale(xdra_decoder *p, int ch)
{
	int huff_diff_index = 57;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_deocder_normal_channel *src_channel = &p->channels[0];
	xdra_huffman_codebook *book = IS_SHORT_WIN(channel->win_type) ? &g_huffman_codebook7 : &g_huffman_codebook6;
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		int max_act_cb =src_channel->clusters[cl].max_act_cb;
		for(int ba = p->header.jic_cb; ba < max_act_cb; ba++)
		{
			huff_diff_index = xdra_huffman_decode_diff(&p->bs, book, huff_diff_index);
			if( huff_diff_index < 0)
			{
				return -1;
			}
			 cluster->bands[ba].jic_step_index = huff_diff_index;
		}
	}

	return 0;
}


void xdra_decoder_jic(xdra_decoder *p, int ch)
{
	if(ch  == 0)
	{
		return ;
	}
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_deocder_normal_channel *src_channel = &p->channels[0];
	xdra_freq_band_edge_table *table =  &(IS_SHORT_WIN(channel->win_type) ? g_freq_band_s_edge_tables : g_freq_band_l_edge_tables)[p->header.sample_rate_index];
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		int bin0 = cluster->bin0;
		int max_act_cb = src_channel->clusters[cl].max_act_cb;
		for(int ba = p->header.jic_cb; ba < max_act_cb; ba++)
		{
			int start = cluster->bin0 + cluster->block_num * table->table[ba];
			int end = cluster->bin0 + cluster->block_num * table->table[ba + 1];
			float step =  g_xdra_qstep_table_jic[ba];//g_xdra_qstep_table[cluster->bands[ba].jic_step_index]/g_xdra_qstep_table[57];
			for(int bin = start; bin < end; bin++)
			{
				channel->factor[bin] = (src_channel->factor[bin] * step);
			}
		}
	}
}

void xdra_decoder_sum_diff(xdra_decoder *p, int ch)
{
	
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	xdra_deocder_normal_channel *sum_channel = &p->channels[ch - 1];
	xdra_freq_band_edge_table *table =  &(IS_SHORT_WIN(channel->win_type) ? g_freq_band_s_edge_tables : g_freq_band_l_edge_tables)[p->header.sample_rate_index];
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		int max_act_cb_sum = sum_channel->clusters[cl].max_act_cb;
		int max_act_cb = XDRA_MAX(max_act_cb_sum, cluster->max_act_cb);
		if(p->header.jic_cb > 0)
		{
			max_act_cb = XDRA_MIN(max_act_cb, p->header.jic_cb);
		}
		if(cluster->sum_diff_all_off == 0)
		{
			for(int ba = 0; ba < max_act_cb; ba++)
			{
				if(cluster->bands[ba].sum_diff_on)
				{
					int start = cluster->bin0 + cluster->block_num * table->table[ba];
					int end = cluster->bin0 + cluster->block_num * table->table[ba + 1];
					for(int bin = start; bin < end; bin++)
					{
						float sum = sum_channel->factor[bin] + channel->factor[bin];
						float diff = sum_channel->factor[bin] - channel->factor[bin];
						sum_channel->factor[bin] = sum;
						channel->factor[bin] = diff;
					}
				}
			}
		}
	}

}

void xdra_decoder_deinterlace_factor(xdra_decoder *p, int ch)
{
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	if(!IS_SHORT_WIN(channel->win_type))
	{
		return ;
	}	
	for(int cl = 0; cl< channel->num_cluster; cl++)
	{
		xdra_deocder_cluster *cluster = &channel->clusters[cl];
		for(int bl = 0; bl < cluster->block_num; bl++)
		{
			int q = cluster->bin0 + bl;
			float *factor = channel->deinterlace_factor +  cluster->bin0 + bl * 128;
			for(int n = 0; n < 128; n++,q += cluster->block_num)
			{
				factor[n] = channel->factor[q];
			}
		}
	}
}


int xdra_decoder_get_short_window_type(xdra_decoder *p, int ch)
{
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	if(!IS_SHORT_WIN(channel->win_type))
	{
		channel->blocks[0].win_type = channel->win_type;
		
		return 0;
	}


	if(channel->win_type == WIN_SHORT_SHORT2SHORT 
		||channel->win_type == WIN_SHORT_SHORT2BRIEF )
	{
		channel->blocks[0].win_type = WIN_SHORT_SHORT2SHORT;
		switch(channel->last_win_type)
		{
		case WIN_SHORT_BRIEF2BRIEF:
			channel->blocks[0].win_type = WIN_SHORT_BRIEF2SHORT;
			break;
		case WIN_LONG_LONG2SHORT:
		case WIN_LONG_SHORT2SHORT:
		case WIN_LONG_BRIEF2SHORT:
		case WIN_SHORT_SHORT2SHORT:
		case WIN_SHORT_BRIEF2SHORT:
			break;
		default:
			break;
			return -1;
		}
	}
	else
	{
		channel->blocks[0].win_type = WIN_SHORT_BRIEF2BRIEF;
		switch(channel->last_win_type)
		{
		case WIN_SHORT_BRIEF2BRIEF:
		case WIN_SHORT_SHORT2BRIEF:
		case WIN_LONG_LONG2BRIEF:
		case WIN_LONG_BRIEF2BRIEF:
		case WIN_LONG_SHORT2BRIEF:
			break;
		default:
			break;
			return -1;
		}
	}

	int bl;
	for(bl = 1; bl < p->header.num_block_per_frame; bl++)
	{
		channel->blocks[bl].win_type = WIN_SHORT_SHORT2SHORT;
	}
	bl = 0;
	for(int cl = 0; cl < channel->num_cluster - 1; cl++)
	{
		bl += channel->clusters[cl].block_num;
		channel->blocks[bl].win_type = WIN_SHORT_BRIEF2BRIEF;
	}

	for(bl = 0; bl < p->header.num_block_per_frame; bl++)
	{
		
		if(channel->blocks[bl].win_type == WIN_SHORT_BRIEF2BRIEF)
		{
			if(bl > 0)
			{
				if(channel->blocks[bl - 1].win_type == WIN_SHORT_SHORT2SHORT)
				{
					channel->blocks[bl - 1].win_type = WIN_SHORT_SHORT2BRIEF;
				}

				if(channel->blocks[bl - 1].win_type == WIN_SHORT_BRIEF2SHORT)
				{
					channel->blocks[bl - 1].win_type = WIN_SHORT_BRIEF2BRIEF;
				}

			}
			if(bl < p->header.num_block_per_frame - 1)
			{
				if(channel->blocks[bl + 1].win_type == WIN_SHORT_SHORT2SHORT)
				{
					channel->blocks[bl + 1].win_type = WIN_SHORT_BRIEF2SHORT;
				}
			}
		}
	}
	bl = p->header.num_block_per_frame - 1;
	switch(channel->blocks[bl].win_type)
	{
	case WIN_SHORT_SHORT2SHORT:
		if(channel->win_type == WIN_SHORT_SHORT2BRIEF
			||channel->win_type == WIN_SHORT_BRIEF2BRIEF )
		{
			channel->blocks[bl].win_type = WIN_SHORT_SHORT2BRIEF;
		}
		break;
	case WIN_SHORT_BRIEF2SHORT:
		if(channel->win_type == WIN_SHORT_SHORT2BRIEF
			|| channel->win_type == WIN_SHORT_BRIEF2BRIEF )
		{
			channel->blocks[bl].win_type = WIN_SHORT_BRIEF2BRIEF;
		}
		break;
	default:
		break;
		return -1;
	}	
	return 0;

}

int xdra_decoder_reconstruct_channel(xdra_decoder *p, int ch)
{
	int i, j;
	float *in, *out, *samples, *overlay;
	xdra_deocder_normal_channel *channel = &p->channels[ch];
	if(!IS_SHORT_WIN(channel->win_type))
	{	
		float *win_table = g_win_coeffes_tables[channel->win_type];
		in = channel->factor;
		out = p->out;
		samples = channel->samples;
		overlay = channel->overlay;
		//xdra_imct_orginal_c(in , out, XDRA_FACTOR_NUM);
		xdra_mdct_imdct(p->mdct_2048, in, out);
		for(int i = 0; i < XDRA_FACTOR_NUM; i++)
		{
			samples[i] = out[i] * win_table[i] + overlay[i];
		}
		out += XDRA_FACTOR_NUM;
		win_table += XDRA_FACTOR_NUM;
		for(int i = 0; i < XDRA_FACTOR_NUM; i++)
		{
			overlay[i] = out[i] * win_table[i] ;
		}

		channel->last_win_type = channel->win_type;
	}
	else
	{
		int N = p->header.num_block_per_frame;
		int N2 = (N >> 1);
		int short_len = XDRA_FACTOR_NUM / N;
		int short_len2 = short_len >> 1;		
		int pos = (XDRA_FACTOR_NUM  - short_len) >> 1;

		float *out_last;
		float *win_table = g_win_coeffes_tables[channel->blocks[0].win_type];
		float *win_table_last = NULL;
		in = channel->deinterlace_factor;

		out = p->out;
		samples = channel->samples;
		overlay = channel->overlay;
		memcpy(samples, overlay, pos * sizeof(samples[0]));
		for(i = 0; i < N; i++)
		{
			int k = i & 1;
			win_table_last = win_table + short_len;
			win_table = g_win_coeffes_tables[channel->blocks[i].win_type];
			//xdra_imct_orginal_c(in, out,  short_len);
			xdra_mdct_imdct(p->mdct_256, in, out);
			out_last = out - short_len;
			if(i < N2)
			{
				if(i == 0)
				{
					samples = channel->samples + pos;
					overlay = channel->overlay + pos;
					for(j = 0; j < short_len; j++)
					{
						samples[j] = overlay[j] + out[j] * win_table[j] ;
					}
				}
				else
				{
					
					for(j = 0; j < short_len; j++)
					{
						samples[j] = out_last[j] * win_table_last[j] + out[j] * win_table[j];
					}
				}
			}
			else if(i == N2)
			{
				for( j = 0; j < short_len2; j++)
				{
					samples[j] = out_last[j] * win_table_last[j] + out[j] * win_table[j];
				}
				overlay = channel->overlay - short_len2;
				for(; j < short_len; j++)
				{
					overlay[j] = out_last[j] * win_table_last[j] + out[j] * win_table[j];
				}
				
			}
			else 
			{
				
				for(j = 0; j < short_len; j++)
				{
					overlay[j] = out_last[j] * win_table_last[j] + out[j] * win_table[j];
				}
				
				if(i == N - 1)
				{
					out += short_len;
					overlay += short_len;
					win_table += short_len;
					for(j = 0; j < short_len; j++)
					{
						overlay[j] = out[j] * win_table[j];
					}
					overlay += short_len;
					int len =   (XDRA_FACTOR_NUM - (overlay - channel->overlay)) * sizeof(overlay[0]);
					memset(overlay, 0, len);
					channel->last_win_type = channel->blocks[i].win_type;
				}

			}
			
			in +=  short_len;
			samples += short_len;
			out += (short_len << 1);
			overlay += short_len;
		}
	}
	
	return 0;
}





void xdra_decoder_check_padbits(xdra_decoder *p)
{
	int check = 0;
	uint8_t *start = p->bs.p;
	uint8_t *end = p->bs.p_start + p->header.num_word * 4 - 2;
	if(p->bs.p > end)
	{
		check = 1;
	}
	else if(p->bs.p != end)
	{
		int left = p->bs.i_left;
		for(int i = 0; i < left; i++)
		{
			if(xdra_bitstream_get_bits(&p->bs, 1) != 1)
			{
				check = 1;
				break;

			}
		}
		if(check == 0)
		{
			while(p->bs.p < end )
			{
				if(p->bs.p[0] != 0xFF)
				{
					check = 1;
					break;
				}
				p->bs.p++;
			}			
		}
	}
	if(check)
	{
		printf("err check pad bits \n");
	}

}

void xdra_decoder_save_channel_data(xdra_decoder *p, int ch, const char *filename)
{
	int16_t out_pcm[2048];
	for(int i = 0; i < 1024; i++)
	{
		float t = p->channels[ch].samples[i];
		XDRA_CLIP_32768(t);
		out_pcm[i] = (int16_t)(t);
	}
	static FILE *dst_fp[10] = {0,};
	if(dst_fp[ch] == NULL)
	{
		char temp[256];
		sprintf(temp, "%s-%d.dat", filename, ch);
		dst_fp[ch] = fopen(temp,"wb");
	}
	if(dst_fp[ch])
	{
		//if(p->frame_count)
		{
			fwrite(out_pcm, 1, 2048, dst_fp[ch]);
		}
	}			
}

int xdra_decoder_decode_frame(void *decoder, unsigned char *frame, int len)
{

	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL || frame == NULL || len < 4)
	{
		return -1;
	}
	if(frame[0] != 0x7F || frame[1] != 0xFF)
	{
		return -1;
	}
	if(p->frame_count == 10)
	{
		int tt = 0;
	}
	xdra_frame_header header;
	xdra_bitstream_init(&p->bs, frame + 2, len - 2);
	if( xdra_parse_header_bs(&p->bs, &header) < 0)
	{
		return -1;
	}
	
	if(len < header.num_word * 4)
	{
		return -1;
	}
	
	//printf("len = %d\n ", header.num_word * 4);
	if(header.num_normal_channel + header.num_lfe_channel > XDRA_MAX_CHANNEL_NUM
		|| header.num_block_per_frame != XDRA_MAX_BLOCK_NUM)
	{
		return -1;
	}
	memcpy(&p->header, &header, sizeof(header));
	int ok = 1;
	int consume = 0;
	for(int ch = 0; ch < p->header.num_normal_channel; ch++)
	{
		memset(p->bins, 0, sizeof(int) * XDRA_FACTOR_NUM);
		memset(p->channels[ch].factor, 0, sizeof(p->channels[ch].factor[0]) * XDRA_FACTOR_NUM);
		if(xdra_decoder_parse_win_sequence(p, ch) < 0)
		{
			ok = 0;
			break;
		}

		if(xdra_decoder_parse_code_book(p, ch) < 0)
		{
			ok = 0;
			break;
		}
		if(xdra_decoder_get_act_cb(p, ch) < 0)
		{
			ok = 0;
			break;
		}
		if(xdra_decoder_parse_qindex(p, ch) < 0)
		{
			ok = 0;
			break;
		}

		if(xdra_decoder_parse_qstep_index(p, ch) < 0)
		{
			ok = 0;
			break;
		}
		if(p->header.use_sum_diff && (ch & 1) == 1)
		{
			if(xdra_decoder_parse_sum_diff(p, ch) < 0)
			{
				ok = 0;
				break;
			}

		}


		if(xdra_decoder_inverse_quant(p, ch) < 0)
		{
			ok = 0;
			break;

		}
		if(p->header.use_jic && ch !=  0 )
		{
			if(xdra_decoder_parse_jic_scale(p, ch) < 0)
			{
				ok = 0;
				break;
			}	
			xdra_decoder_jic(p, ch);
		}
		else
		{
			//xdra_decoder_inverse_quant(p, ch);
		}

		if(p->header.use_sum_diff && (ch & 1) == 1)
		{
			xdra_decoder_sum_diff(p, ch);
			xdra_decoder_deinterlace_factor(p, ch - 1);			
			xdra_decoder_deinterlace_factor(p, ch);
		}
		else
		{
			xdra_decoder_deinterlace_factor(p, ch);
		}
		xdra_decoder_get_short_window_type(p, ch);

		consume = (int)(p->bs.p - p->bs.p_start);
		if(p->header.use_sum_diff )
		{
			if((ch&1) == 1)
			{
				xdra_decoder_reconstruct_channel(p,  ch - 1);
				xdra_decoder_reconstruct_channel(p,  ch);
				//xdra_decoder_save_channel_data(p, ch - 1, src_filename);
				//xdra_decoder_save_channel_data(p, ch, src_filename);
			}

		}
		else
		{
			xdra_decoder_reconstruct_channel(p,  ch);
			//xdra_decoder_save_channel_data(p, ch, src_filename);
		}
	}
///////////
	if(ok)
	{
		for(int ch = p->header.num_normal_channel; ch < p->header.num_normal_channel + p->header.num_lfe_channel; ch++)
		{
			memset(p->bins, 0, sizeof(int) * XDRA_FACTOR_NUM);
			memset(p->channels[ch].factor, 0, sizeof(p->channels[ch].factor[0]) * XDRA_FACTOR_NUM);
			if(p->header.num_block_per_frame == 8)
			{
				p->channels[ch].win_type = WIN_LONG_LONG2LONG;
				p->channels[ch].num_cluster = 1;
				p->channels[ch].clusters[0].block_num = 1;
			}
			else
			{
				p->channels[ch].win_type = WIN_SHORT_SHORT2SHORT;
				p->channels[ch].num_cluster = 1;
				p->channels[ch].clusters[0].block_num = p->header.num_block_per_frame;
			}

			if(xdra_decoder_parse_code_book(p, ch) < 0)
			{
				break;
			}
			xdra_decoder_get_act_cb(p, ch);
			if(xdra_decoder_parse_qindex(p, ch) < 0)
			{
				break;
			}

			if(xdra_decoder_parse_qstep_index(p, ch) < 0)
			{
				break;
			}
			if(xdra_decoder_inverse_quant(p, ch) < 0)
			{
				ok = 0;
				break;

			}
			xdra_decoder_get_short_window_type(p, ch);
			xdra_decoder_deinterlace_factor(p, ch);
			xdra_decoder_reconstruct_channel(p,  ch);

		}
	}
///////////

	//xdra_decoder_check_padbits(p);
	consume = (int)(p->bs.p - p->bs.p_start);
	if(ok)
	{
		p->frame_count++;

		return 0;
	}
	else
	{
		for(int ch = 0; ch < p->header.num_normal_channel; ch++)
		{
			int size = sizeof(p->channels[ch].overlay);
			memset(p->channels[ch].overlay, 0, size);
			p->channels[ch].last_win_type = 0;
		}
		printf("err frame %d %d\n",p->frame_count, len);
		return -1;
	}
	
}


void *xdra_decoder_create(unsigned char *buf, int len)
{
	xdra_decoder *p = (xdra_decoder *)xdra_malloc(sizeof(xdra_decoder));
	if(p == NULL)
	{
		return NULL;
	}
	memset(p, 0, sizeof(p[0]));
	p->mdct_256 = xdra_mdct_create(256);
	p->mdct_2048 = xdra_mdct_create(2048);
	if(p->mdct_256 == NULL || p->mdct_2048 == NULL )
	{
		goto err;
	}
	if(buf)
	{
		if(xdra_parse_header(buf,  len, &p->header)  < 0)
		{
			goto err;
		}
	}
	return p;
err:
	xdra_decoder_destroy(p);
	return NULL;
}

int xdra_decoder_get_frame_header(void *decoder, xdra_frame_header *header)
{
	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL)
	{
		return -1;
	}
	memcpy(header, &p->header, sizeof(*header));
	return 0;
}

int xdra_decoder_put_data(void *decoder, unsigned char *data, int len)
{
	
	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL)
	{
		return -1;
	}
	uint8_t * temp = p->data_buf;
	int left = p->data_buf_len - p->data_buf_pos;
	int total = len + left;
	if(total > p->data_buf_size)
	{
		int size  = (total + 4095) /4096 * 4096;
		temp = (uint8_t *) xdra_malloc(size);
		if(temp == NULL)
		{
			return -1;
		}
		p->data_buf_size = size;
		if(p->data_buf)
		{
			if(len > 0 )
			{
				memcpy(temp, p->data_buf + p->data_buf_pos, left);
			}
			xdra_free(p->data_buf);
		}
		p->data_buf = temp;
	}
	else
	{
		if(p->data_buf_pos > 0)
		{
			memcpy(temp, temp + p->data_buf_pos, left);
		}
	}
	memcpy(temp + left,  data, len);
	p->data_buf_pos = 0;
	p->data_buf_len = total;
	return 0;
}

int xdra_decoder_decode_next_frame(void *decoder)
{

	xdra_decoder *p = (xdra_decoder *)decoder;
	if(!p)
	{
		return -1;
	}
	xdra_frame_header header;
	uint8_t *buf = p->data_buf + p->data_buf_pos;
	int len = p->data_buf_len - p->data_buf_pos;
	while(1)
	{
		int pos = xdra_next_sync_word(buf, len);
		if(pos < 0)
		{
			p->data_buf_len = 0;
			p->data_buf_pos = 0;
			return -1;
		}
		else
		{
			buf += pos;
			len -= pos;
			p->data_buf_pos = (int)(buf - p->data_buf);
			if(len - pos < 6)
			{
				return -1;
			}
			
			if(xdra_parse_header(buf,  len, &header)  < 0)
			{
				p->data_buf_pos += 2;
				buf += 2;
				len -= 2;
				continue;
			}
			int frame_size = header.num_word * 4;
			if(frame_size > len)
			{
				return -1;
			}
			p->data_buf_pos += frame_size;
			buf += frame_size;
			len -= frame_size;
			if(xdra_decoder_decode_frame(p, buf - frame_size, frame_size) < 0)
			{
				continue;
			}
			return 0;
		}
	}
	return 0;
}


int xdra_decoder_get_channel_f32(void *decoder, int channel, float *buf, int size)
{

	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL || size < 1024)
	{
		return -1;
	}
	int num = p->header.num_normal_channel + p->header.num_lfe_channel;
	if(channel >= num || channel < 0 || channel >= XDRA_MAX_CHANNEL_NUM)
	{
		return -1;
	}
	if(p->frame_count < 1)
	{
		return -1;
	}
	memcpy(buf, p->channels[channel].samples, 1024 * sizeof(float));
	return 1024;
}

int xdra_decoder_get_channel_s16(void *decoder, int channel, short *buf, int size)
{

	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL || size < 1024)
	{
		return -1;
	}
	int num = p->header.num_normal_channel + p->header.num_lfe_channel;
	if(channel >= num || channel < 0 || channel >= XDRA_MAX_CHANNEL_NUM)
	{
		return -1;
	}
	if(p->frame_count < 1)
	{
		return -1;
	}

	float sample;
	for(int i = 0; i < 1024; i++)
	{
		sample = p->channels[channel].samples[i];
		XDRA_CLIP_32768(sample);
		buf[i] = (int16_t)(sample);
	}
	return 1024;
}

int xdra_decoder_get_stero_s16(void *decoder, short *buf, int size)
{


	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL || size < 2048)
	{
		return -1;
	}
	int num = p->header.num_normal_channel + p->header.num_lfe_channel;
	if(num < 2)
	{
		//xdra_decoder_get_channel_s16(decoder, 0, buf, size);
		float sample;
		for(int i = 0; i < 1024; i++)
		{
			sample = p->channels[0].samples[i];
			XDRA_CLIP_32768(sample);
			

			buf[i * 2 + 1] = buf[i * 2 + 0] = (int16_t)(sample);
			
		}
		
	}
	else
	{
		float sample0, sample1;
		for(int i = 0; i < 1024; i++)
		{
			sample0 = p->channels[0].samples[i];
			XDRA_CLIP_32768(sample0);
			sample1 = p->channels[1].samples[i];
			XDRA_CLIP_32768(sample1);

			buf[i * 2 + 0] = (int16_t)(sample0);
			buf[i * 2 + 1] = (int16_t)(sample1);
		}

	}

	return 2048;
}

void xdra_decoder_reset(void *decoder)
{
	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL)
	{
		return ;
	}
	p->data_buf_pos = 0;
	p->data_buf_len = 0;
	p->frame_count = 0;
	for(int ch = 0; ch < p->header.num_normal_channel + p->header.num_lfe_channel; ch++)
	{
		int size = sizeof(p->channels[ch].overlay);
		memset(p->channels[ch].overlay, 0, size);
		p->channels[ch].last_win_type = 0;
	}
}

void xdra_decoder_destroy(void *decoder)
{

	xdra_decoder *p = (xdra_decoder *)decoder;
	if(p == NULL)
	{
		return ;
	}
	if(p->mdct_256)
	{
		xdra_mdct_destroy(p->mdct_256);
		p->mdct_256 = NULL;
	}

	if(p->mdct_2048)
	{
		xdra_mdct_destroy(p->mdct_2048);
		p->mdct_2048 = NULL;
	}
	if(p->data_buf)
	{
		xdra_free(p->data_buf);
	}
	xdra_free(decoder);

}

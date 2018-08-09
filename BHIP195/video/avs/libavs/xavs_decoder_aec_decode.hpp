/*
decode_decision( ) 
{
	if ( contextWeighting == 1 )
	{
		if  ( ctx1->mps == ctx2->mps )
		{
			predMps = ctx1->mps;
			lgPmps = (ctx1->lgPmps + ctx2->lgPmps) / 2;
		} 
		else
		{
			if ( ctx1->lgPmps < ctx2->lgPmps ) 
			{
				predMps = ctx1->mps;
				lgPmps = 1023 - ((ctx2->lgPmps 每 ctx1->lgPmps) >> 1);
			}
			else
			{
				predMps = ctx2->mps;
				lgPmps = 1023 - ((ctx1->lgPmps 每 ctx2->lgPmps) >> 1);
			}
		}
	} 
	else
	{
		predMps = ctx->mps;
		lgPmps =  ctx->lgPmps;
	}
	if ( rT1 >= (lgPmps >> 2) )
	{
		rS2 = rS1;
		rT2 = rT1 - (lgPmps >> 2 );
		sFlag = 0;
	}
	else
	{
		rS2 = rS1 + 1;
		rT2 = 256 + rT1 - (lgPmps >> 2);
		sFlag = 1;
	}
	if( rS2 > valueS || (rS2 == valueS && valueT >= rT2) )
	{
		binVal = !predMps;
		if ( sFlag == 0 )
			tRlps = lgPmps >> 2;
		else
			tRlps = rT1 + (lgPmps >> 2);
		if ( rS2 == valueS )
			valueT = valueT - rT2;
		else
			valueT = 256 + ((valueT << 1) | read_bits(1)) - rT2;
		while(tRlps < 0x100)
		{
			tRlps = tRlps << 1;
			valueT = (valueT << 1) | read_bits(1);
		}
		rS1 = 0;
		rT1 = tRlps & 0xFF;
		valueS = 0;
		while(valueT < 0x100)
		{
			valueS++;
			valueT = (valueT << 1) | read_bits(1);
		}
		valueT = valueT & 0xFF;
	}
	else
	{
		binVal = predMps;
		rS1 = rS2;
		rT1 = rT2;
	}
	if ( contextWeighting == 1 )
	{
		ctx1 = update_ctx(binVal, ctx1);
		ctx2 = update_ctx(binVal, ctx2);
	} 
	else 
	{
		ctx = update_ctx(binVal, ctx);
	}
	return (binVal);
} 




decode_bypass( ) 
{
	predMps = 0;
	lgPmps =  1023;
	if ( rT1 >= (lgPmps >> 2) )
	{
		rS2 = rS1;
		rT2 = rT1 - (lgPmps >> 2 );
		sFlag = 0;
	}
	else
	{
		rS2 = rS1 + 1;
		rT2 = 256 + rT1 - (lgPmps >> 2);
		sFlag = 1;
	}
	if( rS2 > valueS || (rS2 == valueS && valueT >= rT2) )
	{
		binVal = !predMps;
		if ( sFlag == 0 )
			tRlps = lgPmps >> 2;
		else
			tRlps = rT1 + (lgPmps >> 2);
		if ( rS2 == valueS )
			valueT = valueT - rT2;
		else
			valueT = ((valueT << 1) | read_bits(1)) - rT2 + 256;
		while(tRlps < 0x100)
		{
			tRlps = tRlps << 1;
			valueT = (valueT << 1) | read_bits(1);
		}
		rS1 = 0;
		rT1 = tRlps & 0xFF;
		valueS = 0;
		while(valueT < 0x100)
		{
			valueS++;
			valueT = (valueT << 1) | read_bits(1);
		}
		valueT = valueT & 0xFF;
	}
	else
	{
		binVal = predMps;
		rS1 = rS2;
		rT1 = rT2;
	}
	return (binVal);
} 


decode_aec_stuffing_bit( ) 
{
	predMps = 0;
	lgPmps =  4;
	if ( rT1 >= (lgPmps >> 2) )
	{
		rS2 = rS1;
		rT2 = rT1 - (lgPmps >> 2 );
		sFlag = 0;
	}
	else
	{
		rS2 = rS1 + 1;
		rT2 = 256 + rT1 - (lgPmps >> 2);
		sFlag = 1;
	}
	if( rS2 > valueS || (rS2 == valueS && valueT >= rT2) )
	{
		binVal = !predMps;
		if ( sFlag == 0 )
			tRlps = lgPmps >> 2;
		else
			tRlps = rT1 + (lgPmps >> 2);
		if ( rS2 == valueS )
			valueT = valueT - rT2;
		else
			valueT = 256 + ((valueT << 1) | read_bits(1)) 每 rT2;
		while(tRlps < 0x100)
		{
			tRlps = tRlps << 1;
			valueT = (valueT << 1) | read_bits(1);
		}
		rS1 = 0;
		rT1 = tRlps & 0xFF;
		valueS = 0;
		while(valueT < 0x100)
		{
			valueS++;
			valueT = (valueT << 1) | read_bits(1);
		}
		valueT = valueT & 0xFF;
	}
	else
	{
		binVal = predMps;
		rS1 = rS2;
		rT1 = rT2;
	}
	return (binVal);
} 


update_ctx( ) 
{ 
	if ( ctx->cycno <= 1 ) 
	{
		cwr = 3;
	}
	else if ( ctx->cycno == 2 )
	{
		cwr = 4;
	}
	else 
	{
		cwr = 5;
	}
	if ( binVal != ctx->mps )
	{
		if ( ctx->cycno <= 2 )
		{
			ctx->cycno = ctx->cycno + 1;
		}
		else
		{
			ctx->cycno = 3;
		}
	} 
	else if ( ctx->cycno == 0) 
	{
		ctx->cycno = 1;
	}
	if ( binVal == ctx->mps ) 
	{
		ctx->lgPmps = ctx->lgPmps 每 (ctx->lgPmps >> cwr) - (ctx->lgPmps >> (cwr+2));
	}
	else
	{ 
		switch (cwr) 
		{ 
		case 3:
			ctx->lgPmps = ctx->lgPmps + 197;
			break;
		case 4:
			ctx->lgPmps = ctx->lgPmps + 95;
			break;
		default: 
			ctx->lgPmps = ctx->lgPmps + 46;
			break;
		} 
		if ( ctx->lgPmps > 1023 )
		{
			ctx->lgPmps = 2047 - ctx->lgPmps;
				ctx->mps = !(ctx->mps);
		}  
	} 
	return (ctx);
} 

*/
static int inline aec_update_ctx(xavs_aec_decoder *p, xavs_aec_ctx *ctx, int binVal)
{
	

	/*int cwr;
	if(ctx->cycno <= 1)
	{
		cwr = 3;
	}
	else if(ctx->cycno == 2)
	{
		cwr = 4;
	}
	else 
	{
		cwr = 5;
	}
	if(binVal != ctx->mps)
	{
		if(ctx->cycno <= 2)
		{
			ctx->cycno = ctx->cycno + 1;
		}
		else
		{
			ctx->cycno = 3;
		}
	}
	else if(ctx->cycno == 0)
	{
		ctx->cycno = 1;
	}

	if(binVal == ctx->mps)
	{
		ctx->lgPmps = ctx->lgPmps - (ctx->lgPmps >> cwr) - (ctx->lgPmps >> (cwr + 2));
	}
	else
	{
		
		switch(cwr)
		{
		case 3:
			ctx->lgPmps =  ctx->lgPmps + 197;
			break;
		case 4:
			ctx->lgPmps =  ctx->lgPmps + 95;
			break;
		default:
			ctx->lgPmps =  ctx->lgPmps + 46;
			break;
		}
		
		if(ctx->lgPmps > 1023)
		{
			ctx->lgPmps =  2047 - ctx->lgPmps;
			ctx->mps = !(ctx->mps);

		}
	}
*/

	int cwr;
	const static uint8_t cwr_table[8] = {3,3,4,5, 1, 2, 3 , 3};
	const static uint8_t t_lgPmps_add[8] = {0,0,0, 197,95, 46,0};
	cwr = cwr_table[ctx->cycno];
	if(binVal != ctx->mps)
	{
		
		/*if(ctx->cycno <= 2)
		{
			ctx->cycno = ctx->cycno + 1;
		}
		else
		{
			ctx->cycno = 3;
		}*/
		ctx->cycno = cwr_table[ctx->cycno + 4];
		ctx->lgPmps +=  t_lgPmps_add[cwr];
		if(ctx->lgPmps > 1023)
		{
			ctx->lgPmps =  2047 - ctx->lgPmps;
			ctx->mps = !(ctx->mps);

		}
		//const int mask = (ctx->lgPmps & 0x400) ;
		//ctx->lgPmps =  2047  - ctx->lgPmps;
		//ctx->mps = (ctx->mps) ^ (mask >> 10);
	}
	else
	{
		ctx->lgPmps = ctx->lgPmps - (ctx->lgPmps >> cwr) - (ctx->lgPmps >> (cwr + 2));
		ctx->cycno = cwr - 2;
		//if(ctx->cycno == 0)
		//{
		//	ctx->cycno = 1;
		//}
	}
	return 0;
}

static int inline aec_decode_decision_core(xavs_aec_decoder *p, uint32_t predMps, uint32_t lgPmps)
{
	static const uint8_t xavs_shift[512]= {
		9,8,7,7,6,6,6,6,5,5,5,5,5,5,5,5,
		4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	int binVal;
	uint32_t rS2,rT2,tRlps,sFlag;
	if(p->rT1 >= lgPmps)
	{
		rS2 = p->rS1;
		rT2 = p->rT1 - lgPmps;
		sFlag = 0;
	}
	else
	{
		rS2 = p->rS1 + 1;
		rT2 = 256 + p->rT1 - lgPmps;
		sFlag = 0x7fffffff;

	}
	if(rS2 > p->valueS || (rS2 == p->valueS && p->valueT >= rT2))
	{
		binVal = !predMps;
		/*if(sFlag == 0)
		{
			tRlps = lgPmps;
		}
		else
		{
			tRlps = p->rT1 + lgPmps;
		}*/
		tRlps = (sFlag &  p->rT1) +  lgPmps;
		if(rS2 == p->valueS)
		{
			p->valueT = p->valueT - rT2;
		}
		else
		{
			p->valueT = 256 + ((p->valueT << 1) | xavs_bitstream_get_bit1_fast(p->stream)) - rT2;
		}
		
		int kk = xavs_shift[tRlps];
		tRlps <<= kk;
		p->valueT = (p->valueT << kk) | xavs_bitstream_get_bits(p->stream,kk);

		/*while(tRlps < 0x100)
		{
			tRlps = tRlps << 1;
			p->valueT = (p->valueT << 1) | xavs_bitstream_get_bit1_fast(p->stream);
		}*/
		p->rS1 = 0;
		p->rT1 = tRlps & 0xFF;
		p->valueS = 0;

	
		while(p->valueT < 0x100)
		{
			kk = xavs_shift[p->valueT];
			p->valueS += kk;
			p->valueT = (p->valueT << kk) | xavs_bitstream_get_bits(p->stream,kk);
			if( p->stream->p >= p->stream->p_end )
			{
				break;
			}

		//	p->valueS++;
		//	p->valueT = (p->valueT << 1) | xavs_bitstream_get_bit1_fast(p->stream);
		}
		
		p->valueT = p->valueT & 0xFF;
	}
	else
	{
		binVal = predMps;
		p->rS1 = rS2;
		p->rT1 = rT2;
	}
	return binVal;

}

static int inline aec_decode_decision(xavs_aec_decoder *p, xavs_aec_ctx *ctx)
{
	int binVal;
	binVal = aec_decode_decision_core(p, ctx->mps, ctx->lgPmps >> 2);
	aec_update_ctx(p,ctx, binVal);
	return binVal;
}

static int inline aec_decode_decision_weighting(xavs_aec_decoder *p, xavs_aec_ctx *ctx1, xavs_aec_ctx *ctx2)
{
	int binVal;
	
	if(ctx1->mps == ctx2->mps)
	{
		binVal = aec_decode_decision_core(p, ctx1->mps, (ctx1->lgPmps + ctx2->lgPmps)  >> 3);
	}
	else if(ctx1->lgPmps < ctx2->lgPmps)
	{
		binVal = aec_decode_decision_core(p, ctx1->mps, (1023 - ((ctx2->lgPmps - ctx1->lgPmps) >> 1)) >> 2 );
	}
	else
	{
		binVal = aec_decode_decision_core(p, ctx2->mps, (1023 - ((ctx1->lgPmps - ctx2->lgPmps)  >> 1)) >> 2 );
	}
	aec_update_ctx(p,ctx1, binVal);
	aec_update_ctx(p,ctx2, binVal);
	return binVal;
}


static int inline aec_decode_bypass(xavs_aec_decoder *p)
{
	return aec_decode_decision_core(p, 0, 255);	
}


static int inline aec_decode_stuffing_bit(xavs_aec_decoder *p)
{
	return aec_decode_decision_core(p, 0, 1);
}




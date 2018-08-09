#include "ESParse.h"
//#include "patch/vuipatch.h"

#define STREAM_ID_MAP           0xBC
#define STREAM_ID_1             0xBD
#define STREAM_ID_PADDING       0xBE
#define STREAM_ID_2             0xBF
#define STREAM_ID_ECM           0xF0
#define STREAM_ID_EMM           0xF1
#define STREAM_ID_DSMCC         0xF2
#define STREAM_ID_ISO13522      0xF3
#define STREAM_ID_DIR           0xFF

#define  MAX_SPS_LEN       128

/**
 * Table 7-3: NAL unit type codes
 */
enum NALUnitType {
    NAL_TRAIL_N    = 0,
    NAL_TRAIL_R    = 1,
    NAL_TSA_N      = 2,
    NAL_TSA_R      = 3,
    NAL_STSA_N     = 4,
    NAL_STSA_R     = 5,
    NAL_RADL_N     = 6,
    NAL_RADL_R     = 7,
    NAL_RASL_N     = 8,
    NAL_RASL_R     = 9,
    NAL_BLA_W_LP   = 16,
    NAL_BLA_W_RADL = 17,
    NAL_BLA_N_LP   = 18,
    NAL_IDR_W_RADL = 19,
    NAL_IDR_N_LP   = 20,
    NAL_CRA_NUT    = 21,
    NAL_VPS        = 32,
    NAL_SPS        = 33,
    NAL_PPS        = 34,
    NAL_AUD        = 35,
    NAL_EOS_NUT    = 36,
    NAL_EOB_NUT    = 37,
    NAL_FD_NUT     = 38,
    NAL_SEI_PREFIX = 39,
    NAL_SEI_SUFFIX = 40,
};

enum SliceType {
    B_SLICE = 0,
    P_SLICE = 1,
    I_SLICE = 2,
};

CESParse::CESParse()
: m_pStart(NULL)
, m_uLength(0)
, m_nCurrentBit(0)
, m_bFindPPS(false)
, dependent_slice_segments_enabled_flag(0)
, num_extra_slice_header_bits(0)
{}

unsigned int CESParse::ReadBit()
{
    assert((unsigned int)m_nCurrentBit <= m_uLength * 8);
    int nIndex = m_nCurrentBit / 8;
    int nOffset = m_nCurrentBit % 8 + 1;

    m_nCurrentBit ++;
    return (m_pStart[nIndex] >> (8-nOffset)) & 0x01;
}

unsigned int CESParse::ReadBits(int n)
{
    int r = 0;
    for (int i = 0; i < n; i++)
    {
        r |= ( ReadBit() << ( n - i - 1 ) );
    }
    return r;
}

unsigned int CESParse::ReadExponentialGolombCode()
{
    int r = 0;
    int i = 0;

    while( (ReadBit() == 0) && (i < 32) )
    {
        i++;
    }

    r = ReadBits(i);
    r += (1 << i) - 1;
    return r;
}

unsigned int CESParse::ReadSE()
{
    int r = ReadExponentialGolombCode();
    if (r & 0x01)
    {
        r = (r+1)/2;
    }
    else
    {
        r = -(r/2);
    }
    return r;
}

bool CESParse::ParseSPS(const unsigned char * pStart, unsigned int uLen, int *ipWidth, int *ipHeight)
{
    int nalu_type = (*pStart >> 1) & 0x3f;
    if (nalu_type != NAL_SPS) return false;

    // NOTE: 这里只需取一个SPS的长度
    unsigned char sps_as_rbsp[MAX_SPS_LEN] = { 0x00 };
    for (unsigned int i = 2, j = 0; i < uLen && j < MAX_SPS_LEN; )
    {
        if (0x00 == pStart[i] && 0x00 == pStart[i+1] && 0x03 == pStart[i+2])
        { // 读到0x000003时，丢弃0x03
            sps_as_rbsp[j++] = 0x00;
            sps_as_rbsp[j++] = 0x00;
            i += 3;
        }
        else
        {
            sps_as_rbsp[j++] = pStart[i++];
        }
    }

    m_pStart = sps_as_rbsp;
    m_uLength = MAX_SPS_LEN < uLen ? MAX_SPS_LEN : uLen;
    m_nCurrentBit = 0;

    /*int video_parameter_set_id =*/ ReadBits(4);
    int max_sub_layers_minus1 = ReadBits(3);
    /*int temporal_id_nesting_flag =*/ ReadBit();

    profile_tier_level(max_sub_layers_minus1);

    /* int seq_parameter_set_id =*/ ReadExponentialGolombCode();
    int chroma_format_idc = ReadExponentialGolombCode();

    int separate_colour_plane_flag = 0;
    if (chroma_format_idc == 3)
        separate_colour_plane_flag = ReadBit();

    int pic_width_in_luma_samples = ReadExponentialGolombCode();
    int pic_height_in_luma_samples = ReadExponentialGolombCode();
    int conformance_window_flag = ReadBit();

    int conf_win_left_offset = 0;
    int conf_win_right_offset = 0;
    int conf_win_top_offset = 0;
    int conf_win_bottom_offset = 0;

    if (conformance_window_flag)
    {
        conf_win_left_offset = ReadExponentialGolombCode();
        conf_win_right_offset = ReadExponentialGolombCode();
        conf_win_top_offset = ReadExponentialGolombCode();
        conf_win_bottom_offset = ReadExponentialGolombCode();
    }

    int width  = pic_width_in_luma_samples;
    int height = pic_height_in_luma_samples;

    if (conformance_window_flag)
    {
        int sub_width_c  = ( (1 == chroma_format_idc) || (2 == chroma_format_idc) ) && (0 == separate_colour_plane_flag) ? 2 : 1;
        int sub_height_c = (1 == chroma_format_idc) && (0 == separate_colour_plane_flag) ? 2 : 1;
        width  -= (sub_width_c * conf_win_right_offset + sub_width_c * conf_win_left_offset);
        height -= (sub_height_c * conf_win_bottom_offset + sub_height_c * conf_win_top_offset);
    }

    /*int bit_depth_luma_minus8 =*/ ReadExponentialGolombCode();
    /*int bit_depth_chroma_minus8 =*/ ReadExponentialGolombCode();

    *ipWidth = width;
    *ipHeight = height;

    return true;
}

void CESParse::FindResolution_H265(unsigned char *pInData, unsigned int nDataLen, int *ipWidth, int *ipHeight) 
{
    unsigned int uLen = nDataLen;
    unsigned char *ucpInBuffer = pInData;

    for (unsigned int i = 0; i < uLen; i++)
    {
        if( (ucpInBuffer[i] == 0x00) && (ucpInBuffer[i+1] == 0x00) && (ucpInBuffer[i+2] == 0x01) )
        {
            if (ParseSPS(&ucpInBuffer[i+3], uLen, ipWidth, ipHeight)) return;
        }       
    }
}

void CESParse::profile_tier_level(int max_sub_layers_minus1)
{
    /*int profile_space =*/ ReadBits(2);
    /*int tier_flag =*/ ReadBit();
    /*int profile_idc =*/ ReadBits(5);

    int profile_compatibility_flag[32] = { 0 };
    for (int i = 0; i < 32; i++)
    {
        profile_compatibility_flag[i] = ReadBit();
    }

    /*int progressive_source_flag =*/ ReadBit();
    /*int interlaced_source_flag =*/ ReadBit();

    /*int non_packed_constraint_flag =*/ ReadBit();
    /*int frame_only_constraint_flag =*/ ReadBit();

    // skip reserved_zero_44bits
    ReadBits(16); ReadBits(16); ReadBits(12);

    /*int level_idc =*/ ReadBits(8);

    if (max_sub_layers_minus1 > 0)
    {
        int sub_layer_profile_present_flag[max_sub_layers_minus1];
        int sub_layer_level_present_flag[max_sub_layers_minus1];

        int i = 0;

        for ( ; i < max_sub_layers_minus1; i++)
        {
            sub_layer_profile_present_flag[i] = ReadBit();
            sub_layer_level_present_flag[i] = ReadBit();
        }

        for (i = max_sub_layers_minus1; i < 8; i++)
        {
            /*int reserved_zero_2bits =*/ ReadBits(2);  // reserved_zero_2bits
        }

        for (i = 0; i < max_sub_layers_minus1; i++)
        {
            if (sub_layer_profile_present_flag[i])
            {
                /* int sub_layer_profile_space =*/ ReadBits(2);  // sub_layer_profile_space[i]
                /*int sub_layer_tier_flag =*/ ReadBit();        // sub_layer_tier_flag[i]
                /*int sub_layer_profile_idc =*/ ReadBits(5);    // sub_layer_profile_idc

                for (int n = 0; n < 32; n++)
                    ReadBit();  //sub_layer_profile_compatibility_flag[i]

                /*int sub_layer_progressive_source_flag =*/ ReadBit();  // sub_layer_progressive_source_flag[i]
                /*int sub_layer_interlaced_source_flag =*/ ReadBit();   // sub_layer_interlaced_source_flag[i]
                /*int sub_layer_non_packed_constraint_flag =*/ ReadBit();   // sub_layer_non_packed_constraint_flag[i]
                /*int sub_layer_frame_only_constraint_flag =*/ ReadBit();   // sub_layer_frame_only_constraint_flag[i]

                ReadBits(16); ReadBits(16); ReadBits(12);      // sub_layer_reserved_zero_44bits
            }

            if (sub_layer_level_present_flag[i])
            {
                /*int sub_layer_level_idc =*/ ReadBits(8);  // sub_layer_level_idc[i]
            }
        }
    }
}

void CESParse::Getx265SliceCount(unsigned char *pInData, unsigned int uDataLen, int *ICnt, int *PCnt, int *BCnt)
{
    unsigned int uLen = uDataLen;
    unsigned char *ucpInBuffer = pInData;

    for (unsigned int i = 0; i < uLen; i++)
    {
        if( (ucpInBuffer[i] == 0x00) && (ucpInBuffer[i+1] == 0x00) && (ucpInBuffer[i+2] == 0x01) )
        {
            GetSliceCount(&ucpInBuffer[i+3], uLen, ICnt, PCnt, BCnt);
        }       
    }
}

bool CESParse::GetSliceCount(const unsigned char * pStart, unsigned int uLen, int *ICnt, int *PCnt, int *BCnt)
{
    if (pStart == NULL) return false;

    int nalu_type = (*pStart >> 1) & 0x3f;

    switch (nalu_type)
    {
    case NAL_PPS:
        ParsePPS(pStart, uLen);
        return true;
    case NAL_TRAIL_R:
    case NAL_TRAIL_N:
    case NAL_TSA_N:
    case NAL_TSA_R:
    case NAL_STSA_N:
    case NAL_STSA_R:
    case NAL_BLA_W_LP:
    case NAL_BLA_W_RADL:
    case NAL_BLA_N_LP:
    case NAL_IDR_W_RADL:
    case NAL_IDR_N_LP:
    case NAL_CRA_NUT:
    case NAL_RADL_N:
    case NAL_RADL_R:
    case NAL_RASL_N:
    case NAL_RASL_R:
        break;
    default:
        return false;
    }

    if (m_bFindPPS == false) return false;

    unsigned char slice_as_rbsp[MAX_SPS_LEN] = { 0x00 };
    for (unsigned int i = 2, j = 0; i < uLen && j < MAX_SPS_LEN; )
    {
        if (0x00 == pStart[i] && 0x00 == pStart[i+1] && 0x03 == pStart[i+2])
        { // 读到0x000003时，丢弃0x03
            slice_as_rbsp[j++] = 0x00;
            slice_as_rbsp[j++] = 0x00;
            i += 3;
        }
        else
        {
            slice_as_rbsp[j++] = pStart[i++];
        }
    }

    m_pStart = slice_as_rbsp;
    m_uLength = MAX_SPS_LEN < uLen ? MAX_SPS_LEN : uLen;
    m_nCurrentBit = 0;

    unsigned char first_slice_in_pic_flag = ReadBit();
    unsigned char no_output_of_prior_pics_flag = 0;

    if (nalu_type >= 16 && nalu_type <= 23)
    {
        no_output_of_prior_pics_flag = ReadBit();
    }

    /*unsigned int pps_id =*/ ReadExponentialGolombCode();
    unsigned char dependent_slice_segment_flag = 0;

    if (!first_slice_in_pic_flag)
    {
        if (dependent_slice_segments_enabled_flag)
        {
            dependent_slice_segment_flag = ReadBit();
        }

        ReadExponentialGolombCode(); // slice_address_length
    }

    if (!dependent_slice_segment_flag)
    {
        for (int i = 0; i < num_extra_slice_header_bits; i++)
            ReadBit();  // slice_reserved_undetermined_flag[]
        unsigned int slice_type = ReadExponentialGolombCode();
        switch (slice_type)
        {
        case I_SLICE:
            *ICnt += 1;
            break;
        case P_SLICE:
            *PCnt += 1;
            break;
        case B_SLICE:
            *BCnt += 1;
            break;
        default:
            printf("Unknown slice type for HEVC , type is %u\n", slice_type);
            break;
        }
    }

	return true;
}

bool CESParse::ParsePPS(const unsigned char * pStart, unsigned int uLen)
{
    // NOTE: 这里只需取一个SPS的长度
    unsigned char pps_as_rbsp[MAX_SPS_LEN] = { 0x00 };
    for (unsigned int i = 2, j = 0; i < uLen && j < MAX_SPS_LEN; )
    {
        if (0x00 == pStart[i] && 0x00 == pStart[i+1] && 0x03 == pStart[i+2])
        { // 读到0x000003时，丢弃0x03
            pps_as_rbsp[j++] = 0x00;
            pps_as_rbsp[j++] = 0x00;
            i += 3;
        }
        else
        {
            pps_as_rbsp[j++] = pStart[i++];
        }
    }

    m_pStart = pps_as_rbsp;
    m_uLength = MAX_SPS_LEN < uLen ? MAX_SPS_LEN : uLen;
    m_nCurrentBit = 0;

    ReadExponentialGolombCode();    // pps_pic_parameter_set_id
    ReadExponentialGolombCode();    // pps_seq_parameter_set_id

    dependent_slice_segments_enabled_flag = ReadBit();
    ReadBit();
    num_extra_slice_header_bits = ReadBits(3);
    m_bFindPPS = true;

    return true;
}

unsigned int CESParse::ParsePes(unsigned char *pInData, unsigned int nDataLen,
								unsigned char *pOut, unsigned long long *pOutPts)
{
    //PES长度判断
    if(nDataLen < 10)
    {
        fprintf(stderr, "Length of pes is incorrect, Pes len %d\n", nDataLen);
        return 0;
    }

    //PES包开始码判断
    if((pInData[0] != 0x00) || (pInData[1] != 0x00) || (pInData[2] != 0x01))
    {
        fprintf(stderr, "Head of pes is incorrect, 0x%02x%02x%02x\n", pInData[0], pInData[1], pInData[2]);
        return 0;
    }

    if (pInData[3] == STREAM_ID_PADDING) return 0;

	// Get PTS.
	GetPts(pInData, nDataLen, pOutPts);

	//unsigned int nOutLen = *(unsigned short*)(pInData + 4);
    unsigned int nOutLen = nDataLen - 6;
    unsigned int nOffset = 6;
    //STREAM_ID判断
    if(!((pInData[3] == STREAM_ID_MAP) ||
                (pInData[3] == STREAM_ID_1) ||
                (pInData[3] == STREAM_ID_2) ||
                (pInData[3] == STREAM_ID_ECM) ||
                (pInData[3] == STREAM_ID_EMM) ||
                (pInData[3] == STREAM_ID_DIR)))
    {
        unsigned int nPesHeaderLen = pInData[8];
        nOffset += 3;
        nOffset += nPesHeaderLen;
        nOutLen -= 3;
        nOutLen -= nPesHeaderLen;

		//printf("pOut is 0x%x pInData is 0x%x nOutLen is %d \n",pOut,pInData + nOffset, nOutLen);
        memcpy(pOut, pInData + nOffset, nOutLen);
        return nOutLen;

        /*if (!(pInData[nOffset] == 0x0 && pInData[nOffset+1] == 0x0 
                    && pInData[nOffset+2] == 0x0 && pInData[nOffset+3] == 0x1))
        {
            fprintf(stderr, "0001 assertion failed.\n");
            return 0;
        }

        unsigned int nPatchLen;
        unsigned char *pPatched = EsPatch(pInData + nOffset, nOutLen, &nPatchLen);
        printf("memcmp ... %s\n", memcmp(pPatched, pInData + nOffset, nOutLen) ? "failed" : "succeed");
        memcpy(pOut, pPatched, nPatchLen);
        free(pPatched);
        return nPatchLen;*/
    }

    return 0;
}

void CESParse::GetPts(unsigned char *pInData, unsigned nDataLen, unsigned long long *ipPts)
{
	// Get PTS.
	if ( (pInData[6] & 0xC0 ) == 0x80 )
	{
		if (pInData[7] & 0x80 )
		{
			if (ipPts)
			{
				*ipPts = ((unsigned long long)(pInData[9] & 0x0e) << 29)
					| ((unsigned long long)(pInData[10] << 22))
					| ((unsigned long long)(pInData[11]&0xfe) << 14)
					| ((unsigned long long)(pInData[12] << 7))
					| ((unsigned long long)(pInData[13] >> 1));
			}
		}
	}
}


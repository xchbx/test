#include "IntelES2YUV.h"

// IMSDK全局唯一设备句柄
extern auto_ptr<CHWDevice>     g_hwdev;

static const int TRANS_B_FRAME_BUFFER_SIZE = 5;


IntelES2YUV::IntelES2YUV(int nDecStreamType, int nVppOutWidth, int nVppOutHeight, int nVppOutFrameRate)
: m_nDecStreamType(nDecStreamType)
, m_nFrameRate(0)
, m_bStop(false)
, m_bInit(false)
, m_bHasBFrame(true)    // 目前不能所有类型全部判断帧类型 这里默认全部为B帧视频帧 ToDo
, m_u64Pts(0)
, m_u64PtsPre(0)
, m_pmfxBSExt(NULL)
, m_pGeneralAllocator(NULL)
, m_pmfxAllocatorParams(NULL)
, m_bDecOutSysmem(false)
, m_fourcc(MFX_FOURCC_NV12)
, m_bVppIsUsed(false)
, m_vppOutWidth(nVppOutWidth)
, m_vppOutHeight(nVppOutHeight)
, m_vppOutFrmaeRate(nVppOutFrameRate)
, m_pCurrentFreeSurface(NULL)
, m_pCurrentFreeVppSurface(NULL)
, m_pCurrentFreeOutputSurface(NULL)
, m_pCurrentOutputSurface(NULL)
{
    MSDK_ZERO_MEMORY(m_mfxDecParams);
    MSDK_ZERO_MEMORY(m_mfxVppParams);

    MSDK_ZERO_MEMORY(m_mfxResponse);
    MSDK_ZERO_MEMORY(m_mfxVppResponse);

    MSDK_ZERO_MEMORY(m_VppDoNotUse);
    m_VppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
    m_VppDoNotUse.Header.BufferSz = sizeof(m_VppDoNotUse);
}

IntelES2YUV::~IntelES2YUV()
{
    Close();
}

mfxStatus IntelES2YUV::Init()
{
    mfxStatus sts = MFX_ERR_NONE;

    if (! m_pmfxSession.get())
    {
        mfxVersion version;
        memset((void *)&version, 0, sizeof(mfxVersion));
        version.Major = 1;
        version.Minor = 3;

        // init session
        m_pmfxSession.reset(new MFXVideoSession);
        sts = m_pmfxSession->Init(MFX_IMPL_HARDWARE, &version);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        mfxIMPL impl = 0;
        m_pmfxSession->QueryIMPL(&impl);

        // ******************************************************************************************
        // CreateAllocator
        // linux平台使用硬件加速    
        m_pGeneralAllocator = new GeneralAllocator();

        VADisplay va_dpy = NULL;
        sts = g_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL*)&va_dpy);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        sts = m_pmfxSession->SetHandle(MFX_HANDLE_VA_DISPLAY, va_dpy); 
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        vaapiAllocatorParams *p_vaapiAllocParams = new vaapiAllocatorParams;
        MSDK_CHECK_POINTER(p_vaapiAllocParams, MFX_ERR_MEMORY_ALLOC);

        p_vaapiAllocParams->m_dpy = va_dpy;
        m_pmfxAllocatorParams = p_vaapiAllocParams;

        /* In case of video memory we must provide MediaSDK with external allocator
        thus we demonstrate "external allocator" usage model.
        Call SetAllocator to pass allocator to mediasdk */
        sts = m_pmfxSession->SetFrameAllocator(m_pGeneralAllocator);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // initialize memory allocator
        sts = m_pGeneralAllocator->Init(m_pmfxAllocatorParams);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        // ******************************************************************************************
    }

    // Decode component initialization
    if (! m_pmfxDEC.get())
    {
        // 解码类型
        unsigned int uDecCodecId = MFX_CODEC_AVC;
        if(m_nDecStreamType == AV_CODEC_V_H264)
        {
            uDecCodecId = MFX_CODEC_AVC;
        }
        else if(m_nDecStreamType == AV_CODEC_V_HEVC)
        {
            //加载 h265解码插件
            uDecCodecId = MFX_CODEC_HEVC;
            LoadPlugin(MFX_PLUGINTYPE_VIDEO_DECODE, *m_pmfxSession.get(), MFX_PLUGINID_HEVCD_HW, 1);
        }
        else if(m_nDecStreamType == AV_CODEC_V_MPEG2)
        {
            uDecCodecId = MFX_CODEC_MPEG2;
        }
        else
        {
            printf("Input DecStreamType:%d Not Support\n", m_nDecStreamType);
            return MFX_ERR_UNSUPPORTED;
        }

        m_pmfxDEC.reset(new MFXVideoDECODE(*m_pmfxSession.get()));

        m_mfxDecParams.mfx.CodecId = uDecCodecId;
        //m_mfxDecParams.mfx.ExtendedPicStruct = 0;
        //m_mfxDecParams.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
    }

    sts = m_pmfxDEC->DecodeHeader(m_pmfxBSExt, &m_mfxDecParams);

    if (MFX_ERR_MORE_DATA == sts)
    {
        // 数据没有收集全，解码参数解析不出来，需要收集更多数据
        // log_error("Parser sequence parameter failed.");
        return sts;
    }
    else
    {
        // 成功解析出视频数据信息
        int nWidth = m_mfxDecParams.mfx.FrameInfo.CropW;
        int nHeight = m_mfxDecParams.mfx.FrameInfo.CropH;

        // if frame rate not specified and input stream header doesn't contain valid values use default (30.0)
        if (!(m_mfxDecParams.mfx.FrameInfo.FrameRateExtN * m_mfxDecParams.mfx.FrameInfo.FrameRateExtD))
        {
            m_mfxDecParams.mfx.FrameInfo.FrameRateExtN = 30;
            m_mfxDecParams.mfx.FrameInfo.FrameRateExtD = 1;
        }
        m_nFrameRate = m_mfxDecParams.mfx.FrameInfo.FrameRateExtN / m_mfxDecParams.mfx.FrameInfo.FrameRateExtD;

        log_info("Parser sequence parametar successed width:%d height:%d framerate:%d.", nWidth, nHeight, m_nFrameRate);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // if frame rate not specified and input stream header doesn't contain valid values use default (30.0)
    if (!m_mfxDecParams.mfx.FrameInfo.FrameRateExtN || !m_mfxDecParams.mfx.FrameInfo.FrameRateExtD) 
    {
        msdk_printf(MSDK_STRING("pretending that stream is 30fps one\n"));
        m_mfxDecParams.mfx.FrameInfo.FrameRateExtN = 30;
        m_mfxDecParams.mfx.FrameInfo.FrameRateExtD = 1;
    }
    
    if (!m_mfxDecParams.mfx.FrameInfo.AspectRatioW || !m_mfxDecParams.mfx.FrameInfo.AspectRatioH) 
    {
        msdk_printf(MSDK_STRING("pretending that aspect ratio is 1:1\n"));
        m_mfxDecParams.mfx.FrameInfo.AspectRatioW = 1;
        m_mfxDecParams.mfx.FrameInfo.AspectRatioH = 1;
    }
    
    // 确定是否使用VPP缩放(帧率不相等也需要VPP进行处理)
    m_bVppIsUsed = m_fourcc && (m_fourcc != m_mfxDecParams.mfx.FrameInfo.FourCC);
    if ( (m_mfxDecParams.mfx.FrameInfo.CropW != m_vppOutWidth) || (m_mfxDecParams.mfx.FrameInfo.CropH != m_vppOutHeight) || (m_nFrameRate != m_vppOutFrmaeRate) )
    {
        m_bVppIsUsed |= m_vppOutWidth && m_vppOutHeight;
    }

    if (!m_bVppIsUsed)
        m_mfxDecParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_SYSTEM_MEMORY);
    else
        m_mfxDecParams.IOPattern = (mfxU16)(MFX_IOPATTERN_OUT_VIDEO_MEMORY);

    m_mfxDecParams.AsyncDepth = 4;
    m_bDecOutSysmem = false;


    // VPP component initialization
    if (m_bVppIsUsed && ! m_pmfxVPP.get())
    {
        m_pmfxVPP.reset(new MFXVideoVPP(*m_pmfxSession.get()));

        m_mfxVppParams.AsyncDepth = 4;

        // input frame info
        MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.In, &m_mfxDecParams.mfx.FrameInfo, sizeof(mfxFrameInfo));

        // fill output frame info
        MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.Out, &m_mfxVppParams.vpp.In, sizeof(mfxFrameInfo));

        m_mfxVppParams.vpp.Out.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;

        m_mfxVppParams.vpp.Out.CropW = (mfxU16) m_vppOutWidth;
        m_mfxVppParams.vpp.Out.CropH = (mfxU16) m_vppOutHeight;

        // If stream were interlaced before then 32 bit alignment were applied.
        // Discard 32 bit alignment as progressive doesn't require it.
        m_mfxVppParams.vpp.Out.Height = MSDK_ALIGN16(m_mfxVppParams.vpp.Out.CropH);
        m_mfxVppParams.vpp.Out.Width  = MSDK_ALIGN16(m_mfxVppParams.vpp.Out.CropW);

        int nFBS = m_mfxVppParams.mfx.FrameInfo.FrameRateExtN / m_mfxVppParams.mfx.FrameInfo.FrameRateExtD;
        if (nFBS > m_vppOutFrmaeRate)
        {
            m_mfxVppParams.vpp.Out.FrameRateExtN = (mfxU32)m_vppOutFrmaeRate;
            m_mfxVppParams.vpp.Out.FrameRateExtD = 1;
        }
    }

    sts = AllocFrames();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // Init decode
    if (m_pmfxDEC.get())
    {
        sts = m_pmfxDEC->Init(&m_mfxDecParams);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // Init VPP
    if (m_bVppIsUsed && m_pmfxVPP.get())
    {
        sts = m_pmfxVPP->Init(&m_mfxVppParams);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    sts = m_pmfxDEC->GetVideoParam(&m_mfxDecParams);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    m_bInit = true;
    return sts;
}

mfxStatus IntelES2YUV::AllocFrames()
{
    mfxStatus sts = MFX_ERR_NONE;

    mfxFrameAllocRequest Request;
    mfxFrameAllocRequest VppRequest[2];

    mfxU16 nSurfNum = 0; // number of surfaces for decoder
    mfxU16 nVppSurfNum = 0; // number of surfaces for vpp

    MSDK_ZERO_MEMORY(Request);
    MSDK_ZERO_MEMORY(VppRequest[0]);
    MSDK_ZERO_MEMORY(VppRequest[1]);

    sts = m_pmfxDEC->Query(&m_mfxDecParams, &m_mfxDecParams);
    MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // calculate number of surfaces required for decoder
    sts = m_pmfxDEC->QueryIOSurf(&m_mfxDecParams, &Request);
    if (MFX_WRN_PARTIAL_ACCELERATION == sts)
    {
        msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
        m_bDecOutSysmem = true;
    }
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    Request.Type |= MFX_MEMTYPE_SYSTEM_MEMORY;

    if (m_bVppIsUsed)
    {
        // respecify memory type between Decoder and VPP
        m_mfxDecParams.IOPattern = (mfxU16)( m_bDecOutSysmem ? MFX_IOPATTERN_OUT_SYSTEM_MEMORY : MFX_IOPATTERN_OUT_VIDEO_MEMORY);

        // recalculate number of surfaces required for decoder
        sts = m_pmfxDEC->QueryIOSurf(&m_mfxDecParams, &Request);
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        sts = InitVppParams();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        sts = m_pmfxVPP->Query(&m_mfxVppParams, &m_mfxVppParams);
        MSDK_IGNORE_MFX_STS(sts, MFX_WRN_INCOMPATIBLE_VIDEO_PARAM);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // VppRequest[0] for input frames request, VppRequest[1] for output frames request
        sts = m_pmfxVPP->QueryIOSurf(&m_mfxVppParams, VppRequest);
        if (MFX_WRN_PARTIAL_ACCELERATION == sts) {
            msdk_printf(MSDK_STRING("WARNING: partial acceleration\n"));
            MSDK_IGNORE_MFX_STS(sts, MFX_WRN_PARTIAL_ACCELERATION);
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        if ((VppRequest[0].NumFrameSuggested < m_mfxVppParams.AsyncDepth) ||
            (VppRequest[1].NumFrameSuggested < m_mfxVppParams.AsyncDepth))
            return MFX_ERR_MEMORY_ALLOC;


        // If surfaces are shared by 2 components, c1 and c2. NumSurf = c1_out + c2_in - AsyncDepth + 1
        // The number of surfaces shared by vpp input and decode output
        nSurfNum = Request.NumFrameSuggested + VppRequest[0].NumFrameSuggested - m_mfxDecParams.AsyncDepth + 1;

        // The number of surfaces for vpp output
        nVppSurfNum = VppRequest[1].NumFrameSuggested;

        // prepare allocation request
        Request.NumFrameSuggested = Request.NumFrameMin = nSurfNum;

        // surfaces are shared between vpp input and decode output
        Request.Type = MFX_MEMTYPE_EXTERNAL_FRAME | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN;

        Request.Type |= (m_bDecOutSysmem) ? MFX_MEMTYPE_SYSTEM_MEMORY : MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
    }
    

    // alloc frames for decoder
    sts = m_pGeneralAllocator->Alloc(m_pGeneralAllocator->pthis, &Request, &m_mfxResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (m_bVppIsUsed)
    {
        // alloc frames for VPP
        VppRequest[1].NumFrameSuggested = VppRequest[1].NumFrameMin = nVppSurfNum;
        MSDK_MEMCPY_VAR(VppRequest[1].Info, &(m_mfxVppParams.vpp.Out), sizeof(mfxFrameInfo));

        sts = m_pGeneralAllocator->Alloc(m_pGeneralAllocator->pthis, &VppRequest[1], &m_mfxVppResponse);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // prepare mfxFrameSurface1 array for decoder
        nVppSurfNum = m_mfxVppResponse.NumFrameActual;

        // AllocVppBuffers should call before AllocBuffers to set the value of m_OutputSurfacesNumber
        sts = AllocVppBuffers(nVppSurfNum);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // prepare mfxFrameSurface1 array for decoder
    nSurfNum = m_mfxResponse.NumFrameActual;

    sts = AllocBuffers(nSurfNum);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    for (int i = 0; i < nSurfNum; i++)
    {
        // initating each frame:
        MSDK_MEMCPY_VAR(m_pSurfaces[i].frame.Info, &(Request.Info), sizeof(mfxFrameInfo));

        m_pSurfaces[i].frame.Data.MemId = m_mfxResponse.mids[i];
    }

    // prepare mfxFrameSurface1 array for VPP
    for (int i = 0; i < nVppSurfNum; i++) {
        MSDK_MEMCPY_VAR(m_pVppSurfaces[i].frame.Info, &(VppRequest[1].Info), sizeof(mfxFrameInfo));
        m_pVppSurfaces[i].frame.Data.MemId = m_mfxVppResponse.mids[i];
    }

    return sts;
}

mfxStatus IntelES2YUV::InitVppParams()
{
    m_mfxVppParams.IOPattern = (mfxU16)( m_bDecOutSysmem ? MFX_IOPATTERN_IN_SYSTEM_MEMORY : MFX_IOPATTERN_IN_VIDEO_MEMORY );
    m_mfxVppParams.IOPattern |= MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    // 不能重新拷贝 会覆盖之前已经设置的参数
    //MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.In, &m_mfxDecParams.mfx.FrameInfo, sizeof(mfxFrameInfo));
    //MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.Out, &m_mfxVppParams.vpp.In, sizeof(mfxFrameInfo));

    if (m_fourcc)
    {
        m_mfxVppParams.vpp.Out.FourCC  = m_fourcc;
    }

    if (m_vppOutWidth && m_vppOutHeight)
    {
        m_mfxVppParams.vpp.Out.CropW = m_vppOutWidth;
        m_mfxVppParams.vpp.Out.Width = MSDK_ALIGN16(m_vppOutWidth);
        m_mfxVppParams.vpp.Out.CropH = m_vppOutHeight;
        m_mfxVppParams.vpp.Out.Height = (MFX_PICSTRUCT_PROGRESSIVE == m_mfxVppParams.vpp.Out.PicStruct)?
            MSDK_ALIGN16(m_vppOutHeight) : MSDK_ALIGN32(m_vppOutHeight);
    }

    m_mfxVppParams.AsyncDepth = m_mfxDecParams.AsyncDepth;

    m_VppExtParams.clear();
    m_VppDoNotUse.NumAlg = 4;

    m_VppDoNotUse.AlgList = new mfxU32 [m_VppDoNotUse.NumAlg];
    if (!m_VppDoNotUse.AlgList) return MFX_ERR_NULL_PTR;

    m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE; // turn off denoising (on by default)
    m_VppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS; // turn off scene analysis (on by default)
    m_VppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL; // turn off detail enhancement (on by default)
    m_VppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP; // turn off processing amplified (on by default)
    m_VppExtParams.push_back((mfxExtBuffer*)&m_VppDoNotUse);

    m_mfxVppParams.ExtParam = &m_VppExtParams[0];
    m_mfxVppParams.NumExtParam = (mfxU16)m_VppExtParams.size();
	
    return MFX_ERR_NONE;
}

bool IntelES2YUV::Decode(FrameBitStream & frame)
{
    mfxStatus sts = ReadBS(frame);  // B帧处理
    if (MFX_ERR_UNKNOWN == sts || MFX_ERR_MEMORY_ALLOC == sts) return sts;
	printf("Read a B frame\n");
    if (!m_bInit) // 初始化
    {
        sts = Init();
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        return sts == MFX_ERR_NONE ? true : false;
    }

    do
    {
        // if need more decoded frames
        // decode a frame
        sts = DecodeOneFrame(frame.ullPts);
        if (sts == MFX_ERR_MORE_DATA)
        {
            sts = MFX_ERR_NONE;
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    } while (0);

    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);
    return sts == MFX_ERR_NONE ? true : false;
}

mfxStatus IntelES2YUV::ReadBS(FrameBitStream & frame_in)
{
    // 缓存几帧
    // 缓存之前检查PTS，如果PTS跳变，则清空缓存，重新排序

    FrameBitStream frame = frame_in;

    if (m_bHasBFrame)
    {
        if (!m_FramePoolList.empty())
        {
            FrameBitStream & frame_tmp = m_FramePoolList.back();
            if (frame_tmp.ullPts > frame_in.ullPts && frame_tmp.ullPts - frame_in.ullPts > 50000)
            {
                log_info("[Input] large diff between two pts %llu --> %llu", frame_tmp.ullPts, frame_in.ullPts);
                if(frame_in.ullPts == 0)
                {
                    frame_in.ullPts = frame_tmp.ullPts;
                }
                else
                {
                    FreeBFrames();
                }
            }
        }

        FrameBitStream frame_to_push = frame_in;
        frame_to_push.pData = NULL;
        frame_to_push.pData = (mfxU8 *) malloc(frame_in.uLen);
        if (!frame_to_push.pData)
        {
            log_error("Cannot malloc frame to push");
            return MFX_ERR_MEMORY_ALLOC;
        }
        memcpy(frame_to_push.pData, frame_in.pData, frame_in.uLen);
        m_FramePoolList.push_back(frame_to_push);

        // PTS排序
        list<unsigned long long> PtsList;
        FramePoolList::iterator it = m_FramePoolList.begin();  
        for ( ; it != m_FramePoolList.end(); it++)
        {
            PtsList.push_back(it->ullPts);
        }

        PtsList.sort();

        it = m_FramePoolList.begin();  
        for ( ; it != m_FramePoolList.end(); it++)
        {
            it->ullPts = PtsList.front();
            PtsList.pop_front();
        }

        if (m_FramePoolList.size() < TRANS_B_FRAME_BUFFER_SIZE) return MFX_ERR_UNKNOWN;

        frame = m_FramePoolList.front();
        frame_in.ullPts = frame.ullPts;
    }
    else
    {
        FreeBFrames();
    }

    mfxStatus sts = MFX_ERR_NONE;
    if (NULL == m_pmfxBSExt)
    {
        m_pmfxBSExt = new mfxBitstream();
        MSDK_CHECK_POINTER(m_pmfxBSExt, MFX_ERR_NULL_PTR);
        MSDK_ZERO_MEMORY(*m_pmfxBSExt);

        sts = InitMfxBitstream(m_pmfxBSExt, MFX_BITSTREAM_LEN);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        //m_pmfxBSExt->DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
    }

    if (m_pmfxBSExt->DataOffset)
    {
        memmove(m_pmfxBSExt->Data, m_pmfxBSExt->Data + m_pmfxBSExt->DataOffset, m_pmfxBSExt->DataLength);
        m_pmfxBSExt->DataOffset = 0;
    }

    if (m_pmfxBSExt->MaxLength - m_pmfxBSExt->DataLength > frame.uLen)
    {
        memcpy(m_pmfxBSExt->Data + m_pmfxBSExt->DataLength, frame.pData, frame.uLen);
        m_pmfxBSExt->DataLength += frame.uLen;
    }
    else
    {
        log_info("m_pmfxBSExt Extend mfxBitstream !");
        mfxU32 uSize = m_pmfxBSExt->MaxLength * 2;
        while (uSize - m_pmfxBSExt->DataLength < frame.uLen)
        {
            uSize *= 2;
        }

        sts = ExtendMfxBitstream(m_pmfxBSExt, uSize);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        memcpy(m_pmfxBSExt->Data + m_pmfxBSExt->DataLength, frame.pData, frame.uLen);
        m_pmfxBSExt->DataLength += frame.uLen;
    }

    if (m_bHasBFrame)
    {
        SAFE_FREE(frame.pData);
        m_FramePoolList.pop_front();
    }

    return sts;
}

void IntelES2YUV::Close()
{
    if (m_bStop) return;
    m_bStop = true;

    if (m_pmfxDEC.get())
        m_pmfxDEC->Close();

    if(m_pmfxVPP.get())
        m_pmfxVPP->Close();

    // delete frames
    FreeBuffers();

    m_pCurrentFreeSurface = NULL;
    MSDK_SAFE_FREE(m_pCurrentFreeOutputSurface);
    m_pCurrentFreeVppSurface = NULL;

    if (m_pGeneralAllocator)
    {
        m_pGeneralAllocator->Free(m_pGeneralAllocator->pthis, &m_mfxResponse);;
    }
	
    if (m_pmfxBSExt)
    {
        WipeMfxBitstream(m_pmfxBSExt);
        SAFE_DELETE(m_pmfxBSExt);
    }

    FreeBFrames();

    MSDK_SAFE_DELETE_ARRAY(m_VppDoNotUse.AlgList);
    MSDK_SAFE_DELETE(m_pGeneralAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);
}

mfxStatus IntelES2YUV::DecodeOneFrame(unsigned long long ullPTS)
{
    mfxStatus sts = MFX_ERR_NONE;
    mfxFrameSurface1*   pOutSurface = NULL;
    mfxBitstream*       pBitstream = m_pmfxBSExt;

    while ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) 
    {
        if (pBitstream && MFX_ERR_MORE_DATA == sts)
        {
            break;  // 需要再次获取一帧数据
        }

        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) 
        {
            SyncFrameSurfaces();
            SyncVppFrameSurfaces();
            if (!m_pCurrentFreeSurface) 
            {
                m_pCurrentFreeSurface = m_FreeSurfacesPool.GetSurface();
            }
            if (!m_pCurrentFreeVppSurface) {
                m_pCurrentFreeVppSurface = m_FreeVppSurfacesPool.GetSurface();
            }

            //if (!m_pCurrentFreeSurface || (m_OutputSurfacesPool.GetSurfaceCount() == m_mfxDecParams.AsyncDepth)) 
            if (!m_pCurrentFreeSurface || (!m_pCurrentFreeVppSurface && m_bVppIsUsed) || (m_OutputSurfacesPool.GetSurfaceCount() == m_mfxDecParams.AsyncDepth))
            {
                // we stuck with no free surface available, now we will sync...
                sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
                if (MFX_ERR_MORE_DATA == sts) 
                {
                    msdk_printf(MSDK_STRING("fatal: failed to find output surface, that's a bug!\n"));
                    return MFX_ERR_NONE;
                }
                // note: MFX_WRN_IN_EXECUTION will also be treated as an error at this point
                continue;
            }

            if (!m_pCurrentFreeOutputSurface) 
            {
                m_pCurrentFreeOutputSurface = GetFreeOutputSurface();
            }

            if (!m_pCurrentFreeOutputSurface) 
            {
                sts = MFX_ERR_NOT_FOUND;
                break;
            }
        }

        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) 
        {
            pOutSurface = NULL;
            do {
                sts = m_pmfxDEC->DecodeFrameAsync(pBitstream, &(m_pCurrentFreeSurface->frame), &pOutSurface, &(m_pCurrentFreeOutputSurface->syncp));
                if (pBitstream && MFX_ERR_MORE_DATA == sts && pBitstream->MaxLength == pBitstream->DataLength)
                {
                    ExtendMfxBitstream(pBitstream, pBitstream->MaxLength * 2);
                }

                if (MFX_WRN_DEVICE_BUSY == sts) 
                {
                    mfxStatus _sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
                    // note: everything except MFX_ERR_NONE are errors at this point
                    if (MFX_ERR_NONE == _sts) {
                        sts = MFX_WRN_DEVICE_BUSY;
                    } else {
                        sts = _sts;
                        if (MFX_ERR_MORE_DATA == sts) {
                            // we can't receive MFX_ERR_MORE_DATA and have no output - that's a bug
                            sts = MFX_WRN_DEVICE_BUSY;//MFX_ERR_NOT_FOUND;
                        }
                    }
                }
            } while (MFX_WRN_DEVICE_BUSY == sts);

            if (sts > MFX_ERR_NONE) 
            {
                // ignoring warnings...
                if (m_pCurrentFreeOutputSurface->syncp) {
                    MSDK_SELF_CHECK(pOutSurface);
                    // output is available
                    sts = MFX_ERR_NONE;
                } else {
                    // output is not available
                    sts = MFX_ERR_MORE_SURFACE;
                }
            }
            else if ((MFX_ERR_MORE_DATA == sts) && !pBitstream) {
                // that's it - we reached end of stream; now we need to render bufferred data...
                do {
                    sts = SyncOutputSurface(MSDK_DEC_WAIT_INTERVAL);
                } while (MFX_ERR_NONE == sts);

                if (MFX_ERR_MORE_DATA == sts) {
                    sts = MFX_ERR_NONE;
                }
                break;
            } 
            else if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == sts) {
                //bErrIncompatibleVideoParams = true;
                //// need to go to the buffering loop prior to reset procedure
                //pBitstream = NULL;
                //sts = MFX_ERR_NONE;
                //continue;
                break;
            }
        }

        if ((MFX_ERR_NONE == sts) || (MFX_ERR_MORE_DATA == sts) || (MFX_ERR_MORE_SURFACE == sts)) 
        {
            // if current free surface is locked we are moving it to the used surfaces array
            /*if (m_pCurrentFreeSurface->frame.Data.Locked)*/ {
                m_UsedSurfacesPool.AddSurface(m_pCurrentFreeSurface);
                m_pCurrentFreeSurface = NULL;
            }
        }

        if (MFX_ERR_NONE == sts) 
        {
            if (m_bVppIsUsed)
            {
                do {
                    if ((m_pCurrentFreeVppSurface->frame.Info.CropW == 0) ||
                        (m_pCurrentFreeVppSurface->frame.Info.CropH == 0)) 
                    {
                        m_pCurrentFreeVppSurface->frame.Info.CropW = pOutSurface->Info.CropW;
                        m_pCurrentFreeVppSurface->frame.Info.CropH = pOutSurface->Info.CropH;
                        m_pCurrentFreeVppSurface->frame.Info.CropX = pOutSurface->Info.CropX;
                        m_pCurrentFreeVppSurface->frame.Info.CropY = pOutSurface->Info.CropY;
                    }
                    if (pOutSurface->Info.PicStruct != m_pCurrentFreeVppSurface->frame.Info.PicStruct) {
                        m_pCurrentFreeVppSurface->frame.Info.PicStruct = pOutSurface->Info.PicStruct;
                    }
                    if ((pOutSurface->Info.PicStruct == 0) && (m_pCurrentFreeVppSurface->frame.Info.PicStruct == 0)) {
                        m_pCurrentFreeVppSurface->frame.Info.PicStruct = pOutSurface->Info.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
                    }

                    sts = m_pmfxVPP->RunFrameVPPAsync(pOutSurface, &(m_pCurrentFreeVppSurface->frame), NULL, &(m_pCurrentFreeOutputSurface->syncp));

                    if (MFX_WRN_DEVICE_BUSY == sts) {
                        MSDK_SLEEP(1); // just wait and then repeat the same call to RunFrameVPPAsync
                    }
                } while (MFX_WRN_DEVICE_BUSY == sts);

                // process errors
                if (MFX_ERR_MORE_DATA == sts) { // will never happen actually
                    continue;
                }
                else if (MFX_ERR_NONE != sts) {
                    break;
                }

                m_UsedVppSurfacesPool.AddSurface(m_pCurrentFreeVppSurface);
                msdk_atomic_inc16(&(m_pCurrentFreeVppSurface->render_lock));

                m_pCurrentFreeOutputSurface->surface = m_pCurrentFreeVppSurface;
                m_pCurrentFreeOutputSurface->pts = ullPTS;		// 传入PTS值
                m_OutputSurfacesPool.AddSurface(m_pCurrentFreeOutputSurface);

                m_pCurrentFreeOutputSurface = NULL;
                m_pCurrentFreeVppSurface = NULL;
            }
            else
            {
                msdkFrameSurface* surface = FindUsedSurface(pOutSurface);
                msdk_atomic_inc16(&(surface->render_lock));

                m_pCurrentFreeOutputSurface->surface = surface;
                m_pCurrentFreeOutputSurface->pts = ullPTS;		// 传入PTS值
                m_OutputSurfacesPool.AddSurface(m_pCurrentFreeOutputSurface);
                m_pCurrentFreeOutputSurface = NULL;
            }
        }
    }

    return sts;
}

mfxStatus IntelES2YUV::SyncOutputSurface(mfxU32 wait)
{
    if (!m_pCurrentOutputSurface)
    {
        m_pCurrentOutputSurface = m_OutputSurfacesPool.GetSurface();
    }

    if (!m_pCurrentOutputSurface) 
    {
        return MFX_ERR_MORE_DATA;
    }

    mfxStatus sts = m_pmfxSession->SyncOperation(m_pCurrentOutputSurface->syncp, wait);

    if (MFX_WRN_IN_EXECUTION == sts) 
    {
        return sts;
    }

    if (MFX_ERR_NONE == sts) 
    {
        sts = m_pGeneralAllocator->Lock(m_pGeneralAllocator->pthis, m_pCurrentOutputSurface->surface->frame.Data.MemId, 
                                                        &(m_pCurrentOutputSurface->surface->frame.Data));
        if (MFX_ERR_NONE == sts) 
		{
#if 0
            m_pfnWriteYUVData(&(m_pCurrentOutputSurface->surface->frame), m_pCurrentOutputSurface->pts, m_nFrameRate, m_lpContext);
#else
            WriteYUVDataI420(&(m_pCurrentOutputSurface->surface->frame));
#endif
        }

        if (MFX_ERR_NONE != sts) 
        {
            sts = MFX_ERR_UNKNOWN;
        }
        ReturnSurfaceToBuffers(m_pCurrentOutputSurface);
        m_pCurrentOutputSurface = NULL;
    }

    if (MFX_ERR_NONE != sts) {
        sts = MFX_ERR_UNKNOWN;
    }

    return sts;
}

void IntelES2YUV::FreeBFrames()
{
    if (!m_FramePoolList.empty())
    {
        while (!m_FramePoolList.empty())
        {
            FrameBitStream frame;
            frame = m_FramePoolList.front();
            SAFE_FREE(frame.pData);
            m_FramePoolList.pop_front();
        }

        FramePoolList empty_pool;
        m_FramePoolList.swap(empty_pool);
    }
}

mfxStatus IntelES2YUV::WriteYUVDataI420(mfxFrameSurface1 *pSurface)
{
    printf("WriteYUVDataI420.......................\n");

    MSDK_CHECK_POINTER(pSurface,         MFX_ERR_NULL_PTR);

    mfxFrameInfo &pInfo = pSurface->Info;
    mfxFrameData &pData = pSurface->Data;

    mfxU32 i, j, h, w;
    mfxU32 vid = pInfo.FrameId.ViewId;

    static FILE *m_fDest = fopen("./test.yuv", "wb");
    MSDK_CHECK_POINTER(m_fDest, MFX_ERR_NULL_PTR);

    // Write Y
    switch (pInfo.FourCC)
    {
    case MFX_FOURCC_YV12:
    case MFX_FOURCC_NV12:
        {
            for (i = 0; i < pInfo.CropH; i++)
            {
                MSDK_CHECK_NOT_EQUAL(
                    fwrite(pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX)+ i * pData.Pitch, 1, pInfo.CropW, m_fDest),
                    pInfo.CropW, MFX_ERR_UNDEFINED_BEHAVIOR);
            }
            break;
        }
    default:
        {
            msdk_printf(MSDK_STRING("ERROR: I420 output is accessible only for NV12 and YV12.\n"));
            return MFX_ERR_UNSUPPORTED;
        }
    }

    // Write U and V
    switch (pInfo.FourCC)
    {
    case MFX_FOURCC_YV12:
        {
            for (i = 0; i < (mfxU32) pInfo.CropH/2; i++)
            {
                MSDK_CHECK_NOT_EQUAL(
                    fwrite(pData.U + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX / 2)+ i * pData.Pitch / 2, 1, pInfo.CropW/2, m_fDest),
                    (mfxU32)pInfo.CropW/2, MFX_ERR_UNDEFINED_BEHAVIOR);
            }
            for (i = 0; i < (mfxU32)pInfo.CropH/2; i++)
            {
                MSDK_CHECK_NOT_EQUAL(
                    fwrite(pData.V + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX / 2)+ i * pData.Pitch / 2, 1, pInfo.CropW/2, m_fDest),
                    (mfxU32)pInfo.CropW/2, MFX_ERR_UNDEFINED_BEHAVIOR);
            }
            break;
        }
    case MFX_FOURCC_NV12:
        {
            h = pInfo.CropH / 2;
            w = pInfo.CropW;
            for (i = 0; i < h; i++)
            {
                for (j = 0; j < w; j += 2)
                {
                    MSDK_CHECK_NOT_EQUAL(
                        fwrite(pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + i * pData.Pitch + j, 1, 1, m_fDest),
                        1, MFX_ERR_UNDEFINED_BEHAVIOR);
                }
            }
            for (i = 0; i < h; i++)
            {
                for (j = 1; j < w; j += 2)
                {
                    MSDK_CHECK_NOT_EQUAL(
                        fwrite(pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX)+ i * pData.Pitch + j, 1, 1, m_fDest),
                        1, MFX_ERR_UNDEFINED_BEHAVIOR);
                }
            }
            break;
        }
    default:
        {
            msdk_printf(MSDK_STRING("ERROR: I420 output is accessible only for NV12 and YV12.\n"));
            return MFX_ERR_UNSUPPORTED;
        }
    }

    return MFX_ERR_NONE;
}


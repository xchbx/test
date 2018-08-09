#include "VideoYUV2ES.h"
#include "OSTime.h"

// IMSDK全局唯一设备句柄
extern auto_ptr<CHWDevice>     g_hwdev;

VideoYUV2ES::VideoYUV2ES(TSOutParam &stTSOutParam)
: m_stTSOutParam(stTSOutParam)
, m_bStop(false)
, m_bInit(false)
, m_u64Pts(0)
, m_u64PtsPre(0)
, m_pmfxBS(NULL)
, m_pMFXAllocator(NULL)
, m_pmfxAllocatorParams(NULL)
, m_pEncSurfaces(NULL)
, m_pVppSurfaces(NULL)
{
    MSDK_ZERO_MEMORY(m_mfxVppParams);
	MSDK_ZERO_MEMORY(m_mfxEncParams);
    MSDK_ZERO_MEMORY(m_EncResponse);
    MSDK_ZERO_MEMORY(m_ExtHEVCParam);
    MSDK_ZERO_MEMORY(m_VppResponse);

    MSDK_ZERO_MEMORY(m_VppDoNotUse);
    m_VppDoNotUse.Header.BufferId = MFX_EXTBUFF_VPP_DONOTUSE;
    m_VppDoNotUse.Header.BufferSz = sizeof(mfxExtVPPDoNotUse);

    m_ExtHEVCParam.Header.BufferId = MFX_EXTBUFF_HEVC_PARAM;
    m_ExtHEVCParam.Header.BufferSz = sizeof(m_ExtHEVCParam);
}

VideoYUV2ES::~VideoYUV2ES()
{
	Close();
}

mfxStatus VideoYUV2ES::Init2(int nInWidth, int nInHeight, int nInFrameRate)
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
		// linux平台使用硬件加速
        VADisplay va_dpy = NULL;
		sts = g_hwdev->GetHandle(MFX_HANDLE_VA_DISPLAY, (mfxHDL*)&va_dpy);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

		sts = m_pmfxSession->SetHandle(MFX_HANDLE_VA_DISPLAY, va_dpy); 
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // create system memory allocator
        m_pMFXAllocator = new SysMemFrameAllocator;
        MSDK_CHECK_POINTER(m_pMFXAllocator, MFX_ERR_MEMORY_ALLOC);

        // initialize memory allocator
        sts = m_pMFXAllocator->Init(m_pmfxAllocatorParams);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
		// ******************************************************************************************
	}

    /*
        VPP组件初始化 两个用途
        1.在传入AVS的情况下(原始分辨率) 需要VPP(处理为目的分辨率) 需要yuv数据到Surface的转换
        2.在码流断流的情况下 需要使用VPP组件的m_pVppSurfaces内存存放黑场yuv数据 向外界发送
    */
    if (!m_pmfxVPP.get())
    {
        log_info("Init VPP: In W:%d H:%d --> Out W:%d H:%d", nInWidth, nInHeight, m_stTSOutParam.nWidth, m_stTSOutParam.nHeight);

        m_pmfxVPP.reset(new MFXVideoVPP(*m_pmfxSession.get()));

        m_mfxVppParams.AsyncDepth = 4;
        m_mfxVppParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

        m_mfxVppParams.vpp.In.FourCC    = MFX_FOURCC_NV12;
        m_mfxVppParams.vpp.In.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
        ConvertFrameRate(nInFrameRate, &m_mfxVppParams.vpp.In.FrameRateExtN, &m_mfxVppParams.vpp.In.FrameRateExtD);   // 输入帧率

        // width must be a multiple of 16 height must be a multiple of 16 in case of frame picture and a multiple of 32 in case of field picture
        m_mfxVppParams.vpp.In.Width     = MSDK_ALIGN16(nInWidth);    // 输入分辨率
        m_mfxVppParams.vpp.In.Height    = MSDK_ALIGN16(nInHeight);

        // set crops in input mfxFrameInfo for correct work of file reader  VPP itself ignores crops at initialization
        m_mfxVppParams.vpp.In.CropW = nInWidth;
        m_mfxVppParams.vpp.In.CropH = nInHeight;

        // fill output frame info
        MSDK_MEMCPY_VAR(m_mfxVppParams.vpp.Out,&m_mfxVppParams.vpp.In, sizeof(mfxFrameInfo));

        // only resizing is supported
        m_mfxVppParams.vpp.Out.Width = MSDK_ALIGN16(m_stTSOutParam.nWidth);
        m_mfxVppParams.vpp.Out.Height = MSDK_ALIGN16(m_stTSOutParam.nHeight);

        // configure and attach external parameters
        m_VppDoNotUse.NumAlg = 4;
        m_VppDoNotUse.AlgList = new mfxU32 [m_VppDoNotUse.NumAlg];
        MSDK_CHECK_POINTER(m_VppDoNotUse.AlgList,  MFX_ERR_MEMORY_ALLOC);

        m_VppDoNotUse.AlgList[0] = MFX_EXTBUFF_VPP_DENOISE; // turn off denoising (on by default)
        m_VppDoNotUse.AlgList[1] = MFX_EXTBUFF_VPP_SCENE_ANALYSIS; // turn off scene analysis (on by default)
        m_VppDoNotUse.AlgList[2] = MFX_EXTBUFF_VPP_DETAIL; // turn off detail enhancement (on by default)
        m_VppDoNotUse.AlgList[3] = MFX_EXTBUFF_VPP_PROCAMP; // turn off processing amplified (on by default)
        m_VppExtParams.push_back((mfxExtBuffer *)&m_VppDoNotUse);

        m_mfxVppParams.ExtParam = &m_VppExtParams[0]; // vector is stored linearly in memory
        m_mfxVppParams.NumExtParam = (mfxU16)m_VppExtParams.size();

        int nFBS = m_mfxVppParams.mfx.FrameInfo.FrameRateExtN / m_mfxVppParams.mfx.FrameInfo.FrameRateExtD;
        if (nFBS > m_stTSOutParam.nFrameRate)
        {
            m_mfxVppParams.vpp.Out.FrameRateExtN = (mfxU32)m_stTSOutParam.nFrameRate;
            m_mfxVppParams.vpp.Out.FrameRateExtD = 1;
        }
    }

	// Encode component initialization
	if (! m_pmfxENC.get())
	{
        unsigned int uEncCodecId = MFX_CODEC_AVC;
        if(m_stTSOutParam.nVStreamType == AV_CODEC_V_H264)
        {
            uEncCodecId = MFX_CODEC_AVC;
        }
        else if(m_stTSOutParam.nVStreamType == AV_CODEC_V_HEVC)
        {
            //加载 h265解码插件
            uEncCodecId = MFX_CODEC_HEVC;
            LoadPlugin(MFX_PLUGINTYPE_VIDEO_DECODE, *m_pmfxSession.get(), MFX_PLUGINID_HEVCE_HW, 1);
        }
        //else if(m_stTSOutParam.nVStreamType == AV_CODEC_V_MPEG2)
        //{
        //    uEncCodecId = MFX_CODEC_MPEG2;
        //}
        else
        {
            printf("Input EncStreamType:%d Not Support\n", uEncCodecId);
            return MFX_ERR_UNSUPPORTED;
        }

		m_pmfxENC.reset(new MFXVideoENCODE(*m_pmfxSession.get()));

		m_mfxEncParams.mfx.CodecId = uEncCodecId;
        m_mfxEncParams.mfx.TargetUsage = MFX_TARGETUSAGE_BALANCED;
        m_mfxEncParams.mfx.RateControlMethod = (mfxU16)MFX_RATECONTROL_CBR; // MFX_RATECONTROL_VBR
        m_mfxEncParams.mfx.GopRefDist = 1;  // Sample = 0
        m_mfxEncParams.mfx.GopPicSize = 12;	// I帧间隔
        m_mfxEncParams.mfx.NumRefFrame = 1;
        m_mfxEncParams.mfx.IdrInterval = 1;	// 转码h265必须 否则sps、pps信息只会出现一遍
        m_mfxEncParams.mfx.NumSlice = 0;
        m_mfxEncParams.mfx.FrameInfo.AspectRatioW = m_mfxEncParams.mfx.FrameInfo.AspectRatioH = 0;

        
        ConvertFrameRate(nInFrameRate, &m_mfxEncParams.vpp.In.FrameRateExtN, &m_mfxEncParams.vpp.In.FrameRateExtD);   // 输入帧率
        m_mfxEncParams.mfx.FrameInfo.Width  = MSDK_ALIGN16(m_stTSOutParam.nWidth);
        m_mfxEncParams.mfx.FrameInfo.Height = MSDK_ALIGN16(m_stTSOutParam.nHeight);

        m_mfxEncParams.mfx.FrameInfo.CropX = 0;
        m_mfxEncParams.mfx.FrameInfo.CropY = 0;
        m_mfxEncParams.mfx.FrameInfo.CropW = m_stTSOutParam.nWidth;
        m_mfxEncParams.mfx.FrameInfo.CropH = m_stTSOutParam.nHeight;


        if (m_stTSOutParam.nVRate)
        {
            m_mfxEncParams.mfx.TargetKbps = (mfxU16)m_stTSOutParam.nVRate;
        }
        else
        {
            mfxU16 nBitRate = CalculateDefaultBitrate(m_mfxEncParams.mfx.CodecId, m_mfxEncParams.mfx.TargetUsage,
                m_mfxEncParams.mfx.FrameInfo.Width, m_mfxEncParams.mfx.FrameInfo.Height,
                1.0 * m_mfxEncParams.mfx.FrameInfo.FrameRateExtN / m_mfxEncParams.mfx.FrameInfo.FrameRateExtD);

            m_mfxEncParams.mfx.TargetKbps = nBitRate;
        }

		m_mfxEncParams.mfx.FrameInfo.CropX = 0;
		m_mfxEncParams.mfx.FrameInfo.CropY = 0;
        m_mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_NV12;
        m_mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
        m_mfxEncParams.mfx.FrameInfo.PicStruct    = MFX_PICSTRUCT_PROGRESSIVE;

		m_mfxEncParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

        // In case of HEVC when height and/or width divided with 8 but not divided with 16
        // add extended parameter to increase performance
        if ( ( !((m_mfxEncParams.mfx.FrameInfo.CropW & 15 ) ^ 8 ) ||
            !((m_mfxEncParams.mfx.FrameInfo.CropH & 15 ) ^ 8 ) ) &&
            (m_mfxEncParams.mfx.CodecId == MFX_CODEC_HEVC) )
        {
            m_ExtHEVCParam.PicWidthInLumaSamples = m_mfxEncParams.mfx.FrameInfo.CropW;
            m_ExtHEVCParam.PicHeightInLumaSamples = m_mfxEncParams.mfx.FrameInfo.CropH;
            m_EncExtParams.push_back((mfxExtBuffer*)&m_ExtHEVCParam);
        }

        if (!m_EncExtParams.empty())
        {
            m_mfxEncParams.ExtParam = &m_EncExtParams[0]; // vector is stored linearly in memory
            m_mfxEncParams.NumExtParam = (mfxU16)m_EncExtParams.size();
        }

        m_mfxEncParams.AsyncDepth = 4;
	}

    sts = AllocFrames();
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    // Init VPP
    if (m_pmfxVPP.get())
    {
        sts = m_pmfxVPP->Init(&m_mfxVppParams);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

	// Init encode
	if (m_pmfxENC.get())
	{
		sts = m_pmfxENC->Init(&m_mfxEncParams);
		MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
	}

    //m_bInit = true;
	return sts;
}

mfxStatus VideoYUV2ES::AllocFrames()
{
    mfxStatus sts = MFX_ERR_NONE;
    mfxFrameAllocRequest EncRequest;
    mfxFrameAllocRequest VppRequest[2];
    mfxU16 nEncSurfNum = 0; // number of surfaces for encoder
    mfxU16 nVppSurfNum = 0; // number of surfaces for vpp

    MSDK_ZERO_MEMORY(EncRequest);
    MSDK_ZERO_MEMORY(VppRequest[0]);
    MSDK_ZERO_MEMORY(VppRequest[1]);

    // Calculate the number of surfaces for components.
    // QueryIOSurf functions tell how many surfaces are required to produce at least 1 output.
    // To achieve better performance we provide extra surfaces.
    // 1 extra surface at input allows to get 1 extra output.

    sts = m_pmfxENC.get()->QueryIOSurf(&m_mfxEncParams, &EncRequest);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if (EncRequest.NumFrameSuggested < m_mfxEncParams.AsyncDepth)
        return MFX_ERR_MEMORY_ALLOC;

    // The number of surfaces shared by vpp output and encode input.
    nEncSurfNum = EncRequest.NumFrameSuggested;

    if(m_pmfxVPP.get())
    {
        // VppRequest[0] for input frames request, VppRequest[1] for output frames request
        sts = m_pmfxVPP.get()->QueryIOSurf(&m_mfxVppParams, VppRequest);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        // The number of surfaces for vpp input - so that vpp can work at async depth = m_nAsyncDepth
        nVppSurfNum = VppRequest[0].NumFrameSuggested;
        // If surfaces are shared by 2 components, c1 and c2. NumSurf = c1_out + c2_in - AsyncDepth + 1
        nEncSurfNum += nVppSurfNum - m_mfxEncParams.AsyncDepth + 1;
    }

    // prepare allocation requests
    EncRequest.NumFrameSuggested = EncRequest.NumFrameMin = nEncSurfNum;
    MSDK_MEMCPY_VAR(EncRequest.Info, &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

    if(m_pmfxVPP.get())
    {
        EncRequest.Type |= MFX_MEMTYPE_FROM_VPPOUT; // surfaces are shared between vpp output and encode input
    }

    // alloc frames for encoderm_mfxEncResponse
    sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &EncRequest, &m_EncResponse);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if(m_pmfxVPP.get())
    {
        // alloc frames for vpp if vpp is enabled
        VppRequest[0].NumFrameSuggested = VppRequest[0].NumFrameMin = nVppSurfNum;
        MSDK_MEMCPY_VAR(VppRequest[0].Info, &(m_mfxVppParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

        sts = m_pMFXAllocator->Alloc(m_pMFXAllocator->pthis, &(VppRequest[0]), &m_VppResponse);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
    }

    // prepare mfxFrameSurface1 array for vpp if vpp is enabled
    if (m_pmfxVPP.get())
    {
        // prepare mfxFrameSurface1 array for encoder   暂不使用 由VideoES2YUV模块传入surface(当使用本模块的VPP时 需要使用本结构体)
        m_pEncSurfaces = new mfxFrameSurface1 [m_EncResponse.NumFrameActual];
        MSDK_CHECK_POINTER(m_pEncSurfaces, MFX_ERR_MEMORY_ALLOC);

        for (int i = 0; i < m_EncResponse.NumFrameActual; i++)
        {
            memset(&(m_pEncSurfaces[i]), 0, sizeof(mfxFrameSurface1));
            MSDK_MEMCPY_VAR(m_pEncSurfaces[i].Info, &(m_mfxEncParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

            // get YUV pointers
            sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_EncResponse.mids[i], &(m_pEncSurfaces[i].Data));
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }

        m_pVppSurfaces = new mfxFrameSurface1 [m_VppResponse.NumFrameActual];
        MSDK_CHECK_POINTER(m_pVppSurfaces, MFX_ERR_MEMORY_ALLOC);

        for (int i = 0; i < m_VppResponse.NumFrameActual; i++)
        {
            MSDK_ZERO_MEMORY(m_pVppSurfaces[i]);
            MSDK_MEMCPY_VAR(m_pVppSurfaces[i].Info, &(m_mfxVppParams.mfx.FrameInfo), sizeof(mfxFrameInfo));

            sts = m_pMFXAllocator->Lock(m_pMFXAllocator->pthis, m_VppResponse.mids[i], &(m_pVppSurfaces[i].Data));
            MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        }
    }

    if (NULL == m_pmfxBS)
    {
        m_pmfxBS = new mfxBitstream();
        MSDK_CHECK_POINTER(m_pmfxBS, MFX_ERR_NULL_PTR);
        MSDK_ZERO_MEMORY(*m_pmfxBS);

        sts = InitMfxBitstream(m_pmfxBS, MFX_BITSTREAM_LEN);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
        //m_pmfxBS->DataFlag = MFX_BITSTREAM_COMPLETE_FRAME;
    }

    return MFX_ERR_NONE;
}

bool VideoYUV2ES::Init(int nInFrameRate, int nInWidth, int nInHeight)
{
    if(m_bInit) return true;

    // 初始化
    mfxStatus sts = MFX_ERR_NONE;

    sts = Init2(nInWidth, nInHeight, nInFrameRate);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    if(sts >= MFX_ERR_NONE) m_bInit = true;

    return m_bInit;
}

mfxStatus VideoYUV2ES::Encode(mfxFrameSurface1 *surface, unsigned char* &pOutData, unsigned int &uOutLen, bool bNeedVPP)
{
    mfxStatus sts = MFX_ERR_NONE;

    do
    {
        sts = EncodeOneFrame(surface, pOutData, uOutLen, bNeedVPP);
        if (sts == MFX_ERR_MORE_DATA)
        {
            sts = MFX_ERR_NONE;
        }
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    } while (0);

    MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_DATA);

	return sts;
}

void VideoYUV2ES::Close()
{
    if (m_bStop) return;
    m_bStop = true;

    if (m_pmfxVPP.get())
        m_pmfxVPP->Close();

	if (m_pmfxENC.get())
		m_pmfxENC->Close();


    // delete surfaces array
    MSDK_SAFE_DELETE_ARRAY(m_pEncSurfaces);
    MSDK_SAFE_DELETE_ARRAY(m_pVppSurfaces);

    if (m_pMFXAllocator)
    {
        m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_EncResponse);
        m_pMFXAllocator->Free(m_pMFXAllocator->pthis, &m_VppResponse);
    }

    // delete allocator
    MSDK_SAFE_DELETE(m_pMFXAllocator);
    MSDK_SAFE_DELETE(m_pmfxAllocatorParams);

    if (m_pmfxBS)
    {
        WipeMfxBitstream(m_pmfxBS);
        SAFE_DELETE(m_pmfxBS);
    }

    MSDK_SAFE_DELETE_ARRAY(m_VppDoNotUse.AlgList);

    vector<mfxExtBuffer*> tmpVppExtParams;
    m_VppExtParams.swap(tmpVppExtParams);
}

mfxStatus VideoYUV2ES::EncodeOneFrame(mfxFrameSurface1 *surface, unsigned char* &pOutData, unsigned int &uOutLen, bool bNeedVPP)
{
    mfxStatus sts = MFX_ERR_NONE;
    mfxFrameSurface1* pSurf = NULL;
    mfxSyncPoint Syncp = NULL;
    mfxU16 nEncSurfIdx = 0;     // index of free surface for encoder input (vpp output)

    // Since in sample we support just 2 views
    // we will change this value between 0 and 1 in case of MVC
    mfxU16 currViewNum = 0;

    // main loop, preprocessing and encoding
    while (MFX_ERR_NONE <= sts || MFX_ERR_MORE_DATA == sts)
    {
        // point pSurf to encoder surface
        //pSurf = &m_pEncSurfaces[nEncSurfIdx];
        //pSurf->Data = pExtendedSurface->pSurface->Data;

        pSurf = surface;
        pSurf->Info.FrameId.ViewId = currViewNum;

        if (bNeedVPP && m_pmfxVPP.get())
        {
            // m_pEncSurfaces存储VPP后的数据
            nEncSurfIdx = GetFreeSurface(m_pEncSurfaces, m_EncResponse.NumFrameActual);
            MSDK_CHECK_ERROR(nEncSurfIdx, MSDK_INVALID_SURF_IDX, MFX_ERR_MEMORY_ALLOC);

            for (;;)
            {
                sts = m_pmfxVPP->RunFrameVPPAsync(pSurf, &m_pEncSurfaces[nEncSurfIdx], NULL, &Syncp);
                if (MFX_ERR_NONE < sts && !Syncp) // repeat the call if warning and no output
                {
                    if (MFX_WRN_DEVICE_BUSY == sts)
                        MSDK_SLEEP(1);  // wait if device is busy
                }
                else if (MFX_ERR_NONE < sts && Syncp)
                {
                    sts = MFX_ERR_NONE; // ignore warnings if output is available
                    break;
                }
                else
                    break; // not a warning
            }

            // process errors
            if (MFX_ERR_MORE_DATA == sts)
            {
                //continue;
                return MFX_ERR_MORE_DATA;
            }
            else if (MFX_ERR_MORE_SURFACE == sts)
            {
                //bVppMultipleOutput = true;
            }
            else
            {
                MSDK_BREAK_ON_ERROR(sts);
            }

            // save the id of preceding vpp task which will produce input data for the encode task
            if (Syncp)
            {
                //pExtendedSurface->DependentVppTasks.push_back(Syncp);
                Syncp = NULL;
            }
        }


        for (;;)
        {
            // at this point surface for encoder contains either a frame from file or a frame processed by vpp
            if(bNeedVPP && m_pmfxVPP.get())
            {
                sts = m_pmfxENC->EncodeFrameAsync(NULL, &m_pEncSurfaces[nEncSurfIdx], m_pmfxBS, &Syncp);
            }
            else
            {
                sts = m_pmfxENC->EncodeFrameAsync(NULL, pSurf, m_pmfxBS, &Syncp);
            }


            if (MFX_ERR_NONE < sts && !Syncp) // repeat the call if warning and no output
            {
                if (MFX_WRN_DEVICE_BUSY == sts)
                    MSDK_SLEEP(1); // wait if device is busy
            }
            else if (MFX_ERR_NONE < sts && Syncp)
            {
                sts = MFX_ERR_NONE; // ignore warnings if output is available
                break;
            }
            else if (MFX_ERR_NOT_ENOUGH_BUFFER == sts)
            {
                sts = AllocateSufficientBuffer(m_pmfxBS);
                MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
            }
            else
            {
                // get next surface and new task for 2nd bitstream in ViewOutput mode
                MSDK_IGNORE_MFX_STS(sts, MFX_ERR_MORE_BITSTREAM);
                break;
            }
        }

        // 获取一帧ES数据 退出本次循环
        if (NULL != Syncp)
        {
            sts = m_pmfxSession->SyncOperation(Syncp, MSDK_WAIT_INTERVAL);
            if (MFX_ERR_NONE == sts)
            {
#if 1
                pOutData = m_pmfxBS->Data + m_pmfxBS->DataOffset;
                uOutLen = m_pmfxBS->DataLength;
#else
                static FILE * fp = fopen("test.es", "wb+");
                if (fp) fwrite(m_pmfxBS->Data + m_pmfxBS->DataOffset, 1, m_pmfxBS->DataLength, fp);
#endif
                m_pmfxBS->DataOffset = 0;
                m_pmfxBS->DataLength = 0;
            }
        }

        return MFX_ERR_NONE;
    }

    return sts;
}

mfxStatus VideoYUV2ES::AllocateSufficientBuffer(mfxBitstream* pBS) 
{
	MSDK_CHECK_POINTER(pBS, MFX_ERR_NULL_PTR);

	mfxVideoParam par; 
	MSDK_ZERO_MEMORY(par);

	// find out the required buffer size
	mfxStatus sts = m_pmfxENC->GetVideoParam(&par);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	mfxU32 new_size = 0; 
	// if encoder provided us information about buffer size
	if (0 != par.mfx.BufferSizeInKB)
	{    
		new_size = par.mfx.BufferSizeInKB * 1000;
	}    
	else 
	{    
		// trying to guess the size (e.g. for JPEG encoder)
		new_size = (0 == pBS->MaxLength)
			// some heuristic init value
			? 4 + (par.mfx.FrameInfo.Width * par.mfx.FrameInfo.Height * 3 + 1023)
			// double existing size
			: 2 * pBS->MaxLength;
	}    

	sts = ExtendMfxBitstream(pBS, new_size);
	MSDK_CHECK_RESULT_SAFE(sts, MFX_ERR_NONE, sts, WipeMfxBitstream(pBS));

	return MFX_ERR_NONE;
} 

mfxU16 VideoYUV2ES::GetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize)
{
    mfxU32 SleepInterval = 10; // milliseconds

    mfxU16 idx = MSDK_INVALID_SURF_IDX;

    //wait if there's no free surface
    for (mfxU32 i = 0; i < MSDK_SURFACE_WAIT_INTERVAL; i += SleepInterval)
    {
        idx = GetFreeSurfaceIndex(pSurfacesPool, nPoolSize);

        if (MSDK_INVALID_SURF_IDX != idx)
        {
            break;
        }
        else
        {
            MSDK_SLEEP(SleepInterval);
        }
    }

    if(idx==MSDK_INVALID_SURF_IDX)
    {
        msdk_printf(MSDK_STRING("ERROR: No free surfaces in pool (during long period)\n"));
    }

    return idx;
}

mfxFrameSurface1* VideoYUV2ES::Package(unsigned char *YAddr, int nYLinesize, unsigned char *UAddr, int nULinesize, unsigned char *VAddr, int nVLinesize)
{
    // find free surface for encode vpp input
    mfxU16 nVppSurfIdx = GetFreeSurface(m_pVppSurfaces, m_VppResponse.NumFrameActual);
    if(MSDK_INVALID_SURF_IDX == nVppSurfIdx)
        return NULL;

    mfxFrameSurface1* pSurface = &m_pVppSurfaces[nVppSurfIdx];
    mfxFrameInfo& pInfo = pSurface->Info;
    mfxFrameData& pData = pSurface->Data;

    // this reader supports only NV12 mfx surfaces for code transparency,
    // other formats may be added if application requires such functionality
    if (MFX_FOURCC_NV12 != pInfo.FourCC && MFX_FOURCC_YV12 != pInfo.FourCC)
    {
        return NULL;
    }

    //printf("VideoYUV2ES::Package w:%d h:%d picth:%d CC:%d\n", w, h, pitch, pInfo.FourCC);
    mfxU16 w, h, i;
    if (pInfo.CropH > 0 && pInfo.CropW > 0)
    {
        w = pInfo.CropW;
        h = pInfo.CropH;
    }
    else
    {
        w = pInfo.Width;
        h = pInfo.Height;
    }

    mfxU16 pitch = pData.Pitch;
    mfxU8 *ptr = pData.Y + pInfo.CropX + pInfo.CropY * pData.Pitch;
    int first = 0;
    int second = 1;


    /*
        下面的数据拷贝 以AVSES2YUV.cpp中WriteYUVDataI420函数 以及 IMSDK_Sample中读取文件数据方式 作为参考
        注意 这里h/=2 下面每个i拷贝2*w的数据
    */
    h /= 2;
    int nPicthNum = 0;
    for(i = 0; i < h; ++i)
    {
        //fread(ptr + i * pitch, 1, w, m_fSource);

        // 分批挪数据
        memcpy(ptr + nPicthNum * pitch, YAddr + nYLinesize * (i * 2 + first), w);
        nPicthNum++;

        memcpy(ptr + nPicthNum * pitch, YAddr + nYLinesize * (i * 2 + second), w);
        nPicthNum++;
    }

    // YUV420 is implied
    switch (pInfo.FourCC)
    {
    case MFX_FOURCC_NV12:
        ptr = pData.UV + pInfo.CropX + (pInfo.CropY / 2) * pitch;

        // 注意 这里h之前已经/=2了 这里再次/=2 相当于h/=4
        h /= 2;
        w /= 2;

        // load U
        nPicthNum = 0;
        for (i = 0; i < h; i++)
        {
            //fread(buf, 1, w, m_fSource);
            //for (j = 0; j < w; j++)
            //{
            //    ptr[i * pitch + j * 2] = buf[j];
            //}

            for (mfxU32 j = 0; j < w; j++)
            {
                ptr[nPicthNum * pitch + j * 2] = UAddr[nULinesize * (i * 2 + first) + j];
            }
            nPicthNum++;

            for(mfxU32 j = 0; j < w; j++)
            {
                ptr[nPicthNum * pitch + j * 2] = UAddr[nULinesize * (i * 2 + second) + j];
            }
            nPicthNum++;
        }

        // load V
        nPicthNum = 0;
        for (i = 0; i < h; i++)
        {
            //fread(buf, 1, w, m_fSource);
            //for (j = 0; j < w; j++)
            //{
            //    ptr[i * pitch + j * 2 + 1] = buf[j];
            //}

            for (mfxU32 j = 0; j < w; j++)
            {
                ptr[nPicthNum * pitch + j * 2 + 1] = VAddr[nVLinesize * (i * 2 + first) + j];
            }
            nPicthNum++;

            for (mfxU32 j = 0; j < w; j++)
            {
                ptr[nPicthNum * pitch + j * 2 + 1] = VAddr[nVLinesize * (i * 2 + second) + j];
            }
            nPicthNum++;
        }

        break;
    default:
        return NULL;
    }

    return pSurface;
}


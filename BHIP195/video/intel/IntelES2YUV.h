#ifndef __ES_TRANSCODE_H__
#define __ES_TRANSCODE_H__

#include "IntelCommon.h"
#include "ChannelStruct.h"
#include "OSTime.h"
#include "VideoES2YUV.h"


class IntelES2YUV : public CBuffering, public VideoES2YUV
{
public:
	IntelES2YUV(int nDecStreamType, int nVppOutWidth, int nVppOutHeight, int nVppOutFrameRate);
	virtual ~IntelES2YUV();

    void SetHasBFrame(bool bHasBFrame) { m_bHasBFrame = bHasBFrame; }
	bool Decode(FrameBitStream & frame);
    

private:
	mfxStatus Init();		// 初始化
    void      Close();		// 关闭

	mfxStatus ReadBS(FrameBitStream & frame);

	mfxStatus DecodeOneFrame(unsigned long long ullPTS);
    mfxStatus SyncOutputSurface(mfxU32 wait);
    mfxStatus WriteYUVDataI420(mfxFrameSurface1 *pSurface);

    void      FreeBFrames();
	mfxStatus AllocFrames();
    mfxStatus InitVppParams();

private:
    int                     m_nDecStreamType;	// 解码视频类型
    int                     m_nFrameRate;

    bool                    m_bStop;
	bool					m_bInit;	        // 是否初始化
    bool                    m_bHasBFrame;       // 是否有B帧

    mfxU64                  m_u64Pts;
    mfxU64                  m_u64PtsPre;


private:
	mfxBitstream		           *m_pmfxBSExt;

	// need to work with HW pipeline
    GeneralAllocator*               m_pGeneralAllocator;
    mfxAllocatorParams*             m_pmfxAllocatorParams;
    bool                            m_bDecOutSysmem; // use system memory between Decoder and VPP, if false - video memory

    mfxU32                          m_fourcc; // color format of vpp out, i420 by default

    bool                            m_bVppIsUsed;
    mfxU16                          m_vppOutWidth;
    mfxU16                          m_vppOutHeight;
    mfxU16                          m_vppOutFrmaeRate;
    std::vector<mfxExtBuffer*>      m_VppExtParams;

    // Session  Decoder
    auto_ptr<MFXVideoSession>		m_pmfxSession;
    mfxVideoParam			        m_mfxDecParams;
    mfxVideoParam			        m_mfxVppParams;
    auto_ptr<MFXVideoDECODE>		m_pmfxDEC;
    auto_ptr<MFXVideoVPP>			m_pmfxVPP;

    // for disabling VPP algorithms
    mfxExtVPPDoNotUse               m_VppDoNotUse;


    mfxFrameAllocResponse           m_mfxResponse; // memory allocation response for decoder
    mfxFrameAllocResponse           m_mfxVppResponse;   // memory allocation response for vpp

    msdkFrameSurface*               m_pCurrentFreeSurface; // surface detached from free surfaces array
    msdkFrameSurface*               m_pCurrentFreeVppSurface; // VPP surface detached from free VPP surfaces array
    msdkOutputSurface*              m_pCurrentFreeOutputSurface; // surface detached from free output surfaces array
    msdkOutputSurface*              m_pCurrentOutputSurface; // surface detached from output surfaces array

	// transcoding pipeline specific
    typedef std::list<FrameBitStream>   FramePoolList;
    FramePoolList                   m_FramePoolList;
};

#endif


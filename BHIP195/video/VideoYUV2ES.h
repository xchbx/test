#ifndef __ES_TRANSCODE1_H__
#define __ES_TRANSCODE1_H__

#include "Common.h"
#include "IntelCommon.h"
#include "ChannelStruct.h"
#include <stddef.h>
#include "OSTime.h"


class VideoYUV2ES
{
public:
	VideoYUV2ES(TSOutParam &stTSOutParam);
	virtual ~VideoYUV2ES();

    bool Init(int nInFrameRate, int nInWidth, int nInHeight);  // 初始化
	mfxStatus Encode(mfxFrameSurface1 *surface, unsigned char* &pOutData, unsigned int &uOutLen, bool bNeedVPP = false);

    mfxFrameSurface1* Package(unsigned char *YAddr, int nYLinesize, unsigned char *UAddr, int nULinesize, unsigned char *VAddr, int nVLinesize);

private:
	mfxStatus Init2(int nInWidth, int nInHeight, int nInFrameRate);
    void      Close();		// 关闭

    // 缩放并编码一帧数据
    mfxStatus EncodeOneFrame(mfxFrameSurface1 *surface, unsigned char* &pOutData, unsigned int &uOutLen, bool bNeedVPP);

    mfxStatus AllocFrames();
	mfxStatus AllocateSufficientBuffer(mfxBitstream* pBS);

	mfxU16  GetFreeSurface(mfxFrameSurface1* pSurfacesPool, mfxU16 nPoolSize);


private:
    TSOutParam              m_stTSOutParam;

    bool                    m_bStop;
	bool					m_bInit;	// 是否初始化

    mfxU64                  m_u64Pts;
    mfxU64                  m_u64PtsPre;


private:
	// need to work with HW pipeline
    mfxBitstream		          * m_pmfxBS;
    MFXFrameAllocator             * m_pMFXAllocator;
    mfxAllocatorParams            * m_pmfxAllocatorParams;
    // HEVC
    mfxExtHEVCParam                 m_ExtHEVCParam;

	auto_ptr<MFXVideoSession>		m_pmfxSession;
	auto_ptr<MFXVideoENCODE>		m_pmfxENC;
    auto_ptr<MFXVideoVPP>			m_pmfxVPP;


    mfxFrameSurface1      * m_pEncSurfaces; // frames array for encoder input (vpp output)
    mfxFrameSurface1      * m_pVppSurfaces; // frames array for vpp input

	mfxFrameAllocResponse	m_EncResponse;  // memory allocation response for encoder
    mfxFrameAllocResponse   m_VppResponse;  // memory allocation response for vpp

	mfxVideoParam			m_mfxEncParams;
    vector<mfxExtBuffer*>   m_EncExtParams;
    mfxVideoParam			m_mfxVppParams;
    vector<mfxExtBuffer*>   m_VppExtParams;

    // for disabling VPP algorithms
    mfxExtVPPDoNotUse       m_VppDoNotUse;
};

#endif


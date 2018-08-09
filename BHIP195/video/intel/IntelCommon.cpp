#include "IntelCommon.h"

auto_ptr<CHWDevice>     g_hwdev;


void IMSDK_INIT()
{
    g_hwdev.reset(CreateVAAPIDevice());
    mfxStatus sts = g_hwdev->Init(NULL, 0, MSDKAdapter::GetNumber());
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
}


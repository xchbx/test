#pragma once

#include <stddef.h>
#include <memory>
#include <vector>
#include <list>

#include "Common.h"

#include "vm/atomic_defs.h"
#include "vm/thread_defs.h"

#include "sample_defs.h"
#include "sample_utils.h"

#include "mfxvideo.h"
#include "mfxvideo++.h"

#include "mfxplugin++.h"
#include "plugin_utils.h"
#include "plugin_loader.h"
#include "mfx_buffering.h"
#include "sysmem_allocator.h"

#include "vaapi_device.h"
#include "vaapi_utils.h"
#include "vaapi_allocator.h"
#include "general_allocator.h"


void IMSDK_INIT();


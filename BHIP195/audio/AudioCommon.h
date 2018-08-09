#pragma once

#include "Common.h"

#define AUDIO_BUF_SIZE				8192		// Swr重采样的数据大小

#define AUDIO_AAC_PER_FRAME_SIZE    4096        // AAC每帧数据大小(1024 * 4)
#define AUDIO_MEPG2_PER_FRAME_SIZE  4608        // MPEG2每帧数据大小(1152 * 4)

#define AUDIO_SWR_CHANNELS          2           // 音频重采样声道数
#define AUDIO_SWR_BIT_SAMPLE        16          // 音频重采样bit sample
#define AUDIO_SWR_SAMPLEING         48000       // 音频重采样采样率


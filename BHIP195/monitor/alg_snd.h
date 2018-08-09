#ifndef __ALG_SND_H__
#define __ALG_SND_H__

#include "Common.h"
#include "ChannelStruct.h"


#define SND_SAMPLE_NUM     				1052
#define MAX_PCM_SAMPLE_PER_FRAME		2048

struct ALG_AUDIO_Infos{
    unsigned int  uLVolumeMax;
    unsigned int  uLVolumeMin;
    unsigned int  uRVolumeMax;
    unsigned int  uRVolumeMin;
    unsigned long long  ullLasttime;

    ALG_AUDIO_Infos(): uLVolumeMax(0), uLVolumeMin(0), uRVolumeMax(0), uRVolumeMin(0), ullLasttime(0)
    {}
};


int alg_snd_process(MonitorAThrld &stThrldAudio, ALG_AUDIO_Infos &stAudioInfos, ALG_AUDIO_Infos &stAudioPrintInfos, 
    int arrCalcCnt[][3], int arrWarnCnt[][3], signed short *pData, unsigned int nAudioMode, unsigned int nSamples, unsigned int nBitPerSample, unsigned short *uStatue);

int sound_monitor_info(MonitorAThrld &stThrldAudio, char *p_buffer);
int alg_snd_get_volume(int iCh, int *volumeL, int *volumeR);

#endif


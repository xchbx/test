#include <math.h>
#include "alg_snd.h"
#include "OSTime.h"


#define MAX_SOUND_CHN   		6       //IP95_TRANSCODE_CHN_NUM
#define AUDIO_NO_DATA_RATIO		0.7

#define AUDIO_LOSS_INDEX		0
#define AUDIO_NO_SOUND          1		// 无伴音
#define AUDIO_HIGH_INDEX		2       // 音量过高
#define AUDIO_LOW_INDEX		    3       // 音量过低



// 把得到的音频数据转换为LOG值
unsigned short Decibels(unsigned short nOldValue) 
{
	int nLog ;
	if(nOldValue == 0)
		nLog = -96;
	else
		nLog = (int)(20.0 * log10((double)nOldValue/(double)32767));

	if(nLog < -96 )
	{
		nLog = -96 ;
	}
	return (unsigned short)(nLog + 96);
}

// 将音量分贝值映射到音量柱高度值
double ConvertVolume(unsigned short nValue)
{
	double dValueInLevel = 0.0;

	if (nValue > 96)
		nValue = 96;

	if (nValue <= 60)
	{
		dValueInLevel = (double)(nValue * 25.0) / 60.0;//0~60映射到0～25上	
	}
	else
	{
		//nValue = nValue * 6 - 360;//0~60映射到0～255上	()
		dValueInLevel = (double)((nValue-60.0)*(255.0-25.0)/(96.0-60.0) + 25.0);
		//nValue = (Uint16) nValueInLevel;
	}

	if (dValueInLevel > 255)
		dValueInLevel = 255;

	return dValueInLevel;
}

// 把得到的音频数据转换为DB音量值
double TransformAudioData(unsigned short nValue)
{
	//先进行Log计算
	unsigned short nTemp = Decibels(nValue);

	//再进行区间映射
	return (ConvertVolume(nTemp));
}

int alg_snd_process(MonitorAThrld &stThrldAudio, ALG_AUDIO_Infos &stAudioInfos, ALG_AUDIO_Infos &stAudioPrintInfos, 
    int arrCalcCnt[][3], int arrWarnCnt[][3], signed short *pData, unsigned int nAudioMode, unsigned int nSamples, unsigned int nBitPerSample, unsigned short *uStatue)
{
	unsigned short usTempVolume = 0;
	bool bALossLeft = false, bALossRight = false;
	bool bAHighLeft = false, bAHighRight = false;
	bool bALowLeft = false, bALowRight = false;
	double dLVolumeMax = 0.0, dLVolumeMin = 0.0, dRVolumeMax = 0.0, dRVolumeMin = 0.0;
	double dHighGate = 0.0, dLowGate = 0.0, dLossGate = 0.0,dLossLower = 0.0;
	unsigned int i = 0;
	unsigned short Lstatus = 0, Rstatus = 0;

	//判断采样精度,目前的音频监测算法只支持16或24的采样精度	
	if (nBitPerSample != 16 && nBitPerSample != 24)		
	{
		log_error("BitPerSample %d is not support!", nBitPerSample);
		return -1;    
	}
	
	// 取音量最大最小值
	if (1 == nAudioMode)
	{
		if (nBitPerSample == 16)
		{
			for (i = 0; i < nSamples; i++)
			{	
				usTempVolume = abs(pData[i]);
				if (usTempVolume > stAudioInfos.uLVolumeMax)
					stAudioInfos.uLVolumeMax = usTempVolume;

				if (usTempVolume < stAudioInfos.uLVolumeMin)
					stAudioInfos.uLVolumeMin = usTempVolume;
			}
		}
		else if (nBitPerSample == 24)
		{			for (i = 0; i < nSamples; i++)
			{				
				usTempVolume = abs(pData[2*i+1]);
				if(usTempVolume > stAudioInfos.uLVolumeMax)
					stAudioInfos.uLVolumeMax = usTempVolume;

				if(usTempVolume < stAudioInfos.uLVolumeMin)
					stAudioInfos.uLVolumeMin = usTempVolume;
			}	
		}
	}
	else if (nAudioMode > 1)
	{
		if (nBitPerSample == 16)
		{
			for (i = 0; i < nSamples; i++)
			{
				//左	
				usTempVolume = abs(pData[2*i]);
				if(usTempVolume >  stAudioInfos.uLVolumeMax)
					 stAudioInfos.uLVolumeMax = usTempVolume;
				if(usTempVolume <  stAudioInfos.uLVolumeMin)
					 stAudioInfos.uLVolumeMin = usTempVolume;

				//右
				usTempVolume = abs(pData[2*i+1]);
				if(usTempVolume >  stAudioInfos.uRVolumeMax)
					stAudioInfos.uRVolumeMax = usTempVolume;
				if(usTempVolume < stAudioInfos.uRVolumeMin)
					stAudioInfos.uRVolumeMin = usTempVolume;			
			}	
		}
		else if (nBitPerSample == 24)
		{			for (i = 0; i < nSamples; i++)
			{
				//左	
				usTempVolume = abs(pData[4*i+1]);
				if (usTempVolume >  stAudioInfos.uLVolumeMax)
					 stAudioInfos.uLVolumeMax = usTempVolume;
				if (usTempVolume <  stAudioInfos.uLVolumeMin)
					 stAudioInfos.uLVolumeMin = usTempVolume;

				//右
				usTempVolume = abs(pData[4*i+3]);
				if (usTempVolume >  stAudioInfos.uRVolumeMax)
					stAudioInfos.uRVolumeMax = usTempVolume;
				if (usTempVolume < stAudioInfos.uRVolumeMin)
					stAudioInfos.uRVolumeMin = usTempVolume;			
			}				
		}
	}
	
	// 进行音量值从 16bit的量化系数，到音量柱高度范围（0-255）的转换
	dLVolumeMax = TransformAudioData(stAudioInfos.uLVolumeMax);
	dLVolumeMin = TransformAudioData(stAudioInfos.uLVolumeMin);
	dRVolumeMax = TransformAudioData(stAudioInfos.uRVolumeMax);
	dRVolumeMin = TransformAudioData(stAudioInfos.uRVolumeMin);
	
	//记录音量，给OSD显示
	//g_VolumeMaxL[iCh] = (unsigned int)dLVolumeMax;
	//g_VolumeMaxR[iCh] = (unsigned int)dRVolumeMax;
	
	stAudioPrintInfos.uLVolumeMax = stAudioInfos.uLVolumeMax;
	stAudioPrintInfos.uLVolumeMin = stAudioInfos.uLVolumeMin;
	stAudioPrintInfos.uRVolumeMax = stAudioInfos.uRVolumeMax;
	stAudioPrintInfos.uRVolumeMin = stAudioInfos.uRVolumeMin;

	//为了打印
	int iLVolumeMax = Decibels(stAudioPrintInfos.uLVolumeMax) - 96;
	int iLVolumeMin = Decibels(stAudioPrintInfos.uLVolumeMin) - 96;
	int iRVolumeMax = Decibels(stAudioPrintInfos.uRVolumeMax) - 96;
	int iRVolumeMin = Decibels(stAudioPrintInfos.uRVolumeMin) - 96;
	
	stAudioInfos.uRVolumeMin= 65535;
	stAudioInfos.uRVolumeMax = 0;
	stAudioInfos.uLVolumeMin= 65535;
	stAudioInfos.uLVolumeMax = 0;

	dHighGate = ConvertVolume(96 + stThrldAudio.nHigher);
	dLowGate = ConvertVolume(96 + stThrldAudio.nLower);
	dLossGate = (255 * stThrldAudio.nLost) / 96;
	dLossLower =  ConvertVolume(96 + stThrldAudio.nLostLower);
	//dLossLower =  ConvertVolume(52);

	// 根据门限判断音频丢失、音量过高过低
	bALossLeft = ((dLVolumeMax - dLVolumeMin < dLossGate) || ( dLVolumeMax < dLossLower));
	bALossRight = ((dRVolumeMax - dRVolumeMin < dLossGate) || ( dRVolumeMax < dLossLower));
	bAHighLeft = dLVolumeMax > dHighGate;
	bAHighRight = dRVolumeMax > dHighGate;
	bALowLeft = dLVolumeMax < dLowGate;
	bALowRight = dRVolumeMax < dLowGate;

	//统计采样次数
	arrCalcCnt[0][AUDIO_NO_SOUND]++;	
	arrCalcCnt[0][AUDIO_LOW_INDEX]++;
	arrCalcCnt[0][AUDIO_HIGH_INDEX]++;
	arrCalcCnt[1][AUDIO_NO_SOUND]++;	
	arrCalcCnt[1][AUDIO_LOW_INDEX]++;
	arrCalcCnt[1][AUDIO_HIGH_INDEX]++;
	
	if (bALossLeft)
		arrWarnCnt[0][AUDIO_NO_SOUND]++;	//左
	if (bALossRight)
		arrWarnCnt[1][AUDIO_NO_SOUND]++;	//右
	
	if (bAHighLeft)
		arrWarnCnt[0][AUDIO_HIGH_INDEX]++;	
	if (bAHighRight)
		arrWarnCnt[1][AUDIO_HIGH_INDEX]++;	

	if (bALowLeft)
		arrWarnCnt[0][AUDIO_LOW_INDEX]++;	
	if (bALowRight)
		arrWarnCnt[1][AUDIO_LOW_INDEX]++;	

	//监测周期没到
	unsigned long long ullCur_time = OS_SysTime_Ms();
	if ((ullCur_time - stAudioInfos.ullLasttime < stThrldAudio.nSampleTime))
		return -2;
	stAudioInfos.ullLasttime = ullCur_time;		

    // 音频丢失
    /*if(0x01 & avplay_get_progStatus(iCh)) {
        Lstatus |= (1 << AUDIO_LOSS_INDEX);
        Rstatus |= (1 << AUDIO_LOSS_INDEX);
    }*/

    // 无伴音监测结果
    if (arrWarnCnt[0][AUDIO_NO_SOUND] >= (int)(arrCalcCnt[0][AUDIO_NO_SOUND] * AUDIO_NO_DATA_RATIO))
    {
        Lstatus |= (1 << AUDIO_NO_SOUND);
        *uStatue |= ( 1 << WARN_TYPE_A_NO_SOUND);
    }
    else
    {
		Lstatus &= ~(1 << AUDIO_NO_SOUND);
		*uStatue &= ~(1 << WARN_TYPE_A_NO_SOUND);
    }

    if (arrWarnCnt[1][AUDIO_NO_SOUND] >= (int)(arrCalcCnt[1][AUDIO_NO_SOUND] * AUDIO_NO_DATA_RATIO))
    {
        Rstatus |= (1 << AUDIO_NO_SOUND);
        *uStatue |= ( 1 << WARN_TYPE_A_NO_SOUND);
    }
	else
	{
		Rstatus &= ~(1 << AUDIO_NO_SOUND);
		*uStatue &= ~(1 << WARN_TYPE_A_NO_SOUND);
	}


    // 音量过高监测结果
    if (arrWarnCnt[0][AUDIO_HIGH_INDEX])
        Lstatus |= (1 << AUDIO_HIGH_INDEX);
    if (arrWarnCnt[1][AUDIO_HIGH_INDEX])
        Rstatus |= (1 << AUDIO_HIGH_INDEX);

    // 音量过低监测结果
    if (arrWarnCnt[0][AUDIO_LOW_INDEX] == arrCalcCnt[0][AUDIO_HIGH_INDEX])
        Lstatus |= (1 << AUDIO_LOW_INDEX);
    if (arrWarnCnt[1][AUDIO_LOW_INDEX] == arrCalcCnt[1][AUDIO_HIGH_INDEX])
        Rstatus |= (1 << AUDIO_LOW_INDEX);

    //上报
    //analyse_upload_audio_wr(iCh,Lstatus,Rstatus);

    //清空计数
    memset(&arrCalcCnt[0][0], 0, sizeof(int)*6);
    memset(&arrWarnCnt[0][0], 0, sizeof(int)*6);

    return 0;
}

int alg_snd_get_volume(int iCh, int *volumeL, int *volumeR)
{
	if (iCh < 0 || iCh >= MAX_SOUND_CHN)
		return -1;

	//*volumeL = g_VolumeMaxL[iCh];
	//*volumeR = g_VolumeMaxR[iCh];

	return 0;
}

//int sound_monitor_info(MonitorAThrld &stThrldAudio, char *p_buffer)
//{
//	int i;
//	char *p = p_buffer;
//	//double dHighGate = 0.0, dLowGate = 0.0, dLossGate = 0.0,dLossLower = 0.0;
//	int iLVolumeMax = 0, iLVolumeMin = 0, iRVolumeMax = 0, iRVolumeMin = 0;
//
//	p += sprintf(p,"%-3s|%-19s|%-19s|%-19s|%-19s\r\n"
//					,"Ch"
//					,"Left  Max/min"
//					,"Right Max/min"
//					,"Thrld  High/Low"
//					,"Thrld Loss/LossLowe");
//					
//	for (i = 0; i < MAX_SOUND_CHN; i++)
//	{
//		iLVolumeMax = Decibels(g_snd_audio_printInfo[i].usLVolumeMax) - 96;
//		iLVolumeMin = Decibels(g_snd_audio_printInfo[i].usLVolumeMin) - 96;
//		iRVolumeMax = Decibels(g_snd_audio_printInfo[i].usRVolumeMax) - 96;
//		iRVolumeMin = Decibels(g_snd_audio_printInfo[i].usRVolumeMin) - 96;
//		p += sprintf(p,"%-3d|%-9d/%-9d|%-9d/%-9d|%-9d/%-9d|%-9d/%-9d\r\n"
//					,i
//					,iLVolumeMax,iLVolumeMin
//					,iRVolumeMax,iRVolumeMin
//					, stThrldAudio.nHigher, stThrldAudio.nLower
//                    , stThrldAudio.nLost, stThrldAudio.nLostLower);
//	}	
//
//	return 0;
//}


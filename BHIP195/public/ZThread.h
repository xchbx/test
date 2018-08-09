/*******************************************************************************
* 版权所有 (C) 2010
* 
* 文件名称： 	ZThread.h
* 文件标识： 	
* 内容摘要： 	线程辅助函数，用于Windows和linux的通用软件。
* 其它说明： 	
* 当前版本： 	V1.0
* 作    者： 	周锋
* 完成日期： 	2010-06-08
*******************************************************************************/
#ifndef Z_THREAD_H_7589432758974389257894385849328590438508439589058904328590430
#define Z_THREAD_H_7589432758974389257894385849328590438508439589058904328590430

#if (defined _WIN32) || (defined _WINDOWS_)
#include <Windows.h>
#else
#include <pthread.h>
#endif


namespace Tool
{


#if (defined _WIN32) || (defined _WINDOWS_)
	typedef HANDLE	TH_HANDLE;
	typedef UINT TH_RET;
#else
	typedef pthread_t	TH_HANDLE;
	typedef void  *TH_RET;
	typedef unsigned long       DWORD;
#endif

	typedef TH_RET (*ThreadFuncType) (void *lpContext);

class CZThread
{
private:
	CZThread(void){}
	~CZThread(void){}


public:

	static const TH_HANDLE INVALID_HANDLE;

	//启动线程
	static TH_HANDLE BeginThread(ThreadFuncType pfnFunc, void *lpContext, bool bHighPriority = false);

	//等待线程结束(dwTime单位为ms，如为0表示无限制等待)
	static bool WaitThreadEnd(TH_HANDLE &handle, DWORD dwTime = 0);
};

}

#endif


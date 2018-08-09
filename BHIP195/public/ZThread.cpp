#include "ZThread.h"

#if !(defined _WIN32) && !(defined _WINDOWS_)
#include <signal.h>
#include <sys/time.h>
#else
//#include <afxwin.h>
#include <Windows.h>
#include <process.h> 
#endif


namespace Tool
{

#if (defined _WIN32) || (defined _WINDOWS_)
	const TH_HANDLE CZThread::INVALID_HANDLE = (TH_HANDLE)(0);
#else
	const TH_HANDLE CZThread::INVALID_HANDLE = (TH_HANDLE)(-1);
#endif

	//启动线程
TH_HANDLE CZThread::BeginThread(ThreadFuncType pfnFunc, void *lpContext, bool bHighPriority)
{
	TH_HANDLE handle;
#if (defined _WIN32) || (defined _WINDOWS_)
	//CWinThread *pThread = AfxBeginThread(pfnFunc, lpContext, bHighPriority ? THREAD_PRIORITY_TIME_CRITICAL : THREAD_PRIORITY_NORMAL);
	
	unsigned int thrdaddr;
	TH_HANDLE handleTmp = (TH_HANDLE) _beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))pfnFunc, lpContext, 0, &thrdaddr);

	if (0 == handleTmp)
	{
		return INVALID_HANDLE;
	}
	handle = handleTmp;
#else
	pthread_attr_t attr;
	sched_param param;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	param.sched_priority = sched_get_priority_max(SCHED_RR);
	pthread_attr_setschedparam(&attr, &param); 

	int nRet = pthread_create(&handle, bHighPriority ? &attr : 0, pfnFunc, lpContext);
	if (0 != nRet)
	{
		return INVALID_HANDLE;
	}
#endif

	return handle;
}

//等待线程结束
bool CZThread::WaitThreadEnd(TH_HANDLE &handle, DWORD dwTime)
{
	if (INVALID_HANDLE == handle)
	{
		return false;
	}

	if (0 == dwTime)
	{
#if (defined _WIN32) || (defined _WINDOWS_)
		WaitForSingleObject(handle, INFINITE);
#else
		pthread_join(handle, 0); 
#endif
	}
	else
	{
#if (defined _WIN32) || (defined _WINDOWS_)
		if (WAIT_OBJECT_0 != WaitForSingleObject(handle, dwTime))
		{
			TerminateThread(handle, 7777);
		}
#else
		timespec ts;
		timeval tv;
		gettimeofday (&tv, NULL);
		TIMEVAL_TO_TIMESPEC (&tv, &ts);
		ts.tv_nsec += (dwTime % 1000) * 1000000;
		if (ts.tv_nsec >= 1000000000)
		{
			ts.tv_nsec -= 1000000000;
			++ts.tv_sec;
		}
		ts.tv_sec += dwTime / 1000;
		if (0 != pthread_timedjoin_np (handle, 0, &ts))
		{
			pthread_kill(handle, 0);
		}
#endif
	}

#if (defined _WIN32) || (defined _WINDOWS_)
	CloseHandle(handle);
#endif

	handle = INVALID_HANDLE;
	return true;
}

}

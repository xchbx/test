#ifndef _WIN32
//#if 1
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "Thread.h"
#include <string.h>

struct OS_Thread_Priv
{
	pthread_t hThread;
};

OS_Thread::OS_Thread() 
{
	m_nTHNum = TH_NUM;

	for (int i=0; i<m_nTHNum; ++i)
	{
		m_Priv[i] = 0;
	}
}

OS_Thread::~OS_Thread()
{
	for (int i=0; i<m_nTHNum; ++i)
	{
		if(NULL != m_Priv[i])
		{
			OS_Thread_Priv* priv = (OS_Thread_Priv*) m_Priv[i];
			delete priv;
		}
	}
}

static void* OS_Thread_Proc_Linux1(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH1_V);
	return NULL;
}

static void* OS_Thread_Proc_Linux2(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH2_V);
	return NULL;
}

static void* OS_Thread_Proc_Linux3(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH3_V);
	return NULL;
}

static void* OS_Thread_Proc_Linux4(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH4_V);
	return NULL;
}

int OS_Thread::Run()
{
	// 创建
	for (int i=0; i<m_nTHNum; ++i)
	{
		if (0 != CreateTH(i+1))
		{
			Join(this);
			return -1;
		}
	}

	return 0;
}

int OS_Thread::CreateTH(int nID)
{
	if (nID<=0 || nID>TH_NUM)
	{
		return -1;
	}

	// 创建私有结构
	OS_Thread_Priv* priv = new OS_Thread_Priv;
	if(!priv) return -1;

	// 创建线程
	m_Priv[nID-1] = priv;

	int nRet = 0;
	if (1 == nID)
	{
		nRet = pthread_create(&priv->hThread, NULL, OS_Thread_Proc_Linux1, this);
	}
	else if (2 == nID)
	{
		nRet = pthread_create(&priv->hThread, NULL, OS_Thread_Proc_Linux2, this);
	}
	else if (3 == nID)
	{
		nRet = pthread_create(&priv->hThread, NULL, OS_Thread_Proc_Linux3, this);
	}
	else if (4 == nID)
	{
		nRet = pthread_create(&priv->hThread, NULL, OS_Thread_Proc_Linux4, this);
	}
	else
	{
		return -1;
	}

	if(nRet <0)
	{
		delete priv;
		m_Priv[nID-1] = NULL;
		return -1;
	}

	return 0;
}

void OS_Thread::Join(OS_Thread* thrd)
{
	OS_Thread_Priv* priv = NULL;
	for (int i=0; i<thrd->m_nTHNum; ++i)
	{
		priv = (OS_Thread_Priv*) thrd->m_Priv[i];
		if(priv)
		{
			pthread_join(priv->hThread, NULL);

			// 删除资源
			delete priv;
			thrd->m_Priv[i] = NULL;
		}
	}
}

void OS_Thread::Msleep(int ms)
{
	//::usleep(ms * 1000);
	// 好像使用nanosleep更好

	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

void OS_Thread::Sleep(int s)
{
	::sleep(s);
}

int OS_Thread::Routine(TH1)
{
	return 0;
}

int OS_Thread::Routine(TH2)
{
	return 0;
}

int OS_Thread::Routine(TH3)
{
	return 0;
}

int OS_Thread::Routine(TH4)
{
	return 0;
}

#endif // ! _WIN32

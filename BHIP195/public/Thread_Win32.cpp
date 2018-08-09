#ifdef _WIN32

#include <windows.h>
#include <process.h> 

#include "Thread.h"

struct OS_Thread_Priv
{
	HANDLE hThread;
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
		if(m_Priv[i])
		{
			OS_Thread_Priv* priv = (OS_Thread_Priv*) m_Priv[i];
			delete priv;
		}
	}
}

static unsigned int WINAPI OS_Thread_Proc_Win32_1(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH1_V);

	return 0;
}

static unsigned int WINAPI OS_Thread_Proc_Win32_2(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH2_V);

	return 0;
}

static unsigned int WINAPI OS_Thread_Proc_Win32_3(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH3_V);
	return 0;
}

static unsigned int WINAPI OS_Thread_Proc_Win32_4(void* param)
{
	OS_Thread* thrd = (OS_Thread*) param;
	thrd->Routine(TH4_V);
	return 0;
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
	unsigned int thrdaddr ;
	m_Priv[nID-1] = priv;

	if (1 == nID)
	{
		priv->hThread = (HANDLE) _beginthreadex(NULL, 0, OS_Thread_Proc_Win32_1, this, 0, &thrdaddr);
	}
	else if (2 == nID)
	{
		priv->hThread = (HANDLE) _beginthreadex(NULL, 0, OS_Thread_Proc_Win32_2, this, 0, &thrdaddr);
	}
	else if (3 == nID)
	{
		priv->hThread = (HANDLE) _beginthreadex(NULL, 0, OS_Thread_Proc_Win32_3, this, 0, &thrdaddr);
	}
	else if (4 == nID)
	{
		priv->hThread = (HANDLE) _beginthreadex(NULL, 0, OS_Thread_Proc_Win32_4, this, 0, &thrdaddr);
	}
	else
	{
		return -1;
	}

	if(priv->hThread == NULL)
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
			WaitForSingleObject(priv->hThread, INFINITE);
			CloseHandle(priv->hThread);
			// 删除资源
			delete priv;
			thrd->m_Priv[i] = NULL;
		}
	}
}

void OS_Thread::Msleep(int ms)
{
	::Sleep(ms);
}

void OS_Thread::Sleep(int s)
{
	::Sleep(s * 1000);
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

#endif  //_WIN32



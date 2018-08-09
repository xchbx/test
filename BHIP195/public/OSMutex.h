#ifndef __OSAPI_MUTEX_H__
#define __OSAPI_MUTEX_H__

#include <stdio.h>

class OS_Mutex
{
public:
	OS_Mutex(bool bCreate = false);
	~OS_Mutex();

	int Init(); // ´´½¨

	int Lock();
	int TryLock();
	void Unlock();

private:
	void* m_Priv;
};

class OS_MutexGuard
{
public:
	OS_MutexGuard(OS_Mutex *pMutex)
	{
		m_pMutex = pMutex;
		if (NULL != m_pMutex)
		{
			m_pMutex->Lock();
		}
	}

	~OS_MutexGuard()
	{
		if (NULL != m_pMutex)
		{
			m_pMutex->Unlock();
		}
	}

private:
	OS_Mutex* m_pMutex;
};

#endif

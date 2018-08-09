
#ifndef  _OSAPI_SEMAPHORE_H
#define _OSAPI_SEMAPHORE_H

class OS_Semaphore
{
public:
	OS_Semaphore();
	~OS_Semaphore();

	int Init(int initial_value=1); // ´´½¨

	int Wait();
	int Wait(int ms);
	void Post();

private:
	void* m_Priv;
};


#endif


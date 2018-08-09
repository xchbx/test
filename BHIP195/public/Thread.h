
#ifndef _OSAPI_THREAD_H
#define _OSAPI_THREAD_H

#define TH_NUM	4
#define TH1		void
#define TH1_V	

#define TH2		int
#define TH2_V	0

#define TH3		long,long
#define TH3_V	0,0

#define TH4		long,long,long
#define TH4_V	0,0,0
//...

class OS_Thread
{
public:
	OS_Thread();
	virtual ~OS_Thread();

	void SetTHNum(int nNum)
	{ 
		m_nTHNum = nNum; 
		if (nNum > TH_NUM)
		{
			m_nTHNum = TH_NUM; 
		}
	};

	// 创建并启动
	virtual int Run();

	// 等待和收回资源
	static void Join(OS_Thread* thrd);

	// Sleep函数
	static void Msleep(int ms);
	static void Sleep(int s);

public:
	virtual int Routine(TH1) = 0;
	virtual int Routine(TH2);
	virtual int Routine(TH3);
	virtual int Routine(TH4);
	//...

private:
	int CreateTH(int nID);

private:
	void* m_Priv[TH_NUM];

	int m_nTHNum;
};


#endif


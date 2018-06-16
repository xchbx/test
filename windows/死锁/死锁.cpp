#if 1
//多线程死锁  多个互斥量(2个以上)   请求另外的互斥量并且保持自己已有的互斥量  这种情况下多个线程都在请求额外
//的互斥量并且保持自己的互斥量不释放，那么就会产生死锁
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <fstream>

using namespace std;

class LofFile
{
public:
	LofFile()
	{
		f.open("data.txt");
	}
	void show(string msg, int id)
	{
		lock_guard<mutex> locker(m_mutex);
		lock_guard<mutex> locker2(m_mutex2);
		cout << msg << id << endl;
	}
	void show2(string msg, int id)
	{
		lock_guard<mutex> locker2(m_mutex2);
		lock_guard<mutex> locker(m_mutex);
		cout << msg << id << endl;
	}
private:
	mutex m_mutex;
	mutex m_mutex2;
	ofstream f;			//资源f为保护对象
};

void func(LofFile& lof)
{
	for (int i = 0; i > -100; --i)
	{
		lof.show("func ", i);
	}
}

int main()
{
	LofFile lof;
	thread t1(func,ref(lof));
	
	for (int i = 0; i < 100; ++i)
	{
		lof.show2("main ", i);
	}
	t1.join();
	return 0;
}
#endif
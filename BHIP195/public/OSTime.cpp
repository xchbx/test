#include "OSTime.h"

static long lSystemTimeDiff = 0;

#ifdef _WIN32
#include <windows.h>
void OS_Time::GetLocalTime(OS_TimeVal* tv)
{
	SYSTEMTIME systime;
	::GetLocalTime(&systime);

	tm _tm;
	_tm.tm_year = systime.wYear - 1900;
	_tm.tm_mon = systime.wMonth - 1;
	_tm.tm_mday = systime.wDay;
	_tm.tm_hour = systime.wHour;
	_tm.tm_min = systime.wMinute;
	_tm.tm_sec = systime.wSecond;

	tv->tv_sec = (int) ::mktime(&_tm);
	tv->tv_usec = systime.wMilliseconds * 1000;
}

#else
#include <sys/time.h> 
#include <stdio.h>
#include<unistd.h>

void OS_Time::GetLocalTime(OS_TimeVal* tv)
{
	timeval tv_now;
	gettimeofday(&tv_now, NULL);
	tv->tv_sec = tv_now.tv_sec;
	tv->tv_usec = tv_now.tv_usec;
}

double OS_Time::GetCpuFreq()
{
	ticks tick_start;
	struct timeval tvStart, tvEnd;
	gettimeofday(&tvStart, NULL);
	tick_start = OS_Time::GetTicks();
	sleep(1);
	gettimeofday(&tvEnd, NULL);
	unsigned int temp = (unsigned int)(OS_Time::GetTicks() - tick_start);
	double tempTV = (tvEnd.tv_sec +  tvEnd.tv_usec / 1000000.0f) -
		(tvStart.tv_sec + tvStart.tv_usec / 1000000.0f);
	double hz =  ((double)(temp)) / tempTV;
	return hz / 1000000.0f;
}

ticks OS_Time::GetTicks(void)
{
	unsigned a, d;
	asm("cpuid");
	asm  volatile("rdtsc" : "=a" (a), "=d" (d));
	return ((ticks)a | (((ticks)d) << 32));
}

void OS_Time::GetWallTime(OS_TimeVal* tv)
{
    timeval tv_now;
    gettimeofday(&tv_now, NULL);
    tv->tv_sec = tv_now.tv_sec + lSystemTimeDiff;
    tv->tv_usec = tv_now.tv_usec;
}

void OS_Time::SetWallTimeDiff(int diff)
{
    lSystemTimeDiff += diff;
}

unsigned long long OS_WallTime_Ms()
{
    OS_TimeVal tvCur;
    OS_Time::GetWallTime(&tvCur);

    unsigned long long ullTm = 0;
    ullTm = tvCur.tv_sec;
    ullTm *= 1000;
    ullTm += tvCur.tv_usec / 1000;

    return ullTm;

}
#endif

unsigned long long OS_SysTime_Ms()
{

    OS_TimeVal tvCur;
    OS_Time::GetLocalTime(&tvCur);

    unsigned long long ullTm = 0;
    ullTm = tvCur.tv_sec;
    ullTm *= 1000;
    ullTm += tvCur.tv_usec / 1000;

    return ullTm;
}

string GenerateReportTime(unsigned long long ullTm)
{
    time_t nowtime = ullTm / 1000;
    struct tm *plocal;
    plocal = localtime(&nowtime);
    char buf[80] = { '\0' };
    strftime(buf, 80, "%Y-%m-%d %H:%M:%S",plocal);

    return string(buf, strlen(buf));
}

string GenerateTimeForOsd(unsigned long long ullTm)
{
    time_t nowtime = ullTm / 1000;
    struct tm *plocal;
    plocal = localtime(&nowtime);
    char buf[80] = { '\0' };
    strftime(buf, 80, "%Y-%m-%d %H\\\:%M\\\:%S",plocal);

    return string(buf, strlen(buf));
}

/*************************************************************************
	> File Name: teachThread.c
	> Author: 
	> Mail: 
	> Created Time: 2018年05月26日 星期六 19时55分47秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define COUNT 5000

int count;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *doit()
{
    int i,val;
    for(i=0; i<COUNT; ++i)
    {
        pthread_mutex_lock(&mutex);
        val =count;
        printf("pid = %x, val = %d\n",(unsigned int)pthread_self(),val+1);
        count=val+1;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_t pid_A,pid_B;
    pthread_create(&pid_A,NULL,doit,NULL);
    pthread_create(&pid_B,NULL,doit,NULL);

    pthread_join(pid_A,NULL);
    pthread_join(pid_B,NULL);
    return 0;
}

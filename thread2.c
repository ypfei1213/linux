#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#define MAX 10


int number=1;
pthread_mutex_t mut;



void *thread1()
{
    printf("我是线程1,我开始执行了！\n");
    sleep(1);
    for(int i=0;i<MAX;i++)
    {
        printf("thread1: number = %d \n",number);
        pthread_mutex_lock(&mut);         
        number++;
        pthread_mutex_unlock(&mut);
        sleep(2);
    }
    printf("线程1运行结束！\n");
}

void *thread2()
{    
    printf("我是线程2,我开始执行了！\n");
    sleep(2);
    for(int j=0;j<MAX;j++)
    {
        printf("thread2: number = %d \n",number);
        pthread_mutex_lock(&mut);
        number++;
        pthread_mutex_unlock(&mut);
        sleep(2);
    }
    printf("线程2运行结束！\n");
}


int main()
{
    pthread_t thread[2];
    memset(thread,0,sizeof(thread));//将thread中当前位置后面的 sizeof(thread) 个字节用 0 替换并返回 s
    int result1=pthread_create(thread,NULL,thread1,NULL); //线程1被创建
    if(0==result1)
    {
        printf("线程1创建成功！\n");
    }
    else
    {
        printf("线程1创建失败！\n");
    }
    int result2=pthread_create(thread+1,NULL,thread2,NULL); //线程2被创建
    if(0==result2)
    {
        printf("线程2创建成功！\n");
    }
    else 
    {
        printf("线程2创建失败！\n");
    }
    pthread_join(thread[1],NULL);
    printf("主函数运行结束！\n");
    return 0;
}

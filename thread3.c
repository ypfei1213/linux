#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <semaphore.h>  
#include <unistd.h>

#define SIZE 1024  
      
void *thread_function(void *arg);            
char buffer[SIZE];  
sem_t sem;  //信号量对象
                  
int main()  
{  
    int res;  
    pthread_t a_thread;  
    void *thread_result;   

    res = sem_init(&sem, 0, 0);  //0表示这个信号量是当前进程的局部信号量       0：信号量的初始值
    if (res != 0)  
    {  
        perror("Sem init failed");  //将上个函数发生错误的原因输出到标准输出设备（stderr）
        exit(EXIT_FAILURE);  //没有成功地执行一个程序
    } 

    res = pthread_create(&a_thread, NULL, thread_function, NULL); //创建线程 
    if (res != 0)  
    {  
        perror("Thread create failed");      
        exit(EXIT_FAILURE);  
    } 
    else
    {
        printf("Thread creation success!\n");
    }

    sleep(1);
    printf("Please input some text,you can enter 'end' to finish !\n");  

    while (scanf("%s", buffer)) 
    {
        sem_post(&sem);  
        if (strncmp("end", buffer, 3) == 0)
        {
            break;
        }
    }

    sem_destroy(&sem);  
    exit(EXIT_SUCCESS);  
}   

void *thread_function(void *arg)  
{  
    printf("线程a_thread开始运行了！\n");
    sem_wait(&sem);  
    while (strncmp("end", buffer, 3) != 0)  
    {  
        printf("You have input %d characters !\n", strlen(buffer));  
        sem_wait(&sem);
    }  
    printf("线程a_thread运行结束！\n");
    pthread_exit(NULL);  
}    

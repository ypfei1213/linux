#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


char message[]="hello";

void *thread_method(void *arg) // 线程的执行函数
{
    printf("Thread_method is running!\n");
    printf("Argument was  %s !\n",(char *)arg);
    strcpy(message,"Bye!");
    sleep(2);

    pthread_exit("Thank you for your CPU time!"); // 线程退出
}
int main ()
{
	pthread_t thread; // 线程ID
    void *thread_result;//退出线程的返回值
	
    
    int result=pthread_create(&thread,NULL,thread_method,(void *)message);
	if(result==0)// 判断线程是否创建成功
	{
		printf("Thread creation success!\n");
	}
	else
	{
		exit(EXIT_FAILURE);//表示没有成功地执行一个程序
	}


    printf("Waiting for thread to finish...\n");
    result=pthread_join(thread,&thread_result);//调用线程挂起函数，防止主线程运行过快结束
    if(result==0)
    {
        printf("Thread join success!\n");
    }
    else
    {
        perror("Thread join failed!\n");//用来将上一个函数发生错误的原因输出到标准输出设备(stderr)
        exit(EXIT_FAILURE); 
    }

    printf("Thread joined,it returned  %s \n",(char *)thread_result);
    printf("Message now is  %s \n", message);

    exit(EXIT_SUCCESS); 
}

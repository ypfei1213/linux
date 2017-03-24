#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>
#include<pthread.h>


#define MAXLINE 4096


typedef struct thread_arg //构造结构体用于线程单参数带入 
{
    int client_sockfd;
    char client_addr[20];
}thread_arg;


pthread_t thread[10];//创建线程组，用于不同客户端创建线程
int i=0;


int *msg(thread_arg* threadarg) // 线程函数
{
	thread_arg Threadarg = *(thread_arg *)threadarg;
	int cli_sock=Threadarg.client_sockfd;
	char addr[20];
	memcpy(addr,Threadarg.client_addr,strlen(Threadarg.client_addr));
	

	char message_cli[MAXLINE];
	int n=0;
	char cls[]="close\n";
	int flag=1;
	while(flag)
	{
		n=recv(cli_sock, message_cli, MAXLINE, 0);
        message_cli[n] = '\0';
		printf("receive msg from '%s' : %s\n ",addr,message_cli);
		if(strcmp(message_cli,cls) == 0)
		{
			flag=0;
			printf("客户端 '%s' 退出\n",addr);                                 
		}


		//向客户端发送回应数据 
		if(flag==1)
		{
			if(send(cli_sock, "ok! I have received \n", 21,0) == -1)  
			{
				perror("send error");
			}
		}
		else
		{
			if(send(cli_sock, "byebye！\n", 8,0) == -1)  
			{
				perror("send error");
			}
		}
    }
	printf("退出循环\n");                                    
	shutdown(cli_sock,2);
}



int main ()
{
    char buffer[MAXLINE];


    int server_sockfd=socket(AF_INET, SOCK_STREAM, 0);//创建服务器端套接口描述字
    if(server_sockfd<0)
    {
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0); 
    }

    

    struct sockaddr_in servaddr;  //IP+PORT
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(9010);//设置的端口为9010


    //将本地地址绑定到所创建的套接字上  
    if( bind(server_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {  
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }


    //开始监听是否有客户端连接  
    if( listen(server_sockfd, 10) == -1)
    {  
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    

    printf("~~~~~~~~~waiting for client's request~~~~~~~~~\n");

    //接收客户端的socket 地址
    struct sockaddr_in clientaddr;
    int size_clientaddr=sizeof(clientaddr);
    
    
    while(1)
    {
        memset(buffer,0,MAXLINE);

        int connect_fd;
        //阻塞直到有客户端连接，不然浪费CPU资源。  
        if( ( connect_fd = accept(server_sockfd,(struct sockaddr*) &clientaddr,&size_clientaddr)) == -1)
        {  
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);  
            continue;  
        }
        else
        {
            printf("客户端%s已连上！\n",inet_ntoa(clientaddr.sin_addr));
        }



        //创建线程，接受客户端传过来的数据

        
        thread_arg threadarg;
        threadarg.client_sockfd= connect_fd;
        char *ipaddr = inet_ntoa(clientaddr.sin_addr);
        strcpy(threadarg.client_addr,ipaddr);

        if(pthread_create( thread+i , NULL, (void *)msg, (void *)&threadarg))
        {
            printf("thread error");
        }
        else
        {
            pthread_detach( thread[i] );
        }
        i++;

    }

    shutdown(server_sockfd,2);
    
}



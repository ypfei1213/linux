#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>



#define MYPORT  8887
#define BUFFER_SIZE 1024



int main()
{
    ///定义客户端套接口描述字  
    int client_sockfd = socket(AF_INET,SOCK_STREAM, 0);


    ///定义服务器端 IPV4套接字地址 
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip


    ///连接服务器，成功返回0，错误返回-1   客户端套接字，服务器端 套接字地址 
    if (connect(client_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }



    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];// 缓存空间


    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) // 从标准输入设备键盘输入
    {
        send(client_sockfd, sendbuf, strlen(sendbuf),0); ///发送
        if(strcmp(sendbuf,"exit\n")==0)
		{
           	break;
		} 
       	recv(client_sockfd, recvbuf, sizeof(recvbuf),0); ///接收
        fputs(recvbuf, stdout); //标准设备输出到屏幕
        memset(sendbuf, 0, sizeof(sendbuf));// 重置
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(client_sockfd);
    return 0;
}

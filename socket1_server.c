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
#define QUEUE   20
#define BUFFER_SIZE 1024



int main()
{
      
///定义sockfd 返回一个 非负描述符  监听套接口描述字 
int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
       
         
///定义sockaddr_in  IP+PORT  IPV4套接字地址结构                           
struct sockaddr_in server_sockaddr;
server_sockaddr.sin_family = AF_INET; // 指定该地址家族，一般设置为AF_INET (使用TCP/IP协议)
server_sockaddr.sin_port = htons(MYPORT); // 端口   htons字节序转换函数
server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //绑定默认IP地址
      
         
         
///bind，成功返回0，出错返回-1 将一组固定的地址绑定到server_sockfd上
int result=bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr));
if(result==-1)
{
    perror("bind");
    exit(1);
}
         
//listen，成功返回0，出错返回-1   将未连接的套接口转换成被动套接口，指示内栈应接受指向此套接口的  连接请求;将套接口状态由closed转为listened状态
result=listen(server_sockfd,QUEUE); //QUEUE 最大连接数
if(result==-1)
{
    perror("listen");
    exit(1);
}
      
         
///客户端套接字
char buffer[BUFFER_SIZE];//缓存区
struct sockaddr_in client_addr;//用于在accept函数中接受 客户端 IP+PORT
socklen_t length = sizeof(client_addr);
      
         
///成功返回非负描述字，出错返回-1  内核为每一个已连接的客户端创建一个已连接套接口描述字
int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
if(client_sockfd<0)
{
    perror("connect");
    exit(1);
}
      
         
while(1)
{
    memset(buffer,0,sizeof(buffer));
    int len = recv(client_sockfd, buffer, sizeof(buffer),0);
    if(strcmp(buffer,"exit\n")==0)
    {
        break;
    } 
    fputs(buffer, stdout);
    send(client_sockfd, buffer, len, 0);
}
close(client_sockfd);
close(server_sockfd);
return 0;
}

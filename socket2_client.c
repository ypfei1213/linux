#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>


#define MAXLINE 4096


int main(int argc,char** argv)
{
    char recvbuff[MAXLINE],sendbuff[MAXLINE];


    if(argc!=2 )//不带参数时argc为1,argv[0]里放的是程序的路径,
                //加了参数以后argc=输入参数的个数加1,
                //argv[1]开始才是用户输入的参数
    {
        printf("usage:./client <ipaddress>\n");
        exit(0);// exit（0）：正常运行程序并退出程序；
                // exit（1）：非正常运行导致退出程序；
    }

//  sockaddr_in（在netinet/in.h中定义）：
//  struct sockaddr_in 
//  {
//      short int sin_family;            指定该地址家族，一般设置为AF_INET(使用TCP/IP协议)   
//      unsigned short int sin_port;     端口
//      struct in_addr sin_addr;         IPV4的地址
//      unsigned char sin_zero[8];       暂不使用，一般将其设置为0
//  };
//  struct in_addr  
//  {
//      unsigned long s_addr;            IP地址是四个字节的一个32位值
//  }; 


    struct sockaddr_in servaddr;//定义服务器端地址
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(9010);

    if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<0) //将 点分十进制 转化为 二进制整数
    {
        printf("Create socket error: %s (errno: %d)\n",strerror(errno),errno); // 输出具体错误消息
        exit(0);
    }

    

    //创建客户端套接口描述字
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        printf("Create socket error: %s (errno: %d)\n",strerror(errno),errno); // 输出具体错误消息
    }


    //建立于TCP服务器的连接  向给定服务器地址发送连接请求
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Create socket error: %s (errno: %d)\n",strerror(errno),errno); // 输出具体错误消息
    }

    
    //连接成功，开始通信
    while(1)
    {
        printf("Please input msg sent to server,or input 'close' to exit!\n");
        memset(sendbuff,0,MAXLINE);
        memset(recvbuff,0,MAXLINE);

        fgets(sendbuff,MAXLINE,stdin); // 从标准输入设备键盘 输入字符串 给sendbuff
        
        char cls[]="close";
        
        if( send(sockfd, sendbuff, strlen(sendbuff), 0) < 0) //将 sendbuff 中字符串发送给服务器端 
        {  
            printf("Create socket error: %s (errno: %d)\n",strerror(errno),errno); // 输出具体错误消息
            exit(0);  
        }  
        

        if((recv(sockfd, recvbuff, MAXLINE,0)) == -1) //接受 服务器端发送来的字符串至 recvbuff 
        {  
            perror("recv error");  
            exit(1);  
        } 

        printf("server: %s \n",recvbuff);

        if(strncmp(sendbuff,cls,5) == 0)
        {
            shutdown(sockfd,2);
            exit(0);                                                        
        }
    }

}

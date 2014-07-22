#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netinet/tcp.h> 

#define MAXLINE 4096

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[4096], sendline[4096];
    struct sockaddr_in servaddr;
    
    if(argc != 2 && argc != 3)
    {
        printf("usage: tcpclient <ipaddress> <lable>\n");
        printf("lable:                              \n");
        printf("       L1: shutdown. \n");
        printf("       L2: close. \n");
        printf("       L3: shutdown->close. \n");
        exit(0);
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", argv[1]);
        exit(0);
    }
    
int keepAlive = 1; // 开启keepalive属性
int keepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测 
int keepInterval = 5; // 探测时发包的时间间隔为5 秒
int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

if(3 <= argc) {
    if(0 == strcmp(argv[2], "L1")) goto L1;
    if(0 == strcmp(argv[2], "L2")) goto L2;
    if(0 == strcmp(argv[2], "L3")) goto L3;
}

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    
    printf("send msg to server: \n");
    
    fgets(sendline, 4096, stdin);
    if(send(sockfd, sendline, strlen(sendline), 0) < 0)
    {
        printf("send msg error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    close(sockfd);
    goto L4;
    
L1:
    shutdown(sockfd, SHUT_RDWR); goto L4;
L2:
    close(sockfd); goto L4;
L3:
	  shutdown(sockfd, SHUT_RDWR);
	  close(sockfd);
	  goto L4;
	  
L4:
    sleep(120);
    exit(0);
}


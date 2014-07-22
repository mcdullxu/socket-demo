#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 4096

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    char buff[4096];
    int n;
    
    if(-1 == (listenfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        printf("create socket error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888);
    
    if(-1 == bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("bind socket error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    if(-1 == listen(listenfd, 10))
    {
        printf("listen socket error: %s(errno: %d) \n", strerror(errno), errno);
        exit(0);
    }
    
    printf("========== waiting for client's request ========== \n");
    
    while(1)
    {
        int len = sizeof(clientaddr);
        if(-1 == (connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &len)))
        {
            printf("accept socket error: %s(errno: %d) \n", strerror(errno), errno);
            exit(0);
        }
        
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        printf("recv msg from client IP Address: %s, Port: %d \n",inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        printf("recv msg boby: %s\n", buff);
        close(connfd);
    }
    
    close(listenfd);
}


#include <netinet/in.h>
#include <time.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>


#define MAXLINE     4096    /* max text line length */
#define LISTENQ     1024    /* 2nd argument to listen() */
#define DAYTIME_PORT 3333

int main(int argc, char **argv)
{
    int     listenfd, connfd, forwardfd;
    //int     n;
    struct sockaddr_in servaddr, tunneladdr, clientaddr;
    socklen_t addr_size;
    
    char    buff1[MAXLINE] = "";
    char    buff2[MAXLINE] = "";
    //char recvline[MAXLINE + 1];
    //char ip[MAXLINE];
    //char name[MAXLINE];
    //char service[MAXLINE];
    if(argc != 2)
    {
    	printf("Usage:%s<portNumber>", argv[0]);
    	return -1;
    }
    char *port_num = argv[1];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&tunneladdr, sizeof(tunneladdr));
    tunneladdr.sin_family = AF_INET;
    tunneladdr.sin_addr.s_addr = htonl(INADDR_ANY);
    tunneladdr.sin_port = htons(atoi(port_num)); /* daytime server */

    bind(listenfd, (struct sockaddr *) &tunneladdr, sizeof(tunneladdr));

    listen(listenfd, LISTENQ);


    for ( ; ; ) {
        addr_size = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &addr_size);
        read(connfd, buff1, strlen(buff1));
        printf("server_name%s\n", buff1);
        
        read(connfd, buff2, strlen(buff2));
        printf("port_num%s\n", buff2);
        /*if(!isValidIpAddress(buff1))
        {
           hostname_to_ip(connfd,buff1,buff2,ip,recvline) 
        }*/
        inet_pton(AF_INET,buff1,&servaddr.sin_addr);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(atoi(buff2));
        forwardfd = socket(AF_INET, SOCK_STREAM,0);

        if(forwardfd<0)
        {
            printf("socket erro\n");
            exit(1);
        }
        if(connect(forwardfd,(struct sockaddr*)&servaddr, sizeof servaddr)<0)
        {
            printf("connect error\n");
            exit(1);
        }
        //printf("Received request from client %s port = %s destined to server %s port %s\n", inet_ntoa(clientaddr.sin_addr), port_num, buff1, inet_ntoa(clientaddr.sin_port));
        close(connfd);
    }
}

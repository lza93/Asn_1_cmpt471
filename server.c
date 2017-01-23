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

int main(int argc, char **argv)
{
    int     listenfd, connfd;
    struct sockaddr_in servaddr, clientaddr;
    socklen_t addr_size;
    char    buff[MAXLINE];
    time_t ticks;
    char *port_num;
    if(argc != 2)
    {
    	printf("Usage:%s<portNumber>", argv[0]);
    	return -1;
    }
    port_num = argv[1];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(port_num)); /* daytime server */
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    for ( ; ; ) {
        addr_size = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &addr_size);
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));
        printf("request address: %s\n", inet_ntoa(clientaddr.sin_addr));
        printf("Sending response: %s", buff);
        close(connfd);
    }
}


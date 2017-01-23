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
#include <sys/wait.h>


#define MAXLINE     4096    /* max text line length */
#define LISTENQ     1024    /* 2nd argument to listen() */
#define DAYTIME_PORT 3333
#define DEBUG 0

int isValidIpAddress(char*);
int hostname_to_ip(int, char*,char*,char*,char*);
int ip_to_hostname(int, char*,char*,char*,char*);

int main(int argc, char **argv)
{
    int     listenfd, connfd, forwardfd = 0;
    struct sockaddr_in tunneladdr, clientaddr;
    socklen_t addr_size;
    //pid_t pid;
    char    buff1[MAXLINE] ;
    char    buff2[MAXLINE] ;

    char recvline[MAXLINE + 1];
    char ip[MAXLINE];
    char name[MAXLINE];

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
        printf("Reading server name\n");
        if(read (connfd, buff1, strlen(buff1))==0)
        {
            printf("server name: %s\n", buff1);
        }

        printf("Reading port number\n");
        read (connfd,buff2,strlen(buff2));
        printf("server port: %s\n", buff2);
        /*if((pid=fork())<0)
        {
            printf("Fork Failed\n");
            exit(1);
        }
        if(pid==0) /*Child*
        {
            printf("child id: %d\n", getpid());*/
            /*host name input case*/
            if(!isValidIpAddress(buff1))
            {
                hostname_to_ip(forwardfd, buff1, buff2, ip, recvline); 
                printf("Via Tunnel:%s\nIP Address:%s\nPort Number:%s\n",buff1,ip, buff2);
                if (fputs(recvline, stdout) == EOF) {
                printf("fputs error\n");
                }
                write(connfd, recvline, strlen(recvline)+1);
            }

            else
            {
                ip_to_hostname(forwardfd, buff1, buff2, name, recvline);   
            }
        close(forwardfd);
        close(connfd);

    }
    
    exit(0);
}
/******************************************************************************************
* check if the input is an ip address or not
*
*******************************************************************************************/
int isValidIpAddress(char *server_name)
{
    struct sockaddr_in servaddr;
    int result = inet_pton(AF_INET,server_name,&(servaddr.sin_addr));
    printf("%d\n", result);
    return result !=0;
}

/******************************************************************************************
* convert host name into ip address
*
*******************************************************************************************/
int hostname_to_ip(int connfd, char *server_name, char *port_num, char *ip, char *recvline)
{
    int    sockfd, n;
    struct sockaddr_in *servaddr; 
    struct addrinfo hints, *res, *p;
    bzero(&servaddr, sizeof(servaddr));
     //if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //convert hostname into IP
    getaddrinfo(server_name,port_num, &hints, &res); 
    //if (inet_pton(AF_INET,argv[1], &servaddr.sin_addr) <= 0) {
    for(p= res;p!=NULL;p=p->ai_next)
    {
        servaddr = (struct sockaddr_in *)p->ai_addr;
        strcpy(ip, inet_ntoa(servaddr->sin_addr));
    }
    if( (sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol)) < 0) {
        printf("socket error\n");
        exit(1);
    }
    connfd=connect(sockfd, res->ai_addr, res->ai_addrlen);
    if(connfd<0)
    {
       printf("connect error\n");
        exit(1); 
    }
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;        /* null terminate */

    }
    if (n < 0) {
        printf("read error\n");
        exit(1);
    }
    freeaddrinfo(res);
    return 0;
}

/******************************************************************************************************** 
*convert ip address into host name
*
********************************************************************************************************/
int ip_to_hostname(int connfd, char *server_name, char *port_num, char *name, char *recvline)
{
    

    int    sockfd, n;
    struct hostent *he;
    struct sockaddr_in servaddr; 
    socklen_t len;
    len = sizeof(servaddr);
    /*convert Ip address to hostname*/
    inet_pton(AF_INET,server_name,&servaddr.sin_addr);
    he = gethostbyaddr(&servaddr.sin_addr, len, AF_INET);
    strcpy(name, he->h_name);

#if DEBUG
    int r;
    r=getnameinfo((struct sockaddr *)&servaddr,len, name, sizeof name, port_num,sizeof port_num, 0); 
    printf("%s\n",name);
    if (r)
    {
    printf("%s\n", gai_strerror(r));
    exit(1);
    }
#endif

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port_num));
    if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket error\n");
        exit(1);
    }
    connfd=connect(sockfd, (struct sockaddr *)&servaddr,len);
    if(connfd<0)
    {
       printf("connect error\n");
        exit(1); 
    }

    
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;        /* null terminate */
    }
    if (n < 0) {
        printf("read error\n");
        exit(1);
    }
    return 0;
}
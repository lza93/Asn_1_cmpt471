#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define MAXLINE     4096    /* max text line length */
#define DAYTIME_PORT 3333
#define DEBUG        0
int isValidIpAddress(char*);
int hostname_to_ip(int, char*,char*,char*,char*);
int ip_to_hostname(int, char*,char*,char*,char*,char*);
int main(int argc, char **argv)
{
    int  connfd = 0;
    char recvline[MAXLINE + 1];
    char ip[MAXLINE];
    char name[MAXLINE];
    char service[MAXLINE];
    


    if (argc != 3 && argc != 5) {
        printf("usage: %s <IPaddress and port>", argv[0]);
        exit(1);
    }

    /* No tunnel using case*/
    if (argc==3){
        char *server_name = argv[1];
        char *port_num = argv[2];
        /* hostname input case*/
        if(!isValidIpAddress(server_name)){
        hostname_to_ip(connfd, server_name, port_num, ip, recvline);
        printf("hostname:%s\nIpaddress:%s\n",server_name, ip);
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }

        /* ip input case*/
        else
        {
            ip_to_hostname(connfd, server_name, port_num, name, service, recvline);
            printf("hostname:%s\nIpaddress:%s\n",name,server_name);
            if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }
   
    }
    else{
        char *server_name = argv[3];
        char *port_num = argv[4];
        char *tunnel_name = argv[1];
        char *tunnel_port = argv[2];
        /* hostname input case*/
        if(!isValidIpAddress(tunnel_name)){
        hostname_to_ip(connfd, tunnel_name, tunnel_port, ip, recvline);
        write(connfd,server_name, sizeof server_name);
        write(connfd,port_num, sizeof port_num);
        printf("Via tunnel:%s\ntunnel IP:%s\n",tunnel_name, ip);
        }
        /* ip input case*/
        else
        {
            ip_to_hostname(connfd, tunnel_name, tunnel_port, name, service, recvline);
            printf("Via tunnel:%s\ntunnel IP:%s\n",name,tunnel_name);
            if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }
    }
    exit(0);
}
    #if DEBUG
    else{
        //connect to tunnel
        bzero(&servaddr, sizeof(servaddr));
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        //convert hostname into IP
        getaddrinfo(tunnel_IP_Name,tunnel_port, &hints, &res); 
        //if (inet_pton(AF_INET,argv[1], &servaddr.sin_addr) <= 0) {
        //len = sizeof(servaddr);
        //getnameinfo((struct sockaddr *) &servaddr,len, hostIp, sizeof hostIp, port_num,NI_MAXSERV, NI_NUMERICSERV); 
        //printf("host ip is %s\n", hostIp);

        if( (sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol)) < 0) {
        printf("socket error\n");
        exit(1);
        }
        printf("Ipaddress: %s\n", inet_ntoa(servaddr.sin_addr));
        if(connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
        {
            printf("connect error\n");
            exit(1); 
        }
        while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;        /* null terminate */
        printf("IP Address:%s\nTime:",inet_ntoa(servaddr.sin_addr));
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
    }
    exit(0);
    }
    #endif

/* check if the input is an ip address or not*/
int isValidIpAddress(char *server_name)
{
    struct sockaddr_in servaddr;
    int result = inet_pton(AF_INET,server_name,&(servaddr.sin_addr));
    return result !=0;
}

/* convert host name into ip address*/
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
    if(connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
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
    connfd = sockfd;
    return 0;
}

/* convert ip address into host name*/
int ip_to_hostname(int connfd, char *server_name, char *port_num, char *name, char *service, char *recvline)
{
    

    int    sockfd, n;
    struct sockaddr_in servaddr; 
    socklen_t len;
    len = sizeof(servaddr);
    int r;
    inet_pton(AF_INET,server_name,&servaddr.sin_addr);
    
    /*convert Ip address to hostname*/
    r=getnameinfo((struct sockaddr *)&servaddr,len, name, sizeof name, port_num,sizeof port_num, 0); 
    printf("%s\n",name);
    if (r)
    {
    printf("%s\n", gai_strerror(r));
    exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port_num));
    if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket error\n");
        exit(1);
    }
    if(connect(sockfd, (struct sockaddr *)&servaddr,len)<0)
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
    connfd = sockfd;
    return 0;
}

#if DEBUG
int ip_to_hostname(char *server_name, char *port_num, char *name, char *service, char *recvline)
{
    int     sockfd, n;
    struct sockaddr_in *servaddr; 
    struct addrinfo hints, *res, *p;
    socklen_t len;
    len = sizeof(servaddr);

    bzero(&servaddr, sizeof(servaddr));
     //if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(server_name,port_num, &hints, &res); 
    //if (inet_pton(AF_INET,argv[1], &servaddr.sin_addr) <= 0) {
    for(p= res;p!=NULL;p=p->ai_next)
    {
        servaddr = (struct sockaddr_in *)p->ai_addr;   
    }
    inet_pton(AF_INET,server_name,servaddr.sin_addr);
    int r = getnameinfo((struct sockaddr *) &servaddr,len, name, sizeof name, port_num,sizeof port_num, 0);  
    if (r)
    {
    printf("%s\n", gai_strerror(r));
    exit(1);
    }
    printf("%s\n",hostName);
    if( (sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol)) < 0) {
        printf("socket error\n");
        exit(1);
    }
    if(connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
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
#endif
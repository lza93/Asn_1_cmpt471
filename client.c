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
int hostname_to_ip(char*,char*,char*,char*);
int ip_to_hostname(char*,char*,char*,char*,char*);
int main(int argc, char **argv)
{
    char  recvline[MAXLINE + 1];
    char *server_name = argv[1];
    char *port_num = argv[2];
    //char *tunnel_IP_Name;
    //char *tunnel_port;
    //tunnel_IP_Name = argv[3];
    //tunnel_port = argv[4];
    char hostIp[MAXLINE];
    char hostName[MAXLINE];
    char service[MAXLINE];


    if (argc != 3 && argc != 5) {
        printf("usage: %s <IPaddress and port>", argv[0]);
        exit(1);
    }
    if (argc==3){
        if(!isValidIpAddress(server_name)){
        hostname_to_ip(server_name, port_num, hostIp, recvline);
        printf("hostname:%s\nIpaddress:%s\n",server_name, hostIp);
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }
        else
        {
            ip_to_hostname(server_name, port_num, hostName, service, recvline);
            printf("hostname:%s\nIpaddress:%s\n",hostName,server_name);
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
int isValidIpAddress(char *server_name)
{
    struct sockaddr_in servaddr;
    int result = inet_pton(AF_INET,server_name,&(servaddr.sin_addr));
    return result !=0;
}
int hostname_to_ip(char *server_name, char *port_num, char *hostIp, char *recvline)
{
    int     sockfd, n;
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
        strcpy(hostIp, inet_ntoa(servaddr->sin_addr));
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
    return 0;
}

int ip_to_hostname(char *server_name, char *port_num, char *hostName, char *service, char *recvline)
{
    

    int     sockfd, n;
    socklen_t len;
    struct sockaddr_in servaddr; 
    len = sizeof(servaddr);
    int r;
    if ( (sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket error\n");
        exit(1);
    }
    inet_pton(AF_INET, server_name, &servaddr.sin_addr);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port_num));
    
    if(connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr))<0)
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
    /*convert Ip address to hostname*/
    r=getnameinfo((struct sockaddr *) &servaddr,len, hostName, sizeof hostName, port_num,sizeof port_num, 0); 
    if (r)
    {
    printf("%s\n", gai_strerror(r));
    exit(1);
    }
    printf("%s\n",hostName);
    return 0;
}

#if DEBUG
int ip_to_hostname(char *server_name, char *port_num, char *hostName, char *recvline)
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
    getnameinfo((struct sockaddr *) &servaddr,len, hostName, sizeof hostName, port_num,sizeof port_num, 0);  
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
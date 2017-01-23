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
#define DEBUG        0

int isValidIpAddress(char*);
void hostname_to_ip(int, char*,char*,char*,char*);
void ip_to_hostname(int, char*,char*,char*,char*);
int main(int argc, char **argv)
{
    int  connfd = 0;
    char recvline[MAXLINE + 1];
    char ip[MAXLINE];
    char name[MAXLINE];
    


    if (argc != 3 && argc != 5) {
        printf("usage: %s <IPaddress and port>", argv[0]);
        exit(1);
    }

/*******************************************************************************************
*No tunnel using case
********************************************************************************************/
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
            ip_to_hostname(connfd, server_name, port_num, name,recvline);
            printf("hostname:%s\nIpaddress:%s\n",name,server_name);
            if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }
   
    }
/***********************************************************************************************
* Tunnel Using Case
*
************************************************************************************************/
    else{
        char *server_name = argv[3];
        char *port_num = argv[4];
        char *tunnel_name = argv[1];
        char *tunnel_port = argv[2];

        char stat[MAXLINE];

        /*host name input case*/
        if(!isValidIpAddress(tunnel_name))
        {
            hostname_to_ip(connfd, tunnel_name, tunnel_port, ip, recvline);
            printf("writing server name: %s\n",server_name);
            
            write(connfd, server_name, strlen(server_name)+1);
            
            printf("writing server port: %s\n",port_num);
            write(connfd, port_num, strlen(port_num)+1);
            
            printf("Reading response from server\n");
           
            read(connfd,stat,strlen(stat)+1);
            printf("time: %s\n", stat);

            printf("Via Tunnel:%s\nIP Address:%s\nPort Number:%s\n",tunnel_name,ip, tunnel_port);
            if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            }
        }

        /* ip input case*/
        else
        {
            ip_to_hostname(connfd, tunnel_name, tunnel_port, name, recvline);
            printf("writing server name: %s\n",server_name);
            write(connfd, server_name, strlen(server_name));
            
            printf("writing server port: %s\n",port_num);
            write(connfd, port_num, strlen(port_num));
            
            printf("Reading response from server\n");
            char stat[1];
            read(connfd,stat,1);

            printf("Via Tunnel:%s\nIP Address:%s\nPort Number:%s\n",name,tunnel_name, tunnel_port);
            if (fputs(recvline, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
        }
    }
    close(connfd);
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
    return result !=0;
}

/******************************************************************************************
* convert host name into ip address
*
*******************************************************************************************/
void hostname_to_ip(int connfd, char *server_name, char *port_num, char *ip, char *recvline)
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
}

/******************************************************************************************************** 
*convert ip address into host name
*
********************************************************************************************************/
void ip_to_hostname(int connfd, char *server_name, char *port_num, char *name, char *recvline)
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
}
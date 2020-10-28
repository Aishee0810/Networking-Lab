#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include<unistd.h>
 
#define len 1300

int main(int argc, char *argv[])
{
    struct sockaddr_in  cli_addr;
    int portno = atoi(argv[1]);

    int sockfd,i,recv_len,socklen;
    socklen = sizeof(cli_addr);
    unsigned char buf[len];
     
    sockfd= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
     
    struct sockaddr_in serv_addr;

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     
    bind(sockfd , (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    while(1)
    {
        fflush(stdout);
        bzero(buf, len);
        
        recv_len = recvfrom(sockfd, buf, len, 0, (struct sockaddr *) &cli_addr, &socklen);
         
       // printf("Received packet from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
       // printf("Received text: %s\n" , buf);
        buf[6] = buf[6] - 1;

        sendto(sockfd, buf, recv_len, 0, (struct sockaddr*) &cli_addr, socklen);
    }
 
    close(sockfd);
}
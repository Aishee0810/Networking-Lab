#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#define SA struct sockaddr 
#define r 100

void *sender(void *socket) 
{ 
    char buff[80]; 
    int n,sockfd,p; 
    for (;;) { 
        
        scanf("%d",&p);
        sockfd=*(((int*)socket)+p-1);
        bzero(buff, sizeof(buff)); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
  
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server is Exiting\n"); 
            break; 
        } 
    } 
} 
void *receiver(void *socket)
{
    char buff[80]; 
    int n,sockfd; 
    sockfd=*(int*)socket;
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("Message Received:%s", buff); 
        write(sockfd, buff, sizeof(buff));
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Server is Exiting\n"); 
            break; 
        } 
        
    
    }
}
// Driver function 
int main(int argc, char ** argv) 
{ 
    int sockfd, cfd[r], len,i; 
    struct sockaddr_in servaddr, cli_addr;
    pthread_t th1,th2; 
    int portno=atoi(argv[1]);
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Failed to create socket.\n"); 
        exit(0); 
    } 
    else
        printf("Socket created.\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(portno); 
  
    // Bind new socket to ip 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Failed to bind.\n"); 
        exit(0); 
    } 
    else
        printf("Socket binded.\n"); 
  
    // listening to accept connections
    if ((listen(sockfd, 5)) != 0) { 
        printf("Not ready to accept connections.\n"); 
        exit(0); 
    } 
    else
        printf("Server listening for connections.\n"); 
    len = sizeof(cli_addr); 
    
    
    pthread_create(&th1,NULL,sender,(void*)&cfd) ;
    i=0;
    while(cfd[i]=accept(sockfd, (SA*)&cli_addr, &len))
    {
        printf("Client successfully accepted.\n");
        pthread_create(&th2,NULL,receiver,(void*)&cfd[i]) ;
        i++;
    }
    
    
    //pthread_join(th1,NULL);
    //pthread_join(th2,NULL);
    close(sockfd); 
} 

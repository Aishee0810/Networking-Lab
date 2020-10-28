#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/select.h>
#define sockaddr struct sockaddr 
#define r 100
int cfd[r];
int l=0;
void *sender(void *socket) 
{ 
    char buff[80]; 
    int n,sockfd,p; 
    
    for (;;) { 
       
        scanf("%d",&p);
        sockfd=*(((int*)socket)+p-1);
       
        memset(buff,0,20*(sizeof(buff[0])));
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
    
    for (;;) { 
        sockfd=*(int*)socket;
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        //printf("                    >>%s", buff+2);
        sockfd=cfd[(buff[0]-'0')-1];
        write(sockfd, buff+2, sizeof(buff));
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Server is Exiting\n"); 
            break; 
        } 
        
    
     }
 }

         

// Driver function 
int main(int argc, char ** argv) 
{ 
    int sockfd,len,i; 
    struct sockaddr_in servaddr, cli_addr;
    pthread_t th1,th2; 
    int portno=atoi(argv[1]);
    // creation of socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Failed to create socket.\n"); 
        exit(0); 
    } 
    else
        printf("Socket created.\n"); 
   
    memset(&servaddr,0,sizeof(servaddr));
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(portno); 
  
    // Binding new socket to ip
    if ((bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Failed to bind.\n"); 
        exit(0); 
    } 
    else
        printf("Socket  binded.\n"); 
  
    // Listening for connections
    if ((listen(sockfd, 10)) != 0) { 
        printf("Not ready to accept clients.\n"); 
        exit(0); 
    } 
    else
        printf("Server listening for new connections...\n"); 
    len = sizeof(cli_addr); 
    
    
    
    pthread_create(&th1,NULL,sender,(void*)&cfd) ;
   
   i=0;
    while(cfd[i]=accept(sockfd, (sockaddr*)&cli_addr, &len))
    {
        pthread_create(&th2,NULL,receiver,(void*)&cfd[i]) ;
        i++;
    }
    
    
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    close(sockfd); 
} 

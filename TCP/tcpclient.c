#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>
#define MAX 80 
//#define PORT 8901
#define SA struct sockaddr 
void *sender(void* socket) 
{ 
	char buff[MAX]; 
	int n,sockfd; 
	sockfd=*(int*)socket;
	for (;;) { 
		bzero(buff, sizeof(buff)); 
		//printf("Enter message : "); 
		n = 0; 
		while ((buff[n++] = getchar()) != '\n');
		write(sockfd, buff, sizeof(buff)); 
	//	bzero(buff, sizeof(buff)); 
		//read(sockfd, buff, sizeof(buff)); 
		//printf("Server sends back: %s \n",buff); 
		// if ((strncmp(buff, "exit", 4)) == 0) { 
		// 	printf("Client is Exiting\n"); 
		// 	exit(0);
		if(strlen(buff)==0)
		{
			close(sockfd);
			
		}
		} 
	} 

void *receiver(void* socket)
{
	char buff[MAX]; 
	int n,sockfd; 
	sockfd=*(int*)socket;
	while(1)
		{
			bzero(buff, sizeof(buff)); 
		read(sockfd, buff, sizeof(buff)); 
		printf("    >> %s \n", buff); 
		// if ((strncmp(buff, "exit", 4)) == 0) { 
		// 	printf("cli_addrent is Exiting\n"); 
		// 	exit(0); 
		if(strlen(buff)==0)
		{
			printf("Client is exiting\n");
			close(sockfd);

		}
		} 
	}


int main(int argc, char *argv[]) 
{ 
	int sockfd, cfd; 
	struct sockaddr_in servaddr, cli_addr; 
	pthread_t th1,th2; 
	int portno=atoi(argv[1]);
	// Create socket
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
      
      servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//servaddr.sin_addr.s_addr = inet_addr("10.2.64.181");
	servaddr.sin_port = htons(portno); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("Failed to connect to server.\n"); 
		exit(0); 
	} 
	else
		printf("Connected to server.\n"); 

	
	pthread_create(&th1,NULL,sender,(void*)(&sockfd)) ;
    pthread_create(&th2,NULL,receiver,(void*)(&sockfd)) ;
	
	pthread_join(th1,NULL);
    pthread_join(th2,NULL);
	close(sockfd);
}

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 516  //buffer size
#define PORT 69

int get_sock_fd()
{
    int sockfd;
    sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   	return sockfd;
}
void fill_struct(char* server_host, int portno ,struct sockaddr_in *saddr , int addrlen)
{
    memset((char *)saddr, 0, addrlen);
    saddr->sin_family = AF_INET;
    saddr->sin_port = htons(portno);
     
    inet_aton(server_host , &(saddr->sin_addr));
}

struct sockaddr_in saddr;
int sockfd, serv_len;


void sendfile(char *host ,char *file)
{
	FILE *fptr;
	fptr = fopen(file, "r");
	int filelen = strlen(file);
	// first send a Write Request to port 69
	unsigned char message[516]  , buf[516];
	bzero(message, 516);
	message[0]=0;
	message[1]=2; //the Write Request opcode
	strcpy(message+2, file);
	strcpy(message+2+filelen+1 ,"octet");
	int rlen=2+strlen(file)+1+strlen("octet")+1;

	sendto(sockfd, message, rlen , 0 , (struct sockaddr *) &saddr, serv_len);
  
    printf("Write Request sent.\n");
  

    int n;
    int bno=0;
    struct sockaddr_in recvaddr ;
    int addrlen= sizeof(recvaddr);

    int i;
    int ackblock;
    while(1)
    {
    	//int f=0;
    	for(i=1;i<=10;i++)
    	{
    		bzero(buf,SIZE);
    		recvfrom(sockfd, buf, SIZE, 0, (struct sockaddr *) &recvaddr, &addrlen);
		    ackblock = (buf[2]<<8) + buf[3];
		   

		    if((buf[1]==5) || (ackblock==(bno-1)))
		    {
		    	printf("Error sending block number %d.\nRetrying...\n", bno);
		    	sendto(sockfd, message, SIZE , 0 , (struct sockaddr *) &recvaddr, addrlen);

		    }
			 else
		    	break;
    	}
    	if(i>10)
    		{
    			printf("Sending aborted. \n" );
    			return ;
    		}
    	

	    printf("ACK received for block %d.\n", bno);
	    fflush(stdout);

	    bno++;
    	bzero(message, SIZE);
    	message[1]=3;
    	message[2]=bno>>8;
		message[3]=bno%(256);

    	n = fread(message+4 , 1 , 512 , fp);
    	printf("Block %d of %d bytes being sent.\n", bno,n);
    	if((bno%25)!=0)
    	{
    		sendto(sockfd, message, n+4 , 0 , (struct sockaddr *) &recvaddr, addrlen);
	    }	
    	if(n<512){
    		fclose(fp);
    		break;
    	}
    }
    
    printf("Transfer successful.\n");


}


void recfile(char *host ,char *file)
{
	FILE *fp;
	fp = fopen(file, "w");
	int file_len = strlen(file);

	/// first send a Read Request to port 69
	unsigned char message[516]  , buf[516];
	bzero(message, SIZE);
	message[0]=0;
	message[1]=1; //the read request opcode
	strcpy(message+2, file);
	strcpy(message+2+file_len+1 , "octet");

	//send Read Request
	sendto(sockfd, message, 516 , 0 , (struct sockaddr *) &saddr, serv_len);
    printf(" Read request sent.\n");


    int n;
    int bno=1;
    struct sockaddr_in recvaddr ;
    int addrlen= sizeof(recvaddr);

    while(1)
    {
    	addrlen= sizeof(recvaddr);
    	bzero(buf,516);
    	n = recvfrom(sockfd, buf, 516, 0, (struct sockaddr *) &recvaddr, &addrlen);
    	
    	//if (n == -1)
	    n -= 4;
	    if(buf[1]==5)
	    	perror("Server transfer failed.");
	    	     
	    //printf("Received data : %s\n",buf+4);

	    fwrite(&buf[4],1,n,fp);
	    printf("Received block of size  %d\n", n);

	    bzero(message, SIZE);
		message[0]=0;
		message[1]=4;
		message[2]=bno>>8;
		message[3]=bno%(256);

		sendto(sockfd, message, 4 , 0 , (struct sockaddr *) &recvaddr, addrlen);
	    printf("Sent ACK for block %d.\n", bno);
		
		bno++;
	    if(n<512)
	    {
	    	break;
	    }

    }

    fclose(fp);
    printf("Transfer complete.\n");

}
int main(int argc , char **argv)
{
	
    char *server_ip = argv[1];

    int  i;

    serv_len=sizeof(saddr);
    sockfd = get_sock_fd();
    fill_struct(server_ip, 69 , &saddr , serv_len);
    struct timeval tout;      
    tout.tv_sec = 10;
    tout.tv_usec = 0;

    setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tout,sizeof(tout));
        

    setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tout,sizeof(tout));
      

	char cmd[100] , file[100];
	while(1)
	{
		printf("tftp > ");
		scanf("%s",cmd);
		if(strcmp(cmd,"quit")==0)
		{
			break;
		}
				
		if(strcmp(cmd,"put")==0)
		{
			scanf("%s",file);
			sendfile(server_ip, file);
		}

		if(strcmp(cmd,"get")==0)
		{
			scanf("%s",file);
			recfile(server_ip, file);
		}

	}
	return 0;
}

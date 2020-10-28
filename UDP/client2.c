#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>
 
#define size 1300  
 
struct timeval tv,tv1,tvi;

void swap(unsigned long int* x, unsigned long int* y)
{
    unsigned long int temp=*x;
    *x=*y;
    *y=temp;
}
void bubblesort(unsigned long int arr[],int n)
{
    int i,j;
    for (i = 0; i < n-1; i++)       
        { 
            for (j = 0; j< n-i-1; j++)  
            {
                if (arr[j] > arr[j+1])
                {
                    
                    swap(&arr[j],&arr[j+1]);
                } 
            }
        }
}
int main(int argc , char *argv[])
{

    int portno =  atoi(argv[1]);
    int seqno=0,ttl=atoi(argv[2]),t;
    char *serverip=argv[3];
    struct sockaddr_in saddr;
    int sockfd, i,j,k,x=0, ssize=sizeof(saddr),s=atoi(argv[4]),np=atoi(argv[5]);
    unsigned long int rtt,sum,mrtt[np],median[13];
 
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&saddr, 0, ssize);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(portno);
    inet_aton(serverip , &(saddr.sin_addr));

    gettimeofday(&tvi,NULL);
    unsigned long int initial = tvi.tv_sec * 1000000 + tvi.tv_usec;

    while(s<=1300)
    {
       
        unsigned char buf[s];
        unsigned char message[s];
        j=0;
        sum=0;
        while(j<np)
        {
            bzero(buf, s);
            bzero(message, s);
            strcpy(buf,"x");
            for(i=7;i<s;i++)
                memcpy(message+i,buf,1);
            message[s] = '\0';
    
            message[0] = (seqno+1)/256;
            message[1] = (seqno+1)%256;

            message[6] = ttl;

            gettimeofday(&tv,NULL);

            unsigned long int begin= tv.tv_sec * 1000000 + tv.tv_usec;
            unsigned long int t1 =begin - initial;
            unsigned long int t=(t1%(256*256*256));

            message[2] = t1>>24;
            message[3]=t>>16;
            message[4]=(t%(256*256))>>8;
            message[5]=(t%(256*256)%256;
            

            sendto(sockfd, message, s , 0 , (struct sockaddr *) &saddr, ssize);
         
            memset(buf,'\0', s);
        
           recvfrom(sockfd, buf, s, 0, (struct sockaddr *) &saddr, &ssize);


            while(1)
            {
                buf[6] = buf[6]-1;

                if(buf[6] == 0)
                {
                    gettimeofday(&tv1,NULL);
                    break;
                }

                sendto(sockfd, buf, s , 0 , (struct sockaddr *) &saddr, ssize);
         
                memset(buf,'\0', s);
            
                recvfrom(sockfd, buf, s, 0, (struct sockaddr *) &saddr, &ssize);
                
            } 
        
            unsigned long int end = tv1.tv_sec * 1000000 + tv1.tv_usec;

            unsigned long int d = (buf[2]<<24)+(buf[3]<<16)+(buf[4]<<8)+buf[5];
            

            unsigned long int t2 = end - initial;
            // rtt = t2 - d;
            // sum=sum+rtt;
            mrtt[j] = t2 - d;

          
            seqno++;
            j++;
        }
       bubblesort(mrtt,np);

        if((np%2)!=0)
            sum = mrtt[np/2];
        else
            sum = (mrtt[(np-1)/2]+mrtt[np/2])/2;

         median[x] = sum;

      
        x++;
        s = s+100;
    }
    FILE *fptr;
    fptr=fopen(argv[6],"w+");
    for(i=100;i<=1300;i+=100)
        fprintf(fptr,"%d\t%lu \n",i,median[((i)/100)-1]);
    //fprintf(fptr,"%d\t%lu \n",s,sum);
    fclose(fptr);
    close(sockfd);
    return 0;
}
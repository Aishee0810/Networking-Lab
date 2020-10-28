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

int main(int argc , char *argv[])
{

    int portno =  atoi(argv[1]);
    int seqno=1,ttl=atoi(argv[2]),t;
    char *sip=argv[3];
    struct sockaddr_in saddr;
    int sockfd, i,j,k,x=0, ssize=sizeof(saddr),s=atoi(argv[4]),np=atoi(argv[5]);
    unsigned long int rtt,sum;
 
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset(&saddr, 0, ssize);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(portno);
    inet_aton(sip , &(saddr.sin_addr));

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
            memset(buf,0,s);
            memset(message,0,s);
            strcpy(buf,"x");
            for(i=7;i<s;i++)
                memcpy(message+i,buf,1);
            message[s] = '\0';
    
            message[0] = (seqno)/256;
            message[1] = (seqno)%256;

            message[6] = ttl;

            gettimeofday(&tv,NULL);

            unsigned long int begin= tv.tv_sec * 1000000 + tv.tv_usec;
            unsigned long int t1 = begin- initial;
            unsigned long int t=(t1%(256*256*256));

            message[2] = t1>>24;
            message[3]=t>>16;
            message[4]=(t%(256*256))>>8;
            message[5]=(t%(256*256))%(256);
            

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

            unsigned long int d= (buf[2]<<24)+(buf[3]<<16)+(buf[4]<<8)+buf[5];
            

            unsigned long int t2 = end - initial;
            rtt = t2 - d;
            sum=sum+rtt;
                 
            seqno++;
            j++;
        }
        sum = sum/np;
      // printf("%lu\n",sum);
        FILE *fptr;
        fptr=fopen(argv[6],"a");
        fprintf(fptr,"%d\t%lu \n",s,sum);
        fclose(fptr);
        x++;
        s = s+100;
    }
     close(sockfd);
    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<dos.h>

#define swint 0x60
#define maxpack 200
unsigned char opack[64];
unsigned char type[]="\xff\xff";
unsigned char ipack[maxpack];
unsigned char a[2];
unsigned char mac[6];
unsigned char destmac[]="\x08\x00\x27\x25\xdb\x2d";//mac address of sender
unsigned char broadcast_mac[]="\xff\xff\xff\xff\xff\xff";
unsigned char g[200];
unsigned char x[10];
int i,ipacklen,handle,p,k;
void getmymac()
{
    union REGS iregs,oregs;
    struct SREGS segregs;
    char far *bfr;
    segread(&segregs);
    bfr=(char far *)mac;
    segregs.es=FP_SEG(bfr);
    iregs.x.di=FP_OFF(mac);
    iregs.x.cx=6;
    iregs.h.ah=6;
    int86x(swint,&iregs,&oregs,&segregs);
}
void fillpacketheaders()
{
        memcpy(opack,destmac,6);
        memcpy(opack+6,mac,6);
        memcpy(opack+12,type,2);
}
void fillmsg(unsigned char *m,int l)
{
//        memcpy(opack+15,m,l);
        opack[15]=atoi(m);
        for(i=l+15;i<60;i++)
        {
                opack[i]=0;
        }
}
void sendpacket()
{
        union REGS iregs,oregs;
        struct SREGS segregs;
        segread(&segregs);
        iregs.h.ah=4;
        segregs.ds=FP_SEG(opack);
        iregs.x.si=FP_OFF(opack);
        iregs.x.cx=64;
        int86x(swint,&iregs,&oregs,&segregs);
}
long int factorial(int x)
{ 
        if(x==0 || x==1)
                return 1;
        else
                return(x*factorial(x-1));
}        
void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags)
unsigned short bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags;
{
        if(ax==0)
        {
                if(cx>maxpack)
                {
                        es=0;
                        di=0;
                        return;
                }
                es=FP_SEG(ipack);
                di=FP_OFF(ipack);
                ipacklen=cx;
         }
         else
         {
              if(ipack[14]==1)
              {
              opack[14]=2;  
              if(memcmp(ipack,broadcast_mac,6)==0)
              {
                return;
              }
         for(i=6;i<12;i++)
          {
               cprintf("%x:",ipack[i]);
          }
        cprintf("\r\n");
               k=0;
              for(i=15;i<ipacklen;i++)
              {
                    if(ipack[i]==0x00)
                        break;
                   // cprintf("%x ",ipack[i]-'0');
                        cprintf("%d ",ipack[i]);
                    k++;
              }
           //  putch('\r');
             //putch('\n');
               p=factorial(ipack[15]);
               cprintf("\n");
               cprintf("The  factorial of %d is:%d\n",ipack[15],p);
               opack[15]=p%256;
               opack[16]=p/256;
               for(i=17;i<40;i++)
               {
                  opack[i]=0;
               }
               sendpacket();
               cprintf("\r\n");
               for(i=0;i<18;i++)
               {
                           cprintf("%x:",opack[i]);
                       }
 //               opack[14]='s';
                cprintf("\n");
                return;
               }
               else if(ipack[14]==2)
               {
                        opack[14]=1;
                        if(memcmp(ipack,broadcast_mac,6)==0)
              {
                return;
              }
        /* for(i=6;i<12;i++)
          {
               cprintf("%x:",ipack[i]);
          }*/
        cprintf("\r\n");
               k=0;
              for(i=15;i<ipacklen;i++)
              {
                    if(ipack[i]==0x00)
                        break;
             }
             cprintf("%d",ipack[16]*256+ipack[15]);
            cprintf("\n");
             for(i=0;i<20;i++)
             {
                        cprintf("%x ",ipack[i]);
                }
            // putch('\n');
             cprintf("\n");
             return;
             }
              return;
             }
}
int accesstype()
{
        union REGS iregs,oregs;
        struct SREGS segregs;
        iregs.h.al=1;
        iregs.x.bx=-1;
        iregs.h.dl=0;        
        iregs.x.cx=0;
        iregs.h.ah=2;
        segregs.es=FP_SEG(receiver);
        iregs.x.di=FP_OFF(receiver);
        a[0]=0xff;
        a[1]=0xff;
        segregs.ds=FP_SEG(a);
        iregs.x.si=FP_OFF(a);
        int86x(swint,&iregs,&oregs,&segregs);
        printf("Carry flag access type:%d\n",oregs.x.cflag);
        printf("Handle: %d\n",oregs.x.ax);
        handle=oregs.x.ax;
        return oregs.x.cflag;
}
void releasetype()
{
        union REGS iregs,oregs;
        struct SREGS segregs;
        iregs.h.ah=3;
        iregs.x.bx=handle;
        int86x(0x60,&iregs,&oregs,&segregs);
}
int main()
{
        getmymac();
        fillpacketheaders();
 //       opack[14]='s';
        accesstype();
        while(1)
        {
               x[0]=getchar();
               fflush(stdin);
                opack[14]=1;
               if(x[0]!='n' && x[0]!=0x0D && x[0]!=0x0A)
               {
                        fillmsg(x,strlen(x));
                        sendpacket();
                }
                else if(x[0]==0x0D && x[0]==0x0A)
                        continue;
                else
                        break;
               for(i=0;i<18;i++)
               {
                        cprintf(" %x",opack[i]);
                }
                cprintf("\n");
              /* if(getchar()!='n')
                {
                        continue;
                }
                else
                 break;*/
               
        }
        releasetype();
        return 0;
}        

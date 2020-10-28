#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<dos.h>
#define swint 0x60
#define maxpack 200
unsigned char ipack[maxpack];
unsigned char a[2];
unsigned char mac[6];
unsigned char broadcast_mac[]="\xff\xff\xff\xff\xff\xff";
int i,ipacklen,handle,k,p;
int number_i[];
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
              if(memcmp(ipack,broadcast_mac,6)==0)
              {
                return;
              }
         // for(i=6;i<12;i++)
           // {
             //  cprintf("%02x:",ipack[i]);
            //}
         // cprintf(">");
              k=0;
              for(i=14;i<ipacklen;i++)
              {
                    if(ipack[i]=0x00)
                        break;
                    cprintf("%c",ipack[i]);
                    number_i[k]=ipack[i];
                    k++;
                   //utch(ipack[i]);
               }
               putch('\r');
               putch('\n');
               p=atoi(number_i);
               printf("The  factorial of %d is:%d\n",p,factorial(p));
               //rintf("Message printed!");
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
        accesstype();
        while(1)
        {
               if(getchar()!='n')
                {
                        continue;
                }
                else
                 break;
        }
        releasetype();
}        

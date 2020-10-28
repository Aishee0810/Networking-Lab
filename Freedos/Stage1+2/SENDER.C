#include<dos.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<string.h>

#define swint 0x60
#define maxbuf 100
#define maxpack 200

unsigned char opack[64];
unsigned char mymac[6];
unsigned char c[2];
unsigned char type[]="\xff\xff";
int i,ipacklen,handle;
unsigned char broadcast_mac[]="\xff\xff\xff\xff\xff\xff";
unsigned char ipack[maxpack];
unsigned char a[2];
//unsigned char destmac[]="\x08\x00\x27\x55\x3d\xed";//mac of clone
unsigned char destmac[]="\x08/x00\x27\x0f\x20\x6d";
//unsigned char destmac[]="\x08\x00\x27\x53\x37\xda";
unsigned char x[100];
void  getmymac()
{
        union REGS iregs,oregs;
        struct SREGS segregs;
        char far *bfr;
        segread(&segregs);
        bfr=(char far *)mymac;
        segregs.es=FP_SEG(bfr);
        iregs.x.di=FP_OFF(mymac);
        iregs.x.cx=6;
        iregs.h.ah=6;
        int86x(swint,&iregs,&oregs,&segregs);
}
void fillpacketheaders()
{
        memcpy(opack,destmac,6);
        memcpy(opack+6,mymac,6);
        memcpy(opack+12,type,2);
}
void fillmsg(unsigned char *m,int l)
{
//       memcpy(opack+14,m,l);// entering zero to the others
       // for(i=14;i<l;i++)
       // {   
        opack[14]=atoi(m);
       // }
           for(i=l+14;i<60;i++)
                opack[i]=0;
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
void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags)
unsigned short bp,di,si,ds,es,dx,cx,ax,bx,ip,cs,flags;
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
             /*   for(i=6;i<12;i++)
                {
                        cprintf(" %02x ",ipack[i]);
                }*/
                cprintf("\r\n");
                for(i=14;i<ipacklen;i++)
                {
                        if(ipack[i]==0x00)
                                break;
                      //  cprintf("%d ",ipack[i]);
                }
                cprintf("%d",ipack[15]*256+ipack[14]);
                cprintf("\n");
                for(i=0;i<20;i++)
                {
                        cprintf("%x ",ipack[i]);
                }
                //cprintf("\n%d",ipack[15]*256+ipack[14]);
                putch('\r');
                putch('\n');
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
        iregs.h.ah=2;
        iregs.x.cx=0;
        segregs.es=FP_SEG(receiver);
        iregs.x.di=FP_OFF(receiver);
        a[0]=0xff;
        a[1]=0xff;
        segregs.ds=FP_SEG(a);
        iregs.x.si=FP_OFF(a);
        int86x(swint,&iregs,&oregs,&segregs);
        printf("Carry flag access type:%d\n",oregs.x.cflag);
        printf("Handle:%d\n",oregs.x.ax);
        handle=oregs.x.ax;
        return oregs.x.cflag;
}
void releasetype()
{
        union REGS iregs,oregs;
        struct SREGS segregs;
        iregs.h.ah=3;
        iregs.x.bx=handle;
        int86x(swint,&iregs,&oregs,&segregs);
}
void main()
{
        getmymac();
        fillpacketheaders();
        accesstype();
        while(1)
        {
        x[0]=getchar();
        fflush(stdin);
         //  cscanf(%"d",&x);
        if(x[0]!='n' && x[0]!=0x0D && x[0]!=0x0A)
        {
                fillmsg(x,strlen(x));
                sendpacket();
        }
        else if(x[0]==0x0D || x[0]== 0x0A)
                continue;
        else
                break;
        for(i=0;i<18;i++)
        {
                printf(" %x",opack[i]);
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
}

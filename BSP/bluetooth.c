#include "config.h"
#include "bluetooth.h"
#include "option.h"
#include "2416addr.h"  
#include "2416lib.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

extern OS_EVENT  *Tx_Sem,*Rx_Sem,*RxD_Sem;
extern U32 m_pclk;

#define TXD0READY (1 << 2)
#define RXD0READY (1 << 0)
void BToothCS(char cs)
{
    if(cs==1)
    rGPFDAT |=(1<<5);
    else
    rGPFDAT &=~(1<<5);
}
void Bluetooth_Serial_Init(int pclk,int baud)
{
     if(pclk==0)
            pclk=m_pclk;  
          	
     rUFCON2=0x0;    
     rUMCON2=0x0;   
     rULCON2=0x3; 
     rUCON2=(2<<10)|0x5; 
     rUBRDIV2=((int)(pclk/16./baud+0.5) -1); 
     while(!(rUTRSTAT2 & 0x4));
     rGPFCON &=~(3<<10);
     rGPFCON |=(1<<10);
     rGPFUDP = rGPFUDP & (~(1<<5));
}

void Bluetooth_Putbyte(unsigned  char chd)
{	
	rUTXH2 = chd;
	while(!(rUTRSTAT2 & TXD0READY));//等待上个字符发送完毕
}
char Bluetooth_Getdata(void)
{
     unsigned char get_data=0;
     if(rUTRSTAT2 & 0x1)
     {   
         return  RdURXH2();
     }
     else
     {
         return 0; 
     }
}
void Bluetooth_SendByte(char ch)
{
   if(ch=='\n')
   {
       while(!(rUTRSTAT2 &0x2))
       WrUTXH2('\r'); 
   }
   while(!(rUTRSTAT2 &0x2));
   WrUTXH2(ch);
}
void Bluetooth_Send(char *str)
{
    while(*str)
    {
       Bluetooth_SendByte(*str++);
    }
}
void Bluetooth_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Bluetooth_Send(string);
    va_end(ap); 
}
void RxAndTxIRQ(void)
{
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif 
    if(rSUBSRCPND&BIT_SUB_RXD2)
    {
        OSSemPost(Rx_Sem);     
    }
    OS_ENTER_CRITICAL(); 
    rSUBSRCPND|=BIT_SUB_RXD2;   
    ClearPending(BIT_UART2);
    OS_EXIT_CRITICAL();
}
void Change2DMARxMode(U8 index)
{ 
    if(index==0)
    rUCON0=rUCON0&(~(3<<0))|(3<<0);
    else if(index==2)
    rUCON2=rUCON2&(~(3<<0))|(3<<0);   
}
void Change2IRQRxMode(U8 index)
{
    if(index==0)
    rUCON0=rUCON0&(~(3<<0))|(1<<0); 
    else if(index==2)
    rUCON2=rUCON2&(~(3<<0))|(1<<0); 
}
void Change2DMATxMode(void)
{ 
    rUCON2=rUCON2&(~(3<<2))|(2<<2);    
}

void Change2IRQTxMode(void)
{
    rUCON2=rUCON2&(~(3<<2))|(1<<2); 
}
void RxIRQStart(void)
{
    EnableIrq(BIT_UART2);
    EnableSubIrq(BIT_SUB_RXD2);
    ClearPending(BIT_UART2);
    rSUBSRCPND=BIT_SUB_RXD2;
    pISR_UART2=(U32)RxAndTxIRQ;
}

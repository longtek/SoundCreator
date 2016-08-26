#include "2416addr.h" 
#include "config.h"
extern OS_EVENT  *Tx_Sem,*Rx_Sem,*RxD_Sem,*Sound_Sem;
extern S16  SoundData[64];
extern U32 WhFlag;
void DMAIntSeverInit(void)
{
      EnableIrq(BIT_DMA); 
      pISR_DMA =(unsigned int)DMA_IRQ;   
}
void DMA_Init(short *pData,unsigned int nSoundLend)
{
       rINTMOD1 = 0x0;           
       //ClearPending(BIT_DMA);               
       //rSUBSRCPND|=BIT_SUB_DMA2; 
       rDMAREQSEL2=(4<<1)|(1<<0);    
       rDISRC2 = (unsigned int )(pData);  
       rDISRCC2= (0<<1)|(0<<0);//AHB,置0时数据地址在传输完成之后自动w增加数据大小;
       rDIDST2 = (unsigned int )&rIISTXD; 
       rDIDSTC2= (0<<2)|(1<<1)|(1<<0);//APB,fixed;
       rDCON2  = (1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<22)+(1<<20)+nSoundLend;
       //Handshake,sync PLCK,TC int,single tx,27 single service,24I2SSDO,23I2Srequest,22 Auto-reload,half-word,size/2;
       rDMASKTRIG2=(0<<2)|(0<<1)|0;
       EnableSubIrq(BIT_SUB_DMA2);      
       //No-stop,DMA2 channel on,No-sw trigger5
}
void InitDMATxMode(char *pData,unsigned int nDataLend)
{
    rDMAREQSEL0=(23<<1)|(1<<0); 
    rDISRC0 = (unsigned int )(pData);
    rDISRCC0= (0<<1)|(0<<0);
    rDIDST0 = (unsigned int )&rUTXH2;
    rDIDSTC0= (0<<2)|(1<<1)|(1<<0);
    rDCON0  = (1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<22)+(0<<20)+nDataLend;
    rDMASKTRIG0=(0<<2)|(0<<1)|0;
    EnableSubIrq(BIT_SUB_DMA0);
}
void InitDMARxMode( char *pData,unsigned int nDataLend,unsigned char index)
{
    rDMAREQSEL1=((20+index*2)<<1)|(1<<0);
    if(index==2) 
    {rDISRC1 = (unsigned int )&rURXH2;}
    else if(index==0)
    {rDISRC1 = (unsigned int )&rURXH0;}
    rDISRCC1= (1<<1)|(1<<0);
    rDIDST1 = (unsigned int )(pData);
    rDIDSTC1= (0<<2)|(0<<1)|(0<<0);
    rDCON1  = (1<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(0<<24)+(1<<22)+(0<<20)+nDataLend;
    rDMASKTRIG1=(0<<2)|(1<<1)|0;
    EnableSubIrq(BIT_SUB_DMA1);
}
void DMA_IRQ(void) 
{  
    unsigned int DMA_Channel;
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif 
    OS_ENTER_CRITICAL(); 
    DMA_Channel=rSUBSRCPND;    
    if(DMA_Channel&(BIT_SUB_DMA2))
    {  
      if(WhFlag)
      {  
        rDISRC2 = (U32)(&SoundData[0]);
      }
      else
      {
        rDISRC2 = (U32)(&SoundData[32]);
      }
      rDMASKTRIG2=(0<<2)|(1<<1)|0; 
      OSSemPost(Sound_Sem);
    }        
    if(DMA_Channel&(BIT_SUB_DMA1))
    {  
       OSSemPost(RxD_Sem);                           
    } 
    if(DMA_Channel&(BIT_SUB_DMA0))
    {         
       OSSemPost(Tx_Sem);                           
    } 
    rSUBSRCPND|=DMA_Channel;        
    ClearPending(BIT_DMA);  
    OS_EXIT_CRITICAL(); 
}
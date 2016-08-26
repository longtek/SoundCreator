#include "config.h"
#define	EnableIrq(bit)		rINTMSK1 &= ~(bit)
#define	EnableSubIrq(bit)	rINTSUBMSK &= ~(bit)
#define DelayTime 900
#define Daddr  0x34
#define Reg15  0xf
#define Reg9   0x9
#define Reg8   0x8
#define Reg7   0x7
#define Reg6   0x6
#define Reg5   0x5
#define Reg4   0x4
#define Reg2   0x2
#define Reg3   0x3
static void udelay(int time) {
    while(time--);
}
static void IIC_Init(void)
{
    rGPEDAT|=GPX_X_WRITE(SDA);
    rGPEDAT|=GPX_X_WRITE(SCL);
}
void Wm8731AndIISPortInit()
{
     rGPECON = rGPECON& (~(0x3ff))| 0x2aa;//GPE0.1.2.3.4 out ;   10 1010 1010 
     rGPEUDP = rGPEUDP& (~(0x3ff));  //disable pull up function ; 
     rGPECON = rGPECON& (~(3<<28))|(1<<28);//SCL  GPE14
     rGPECON = rGPECON& (~(3<<30))|(1<<30);//SDA  GPE15
}
static void IIC_start(void)
{
    int i=0;
    rGPEDAT|=GPX_X_WRITE(SDA);
    for(i=0;i<DelayTime;i++);
    rGPEDAT|=GPX_X_WRITE(SCL);   
    for(i=0;i<DelayTime;i++);
    rGPEDAT&=GPX_X_CLEAR(SDA);
    for(i=0;i<DelayTime;i++);  
    rGPEDAT&=GPX_X_CLEAR(SCL);
    for(i=0;i<DelayTime;i++); 
}
static void IIC_stop(void)
{
    int i=0;
    rGPEDAT&=GPX_X_CLEAR(SCL);
    rGPEDAT&=GPX_X_CLEAR(SDA);
    for(i=0;i<DelayTime;i++);
    rGPEDAT|=GPX_X_WRITE(SCL);
    for(i=0;i<DelayTime;i++);  
    rGPEDAT|=GPX_X_WRITE(SDA);
    for(i=0;i<DelayTime;i++); 
}
static U8 Read_Ack(void)
{
    int i=0,ack=0;  
    rGPEDAT|=GPX_X_WRITE(SDA);
    for(i=0;i<DelayTime;i++); 
    rGPEDAT|=GPX_X_WRITE(SCL);
    for(i=0;i<DelayTime;i++);
    rGPECON = rGPECON&(~(3<<30));
    if(rGPEDAT&(1<<15))
    {
       ack=1;
    }
    rGPEDAT&=GPX_X_CLEAR(SCL); 
    for(i=0;i<DelayTime;i++);
    rGPECON = rGPECON&(~(3<<30))|(1<<30);
    return ack;
}
static void IIC_Write(char byte)
{
     int i=0,k=0;
     for(k=0;k<8;k++)
     {
         if(byte&0x80)
         {
              rGPEDAT|=GPX_X_WRITE(SDA); 
         }
         else
         {
              rGPEDAT&=GPX_X_CLEAR(SDA);
         }
         byte=byte<<1;
         for(i=0;i<DelayTime;i++);
         rGPEDAT|=GPX_X_WRITE(SCL);
         for(i=0;i<DelayTime;i++); 
         rGPEDAT&=GPX_X_CLEAR(SCL);
         for(i=0;i<DelayTime;i++); 
     } 
}
static void Wm8731WriteReg(unsigned char reg ,unsigned int data)
{	 	 
	 unsigned char waddr = (reg<<1)|((data>>8)&0x1);
	 unsigned char wData = data&0xff;
	 IIC_start();
	 IIC_Write(Daddr);
	 if(Read_Ack())
	 {
	     Uart_Printf("Daddr ack failed\n");
	 }
	 IIC_Write(waddr);
	 if(Read_Ack())
	 {
	     Uart_Printf("waddr ack failed\n");
	 }
	 IIC_Write(wData);
	 if(Read_Ack())
	 {
	     Uart_Printf("wData ack failed\n");
	 }
	 IIC_stop();
}
void Wm8731RegInit(void)
{
     Wm8731WriteReg(Reg15,0);//reset;
     Wm8731WriteReg(Reg6, 0x67);//
     Wm8731WriteReg(Reg4, 0x38);	  	
	 Wm8731WriteReg(Reg8, 0x20);//  	 
	 Wm8731WriteReg(Reg7, 0xe);// 16bit iis
	 Wm8731WriteReg(Reg5, 0x04);// 
	 Wm8731WriteReg(Reg9, 0x1);//
	// Wm8731WriteReg(Reg2,0xe0);
	// Wm8731WriteReg(Reg3,0xe0);
}
void IIS_Init(void)
{
     rCLKDIV1 &= ~(0xf<<12);
	 rCLKDIV1 |= (0<<12); // IIS clock 96M
	 rIISFIC=TFLUSH(1)|RFLUSH(0);
     rIISFIC=TFLUSH(0)|RFLUSH(0);
     rIISPSR=PSRAEN(1)|PSVALA(5);  
     rIISCON = 0;
     rIISCON=FTXURSTATUS(0)|FTXURINTEN(0)|TXDMAPAUSE(0)|RXDMAPAUSE(0)|TXCHPAUSE(0)|\
             RXCHPAUSE(0)|TXDMACTIVE(1)|RXDMACTIVE(0)|I2SACTIVE(1) ;        
     rIISMOD=(0<<16)|ChBitLength(DATA_16BIT)|CDCLKCON(0)|IISModeSelet(0)|TXRModeSelet(TxOnly)|\
             SDataFat(IIS)|CodeClkSelet(MSLK_256FS)|BitClkSelet(BFS_32FS);      
}

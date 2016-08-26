#include "config.h"
void TargetInit(void)
{
    int i;          
/*1. 设定系统时钟*/
	U8  key;
	U32 mpll_val=0;
    #if ADS10   
    __rt_lib_init(0,0); //for ADS 1.0
 
    #endif
			
	//ChangeMPllValue((mpll_val>>14)&0x1ff, (mpll_val>>5)&0x3f, mpll_val&3);
	//ChangeClockDivider(key,12,12);   

    rSCLKCON=rSCLKCON&(~(1<<9));
    //rPCLKCON=rPCLKCON&(~(1<<9));
/*端口初始化*/
  	Port_Init();
    CalcBusClk();  
/* 串口初始化*/
   // Delay(0);    
    Uart_Init(0,115200);
    Uart_Select(0);
    Uart_SendString("\nhello Sounding!\n");
	Bluetooth_Serial_Init(0,9600);
    BToothCS(TRUE);
    /* LED指示灯初始化  */
    led_Init();
    Led1_On();
 }
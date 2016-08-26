#include "led.h"
#include "2416addr.h"

void led_Init(void)
{
	rGPGCON &= 0xfffffc3f ;
	rGPGCON |= (1<<6)|(1<<8) ;
	rGPGDAT |= (0<<3)|(0<<4);
	
	rGPCCON &=~(3<<0);
	rGPCCON |=(1<<0);
	rGPCDAT |=(1<<0);
}









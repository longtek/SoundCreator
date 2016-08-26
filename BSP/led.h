#ifndef __LED_H__
#define __LED_H__

#include "2416addr.h"

#define Led0_On()    (rGPGDAT |= (1 <<3)) //red
#define Led0_Off()   (rGPGDAT &= ~(1 <<3)) 
#define Led1_On()    (rGPGDAT |= (1 <<4)) //yellow
#define Led1_Off()   (rGPGDAT &= ~(1 <<4)) 
#define Led2_Off()   (rGPCDAT |= (1 <<0))  
#define Led2_On()    (rGPCDAT &= ~(1 <<0)) 
extern void led_Init(void);

#endif






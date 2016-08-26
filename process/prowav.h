#ifndef _PROWAV_H_
#define _PROWAV_H_
#include "def.h"
extern void  ProcessBasicData(U32 *Fredata,U32 *Phase);
extern void  ProcessSpeedGain(U16 *Speeddata,float *SpeedGain);
extern void  ProcessThrottleGain(U16 *Thottledata,float *ThottleGain);
#endif
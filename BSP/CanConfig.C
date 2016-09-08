#include "canconfig.h"
#include "def.h"
CanConfig canconfig=0;
void GetCanConfigInfo(void)
{
     canconfig.CAN_bandrate=0;          
	canconfig.RPM.ID=94; 
	canconfig.RPM.BYTENUM=5;
	canconfig.RPM.BITPOS=7;
	canconfig.RPM.DATALEN=16;
	canconfig.RPM.ENDIAN=0;
	canconfig.RPM.DATACOEF=0.15625;
	canconfig.RPM.DEFAULT=16;
	canconfig.SPEED.ID=365;
	canconfig.SPEED.BYTENUM=8;
	canconfig.SPEED.BITPOS=7;
	canconfig.SPEED.DATALEN=8;
	canconfig.SPEED.ENDIAN=0;
	canconfig.SPEED.DATACOEF=0.390625;
	canconfig.SPEED.DEFAULT=0;
	canconfig.THROTTLE.ID=999;                 
	canconfig.THROTTLE.BYTENUM=8;
	canconfig.THROTTLE.BITPOS=7;
	canconfig.THROTTLE.DATALEN=8;
	canconfig.THROTTLE.ENDIAN=0;
	canconfig.THROTTLE.DATACOEF=0.390625;
	canconfig.THROTTLE.DEFAULT=0;
}
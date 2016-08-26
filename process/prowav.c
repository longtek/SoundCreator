#include "config.h"
#include "def.h"
#define  TPI 6.2831853
#define  SAMPLERATE  4410000
/*====================================================
// 语法格式：ProcessBasicData(U32 *Fredata,U32 *Phase)
   实现目标：1)读取初始相位
             2)计算各个转速下不同阶次每个样本点应该增加的相位值;
   
// 功能描述:
// 入口参数: 
// 出口参数: 
====================================================*/
void  ProcessBasicData(U32 *Fredata,U32 *Phase)
{
   U32 iOrder,iRpm;
   for(iOrder=0;iOrder<40;iOrder++)
   {
	   Phase[iOrder]=m_RpmPhase[iOrder]; //获取初始相位 
   }
   //??????
   for(iRpm=0;iRpm<1366;iRpm++)
   {
       for(iOrder=0;iOrder<40;iOrder++)
       {
         *(Fredata+iOrder+iRpm*40)=(int)(BasicFre[iRpm]*OrderData[iOrder]+0.5)<<1;//计算每个转速下各个阶次相位的增加
       }
   }   
}
/*====================================================
// 语法格式：ProcessSpeedGain(U16 *Speeddata,float *SpeedGain)
   实现目标：
             计算随车速变化声音幅值的增益系数;
   
// 功能描述:
// 入口参数: 
// 出口参数: 
====================================================*/
void ProcessSpeedGain(U16 *Speeddata,float *SpeedGain)
{
   U32 i;
   for(i=0;i<256;i++)
   {
       Speeddata[i]=65535;
       SpeedGain[i]=(float)Speeddata[i]/65535;
   }
}
/*====================================================
// 语法格式：ProcessThrottleGain(U16 *Throttledata,float *ThrottleGain)
   实现目标：计算随油门开度变化声音幅值的增益系数;
   
// 功能描述:
// 入口参数: 
// 出口参数: 
====================================================*/
void ProcessThrottleGain(U16 *Thottledata,float *ThottleGain)
{
   U32 i;
   for(i=0;i<128;i++)
   {
       Thottledata[i]=65535;
       ThottleGain[i]=(float)Thottledata[i]/65535;
   }
}








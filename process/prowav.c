#include "config.h"
#include "def.h"
#define  TPI 6.2831853
#define  SAMPLERATE  4410000
/*====================================================
// �﷨��ʽ��ProcessBasicData(U32 *Fredata,U32 *Phase)
   ʵ��Ŀ�꣺1)��ȡ��ʼ��λ
             2)�������ת���²�ͬ�״�ÿ��������Ӧ�����ӵ���λֵ;
   
// ��������:
// ��ڲ���: 
// ���ڲ���: 
====================================================*/
void  ProcessBasicData(U32 *Fredata,U32 *Phase)
{
   U32 iOrder,iRpm;
   for(iOrder=0;iOrder<40;iOrder++)
   {
	   Phase[iOrder]=m_RpmPhase[iOrder]; //��ȡ��ʼ��λ 
   }
   //??????
   for(iRpm=0;iRpm<1366;iRpm++)
   {
       for(iOrder=0;iOrder<40;iOrder++)
       {
         *(Fredata+iOrder+iRpm*40)=(int)(BasicFre[iRpm]*OrderData[iOrder]+0.5)<<1;//����ÿ��ת���¸����״���λ������
       }
   }   
}
/*====================================================
// �﷨��ʽ��ProcessSpeedGain(U16 *Speeddata,float *SpeedGain)
   ʵ��Ŀ�꣺
             �����泵�ٱ仯������ֵ������ϵ��;
   
// ��������:
// ��ڲ���: 
// ���ڲ���: 
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
// �﷨��ʽ��ProcessThrottleGain(U16 *Throttledata,float *ThrottleGain)
   ʵ��Ŀ�꣺���������ſ��ȱ仯������ֵ������ϵ��;
   
// ��������:
// ��ڲ���: 
// ���ڲ���: 
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








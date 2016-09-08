#include "config.h"
#include "app_cfg.h"
OS_STK   MainTaskStk[MainTaskStkLengh];
OS_STK   BToothTaskStk[BToothTaskStkLengh];
OS_STK   Can2515TaskStk[Can2515TaskStkLengh];
OS_STK   CheckTaskStk[CheckTaskStkLengh];
OS_STK   SoundTaskStk[SoundTaskStkLengh];
OS_STK   BTSendTaskStk[BTSendTaskStkLengh];
OS_STK   ProcessTaskStk[ProcessTaskStkLengh];
OS_EVENT *UartRx_Sem,*Sound_Sem,*Tx_Sem,*Rx_Sem,*Process_Sem; 
extern CanConfig canconfig;            
U8  Index=0;                            //selet DMA channel
S16 SoundData[64];                    //data cache buffer
U32 FileDataLenth=0,WhFlag=0;  
U8  m_CanFlash=0,m_Receive=0;         //CAN运行标示;
U8  m_IsSuspend=TRUE;                 //是否挂起Sound Task =1挂起;

        
float m_RpmVal=800,m_SpeedVal=50,m_ThrottleVal=50;       //三个CAN信息
U8  m_AmtIndex=0;       //转速位置;
U32 m_RpmIndex=0;
U16 pAmt[129][40];
U32 m_FreData[1366][40];  //记录进行时各阶次相位增加
U32 m_PhaseCnt[40];       //记录各阶次进行时相位
U16 m_AmtCnt[40];         //记录各阶次进行时幅值

float m_SpeedGain[256];   //速度增益
U32 m_SpeedIndex=0;
float m_ThrottleGain[128]; 
U32   m_ThrottleIndex;
char SendBuffer[16];      //发送数据缓冲区
U8   m_PhaseFixFlash[40];              //相位周期修正标志


int Main(void)
{  
    TargetInit();
    OSInit();
    OSTimeSet(0);   
    OSTaskCreate (MainTask,(void *)0, &MainTaskStk[MainTaskStkLengh - 1], MainTaskPrio);	
    OSStart();
    return 0;
}

void  MainTask(void *pdata)
{

    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif
    OS_ENTER_CRITICAL();
    Timer0Init();//initial timer0 for ucos time tick
    ISRInit();   //initial interrupt prio or enable or disable
    DMAIntSeverInit();
    OS_EXIT_CRITICAL();
    OSStatInit();
    OSTaskCreate(Can2515Task,(void *)0, &Can2515TaskStk[Can2515TaskStkLengh - 1], Can2515TaskPrio);   
    OSTaskCreate(BToothTask,(void *)0, &BToothTaskStk[BToothTaskStkLengh - 1], BToothTaskPrio); 
    OSTaskCreate(SoundTask,(void *)0, &SoundTaskStk[SoundTaskStkLengh - 1], SoundTaskPrio);    	    
    OSTaskCreate(ProcessTask,(void *)0, &ProcessTaskStk[ProcessTaskStkLengh - 1], ProcessTaskPrio); 
   // OSTaskCreate(CheckTask,(void *)0, &CheckTaskStk[CheckTaskStkLengh - 1], CheckTaskPrio);     
    OSTaskCreate(BTSendTask,(void *)0, &BTSendTaskStk[BTSendTaskStkLengh - 1], BTSendTaskPrio);
    while(1)
    {     
       if(m_CanFlash)
          Led1_On();
       Led0_On();
       OSTimeDlyHMSM(0,0,1,0);
       m_Receive=0;
       if(m_CanFlash)
          Led1_Off();
       Led0_Off();
       OSTimeDlyHMSM(0,0,1,0);
       m_Receive=1; 
    }
	
}



void ProcessTask(void *pdata)
{
    int i=0,j=0;
    U8 err;
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif
    Process_Sem=OSSemCreate(0);
    while(1)
    {        
       memset(m_PhaseFixFlash,0,sizeof(m_PhaseFixFlash));  
       ProcessBasicData(&m_FreData[0][0],&m_PhaseCnt[0]);
       ProcessSpeedGain(SpeedData,m_SpeedGain);
       ProcessThrottleGain(ThrottleData,m_ThrottleGain);
       OS_ENTER_CRITICAL(); 
       OSTaskResume(SoundTaskPrio);           
       Uart_Printf("m_IsSuspend = %d  resumesound \n",m_IsSuspend);        
       OSSemPend(Process_Sem,0,&err); 
       OS_EXIT_CRITICAL();         
    }   
}


void BToothTask(void *pdata)
{
    U8 data=255,err;   
    char g_buffer[120]={'0','#'};
    char gVCP[3]={'V','_','1'};
    char g_filestatu=0;                                   //文件状态标识,0 无文件 =1 CCG文件,=2 box文件;
    char g_SumOfCCG=0;                                    //有效数据数量;
    char g_validstatu=0;                                  //是否有有效数据;
    int  g_count=0,g_CCGCnt=0;
    char g_IsV1=0;                                        //是否包含V_1;
    char g_V1Cnt=0;                                       
    char IsDelay=TRUE;                                    //查询延时开关;
      
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif    
    while(1)
    {                                            
         if(IsDelay||m_IsSuspend)                               //是否准备接受数据,
         {    
           OSTimeDlyHMSM(0,0,0,10);                
         }      
         else
         { 
           while(!(rUTRSTAT2 & 0x1));                           //接受Buffer是否满;
         }
         data=RdURXH2();
         if(g_validstatu)
	     {
			if(data==gVCP[g_V1Cnt])                             //是否含有V_1 
			{
			    g_IsV1=1;
			    if(++g_V1Cnt==3)
			    {
			       g_SumOfCCG=V1CCGSIZE;                        //包含V_1 有效数据个数V1CCGSIZE;
			       g_buffer[0]=data;
			       g_buffer[1]='#';
			    }								
			}
			else 
			{
			    g_V1Cnt=0;
			    g_IsV1=0;
			}
			if(!g_IsV1&&data>0)
			{
                g_validstatu=RXAndProCCGData(data,g_buffer,&g_count,&g_CCGCnt);
				if(g_CCGCnt==g_SumOfCCG)
				{
				   g_CCGCnt=0;
				   g_count=0;
				   g_filestatu=STOP;
				   GetValidCanData(g_buffer ,&canconfig,g_SumOfCCG);
				   IsDelay= TRUE;                                  //CCG文件传输完毕，打开查询延时; 
				   OS_ENTER_CRITICAL();
				   Init_MCP2515(canconfig);
				   OSTaskResume(SoundTaskPrio);                    //恢复SoundTask;
				   OS_EXIT_CRITICAL();
				}							
			}          
		 }
         switch(g_filestatu)
         {
             case START:if(data=='*')
                         {                                         //*CCG文件头
                             if(++g_count>5){
                                 g_filestatu= CCG; 
                                 g_count=2;
                                 g_SumOfCCG=NORMAL;                                
                                 m_IsSuspend=TRUE;               //有CCG文件进去,挂起Sound;
                                 IsDelay= FALSE;                 //关闭查询延时;     
                             }                         
                         } break;                     
             case CCG:  if(g_validstatu==INVALID&&data=='\n')      //CCG文件有效数据起始;
                           g_validstatu=VALID;                       
                        break;
             case BOX:
                        break;
             case STOP:g_filestatu= START;
                        break;
         } 
    }   
}

void BTSendTask(void *pdata)
{
    U8 err;
    int itVal[3],i,wch,ioff;
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif
    Change2DMATxMode();
    OS_ENTER_CRITICAL(); 
    InitDMATxMode(SendBuffer,16);
    OS_EXIT_CRITICAL();
    SendBuffer[0]='R';
    SendBuffer[5]='S';
    SendBuffer[10]='T';
    SendBuffer[15]='E'; 
    Tx_Sem=OSSemCreate(0);    
    while(1)
    {    
         itVal[0]=(int)m_RpmVal;
         itVal[1]=(int)m_SpeedVal;
         itVal[2]=(int)m_ThrottleVal;
         for(wch=0;wch<3;wch++)
         {
            ioff=5*wch;
            for(i=4;i>0;i--)
            {
                SendBuffer[i+ioff]=itVal[wch]%10+'0';
                itVal[wch]/=10;
            }
            for(i=1;i<4;i++)
            {
                if(SendBuffer[i+ioff]=='0')
                   SendBuffer[i+ioff]-='0';
                else
                   break;
            }
         }
         rDMASKTRIG0=(0<<2)|(1<<1)|0;             
         OSSemPend(Tx_Sem,0,&err);        
         while(!(rUTRSTAT2 & (1 << 2)));                                      
         OSTimeDlyHMSM(0,0,0,200);           
    }
}



void Can2515Task(void *pdata)
{   
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif    
    pdata=pdata;
    GetCanConfigInfo();
    Init_MCP2515(canconfig);
    Can_2515Setup();
    while(1)
    {
        //CAN_2515_RX();       
        OSTimeDlyHMSM(0,0,0,5);       
    }
}


void SoundTask(void *pdata)
{
    unsigned char buffId=0,id;
    short music=0,Oldamt=0,Newamt;
    int imusic[16]; 
    U32 iphasecnt=0,PhaseOff=0,PhaseInit=0;
    U8 err,iOrder,PhaseFixFlash;
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif
    Wm8731AndIISPortInit();     
    Wm8731RegInit();
    OS_ENTER_CRITICAL(); 
    IIS_Init();
    DMA_Init(&SoundData[0],32);      
    OS_EXIT_CRITICAL();
    Sound_Sem=OSSemCreate(1); 
    m_AmtIndex=(int)m_RpmVal>>6;       //转速位置;
    m_RpmIndex=(int)m_RpmVal/6;
    while(1)
    {                   
         OSSemPend(Sound_Sem,0,&err);
         if(m_IsSuspend)
         {            
            m_IsSuspend=FALSE;
            Uart_Printf("SuspendSound \n");
            OSTaskSuspend(SoundTaskPrio);
         }
         rDMASKTRIG2=(0<<2)|(1<<1)|0;   
         for(iOrder=0;iOrder<40;iOrder++) 
         {
             iphasecnt= m_PhaseCnt[iOrder];     
             PhaseOff = m_FreData[m_RpmIndex][iOrder];   //频率增加?
             Oldamt   = m_AmtCnt[iOrder];                //记录当前幅值
             Newamt   = m_RpmAmt[m_AmtIndex][iOrder];    //
             Newamt   = Newamt *m_SpeedGain[m_SpeedIndex];
             Newamt   = Newamt *m_ThrottleGain[m_ThrottleIndex];
             PhaseFixFlash = m_PhaseFixFlash[iOrder];
             PhaseInit = m_RpmPhase[iOrder]; 
             for(buffId=0;buffId<16;buffId++)
             {
                 imusic[buffId]+=Oldamt*rawDataSin[iphasecnt];            
                 iphasecnt+=PhaseOff;
                 if(Oldamt<Newamt)    //保证幅值变化的连续性
                 {
                    Oldamt+=1;
                 }
                 else if(Oldamt>Newamt)
                 {
                    Oldamt-=1;
                 } 
                 if(iphasecnt>=434000)
                 {
                    iphasecnt-=434000;
                    PhaseFixFlash=TRUE;
                 } 
                 if(PhaseFixFlash&&(iphasecnt>PhaseInit))
                 {
                    PhaseFixFlash=FALSE;
                 }                
             }
             m_AmtCnt[iOrder]  = Oldamt;
             m_PhaseCnt[iOrder]= iphasecnt;
             m_PhaseFixFlash[iOrder]=PhaseFixFlash;
         } 
		 
         if(WhFlag)
         {
             for(buffId=32;buffId<64;buffId+=2)
             {               
               music =imusic[(buffId&0xdf)>>1]>>14;              
               SoundData[buffId]  = music;  
               SoundData[buffId+1]= music; 
               imusic[(buffId&0xdf)>>1]=0;
             }              
             WhFlag=0;
         } 
         else
         {
             for(buffId=0;buffId<32;buffId+=2)
             {
               music =imusic[buffId>>1]>>14;            
               SoundData[buffId]  = music;  
               SoundData[buffId+1]= music;
               imusic[buffId>>1]=0;
             }             
             WhFlag = 32;
         } 
    }
}






void CheckTask(void *pdata)
{
    char data=0;
    char g_buffer[120]={'0','#'},gVCP[3]={'V','_','1'};
    char g_filestatu=0,g_SumOfCCG=0,g_validstatu=0;
    int  g_count=0,g_CCGCnt=0;
    char g_IsV1=0,g_V1Cnt=0,IsDelay=TRUE;
    U8 err;
    #if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
    #endif  
    while(1)
    {     
         if(IsDelay||m_IsSuspend)
         {    
           OSTimeDlyHMSM(0,0,0,5); 
         }      
         else
         { 
           while(!(rUTRSTAT0 & 0x1));
         }
         data=RdURXH0();       
		 if(g_validstatu)
	     {
			if(data==gVCP[g_V1Cnt])                               //是否含有V_1 
			{
			    g_IsV1=1;
			    if(++g_V1Cnt==3)
			    {
			       g_SumOfCCG=V1CCGSIZE;
			       g_buffer[0]=data;
			       g_buffer[1]='#';
			    }								
			}
			else 
			{
			    g_V1Cnt=0;
			    g_IsV1=0;
			}
			if(!g_IsV1&&data>0)
			{
                g_validstatu=RXAndProCCGData(data,g_buffer,&g_count,&g_CCGCnt);
				if(g_CCGCnt==g_SumOfCCG)
				{
				   g_CCGCnt=0;
				   g_count=0;
				   g_filestatu=STOP;
				   GetValidCanData(g_buffer ,&canconfig,g_SumOfCCG);
				   IsDelay= TRUE;
				   Init_MCP2515(canconfig);
				   OSTaskResume(SoundTaskPrio); 
				}							
			}          
		 }
         switch(g_filestatu)
         {
             case START:if(data=='*')
                         {                                         //6CCG文件头
                             if(++g_count>5){                               
                                 g_filestatu= CCG; 
                                 g_count=2;
                                 g_SumOfCCG=NORMAL;
                               //OS_ENTER_CRITICAL();
                                 m_IsSuspend=TRUE;
                                 IsDelay= FALSE;
                               //OS_EXIT_CRITICAL();
                             }                         
                         } break;                     
             case CCG:  if(g_validstatu==INVALID&&data=='\n')      //CCG文件有效数据起始;
                           g_validstatu=VALID;                       
                        break;
             case BOX:
                        break;
             case STOP: g_filestatu= START;
                        break;
         }                
    }
}




































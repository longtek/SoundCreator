#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#define MainTaskStkLengh       1024
#define BToothTaskStkLengh     2048
#define Can2515TaskStkLengh    1024
#define Can2510TaskStkLengh    1024
#define CheckTaskStkLengh      2048*2
#define SoundTaskStkLengh      1024
#define BTSendTaskStkLengh     1024

#define ProcessTaskStkLengh    1024

#define N_MESSAGES 128
#define NormalTaskPrio    5
#define MainTaskPrio      NormalTaskPrio+6
#define BToothTaskPrio    NormalTaskPrio+1
#define BTSendTaskPrio    NormalTaskPrio+5
#define Can2515TaskPrio   NormalTaskPrio+3
#define CheckTaskPrio     NormalTaskPrio+2
#define SoundTaskPrio     NormalTaskPrio
#define ProcessTaskPrio   NormalTaskPrio+4


void MainTask(void *pdata);
void BToothTask(void *pdata);
void Can2515Task(void *pdata);
void SoundTask(void *pdata);
void BTSendTask(void *pdata);
void ProcessTask(void *pdata);
void CheckTask(void *pdata);
#endif
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#define MainTaskStkLengh       1024
#define BToothTaskStkLengh     1024
#define Can2515TaskStkLengh    1024
#define Can2510TaskStkLengh    1024
#define CheckTaskStkLengh      1024
#define SoundTaskStkLengh      1024
#define BTSendTaskStkLengh     1024
#define BTSaveTaskStkLengh     1024
#define ProcessTaskStkLengh    1024

#define N_MESSAGES 128
#define NormalTaskPrio    5
#define MainTaskPrio      NormalTaskPrio+6
#define BToothTaskPrio    NormalTaskPrio+2
#define BTSendTaskPrio    NormalTaskPrio+5
#define Can2515TaskPrio   NormalTaskPrio+1
#define CheckTaskPrio     NormalTaskPrio+3
#define SoundTaskPrio     NormalTaskPrio
#define BTSaveTaskPrio    NormalTaskPrio+7
#define ProcessTaskPrio   NormalTaskPrio+4


void MainTask(void *pdata);
void BToothTask(void *pdata);
void Can2515Task(void *pdata);
void BTSaveTask(void *pdata);
void SoundTask(void *pdata);
void BTSendTask(void *pdata);
void ProcessTask(void *pdata);
void CheckTask(void *pdata);
#endif
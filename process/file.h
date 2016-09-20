#ifndef _FILE_H_
#define _FILE_H_
#include "def.h"
void DataChange(char *str,char isint,int *canval,float *canfval);
void GetValidCanData(char *buffer , CanConfig *candata,char);
char RXAndProCCGData(char data,char *buffer,int *count,int *CCGCnt);
BOOL getboxdata(char data,SinData );
#endif
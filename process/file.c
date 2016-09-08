#include "file.h"
#include <string.h>
#include "def.h"
#include "stdlib.h"


/****************************************************************************
【功能说明】RXAndProCCGData(char data,char *buffer,int *count,int *CCGCnt)
* 入口参数 ：串口数据data  ；  存放数据buffer首地址  ；数据计数 count地址；
*            有效数据计数地址CCGCnt ；
* 描述：每一行首字符开始：ASII 0~9，.,- ;存入buffer中，有效字符串以空格结束添加#标识，
*       或者遇到换行返回停止标识;
* 返回 validstatu：=1 说明该字符为有效字符串中 ，=0 为无效字符;
* 
****************************************************************************/
char RXAndProCCGData(char data,char *buffer,int *count,int *CCGCnt)
{
     static char validstatu = 0;
    // Uart_SendByte(data);
     if((data>='0'&&data<='9')||data=='.'||data=='-') 
     {
        buffer[(*count)++]=data;
        validstatu = 2;
     }
     else if(data==' '||data=='\t'||data=='\r')
     {
        if(validstatu == 2)
        {
           buffer[(*count)++]='#';
           ++*CCGCnt;
        }
        validstatu = INVALID; 
     }
     else 
        validstatu = INVALID;
     return validstatu; 
}

/****************************************************************************
【功能说明】字符串转换数值 DataChange(char *str,char isint,int *canval,float *canfval)
* 入口参数 ：字符串首地址str  ,整型标识 isint  ，整型取数地址*canval，浮点型*canfval
* 描述：
* 
* 
****************************************************************************/
void DataChange(char *str,char isint,int *canval,float *canfval)
{
     int i=0,val=0;
     float fval=0.0;
     float fcoef=10;
     int nega=1;
     if(isint)   //整型数据;
     {   //是否为负数;
         if(str[i]=='-')
         {
            nega=-1;
            i=1;
         }
         for(;i<10;i++)
         {
            if(str[i]=='\0')
               break;           
             val=val*10+(str[i]-'0')*nega;
             str[i]='\0';
         }
         *canval= val;
        // Uart_Printf("%d\n",val);
      }
      else
      {  //float 数据;
         for(i=0;i<10;i++)
         {
            if(str[i]=='\0')
              break;
            if(str[i]=='.')
            {
              fcoef=0.1;
              str[i]='\0'; 
              continue;
            } 
            if(fcoef>1)
            {
               fval = fval*10+(str[i]-'0');
            }
            else
            {
               fval = fval+(str[i]-'0')*fcoef;  
               fcoef=0.1*fcoef;             
            }
            str[i]='\0';            
         } 
         *canfval = fval;
       //  Uart_Printf("%7f\n",(float)fval);        
      }
}

/****************************************************************************
【功能说明】将buffer中存放的字符串区分，存放入对应的CanConfig 结构体中;
* 入口参数 ：数据buffer ，CanConfig 结构体,有效数据个数g_SumOfCCG
* 
****************************************************************************/

void GetValidCanData(char *buffer ,CanConfig *candata,char g_SumOfCCG)
{
     char VIndex=7;
     int i=0,ind=0,canval,dataIndex=0;
     char IsInt=TRUE,IsV1=TRUE;
     char str[10];
     float canfval;
     int  *can =(int *)candata;
     memset(str,'\0',10);
     if(buffer[0]=='0')   //0 表示无V_1  1 = 有V_1  
        IsV1=FALSE;
     for(i=2;i<120;i++)
     {
        if(buffer[i]=='#')  //数据间隔标志;
        {
           ind=0;          
           DataChange(str,IsInt,&canval,&canfval);
           if(!IsInt)
           {
              *(float*)can++ = canfval;
              if(!IsV1)*(int*)can++ = 0;
              IsInt  = TRUE ; 
           }
           else
           { 
              *can++ = canval;               
           }
           if(++dataIndex==g_SumOfCCG)break;
           continue;
        }
        if(dataIndex==6||(dataIndex==(13+IsV1))||(dataIndex==(20+(IsV1<<1))))
          IsInt=FALSE;
        str[ind++]= buffer[i];
     }
     buffer[0]='0';
     Uart_Printf("%d\n",candata->CAN_bandrate);
     Uart_Printf("%d\n",candata->RPM.ID);
     Uart_Printf("%d\n",candata->RPM.BYTENUM);
     Uart_Printf("%d\n",candata->RPM.BITPOS);
     Uart_Printf("%d\n",candata->RPM.DATALEN);
     Uart_Printf("%d\n",candata->RPM.ENDIAN);
     Uart_Printf("%f\n",candata->RPM.DATACOEF);
     Uart_Printf("%d\n",candata->RPM.OFFSET);
     Uart_Printf("%d\n",candata->RPM.DEFAULT);
     Uart_Printf("%d\n",candata->SPEED.ID);
     Uart_Printf("%d\n",candata->SPEED.BYTENUM);
     Uart_Printf("%d\n",candata->SPEED.BITPOS);
     Uart_Printf("%d\n",candata->SPEED.DATALEN);
     Uart_Printf("%d\n",candata->SPEED.ENDIAN);
     Uart_Printf("%f\n",candata->SPEED.DATACOEF);
     Uart_Printf("%d\n",candata->SPEED.OFFSET);
     Uart_Printf("%d\n",candata->SPEED.DEFAULT);
     Uart_Printf("%d\n",candata->THROTTLE.ID);
     Uart_Printf("%d\n",candata->THROTTLE.BYTENUM);
     Uart_Printf("%d\n",candata->THROTTLE.BITPOS);
     Uart_Printf("%d\n",candata->THROTTLE.DATALEN);
     Uart_Printf("%d\n",candata->THROTTLE.ENDIAN);
     Uart_Printf("%f\n",candata->THROTTLE.DATACOEF);
     Uart_Printf("%d\n",candata->THROTTLE.OFFSET);
     Uart_Printf("%d\n",candata->THROTTLE.DEFAULT);
}

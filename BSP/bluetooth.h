#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#define Bluetooth 	0x02
#define RxCmdState  0
#define IRQRxState  1

void Bluetooth_Putbyte(unsigned char chd);
char Bluetooth_Getdata(void);
void Bluetooth_Serial_Init(int pclk,int baud);
void Bluetooth_SendByte(char ch);
void Bluetooth_Send(char *str);
void BToothCS(char cs);
void Bluetooth_Printf(char *fmt,...);
void RxAndTxIRQ(void);
void RxIRQStart(void);
void Change2DMATxMode(void);
void Change2IRQTxMode(void);
void Change2DMARxMode(U8 index);
void Change2IRQRxMode(U8 index);
#endif
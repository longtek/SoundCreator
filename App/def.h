#ifndef __DEF_H__H
#define __DEF_H__H

#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define	S8  char

#define TRUE 	1   
#define FALSE 	0
#define OK		1
#define FAIL	0
typedef enum{
	BandRate_10kbps,
	BandRate_125kbps,
	BandRate_250kbps,
	BandRate_500kbps,
	BandRate_1Mbps
}CanBandRate;
typedef struct{
    int ID;
    int BYTENUM;
    int BITPOS;
    int DATALEN;
    float DATACOEF;
}CANELE;
typedef struct{
       int      CAN_bandrate;
       int      CAN_IsExt;
       int      CAN_endian;              
       CANELE   RPM; 
       CANELE   SPEED; 
       CANELE   THROTTLE;                 
}CanConfig;
#define ESC_KEY	('q')	// 0x1b
#endif /*__DEF_H__*/


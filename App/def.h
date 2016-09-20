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
#define CCG     1
#define BOX     2
#define START   0
#define STOP    3
#define V1CCGSIZE     25
#define NORMAL  22
#define BOXVALID   1
#define VALID   1
#define INVALID 0
typedef char BOOL;
typedef enum{
	BandRate_10kbps,
	BandRate_125kbps=16,
	BandRate_250kbps=8,
	BandRate_500kbps=4,
	BandRate_1Mbps
}CanBandRate;
typedef struct{
    int ID;
    int BYTENUM;
    int BITPOS;
    int DATALEN;
    int ENDIAN;  
    float DATACOEF;
    int OFFSET;
    int DEFAULT;
}CANELE;
typedef struct{
       int      CAN_bandrate;                  
       CANELE   RPM; 
       CANELE   SPEED; 
       CANELE   THROTTLE;                 
}CanConfig;
typedef struct{
       char *pDATA[6];
       U32  DATASIZE[6];
}SinData;
#define ESC_KEY	('q')	// 0x1b
#endif /*__DEF_H__*/


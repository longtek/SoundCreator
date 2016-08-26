#ifndef _WM8731_H_
#define _WM8731_H_

#define  TRUE   1
#define  FALSE  0

#define L3CLOCK 1<<4
#define L3DATA  1<<3
#define L3CSB  1<<2
#define GPX_X_WRITE(x) (1<<x)
#define GPX_X_CLEAR(x) (~(1<<x))

#define SDA 15
#define SCL 14

// IISCON Define
#define FTXURSTATUS(x)  (x<<17)
#define FTXURINTEN(x)   (x<<16)
#define TXDMAPAUSE(x)   (x<<6)
#define RXDMAPAUSE(x)   (x<<5)
#define TXCHPAUSE(x)    (x<<4)
#define RXCHPAUSE(x)    (x<<3)
#define TXDMACTIVE(x)   (x<<2)
#define RXDMACTIVE(x)   (x<<1)
#define I2SACTIVE(x)    (x<<0)
 

//IISMODE
#define ChBitLength(x) (x<<13)
#define DATA_16BIT 0
#define CDCLKCON(x) (x<<12)
#define IISModeSelet(x) (x<<10)
#define MasterPclk 0
#define MasterEpll 1
#define SlavePclk  2
#define SlaveEpll  3
#define TXRModeSelet(x) x<<8
#define TxOnly  0
#define RxOnly  1
#define TxAndRx 2
#define SDataFat(x) (x<<5)
#define IIS 0
#define MSB 1
#define LSB 2
#define CodeClkSelet(x) (x<<3)
#define MSLK_256FS 0
#define MSLK_384FS 2 
#define BitClkSelet(x) (x<<1)
#define BFS_32FS 0

#define TFLUSH(x) (x<<15)
#define RFLUSH(x)  (x<<7)

#define PSRAEN(x) (x<<15)
#define PSVALA(x) (x<<8)


void Wm8731AndIISPortInit();
static void Wm8731WriteReg(unsigned char reg ,unsigned int data);
void Wm8731RegInit(void);
void IIS_Init(void);

#endif
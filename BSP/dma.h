#ifndef _DMA_H_
#define _DMA_H_
void DMA_Init(short *pData,unsigned int nSoundLend);
void DMA_IRQ(void);
void InitDMARxMode(char *pData,unsigned int nDataLend,unsigned char index); 
void InitDMATxMode(char *pData,unsigned int nDataLend);
void DMAIntSeverInit(void);
#endif
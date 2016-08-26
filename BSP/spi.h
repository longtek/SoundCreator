#ifndef _SPI_H_
#define _SPI_H_
void SpiRegInit(void);
void SpiPortInit(void);
void SpiSendData(unsigned char  data);
unsigned char SpiReadData(void);
#endif
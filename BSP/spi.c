#include "spi.h"
#include "2416addr.h"
#define SPI_CS             (rGPLCON = rGPLCON&(~(3<<26))|(2<<26))
#define SPI_CS_DISPULL     (rGPLUDP = rGPLUDP&(~(3<<26)))
#define SPI_MOSI           (rGPECON = rGPECON&(~(3<<24))|(2<<24))
#define SPI_MOSI_DISPULL   (rGPEUDP = rGPEUDP&(~(3<<24)))
#define SPI_MISO           (rGPECON = rGPECON&(~(3<<22))|(2<<22))
#define SPI_MISO_DISPULL   (rGPEUDP = rGPEUDP&(~(3<<22)))
#define SPI_SCK            (rGPECON = rGPECON&(~(3<<26))|(2<<26))
#define SPI_SCK_DISPULLUP  (rGPEUDP = rGPEUDP&(~(3<<26)))
void SpiRegInit(void)
{
    
}
void SpiPortInit(void)
{
     SPI_CS; 
     SPI_CS_DISPULL;
     SPI_MISO;
     SPI_MISO_DISPULL;
     SPI_MOSI;
     SPI_MOSI_DISPULL;
     SPI_SCK;
     SPI_SCK_DISPULLUP;
}
void SpiSendData(U8 data)
{

}
unsigned char SpiReadData(void)
{

}
 
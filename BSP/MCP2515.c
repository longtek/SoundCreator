//======================================================================
//  ÎÄ¼þÃû£º  MCP2515.c
//  ¹¦ÄÜÃèÊö£º	Í¨¹ýSPI½Ó¿Ú£¬¿ØÖÆMCP2515
//	Î¬»¤¼ÇÂ¼£º	2009-08-14	v1.0    
//				2009-10-28  v1.1    
//======================================================================
#include "def.h"
#include "option.h"
#include "2416addr.h"
#include "2416lib.h"
#include "2416slib.h" 
#include "MCP2515.h"
extern CanConfig canconfig;
extern float m_RpmVal,m_SpeedVal,m_ThrottleVal;
extern U8  m_ToChange;
extern U8  m_CanFlash;
extern U8  m_AmtIndex;
extern U32 m_RpmIndex;
extern U32 m_SpeedIndex;
extern U32 m_ThrottleIndex;

/****************************************************************************
MCP2515_CS		GPB7		output		( nSS0 )
MCP2515_SI		GPE12		output		( SPIMOSI0 )
MCP2515_SO		GPE11		input		( SPIMISO0 )
MCP2515_SCK		GPE13		output		( SPICLK0 )
MCP2515_INT		GPG0		input		( EINT8 )
****************************************************************************/
#define MCP2515_DEBUG    1
#define DELAY_TIME		800

#define MCP2515_CS_OUT       ( rGPLCON = rGPLCON & (~(3<<26)) | (1<<26) )        //GPL13
#define MCP2515_CS_PULLUP    ( rGPLUDP = rGPLUDP &(~(3<<26))|(2<<26)) 
#define MCP2515_CS_DISPULLUP ( rGPLUDP = rGPLUDP &(~(3<<26)))  
#define MCP2515_CS_H         ( rGPLDAT |= (1<<13))    
#define MCP2515_CS_L         ( rGPLDAT = rGPLDAT & (~(1<<13))) 

#define MCP2515_SI_OUT		( rGPECON = rGPECON & (~(3<<22)) | (1<<22) )		//GPE11
#define MCP2515_SI_PULLUP   ( rGPEUDP = rGPEUDP & (~(3<<22)))
#define MCP2515_SI_H		( rGPEDAT = rGPEDAT | (1<<11))
#define MCP2515_SI_L		( rGPEDAT = rGPEDAT & (~(1<<11)) )

#define MCP2515_SCK_OUT		( rGPECON = rGPECON & (~(3<<26)) | (1<<26) )		//GPE13
#define MCP2515_SCK_PULLUP  ( rGPEUDP = rGPEUDP & (~(1<<26)) )
#define MCP2515_SCK_H		( rGPEDAT = rGPEDAT | (1<<13) )
#define MCP2515_SCK_L		( rGPEDAT = rGPEDAT & (~(1<<13)) )

#define MCP2515_SO_IN		 ( rGPECON = rGPECON & (~(3<<24)) | (0<<24))		//GPE12
#define MCP2515_SO_GET		 ( rGPEDAT &= (1<<12))	
#define MCP2515_SO_DISPULLUP ( rGPEUDP = rGPEUDP & (~(3<<24)) )
#define MCP2515_SO_PULLUP	 ( rGPEUDP = rGPEUDP | (1<<24) )

	
/********************** MCP2515 Instruction *********************************/
#define MCP2515INSTR_RESET		0xc0		//¸´Î»ÎªÈ±Ê¡×´Ì¬£¬²¢Éè¶¨ÎªÅäÖÃÄ£Ê½
#define MCP2515INSTR_READ		0x03		//´Ó¼Ä´æÆ÷ÖÐ¶Á³öÊý¾Ý
#define MCP2515INSTR_WRITE		0x02		//Ïò¼Ä´æÆ÷Ð´ÈëÊý¾Ý
#define MCP2515INSTR_RTS		0x80		//Æô¶¯Ò»¸ö»ò¶à¸ö·¢ËÍ»º³åÆ÷µÄ±¨ÎÄ·¢ËÍ
#define MCP2515INSTR_RDSTAT		0xa0		//¶ÁÈ¡×´Ì¬
#define MCP2515INSTR_BITMDFY	0x05		//Î»ÐÞ¸Ä
//***************************************************************************


/****************************************************************************   
¡¾¹¦ÄÜËµÃ÷¡¿SPI½Ó¿ÚIOÆ¬Ñ¡³õÊ¼»¯   
****************************************************************************/   
static void MCP2515_IO_CS_Init( void )    
{
   U16 k;   
   MCP2515_CS_OUT ;  
  // MCP2515_CS_PULLUP; 
   MCP2515_SI_OUT ;   
   MCP2515_SCK_OUT ;   
   MCP2515_SO_IN ;   
   //MCP2515_SO_PULLUP ;      //ÔÊÐíÉÏÀ­    
   //MCP2515_SO_DISPULLUP ;     //½ûÖ¹ÉÏÀ­    
   MCP2515_SI_L ;       //SI put 0     
   MCP2515_SCK_L ;      //SCK put 0    
   for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
   MCP2515_CS_H ;           // unselect the MCP2515    
   for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
}   
   
/****************************************************************************  
¡¾¹¦ÄÜËµÃ÷¡¿SPI½Ó¿Ú¶ÁÐ´¿ªÊ¼£¬Æ¬Ñ¡ÓÐÐ§  
****************************************************************************/   
static void MCP2515_RW_Start( void )    
{
   U16 k;   
   MCP2515_SI_L ;       //SI put 0    
   MCP2515_SCK_L ;      //SCK put 0    
   for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
   MCP2515_CS_L ;           // Select the MCP2515    
   for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
}   
   
/****************************************************************************  
¡¾¹¦ÄÜËµÃ÷¡¿SPI½Ó¿ÚÐ´ÈëÊý¾Ý  
****************************************************************************/   
static void Spi_Write( U8 Data )    
{   
    U8 m ;
    U16 k;   
   
    for( m = 0; m < 8; m++ )   
    {   
        if( (Data&0x80)==0x80 )
        {  
            MCP2515_SI_H;       //SI put 1 
        }   
        else
        {   
            MCP2515_SI_L;       //SI put 0  
        }  
   
		for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
        MCP2515_SCK_H ;     //SCK put 1    
        Data = Data<<1 ;   
        MCP2515_SCK_L ;     //SCK put 0    
		for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
    }   
}   
   
/****************************************************************************  
¡¾¹¦ÄÜËµÃ÷¡¿SPI½Ó¿Ú¶Á³öÊý¾Ý  
****************************************************************************/   
static U8 Spi_Read( void)   
{   
    U8 m ;   
    U8 data = 0 ;
    U16 k;   
   
    for( m = 0; m < 8; m++ )   
    {   
        MCP2515_SCK_H ;     //SCK put 1    
		for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
        data = data<<1;  
        if( MCP2515_SO_GET != 0 )   
            data |= 0x01 ;   
        else   
            data &= 0xfe;   
   
		for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
        MCP2515_SCK_L ;     //SCK put 0    
		for (k = 0; k <= DELAY_TIME; k++);  //ÑÓÊ±ÖÁÉÙ300ns    
    }   
   
    return (data);   
}   

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿ Send Command to MCP2515 via SPI 
****************************************************************************/
static void SendCMDMCP2515( U8 CMD )
{
   MCP2515_RW_Start() ;		//Initial IO port and CS is select
   Spi_Write( CMD );
   MCP2515_CS_H ;			// Deselect the MCP2515
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿Èí¼þ¸´Î»MCP2515
****************************************************************************/
static void MCP2515_Reset()
{
	MCP2515_RW_Start() ;
	Spi_Write( MCP2515INSTR_RESET );
	MCP2515_CS_H ;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿ÏòMCP2515Ö¸¶¨µØÖ·Ð´ÈëÒ»¸ö×Ö½Ú
****************************************************************************/
static void MCP2515_Write( U8 address, U8 value)
{
	MCP2515_RW_Start() ;

	Spi_Write(MCP2515INSTR_WRITE);
	Spi_Write( address );
	Spi_Write( value );

	MCP2515_CS_H ;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿ÐÞ¸ÄÖ¸¶¨µØÖ·¼Ä´æÆ÷µÄÄ³Ð©Î»
****************************************************************************/
static void MCP2515_WriteBits( U8 address, U8 data, U8 mask )
{
	MCP2515_RW_Start() ;

	Spi_Write( MCP2515INSTR_BITMDFY );
	Spi_Write( address);
	Spi_Write( mask);
	Spi_Write( data);

	MCP2515_CS_H ;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿              Read often used status
//Status 	 7    	6    	5    	4    	3    	2  	1	0
//		|	|	|	|	|	|	|	|									
//		|	|	|	|	|	|	|	|___CANINTF.RX0IF
//		|	|	|	|	|	|	|_______CANINTF.RX1IF
//		|	|	|	|	|	|___________TXB0CTRL.TXREQ
//		|	|	|	|	|_______________CANINTF.TX0IF
//		|	|	|	|___________________TXB1CTRL.TXREQ
//		|	|	|_______________________CANINTF.TX1IF
//		|	|___________________________TXB2CTRL.TXREQ
//		|_______________________________CANINTF.TX2IF
****************************************************************************/
static unsigned char MCP2515_ReadStatus()
{
	unsigned char result;

	MCP2515_RW_Start() ;

	Spi_Write(MCP2515INSTR_RDSTAT);
	result = Spi_Read() ;
	Spi_Write( 0 ) ;		//Êý¾ÝÖØ¸´Êä³ö
	MCP2515_CS_H ;
	 // if( MCP2515_DEBUG )		Uart_Printf( "StatusREG = 0x%x\n", result ) ;
	return result;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿´ÓMCP2515Ö¸¶¨µØÖ·ÖÐ¶Á³öÒ»¸ö×Ö½Ú
****************************************************************************/
static unsigned char MCP2515_Read( U8 address )
{
	unsigned char result;
	MCP2515_RW_Start() ;

	Spi_Write(MCP2515INSTR_READ) ;		//0x03
	Spi_Write( address ) ;
	result = Spi_Read() ;

	MCP2515_CS_H ;
	return result ;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿ÐòÁÐ¶ÁÈ¡MCP2515Êý¾Ý	
****************************************************************************/
static void MCP2515_SRead( U8 address, unsigned char* pdata, U8 nlength )
{
	int i;

	MCP2515_RW_Start() ;
	Spi_Write(MCP2515INSTR_READ);
	Spi_Write( address );

	for (i=0; i<nlength; i++)
	{
		*pdata=Spi_Read();
		   //if( MCP2515_DEBUG )    Uart_Printf( "  0x%x\n", (unsigned char)*pdata ) ;
		pdata++;
	}
	MCP2515_CS_H ;
}


/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿ÐòÁÐÐ´ÈëMCP2515Êý¾Ý	
****************************************************************************/
static void MCP2515_Swrite( U8 address, unsigned char* pdata, U8 nlength)
{
	int i;
	MCP2515_RW_Start() ;

	Spi_Write(MCP2515INSTR_WRITE);
	Spi_Write((unsigned char)address);

	for (i=0; i < nlength; i++) 
	{
		Spi_Write( (unsigned char)*pdata );
		//if( MCP2515_DEBUG )    Uart_Printf( "0x%x\n", (unsigned char)*pdata ) ;
		pdata++;
	}
	MCP2515_CS_H ;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿
****************************************************************************/
static void MCP2515_SetBandRate(CanBandRate bandrate, int IsBackNormal)
{
	U8 value=0;
	U8 ReadBackCNT = 0;

	// Bit rate calculations.
	//
	//Input clock fre=16MHz
	// In this case, we'll use a speed of 125 kbit/s, 250 kbit/s, 500 kbit/s.
	// If we set the length of the propagation segment to 7 bit time quanta,
	// and we set both the phase segments to 4 quanta each,
	// one bit will be 1+7+4+4 = 16 quanta in length.
	//
	// setting the prescaler (BRP) to 0 => 500 kbit/s.
	// setting the prescaler (BRP) to 1 => 250 kbit/s.
	// setting the prescaler (BRP) to 3 => 125 kbit/s.
	//
	// If we set the length of the propagation segment to 3 bit time quanta,
	// and we set both the phase segments to 1 quanta each,
	// one bit will be 1+3+2+2 = 8 quanta in length.
	// setting the prescaler (BRP) to 0 => 1 Mbit/s.

	// Go into configuration mode
	MCP2515_Write(MCP2515REG_CANCTRL, MODE_CONFIG);

	if( MCP2515_DEBUG )  Uart_Printf( "MCP2515REG_CANCTRL =  0x%x\n", MCP2515_Read(MCP2515REG_CANCTRL) );

	while( ReadBackCNT<8 )
	{
		value = ( MCP2515_Read( MCP2515REG_CANSTAT ) & 0xe0 );
		if(value == MODE_CONFIG ){
			Uart_Printf( "ReadBackCNT = 0x%x\n", ReadBackCNT );
			break;
		}
		ReadBackCNT++ ;
		Uart_Printf( "ReadBackCNT = 0x%x,value=0x%x\n", ReadBackCNT,value );
	}
	
	if( ReadBackCNT == 8 ) 			//Set mcp2515's mode failed,redo it again
	{
		Uart_Printf( "Set config mode is failed! CANCTRL = 0x%x\n", value );
		MCP2515_Reset();
		MCP2515_Write(MCP2515REG_CANCTRL, MODE_CONFIG);		//redo to set mcp2515 mode
		Delay( 150 );
		value = ( MCP2515_Read(MCP2515REG_CANCTRL) & 0xe0 );	//read back mode from CANSTAT Register
		Uart_Printf( "Set is 0x%x , Read is 0x%x\n", MODE_CONFIG, value ) ;
	}
	Uart_Printf( "bandrate  is %d \n", bandrate) ;
	switch(bandrate){
	case BandRate_10kbps:
		MCP2515_Write(CNF1, 0x31);	//10k	16TQ
		MCP2515_Write(CNF2, 0xb0);  //PS1=7 TQ  PSeg=1 TQ
		MCP2515_Write(CNF3, 0x06);  //PS2=7 TQ SYNC=1 TQ	
		break;
	case BandRate_125kbps:
		MCP2515_Write(CNF1, SJW1|BRP2);	//Synchronization Jump Width Length =1 TQ
		MCP2515_Write(CNF2, BTLMODE_CNF3|(SEG4<<3)|SEG7); // Phase Seg 1 = 4, Prop Seg = 7
		MCP2515_Write(CNF3, SEG4);// Phase Seg 2 = 4
		break;
	case BandRate_250kbps:
		MCP2515_Write(CNF1, SJW1|BRP1);	//Synchronization Jump Width Length =1 TQ
		MCP2515_Write(CNF2, BTLMODE_CNF3|(SEG4<<3)|SEG7); // Phase Seg 1 = 4, Prop Seg = 7
		MCP2515_Write(CNF3, SEG4);// Phase Seg 2 = 4
		break;
	case BandRate_500kbps:
	    Uart_Printf( "BandRate_500kbps \n") ;
		MCP2515_Write(CNF1, SJW1|BRP1);	//Synchronization Jump Width Length =1 TQ
		MCP2515_Write(CNF2, BTLMODE_CNF3|(SEG3<<3)|SEG2); // Phase Seg 1 = 3, Prop Seg = 2
		MCP2515_Write(CNF3, SEG2);// Phase Seg 2 = 2
		break;
	default:
		MCP2515_Write(CNF1, SJW1|BRP1);	//Synchronization Jump Width Length =1 TQ
		MCP2515_Write(CNF2, BTLMODE_CNF3|(SEG3<<3)|SEG2); // Phase Seg 1 = 2, Prop Seg = 3
		MCP2515_Write(CNF3, SEG2);// Phase Seg 2 = 2
		break;
	}

	if( IsBackNormal == TRUE  )
	{
		//Enable clock output
		MCP2515_Write(CLKCTRL, MODE_NORMAL | CLKEN | CLK8);
	}

}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿¶ÁÈ¡MCP2515 CAN×ÜÏßID
²ÎÊý: addressÎªMCP2515¼Ä´æÆ÷µØÖ·
	can_idÎª·µ»ØµÄIDÖµ
·µ»ØÖµ
TRUE£¬±íÊ¾ÊÇÀ©Õ¹ID(29Î»)
FALSE£¬±íÊ¾·ÇÀ©Õ¹ID(11Î»)
****************************************************************************/
static int MCP2515_Read_Can_ID( U8 address, U32* can_id)
{
	U32 tbufdata;
	unsigned char* p=(unsigned char*)&tbufdata;

	MCP2515_SRead(address, p, 4);
	*can_id = (tbufdata<<3)|((tbufdata>>13)&0x7);
	*can_id &= 0x7ff;

	if ( (p[MCP2515LREG_SIDL] & TXB_EXIDE_M) ==  TXB_EXIDE_M ) {
		*can_id = (*can_id<<2) | (p[MCP2515LREG_SIDL] & 0x03);
		*can_id <<= 16;
		*can_id |= tbufdata>>16;
		return TRUE;
	}
	return FALSE;
}

/***********************************************************\
*	¶ÁÈ¡MCP2515 ½ÓÊÕµÄÊý¾Ý							*
*	²ÎÊý: nbufferÎªµÚ¼¸¸ö»º³åÇø¿ÉÒÔÎª3»òÕß4	*
*			can_idÎª·µ»ØµÄIDÖµ							*
*			rxRTR±íÊ¾ÊÇ·ñÊÇRXRTR						*
*			data±íÊ¾¶ÁÈ¡µÄÊý¾Ý						*
*			dlc±íÊ¾data length code							*
*	·µ»ØÖµ												*
*		TRUE£¬±íÊ¾ÊÇÀ©Õ¹×ÜÏß						*
*		FALSE£¬±íÊ¾·ÇÀ©Õ¹×ÜÏß						*
\***********************************************************/
static int MCP2515_Read_Can(U8 nbuffer, int* rxRTR, U32* can_id, U8* data , U8* dlc)
{

	U8 mcp_addr = (nbuffer<<4) + 0x31, ctrl;
	int IsExt;

	IsExt=MCP2515_Read_Can_ID( mcp_addr, can_id);
	ctrl=MCP2515_Read(mcp_addr-1);
	*dlc=MCP2515_Read( mcp_addr+4);
	if ((ctrl & 0x08)) {
		*rxRTR = TRUE;
	}
	else{
		*rxRTR = FALSE;
	}
	*dlc &= DLC_MASK;	
	//MCP2515_SRead(mcp_addr+5, data, *dlc);
	MCP2515_SRead(mcp_addr+5, data, 8); 
	return IsExt;
}


/***********************************************************\
*	Ð´ÈëMCP2515 ·¢ËÍµÄÊý¾Ý							*
*	²ÎÊý: nbufferÎªµÚ¼¸¸ö»º³åÇø¿ÉÒÔÎª0¡¢1¡¢2	*
*			ext±íÊ¾ÊÇ·ñÊÇÀ©Õ¹×ÜÏß					*
*			can_idÎª·µ»ØµÄIDÖµ							*
*			rxRTR±íÊ¾ÊÇ·ñÊÇRXRTR						*
*			data±íÊ¾¶ÁÈ¡µÄÊý¾Ý						*
*			dlc±íÊ¾data length code							*
*		FALSE£¬±íÊ¾·ÇÀ©Õ¹×ÜÏß						*
\***********************************************************/
static void MCP2515_Write_Can( U8 nbuffer, int ext, U32 can_id, int rxRTR, U8* data,U8 dlc )
{
	U8 mcp_addr = (nbuffer<<4) + 0x31;
	MCP2515_Swrite(mcp_addr+5, data, dlc );  // write data bytes
	MCP2515_Write_Can_ID( mcp_addr, can_id,ext);  // write CAN id
	if (rxRTR)
		dlc |= RTR_MASK;  // if RTR set bit in byte
	MCP2515_Write((mcp_addr+4), dlc);            // write the RTR and DLC
}

/*******************************************\
*	ÉèÖÃMCP2515 CAN×ÜÏßID				*
*	²ÎÊý: addressÎªMCP2515¼Ä´æÆ÷µØÖ·*
*			can_idÎªÉèÖÃµÄIDÖµ			*
*			IsExt±íÊ¾ÊÇ·ñÎªÀ©Õ¹ID	*
\*******************************************/
static void MCP2515_Write_Can_ID(U8 address, U32 can_id, int IsExt)
{
	U32 tbufdata;

	if (IsExt) {
		can_id&=0x1fffffff;	//29Î»
		tbufdata=can_id &0xffff;
		tbufdata<<=16;
		tbufdata|=(can_id>>(18-5)&(~0x1f));
		tbufdata |= TXB_EXIDE_M;
	}
	else{
		can_id&=0x7ff;	//11Î»
		tbufdata= (can_id>>3)|((can_id&0x7)<<13);
	}
	MCP2515_Swrite(address, (unsigned char*)&tbufdata, 4);
}

/***********************************************************************************\
								·¢ËÍÊý¾Ý
	²ÎÊý:
		data£¬·¢ËÍÊý¾Ý

	Note: Ê¹ÓÃÈý¸ö»º³åÇøÑ­»··¢ËÍ£¬Ã»ÓÐ×ö»º³åÇøÓÐÐ§¼ì²â
\***********************************************************************************/
static void Can_Write(U32 id, U8 *pdata, unsigned char dlc, int IsExt, int rxRTR)
{
	unsigned char err ;
	static int ntxbuffer=0;
	MCP2515_Write_Can(ntxbuffer, IsExt, id, rxRTR, pdata, dlc);

	switch(ntxbuffer){
	case 0:
		MCP2515_WriteBits(TXB0CTRL, (TXB_TXREQ_M|TXB_TXP10_M), 0xff) ;
		//do { err = MCP2515_Read(TXB0CTRL) ; }
		//while( (err &0x08)==0x08 )  ;
		if( (err &0x70) != 0 )  Uart_Printf( "  Can Send Err = 0x%x\n", err  );
		ntxbuffer=1;
		break;
	case 1:
		MCP2515_WriteBits(TXB1CTRL, (TXB_TXREQ_M|TXB_TXP10_M), 0xff) ;
		//do { err = MCP2515_Read(TXB1CTRL) ; }
		//while( (err &0x08)==0x08 )  ;
		if( (err &0x70) != 0 )  Uart_Printf( "  Can Send Err = 0x%x\n", err  );
		ntxbuffer=2;
		break;
	case 2:
		MCP2515_WriteBits(TXB2CTRL, (TXB_TXREQ_M|TXB_TXP10_M), 0xff) ;
		//do { err = MCP2515_Read(TXB2CTRL) ; }
		//while( (err &0x08)==0x08 )  ;
		if( (err &0x70) != 0 )  Uart_Printf( "  Can Send Err = 0x%x\n", err  );
		ntxbuffer=0;
		break;
	}

}


/***********************************************************************************\
								²éÑ¯ÊÇ·ñÊÕµ½Êý¾Ý
	·µ»ØÖµ:Èç¹ûÃ»ÓÐÊý¾Ý£¬Ôò·µ»Ø-1£¬
			·ñÔò£¬·µ»ØÊÕµ½Êý¾ÝµÄ»º³åÇøºÅ
	Note: Èç¹ûÁ½¸ö»º³åÇø¶¼ÊÕµ½Êý¾Ý£¬Ôò·µ»ØµÚÒ»¸ö»º³åÇø
\***********************************************************************************/
static int Can2515_Poll()
{
	if( MCP2515_ReadStatus()&RX0INT )
		return 0;
	
	if( MCP2515_ReadStatus()&RX1INT )
		return 1;

	return -1;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿
****************************************************************************/
static int Can_Read(int n, U32* id, U8 *pdata,  U8*dlc, int* rxRTR, int *isExt)
{
	U8 byte;
	byte = MCP2515_Read(CANINTF);

	if(n==0)
	{
		if(byte & RX0INT)
		{
			*isExt=MCP2515_Read_Can(n+3, rxRTR, id, pdata, dlc);
			MCP2515_WriteBits(CANINTF, (U8)(~(RX0INT)), RX0INT); // Clear interrupt
			return TRUE ;
		}
		Uart_Printf( "Error! 0 bytes is Read!!! CANINTF=0x%x\n", byte ) ;
		return FALSE;
	}
	else if(n ==1 )
	{
		if(byte & RX1INT)
		{
			*isExt=MCP2515_Read_Can(n+3, rxRTR, id, pdata, dlc);
			MCP2515_WriteBits(CANINTF, (U8)(~(RX1INT)), RX1INT); // Clear interrupt
			return TRUE ;
		}
		Uart_Printf( "0 bytes is Read!!! CANINTF=0x%x\n", byte ) ;
		return FALSE;
	}

	Uart_Printf( "Error! Receive channel=0x%x\n", n ) ;
	return FALSE;
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿
****************************************************************************/
// Setup the CAN buffers used by the application.
// We currently use only one for reception and one for transmission.
// It is possible to use several to get a simple form of queue.
//
// We setup the unit to receive all CAN messages.
// As we only have at most 4 different messages to receive, we could use the
// filters to select them for us.
//
// Init_MCP2515() should already have been called.
void Can_2515Setup(void)
{
    // As no filters are active, all messages will be stored in RXB0 only if
    // no roll-over is active. We want to recieve all CAN messages (standard and extended)
    // (RXM<1:0> = 11).
    // SPI_mcp_write_bits(RXB0CTRL, RXB_RX_ANY, 0xFF);
    // SPI_mcp_write_bits(RXB1CTRL, RXB_RX_ANY, 0xFF);
    // But there is a bug in the chip, so we have to activate roll-over.
	MCP2515_WriteBits(RXB0CTRL, RXB_RX_STD|RXB_BUKT, 0xFF);		//¹Ø±ÕÆÁ±ÎÂË²¨¹¦ÄÜ£¬½ÓÊÕËùÓÐ±¨ÎÄ£¬ÔÊÐí¹ö´æ 
	MCP2515_WriteBits(RXB1CTRL, RXB_RX_STD, 0xFF);		//¹Ø±ÕÆÁ±ÎÂË²¨¹¦ÄÜ£¬½ÓÊÕËùÓÐ±¨ÎÄ
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿
****************************************************************************/
void Init_MCP2515(CanConfig Canfig)
{
	unsigned char i,j,a;

	MCP2515_IO_CS_Init() ;
	MCP2515_Reset();

	MCP2515_SetBandRate(Canfig.CAN_bandrate,FALSE);		//ÉèÖÃ²¨ÌØÂÊ

	// Disable interrups.
	MCP2515_Write(CANINTE, NO_IE);  		//½ûÖ¹ËùÓÐÖÐ¶Ï

	// Mark all filter bits as don't care:
	MCP2515_Write_Can_ID(RXM0SIDH, 0x7ff,0);
	MCP2515_Write_Can_ID(RXM1SIDH, 0x7ff,0);
	// Anyway, set all filters to 0:
	MCP2515_Write_Can_ID(RXF0SIDH, Canfig.RPM.ID, 0);
	MCP2515_Write_Can_ID(RXF1SIDH, Canfig.THROTTLE.ID, 0);
	MCP2515_Write_Can_ID(RXF2SIDH, Canfig.SPEED.ID, 0);
	MCP2515_Write_Can_ID(RXF3SIDH, 0, 0);
	MCP2515_Write_Can_ID(RXF4SIDH, 0, 0);
	MCP2515_Write_Can_ID(RXF5SIDH, 0, 0);

	//MCP2515_Write(CLKCTRL, MODE_LOOPBACK| CLKEN | CLK8);//»Ø»·Ä£Ê½
    //Èç¹û²»ÄÜÓÃÁ½Ì¨Éè±¸Áª»úÊµÑéµÄ»°£¬¿ÉÒÔÑ¡Ôñ»Ø»·Ä£Ê½
    MCP2515_Write(CLKCTRL, MODE_NORMAL| CLKEN | CLK8);//±ê×¼Ä£Ê½
  
	// Clear, deactivate the three transmit buffers
	a = TXB0CTRL;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 14; j++) {
			MCP2515_Write(a, 0);
			a++;
	        }
       	a += 2; // We did not clear CANSTAT or CANCTRL
	}
	// and the two receive buffers.
	MCP2515_Write(RXB0CTRL, 0);
	MCP2515_Write(RXB1CTRL, 0);

	// The two pins RX0BF and RX1BF are used to control two LEDs; set them as outputs and set them as 00.
	MCP2515_Write(BFPCTRL, 0x3C);	
	//Open Interrupt
	MCP2515_Write(CANINTE, RX0IE|RX1IE);
}

/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿MCP2515ÊµÑé³ÌÐò
****************************************************************************/
void CAN_2515_TEXT(void)
{
	int i;
	U32 id;
	unsigned char dlc;
	int rxRTR,isExt;
	int temp;
	static U32 count=0;  
	U8 data_write[8]={1,2,3,4,5,6,7,8};
	U8 data_read[8];   
    {
		Can_Write( 250, data_write, 8, FALSE, FALSE);
		if((i=Can2515_Poll())!=-1 ) 
	    {
            count++;
            for( temp=0; temp<8; temp++)
        	   data_read[temp] = 0 ;   
            temp = Can_Read(i, &id, data_read, &dlc, &rxRTR, &isExt);
            Uart_Printf( "\ni=%d ID=%d \n",i ,id );   
            Uart_Printf( "count=%d,Reveice Data=%x,%x,%x,%x,%x,%x,%x,%x\n",count,data_read[0],data_read[1],data_read[2],data_read[3],data_read[4],data_read[5],data_read[6],data_read[7] );                            
            if(id==canconfig.RPM.ID)
            {
               Can_Data_Process(data_read,canconfig.RPM,&m_RpmVal);           
            }
            else if(id== canconfig.SPEED.ID)
            {
               Can_Data_Process(data_read,canconfig.SPEED,&m_SpeedVal);
            }
            else if(id==canconfig.THROTTLE.ID )
            {
               Can_Data_Process(data_read,canconfig.THROTTLE,&m_ThrottleVal);
            }
		}
	 }	
}

void CAN_2515_RX(void)   
{   
    int i;   
    U32 id; 
    unsigned char dlc,CTRL;   
    int rxRTR, isExt;   
    int temp;   
    U8 data_read[8]; 
    if( (i=Can2515_Poll())!=-1 )  
    {   
        temp = Can_Read(i, &id, data_read, &dlc, &rxRTR, &isExt); 
        m_CanFlash=TRUE; 
        //Uart_Printf( "count=%d,Reveice Data=%x,%x,%x,%x,%x,%x,%x,%x\n",count,data_read[0],data_read[1],data_read[2],data_read[3],data_read[4],data_read[5],data_read[6],data_read[7] );                            
        if(id==canconfig.RPM.ID)
        {
            Can_Data_Process(data_read,canconfig.RPM,&m_RpmVal);
            //Uart_Printf("%f\n",(float)m_Rpm); 
            m_RpmIndex=m_RpmVal/6;     //0.1HzÈ¡Öµ; 
            m_AmtIndex=((int)m_RpmVal>>6)&0xff;
            if(m_AmtIndex>=128)m_AmtIndex=128;    
        }
        else if(id== canconfig.SPEED.ID)
        {
            Can_Data_Process(data_read,canconfig.SPEED,&m_SpeedVal);
            m_SpeedIndex=(U32)m_SpeedVal;
            if(m_SpeedIndex>255)m_SpeedIndex=255;
           //Uart_Printf( "\ni=%d ID=%d \n",i,id );
           //Uart_Printf("%f\n",(float)m_Speed);
        }
        else if(id==canconfig.THROTTLE.ID )
        {
            Can_Data_Process(data_read,canconfig.THROTTLE,&m_ThrottleVal);
            m_ThrottleIndex=(U32)m_ThrottleVal;
            if(m_ThrottleIndex>127)m_ThrottleIndex=127;
            Uart_Printf( "\ni=%d ID=%d \n",i,id );
            Uart_Printf("%f\n",(float)m_ThrottleVal);
        }  
    }
}
/****************************************************************************
¡¾¹¦ÄÜËµÃ÷¡¿MCP2515 Êý¾Ý´¦Àí³ÌÐò
    È¡³öÊý¾ÝµÄÓÐÐ§Öµ£¬
    1£©ÅÐ¶Ï´ó¶ËÐ¡¶Ë£¬2£©¸´ÖÆÓÐÐ§µÄ×Ö½ÚÊý¾Ý 3)ÒÆ³ýÎÞÐ§Î»ÐÅÏ¢
    0 = big 1 = little
    address    0    1    2    3     4     5     6     7   
    big        MSB  a    b    c     d                 LSB
    little     LSB  d    c    b     a                 MSB
   when can data's in big endian mode,the program will read an int start from 1th to 4th 
 so the  value  of the int is 'dcba' ,it's wrong ,we should exchange their positions
 so that we could read the data we want 'abcd'.
   when can data is in little endian mode,we don't need to exchange their positions.
 If the valible bits isn't start from 7th,we should shift the extra bits before we caculate
 finally value. 
    byte      7  6 5 4 3 2 1 0
                   ^
                   pos   
****************************************************************************/ 
void Can_Data_Process(U8 *data_read,CANELE canelem,float *CanVal)
{
        U8 data[4]={0,0,0,0};
        U32 *p=(U32*)data,j,val=0;
        if(canelem.ENDIAN==0)         
        {   
            for(j=0;j<4;j++)               //¸´ÖÆÓÐÐ§×Ö½Ú¿ªÊ¼µÄ4¸ö×Ö½ÚÊý¾Ý,×éºÏ³ÉÒ»¸öintÊý¾Ý
            {                              //³ÌÐòÊÇlittle ÐèÒª½»»»Êý¾ÝÎ»ÖÃ£
                if(canelem.BYTENUM+2-j>7) //canelem.BYTENUM+2-j =canelem.BYTENUM-1+3-j 
                   data[j]=0;
                else  
                   data[j]=data_read[canelem.BYTENUM+2-j];
            } 
            val=*p<<(7-canelem.BITPOS)>>(32-canelem.DATALEN); //È¡ÓÐÐ§µÄÊý¾Ý£¬ÒÆ³ýposÖ®Ç°µÄbit,È»ºóÓÐÐ§Êý¾ÝÒÆµ½µÍÎ»  
        }
        else
        {  
            
            for(j=0;j<4;j++)
            {
                if(canelem.BYTENUM-1+j>7)
                   data[j]=0;
                else
                   data[j]=data_read[canelem.BYTENUM-1+j];
            }
            data[0]<<=7-canelem.BITPOS;      //Ô¤ÏÈ´¦Àíµô¸ßÎ»ÎÞÐ§Êý¾Ý; 
            val=(*p>>(7-canelem.BITPOS))&(0xffffffff>>(32-canelem.DATALEN));
        } 
        *CanVal=val*canelem.DATACOEF;   
} 
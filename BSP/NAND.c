#include "config.h"
static S3C2416_NAND *S3C2416Nand =(S3C2416_NAND*)0x4E000000;
#define PAGE_SIZE 2048
volatile U8 NF8_Spare_Data[16];
volatile U8 Adv_NF8_Spare_Data[64];
static U8 Adv_se8Buf[64]={
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
void nand_init(void)
{
     rGPACON = (rGPACON &~(0x3f<<17)) | (0x3f<<17);
     rNFCONF = (TACLS  <<  12)|(TWRPH0 <<  8)|(TWRPH1 <<  4)|(0<<0);
     rNFCONT = (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0);
     rNFSTAT = (1<<4);
     nand_reset();
}
void nand_reset(void)
{
    volatile int i;   
	NF_nFCE_L();
	NF_CLEAR_RB();
	NF_CMD(CMD_RESET);	//reset command
	for(i=0;i<10;i++);  //tWB = 100ns. //??????
	NF_DETECT_RB();
	NF_nFCE_H();						// Deselect the flash chip.
}
int IsBadBlock(U32 block)
{
    int i;
    unsigned int blockPage;
	UBYTE  wFlag;
    blockPage = (block<<6);// For 2'nd cycle I/O[7:5] 

    NF_nFCE_L();
    NF_CLEAR_RB();
    
    NF_CMD(CMD_READ);// Spare array read command
    NF_ADDR((PAGE_SIZE+0)&0xff);
	NF_ADDR(((PAGE_SIZE+0)>>8)&0xff);
    NF_ADDR(blockPage&0xff);
    NF_ADDR((blockPage>>8)&0xff);	 // For block number A[24:17]
	if (LB_NEED_EXT_ADDR)
	    NF_ADDR((blockPage>>16)&0xff);  // For block number A[25]
    
	NF_CMD(CMD_READ3);
	NF_DETECT_RB();	 // Wait tR(max 12us)

    wFlag = (UBYTE)(NF_RDDATA8()&0xff);

    NF_nFCE_H(); 
    if(wFlag!=0xff)
    {
    	Uart_Printf("[block %d is a bad block(%x)]\n",block,wFlag );
    	return FAIL;
    }
    else
    {
        Uart_Printf("block %d \n",block);
    	return OK;
    }
}
U16 Read_Status(void)
{
	U16 status;
	int i=0;

	NF_nFCE_L();
    NF_CMD(CMD_STATUS);
    for(i=0; i<10; i++);
    status = NF_RDDATA();	
	NF_nFCE_H();

	return status;
}
U8 Block_Erase(U32 block)
{
   if((Read_Status()&0x80)==0) {
		Uart_Printf("Write protected.\n");
			return  FALSE;
		}
   Uart_Printf("\n%d-block erased.\n", block);
   if(EraseBlock(block)==FALSE)
	   return FALSE;
   return TRUE;
}
static U8 EraseBlock(U32 block)
{
	U32 blockPage=(block<<PAGES_PER_BLOCK);
	int i;
	U8 bRet=TRUE;
	NF_nFCE_L();
    
	NF_CLEAR_RB();
    NF_CMD(CMD_ERASE);
    NF_ADDR((blockPage) & 0xff);
	NF_ADDR((blockPage >> 8) & 0xff);
	if(LB_NEED_EXT_ADDR)
	{
	   NF_ADDR((blockPage >> 16) & 0xff);
	}
    NF_CMD(CMD_ERASE2);
   
    NF_DETECT_RB();
    
	if ( rNFSTAT & STATUS_ILLACC )
	{
		rNFSTAT =  STATUS_ILLACC;
		bRet = FALSE;
	}
	else
	{
        NF_CMD(CMD_STATUS);
		if( NF_RDDATA8() & STATUS_ERROR) {
			bRet = FALSE;
		}
	}

	NF_nFCE_H();

	return bRet;
}
void Block_Write(U32 Start_Block,U32 Start_Page,U8 *Buffer,U32 nCount)
{
	 int i=0;
	 U8 srcPt[2048],flag=1;
     U32 count=0;
	 for(;Start_Block<1024;Start_Block++)
	 {
		 if(IsBadBlock(Start_Block))
		 {
             if(Block_Erase(Start_Block))
			  { 
				  break;
			  }
		 }
	 }
	 while(flag)
	 {
		 memset(srcPt,0,2048);
		 for(i=0;i<2048;i++)
		 {
		     srcPt[i]=*(Buffer+count);		    
		     if(++count>=nCount)
		     {	        
			    flag=0;
				break;
		     }
		 }
		 if(WritePage(Start_Block ,Start_Page,srcPt)==FALSE)
		 {
            Uart_Printf("Write Error.\n");
		 }
		 else
		 {
		    Uart_Printf("%d block %d Page Write OK. \n",Start_Block ,Start_Page);
		 }
		 if(++Start_Page>=64)
		 {
			 Start_Page=0;
             ++Start_Block;
			 for(;Start_Block<1024;Start_Block++)
	         {
		         if(IsBadBlock(Start_Block))
		         {
                    if(Block_Erase(Start_Block))
			        { 
				       break;
			        }
		         }
	         }
		 }
	 }
}
static int WritePage(U32 block ,U32 page,U8 *buffer)
{
     int i=0;
	 U32 blockpage;
	 blockpage=block<<PAGES_PER_BLOCK+page;

	 NF_nFCE_L();
     NF_CLEAR_RB();
	 NF_CMD(CMD_WRITE);

	 NF_ADDR((0)&0xff);
	 NF_ADDR((0>>8)&0xff);
	 NF_ADDR((blockpage)&0xff);
	 NF_ADDR((blockpage>>8)&0xff);
	 if (LB_NEED_EXT_ADDR)
	 {
	 	NF_ADDR((blockpage>>16)&0xff);
	 }
     for(i=0; i<PAGE_SIZE; i++)
     {
		WrNFDat8(buffer[i]);
	 }
     NF_CLEAR_RB();
	 NF_CMD(CMD_WRITE2);	 // Write 2nd command
	 NF_DETECT_RB();
     
	 NF_CMD(CMD_STATUS); 
     for(i=0;i<3;i++);
  
     if(NF_RDDATA()&0x1) 
	 {// Page write error
		NF_nFCE_H();
		return FAIL;
	 } 
	 else 
	 {
		NF_nFCE_H();
		return OK;
	 }
}
void Block_Read(U32 Start_Block,U32 Start_Page,U8 *Buffer,U32 nCount)
{
	 U32 i=0,count=0;
	 U8 downPt[2048],flag=1;
	 memset(downPt,0,2048);
	 for(;Start_Block<2048;Start_Block++)
	 {
	      if(IsBadBlock(Start_Block))
	      {
	         break;  
	      }
	  }
	 while(flag)
	 {
	   
		ReadPage(Start_Block,Start_Page, (U8 *)downPt);
		Uart_Printf("Read data(%d-block,%d-page)\n", Start_Block, Start_Page);
		for(i=0;i<2048;i++)
		{
		   *(Buffer+count)=downPt[i];
		    if(++count >= nCount)
		    {
		       flag=0;
		       break;
		    }
		}
		if(++Start_Page>=64)
		{
		    Start_Page=0;
		    ++Start_Block;
		    for(;Start_Block<2048;Start_Block++)
	        {
	          if(IsBadBlock(Start_Block))
	          {
	              break;  
	          }
	        }
		}
	 }
}
static void ReadPage(U32 block ,U32 page,U8 *buffer)
{
     U32 blockpage,i,Mecc, Secc;
	 blockpage=block<<PAGES_PER_BLOCK+page;
	 for (i=0; i<100000; i++);	// 獨0.25ms
	 NF_RSTECC();		// NFCONT [5:4]를 11로 셋팅. ; Init main ECC, Init spare ECC.
	 NF_MECC_UnLock();	// NFCOnT [6]를 0으로 클리어. ; Unlock Spare ECC

	 NF_nFCE_L();
     NF_CLEAR_RB();
	 NF_CMD(CMD_READ);

	 NF_ADDR((0)&0xff);
	 NF_ADDR(((0)>>8)&0xff);
	// 3cycle + 4cycle + 5cycle = row address -> block의 위치 표시(2048개의 block, 11bit)
	 NF_ADDR((blockpage) & 0xff);
	 NF_ADDR((blockpage >> 8) & 0xff);
	 if (LB_NEED_EXT_ADDR)
		NF_ADDR((blockpage >> 16) & 0xff);
     NF_CMD(CMD_READ3);

	 NF_DETECT_RB();	

     for(i=0; i<PAGE_SIZE; i++)
     {
		*buffer++=NF_RDDATA8();
     }
     NF_MECC_Lock();		// NFCONT [6]를 1로 셋팅. ; Lock Spare ECC


	 NF_SECC_UnLock();
	 Adv_NF8_Spare_Data[0]=NF_RDDATA8();
	 Mecc=NF_RDDATA();
	 rNFMECCD0=((Mecc&0xff00)<<8)|(Mecc&0xff);
	 rNFMECCD1=((Mecc&0xff000000)>>8)|((Mecc&0xff0000)>>16);

	 Adv_NF8_Spare_Data[1]=Mecc&0xff;
	 Adv_NF8_Spare_Data[2]=(Mecc&0xff00)>>8;
	 Adv_NF8_Spare_Data[3]=(Mecc&0xff0000)>>16;
	 Adv_NF8_Spare_Data[4]=(Mecc&0xff000000)>>24;

	 Adv_NF8_Spare_Data[5]=NF_RDDATA8();  // read 5
	 Adv_NF8_Spare_Data[6]=NF_RDDATA8();  // read 6
	 Adv_NF8_Spare_Data[7]=NF_RDDATA8();  // read 7
     NF_SECC_Lock();
     Secc=NF_RDDATA();
	 rNFSECCD=((Secc&0xff00)<<8)|(Secc&0xff);
	 Adv_NF8_Spare_Data[8]=Secc&0xff;
	 Adv_NF8_Spare_Data[9]=(Secc&0xff00)>>8;
	 Adv_NF8_Spare_Data[10]=(Secc&0xff0000)>>16;
	 Adv_NF8_Spare_Data[11]=(Secc&0xff000000)>>24;

	 for(i=12;i<64;i++) 
	 {
		Adv_NF8_Spare_Data[i]=NF_RDDATA8();	// Read spare array with 4byte width
	 }
	 NF_nFCE_H();
}
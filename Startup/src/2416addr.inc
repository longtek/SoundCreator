;=====================================================================================
;	Fin = 12MHz,
;
;	MPLLout = (2m x Fin)/(p x 2**s), m=MDIV+8, p=PDIV, s=SDIV, Fin=10~30MHz
;	(17,1,1)=300Mhz, (92,3,1)=400Mhz, (67,2,1)=450Mhz, (81,2,1)=534Mhz, 
;	(17,1,0)=600Mhz, (92,3,0)=800Mhz
;
;	EPLLout = (m x Fin)/(p x 2**s), m=MDIV+8, p=PDIV+2, s=SDIV, Fin=10~100MHz
;	(28,1,2)=36Mhz, (40,1,2)=48Mhz, (22,1,1)=60Mhz, (28,1,1)=72Mhz, (34,1,1)=84Mhz
;	(40,1,1)=96Mhz
;=====================================================================================
	GBLA	CLKVAL
	GBLA	DVSON		
	GBLA	HCLKVAL
CLKVAL	SETA	400133


	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_MPLL		EQU	800000000		;	800000000
Startup_Mdiv		EQU 	400
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	1			;	1

Startup_ARMCLKdiv	EQU	1			;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	2			;	0  :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv
Startup_HCLKdiv		EQU	1			;	0  :	HCLK	= PREDIV_CLK
							;	0x1 :	HCLK	= PREDIV_CLK/2
							;	0x3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1			;	0	 :	PCLK	= HCLK
							;	1	 :	PCLK	= HCLK/2
	
	

	
Startup_EMdiv		EQU 	32			; EPLL -> 96Mhz
Startup_EPdiv		EQU     1
Startup_ESdiv		EQU 	2	

S3C2450_BASE_REG_PA_IOPORT    EQU             (0x56000000)    
GPKCON		EQU		(S3C2450_BASE_REG_PA_IOPORT  + 0xE0)
GPKDAT		EQU		(S3C2450_BASE_REG_PA_IOPORT  + 0xE4)
GPKUDP		EQU		(S3C2450_BASE_REG_PA_IOPORT  + 0xE8)

GPGCON		EQU		(S3C2450_BASE_REG_PA_IOPORT	 + 0x60)
GPGDAT		EQU		(S3C2450_BASE_REG_PA_IOPORT  + 0x64)
GPGUDP		EQU		(S3C2450_BASE_REG_PA_IOPORT  + 0x68)


;=================
; Memory control
;=================
BANKCFG     EQU   0x48000000	    ;Bus width & wait status
BANKCON1	EQU  0x48000004     ;Boot ROM control
BANKCON2	EQU  0x48000008     ;BANK1 control
BANKCON3	EQU  0x4800000c     ;BANK2 control
REFRESH  	EQU  0x48000010     ;BANK3 control
TIMEOUT  	EQU  0x48000014     ;BANK4 control



BANKCFGVAL   EQU		((RASBW0<<17)+(RASBW1<<14)+(CASBW0<<11)+(CASBW1<<8)+(ADDRCFG0<<6)+(ADDRCFG1<<4)+(MEMCFG<<1)+(BW<<0))
BANKCON1VAL  EQU		((DQSDelay<<28)+(1<<26)+(1<<8)+(BStop<<7)+(WBUF<<6)+(AP<<5)+(PWRDN<<4))
BANKCON2VAL  EQU		((tRAS<<20)+(tARFC<<16)+(CL<<4)+(tRCD<<2)+(tRP<<0))
;---------------------------------------
; BANKCFG register  : DDR2 SDRAM configure
;---------------------------------------
RASBW0				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
RASBW1				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
CASBW0				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
CASBW1				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
ADDRCFG0			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
ADDRCFG1			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
MEMCFG				EQU		1			; 	Ext.Mem 000=SDR,010=MSDR,100=DDRz,110=MDDR,001=DDR2
BW				    EQU		1			; 	Bus width 00=32bit,01=16bit

;---------------------------------------
; BANKCON1 register : DDR2 SDRAM timing control
;---------------------------------------
PADLOAD				EQU		1
BStop				EQU		0			;	read burst stop control
WBUF				EQU		1			;	write buffer control
AP				    EQU		0			;	auto precharge control
;;PWRDN				EQU		0			;	power down mode
PWRDN				EQU		1			;	power down mode
DQSDelay			EQU		4			;	DQS delay

;-----------------------[DDR2 SDRAM]--------------------------
;    HCLK		tRAS		tARFC		CAS		tRCD		tRP		REFCYC
;---------------------------------------------------------------
;18MHz			0		1		3		0		0		140
;36MHz			1		3		3		1		1		280
;100Mhz			4		10		3		1		1		780
;110Mhz			4		11		3		1		1		858
;120Mhz			4		12		3		1		1		936
;133Mhz			5		13		3		1		1		1037		
;---------------------------------------------------------------
;------------------------------------
; BANKCON2 register : DDR2 SDRAM timing control
;---------------------------------------
	[ HCLKVAL = 18
tRAS			EQU		0			;	Row active time
tARFC			EQU		1			;	Row cycle time
CL				EQU		3			;	CAS latency control
tRCD			EQU		0			;	RAS to CAS delay
tRP				EQU		0			; 	Row pre-charge time
	]
	[ HCLKVAL = 133
tRAS			EQU		5			;	Row active time
tARFC			EQU		13			;	Row cycle time
CL				EQU		3			;	CAS latency control
tRCD			EQU		1			;	RAS to CAS delay
tRP				EQU		1			; 	Row pre-charge time
	]
	[ HCLKVAL = 100
tRAS			EQU		4			;	Row active time
tARFC			EQU		10			;	Row cycle time
CL				EQU		3			;	CAS latency control
tRCD			EQU		1			;	RAS to CAS delay
tRP				EQU		1			; 	Row pre-charge time
	]

;---------------------------------------
; BANKCON3 register : DDR2 SDRAM MRS/EMRS register
;---------------------------------------
BA_EMRS1			EQU		1			;	BA : EMRS
DLL_DIS				EQU		1
DLL_EN				EQU		0
nDQS_DIS			EQU		1
RDQS_DIS			EQU		0
OCD_MODE_EXIT			EQU		0
OCD_MODE_DEFAULT		EQU		7
BA_EMRS2			EQU		2			;	BA : EMRS
BA_EMRS3			EQU		3			;	BA : EMRS
DS				EQU		0			;	Driver strength
PASR				EQU		0			;	PASR
BA_MRS				EQU		0			;	BA : MRS
TM				    EQU		0			; 	Test Mode - mode register set
CL_MRS				EQU		3			; 	CAS Latency
DLL_RESET_HIGH		EQU		1
DLL_RESET_LOW		EQU		0
;---------------------------------------
; REFRESH register : DDR2 SDRAM refresh register
;---------------------------------------
	[ HCLKVAL = 18
REFCYC				EQU		140			; 	refresh cycle
	]
	[ HCLKVAL = 133
REFCYC				EQU		1037			; 	refresh cycle
	]
	[ HCLKVAL = 100
REFCYC				EQU		780			; 	refresh cycle
	]	
;==========================
; CLOCK & POWER MANAGEMENT
;==========================
S3C2450_BASE_REG_PA_CLOCK_POWER    EQU        (0x4C000000)

LOCKCON0        EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x00)
LOCKCON1        EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x04)
MPLLCON         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x10)
EPLLCON         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x18)
HCLKCON         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x30)
PCLKCON         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x34)
SCLKCON         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x38)
CLKDIV0         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x24)
CLKDIV1         EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x28)
CLKSRC          EQU         (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x20)
PWRMODE		    EQU	        (S3C2450_BASE_REG_PA_CLOCK_POWER + 0x40)

RSTSTAT     EQU           0x4C000068  		;//Reset status
WKUPSTAT    EQU           0x4C00006c  		;//Wakeup status
INFORM0     EQU           0x4C000070  		;//Sleep mode information 0
INFORM1     EQU           0x4C000074  		;//Sleep mode information 1
INFORM2     EQU           0x4C000078  		;//Sleep mode information 2
INFORM3     EQU           0x4C00007C  		;//Sleep mode information 3
RSTCON      EQU     	  0x4C000064        ;//Reset Control
;=================
; INTERRUPT
;=================
SRCPND   	EQU  0x4a000000    ;Interrupt request status
INTMOD   	EQU  0x4a000004    ;Interrupt mode control
INTMSK   	EQU  0x4a000008    ;Interrupt mask control
PRIORITY 	EQU  0x4a00000c    ;IRQ priority control           <-- May 06, 2002 SOP
INTPND   	EQU  0x4a000010    ;Interrupt request status
INTOFFSET	EQU  0x4a000014    ;Interruot request source offset
SUSSRCPND	EQU  0x4a000018    ;Sub source pending
INTSUBMSK	EQU  0x4a00001c    ;Interrupt sub mask

;=================
; I/O PORT for LED
;=================
;//chapter4 SSMC
SMBIDCYR0   EQU           0x4F000000	;//Bank0 idle cycle control 
SMBIDCYR1   EQU           0x4F000020	;//Bank1 idle cycle control 
SMBIDCYR2   EQU           0x4F000040	;//Bank2 idle cycle control 
SMBIDCYR3   EQU           0x4F000060	;//Bank3 idle cycle control 
SMBIDCYR4   EQU           0x4F000080	;//Bank0 idle cycle control 
SMBIDCYR5   EQU           0x4F0000A0	;//Bank5 idle cycle control 
SMBWSTRDR0  EQU           0x4F000004	;//Bank0 read wait state control 
SMBWSTRDR1  EQU           0x4F000024	;//Bank1 read wait state control
SMBWSTRDR2  EQU           0x4F000044	;//Bank2 read wait state control
SMBWSTRDR3  EQU           0x4F000064	;//Bank3 read wait state control 
SMBWSTRDR4  EQU           0x4F000084	;//Bank4 read wait state control 
SMBWSTRDR5  EQU           0x4F0000A4	;//Bank5 read wait state control 
SMBWSTWRR0  EQU           0x4F000008	;//Bank0 write wait state control 
SMBWSTWRR1  EQU           0x4F000028	;//Bank1 write wait state control 
SMBWSTWRR2  EQU           0x4F000048	;//Bank2 write wait state control 
SMBWSTWRR3  EQU           0x4F000068	;//Bank3 write wait state control 
SMBWSTWRR4  EQU           0x4F000088	;//Bank4 write wait state control 
SMBWSTWRR5  EQU           0x4F0000A8	;//Bank5 write wait state control 
SMBWSTOENR0 EQU           0x4F00000C	;//Bank0 output enable assertion delay control 
SMBWSTOENR1 EQU           0x4F00002C	;//Bank1 output enable assertion delay control 
SMBWSTOENR2 EQU           0x4F00004C	;//Bank2 output enable assertion delay control
SMBWSTOENR3 EQU           0x4F00006C	;//Bank3 output enable assertion delay control 
SMBWSTOENR4 EQU           0x4F00008C	;//Bank4 output enable assertion delay control
SMBWSTOENR5 EQU           0x4F0000AC	;//Bank5 output enable assertion delay control 
SMBWSTWENR0 EQU           0x4F000010	;//Bank0 write enable assertion delay control 
SMBWSTWENR1 EQU           0x4F000030	;//Bank1 write enable assertion delay control 
SMBWSTWENR2 EQU           0x4F000050	;//Bank2 write enable assertion delay control 
SMBWSTWENR3 EQU           0x4F000070	;//Bank3 write enable assertion delay control 
SMBWSTWENR4 EQU           0x4F000090	;//Bank4 write enable assertion delay control 
SMBWSTWENR5 EQU           0x4F0000B0	;//Bank5 write enable assertion delay control 
SMBCR0      EQU           0x4F000014	;//Bank0 control 
SMBCR1      EQU           0x4F000034	;//Bank1 control 
SMBCR2      EQU           0x4F000054	;//Bank2 control 
SMBCR3      EQU           0x4F000074	;//Bank3 control 
SMBCR4      EQU           0x4F000094	;//Bank4 control 
SMBCR5      EQU           0x4F0000B4	;//Bank5 control 
SMBSR0      EQU           0x4F000018	;//Bank0 status 
SMBSR1      EQU           0x4F000038	;//Bank1 status 
SMBSR2      EQU           0x4F000058	;//Bank2 status 
SMBSR3      EQU           0x4F000078	;//Bank3 status 
SMBSR4      EQU           0x4F000098	;//Bank4 status 
SMBSR5      EQU           0x4F0000B8	;//Bank5 status 
SMBWSTBRDR0 EQU           0x4F00001C	;//Bank0 burst read wait delay control 
SMBWSTBRDR1 EQU           0x4F00003C	;//Bank1 burst read wait delay control 
SMBWSTBRDR2 EQU           0x4F00005C	;//Bank2 burst read wait delay control 
SMBWSTBRDR3 EQU           0x4F00007C	;//Bank3 burst read wait delay control 
SMBWSTBRDR4 EQU           0x4F00009C	;//Bank4 burst read wait delay control 
SMBWSTBRDR5 EQU           0x4F0000BC	;//Bank5 burst read wait delay control 
SSMCSR      EQU           0x4F000200	;//SROMC status 
SSMCCR      EQU           0x4F000204	;//SROMC control

IDCY0				EQU		0x0			;	Idle or turnaround cycles IDCY*HCLK
WSTRD0				EQU		0xe			;	Read wait state = tacc
WSTWR0				EQU		0xe			;	wrie wait state
WSTOEN0				EQU		0			; 	output enable assertion delay from CS
WSTWEN0				EQU		0			; 	write enable assertion delay

BlWriteEn			EQU		1			;bit21-SMBAA signal control:0-1at all times, 1 active for sync
AddrValidWriteEn	EQU		1			;bit20-SMADDRVALD during write:0-always high,1-active for write 	
BurstLenWrite		EQU		0			;bit1819-burst transfer length:0-4,1-8,3-continu(sync only) 	
SyncWriteDev		EQU		0			;bit17-0:async, 1:sync 	
BMWrite				EQU		0			;bit16-burt mode write : 0-non-burst, 1-burst 	
WrapRead			EQU		0			;bit14-0-disable, 1 enable
BlReadEn			EQU		1			;bit13-SMBAA signal :0-1 at all time, 1-active for sync read
AddrValidReadEn		EQU		1			;bit12-SMADDRVALID signal: 0-always HIGH, 1-active for async & sync read
BurstLenRead		EQU		0			;bit1011-burst transfer length:0-4,1-8,2-16,3-cont(sync only)
SyncReadDev			EQU		0			;bit9-sync access :0-async, 1-sync
BMRead				EQU		0			;bit8-burst mode red and async page mode
SMBLSPOL			EQU		0			;bit6-polarit of signal nSMBLS
MW					EQU		1			;bit45-memory width : 00-8bit,01-16bit,10-32bit
WP					EQU		0			;bit3-write protect
WaitEn				EQU		0			;bit2-external wait signal enable
WaitPol				EQU		0			;bit1-polarity of the external wait input for actiation
RBLE				EQU		0			;bit0-read byte lane enable
SMBCR0_0			EQU		((BMRead<<8)+(SMBLSPOL<<6)+(MW<<4)+(WP<<3)+(WaitEn<<2)+(WaitPol<<1)+RBLE)
SMBCR0_1			EQU		((WrapRead<<14)+(BlReadEn<<13)+(AddrValidReadEn<<12)+(BurstLenRead<<10)+(SyncReadDev<<9))
SMBCR0_2			EQU		((BlWriteEn<<21)+(AddrValidWriteEn<<20)+(BurstLenWrite<<18)+(SyncWriteDev<<17)+(BMWrite<<16))

WaitTourErr0			EQU		0			; external wait timeout error flag

WSTBRD0				EQU		0x1f			; burst read wait state

MemClkRatio			EQU		1			; SMMEMCLK :0-HCLK,1-HCLK/2,2-HCLK/3
SMClockEn			EQU		1			; SMCLK enable 0-only active during mem access,1-always running	


;Miscellaneous register
MISCCR  	EQU  0x56000080     ;Miscellaneous control
DCKCON  	EQU  0x56000084     ;DCLK0/1 control
EXTINT0 	EQU  0x56000088     ;External interrupt control register 0
EXTINT1 	EQU  0x5600008c     ;External interrupt control register 1
EXTINT2 	EQU  0x56000090     ;External interrupt control register 2
EINTFLT0	EQU  0x56000094     ;Reserved
EINTFLT1	EQU  0x56000098     ;Reserved
EINTFLT2	EQU  0x5600009c     ;External interrupt filter control register 2
EINTFLT3	EQU  0x560000a0     ;External interrupt filter control register 3
EINTMASK	EQU  0x560000a4     ;External interrupt mask
EINTPEND	EQU  0x560000a8     ;External interrupt pending
GSTATUS0	EQU  0x560000ac     ;External pin status
GSTATUS1	EQU  0x560000b0     ;Chip ID(0x32440000)
GSTATUS2	EQU  0x560000b4     ;Reset type
GSTATUS3	EQU  0x560000b8     ;Saved data0(32-bit) before entering POWER_OFF mode
GSTATUS4	EQU  0x560000bc     ;Saved data1(32-bit) before entering POWER_OFF mode

;Added for 2440					; DonGo
MSLCON		EQU  0x560000cc     ;Memory sleep control register

;=================
; WATCH DOG TIMER  Íê³É
;=================
WTCON 	EQU  0x53000000       ;Watch-dog timer mode
WTDAT 	EQU  0x53000004       ;Watch-dog timer data
WTCNT 	EQU  0x53000008       ;Eatch-dog timer count

        END

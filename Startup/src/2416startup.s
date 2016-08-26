   GET option.inc
   GET Memcfg.inc
   GET 2416addr.inc
    
USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0

UserStack	EQU	(_STACK_BASEADDRESS-0x3800)	;0x33ff4800 ~
SVCStack	EQU	(_STACK_BASEADDRESS-0x2800)	;0x33ff5800 ~
UndefStack	EQU	(_STACK_BASEADDRESS-0x2400)	;0x33ff5c00 ~
AbortStack	EQU	(_STACK_BASEADDRESS-0x2000)	;0x33ff6000 ~
IRQStack	EQU	(_STACK_BASEADDRESS-0x1000)	;0x33ff7000 ~
FIQStack	EQU	(_STACK_BASEADDRESS-0x0)	;0x33ff8000 ~



 	MACRO
$HandlerLabel HANDLER $HandleAddr

$HandlerLabel
	sub	    sp,sp,#4				;decrement sp(to store jump address)
	stmfd	sp!,{r0}			;PUSH the work register to stack(lr does not push because it return to original address)
	ldr     r0,=$HandleAddr		;load the address of HandleXXX to r0
	ldr     r0,[r0]	 			;load the contents(service routine start address) of HandleXXX
	str     r0,[sp,#4]      	;store the contents(ISR) of HandleXXX to stack
	ldmfd   sp!,{r0,pc}     	;POP the work register and pc(jump to ISR)
	MEND
	
	IMPORT  |Image$$RO$$Base|	; Base of ROM code
	IMPORT  |Image$$RO$$Limit|  ; End of ROM code (=start of ROM data)
	IMPORT  |Image$$RW$$Base|   ; Base of RAM to initialise
	IMPORT  |Image$$ZI$$Base|   ; Base and limit of area
	IMPORT  |Image$$ZI$$Limit|  ; to zero initialise	
	
	; 引入链接器产生符号，以确定代码运行位置，编译生成的大小
	;IMPORT  |Image$$ER_ROM0$$Base|
	;IMPORT  |Load$$ER_ROM0$$Length|	
	;IMPORT  |Load$$ER_ROM1$$Length|	
	;IMPORT  |Load$$RW_RAM$$RW$$Length|


		
	IMPORT  Main		; The main entry of mon program
	IMPORT  OS_CPU_IRQ_ISR ; uCOS_II IrqISR//uCOS_II			

	EXPORT  HandleEINT0    ; for os_cpu_a.s//uCOS_II
	
	
	  AREA StartUp2440,CODE,READONLY
	ENTRY
	EXPORT	__ENTRY
__ENTRY	
   
ResetEntry 


    b   ResetHandler  
    b   HandlerUndef
    b   HandlerSWI
    b	HandlerPabort
    b	HandlerDabort
    b	.
    b	HandlerIRQ		;handler for IRQ interrupt
	b	HandlerFIQ		;handler for FIQ interrupt

HandlerFIQ		HANDLER HandleFIQ
HandlerIRQ		HANDLER HandleIRQ
HandlerUndef	HANDLER HandleUndef
HandlerSWI		HANDLER HandleSWI
HandlerDabort	HANDLER HandleDabort
HandlerPabort	HANDLER HandlePabort	
	
ResetHandler	
	
	ldr	r0,=WTCON       ;watch dog disable
	ldr	r1,=0x0
	str	r1,[r0]

	ldr	r0,=INTMSK
	ldr	r1,=0xffffffff  ;all interrupt disable
	str	r1,[r0]
 
	ldr	r0,=INTSUBMSK
	ldr	r1,=0x7fff		;all sub interrupt disable
	str	r1,[r0]
	
	ldr r0, = INTMOD
	mov r1, #0x0			; set all interrupt as IRQ
	str r1, [r0]

   	ldr		r0,=CLKDIV0			;	Set Clock Divider
	ldr		r1,[r0]
	bic		r1,r1,#0x37		; clear HCLKDIV, PREDIV, PCLKDIV
	bic		r1,r1,#(0xf<<9) ; clear ARMCLKDIV
	ldr		r2,=((Startup_ARMCLKdiv<<9)+(Startup_PREdiv<<4)+(Startup_PCLKdiv<<2)+(Startup_HCLKdiv)) 
	orr		r1,r1,r2
	;ldr    r1,=0x22d
	str		r1,[r0]			

	ldr		r0,=LOCKCON0		;	Set lock time of MPLL. added by junon
	mov		r1,#0xe10			;	Fin = 12MHz - 0x800, 16.9844MHz - 0xA00
	str		r1,[r0]	

	ldr		r0,=LOCKCON1		;	Set lock time of EPLL. added by junon
	mov		r1,#0x800			;	Fin = 12MHz - 0x800, 16.9844MHz - 0xA00
	str		r1,[r0]	

	ldr		r0,=MPLLCON			;	Set MPLL
	ldr		r1,=((0<<24)+(Startup_Mdiv<<14)+(Startup_Pdiv<<5)+(Startup_Sdiv))
	str		r1,[r0]			

  	ldr		r0,=EPLLCON			;	Set EPLL
	ldr		r1,=((0<<24)+(Startup_EMdiv<<16)+(Startup_EPdiv<<8)+(Startup_ESdiv))
	str		r1,[r0]			

	ldr		r0,=CLKSRC			;	Select MPLL clock out for SYSCLK
	ldr		r1,[r0]
	orr		r1,r1,#0x50
	str		r1,[r0]	

	
	bl	InitMEM
	bl  InitSSMC
	
	
Somedelay
	mov	r0, #&1000
1
	subs	r0, r0, #1
	bne	    %B1
	
	bl	InitStacks
	
	ldr	r0,=HandleIRQ	;This routine is needed
	ldr	r1,=OS_CPU_IRQ_ISR	;if there is not 'subs pc,lr,#4' at 0x18, 0x1c
	str	r1,[r0]
	
	
	
    ;=========select which way of code from flash to SDRAM============================	
	


InitRam	
    mov	r0,	#0
    ldr	r2, BaseOfZero
	ldr	r3, EndOfBSS
	
1	
	cmp	r2,	r3
	strcc	r0, [r2], #4
	bcc	%B1
	
    ldr	pc, =CEntry		    ;goto compiler address
	
CEntry
 	b	Main	;Do not use main() because ......
 	b	. 
 	
InitStacks
	mrs	r0,cpsr
	bic	r0,r0,#MODEMASK
	orr	r1,r0,#UNDEFMODE|NOINT
	msr	cpsr_cxsf,r1		;UndefMode
	ldr	sp,=UndefStack		; UndefStack=0x33FF_5C00

	orr	r1,r0,#ABORTMODE|NOINT
	msr	cpsr_cxsf,r1		;AbortMode
	ldr	sp,=AbortStack		; AbortStack=0x33FF_6000

	orr	r1,r0,#IRQMODE|NOINT
	msr	cpsr_cxsf,r1		;IRQMode
	ldr	sp,=IRQStack		; IRQStack=0x33FF_7000

	orr	r1,r0,#FIQMODE|NOINT
	msr	cpsr_cxsf,r1		;FIQMode
	ldr	sp,=FIQStack		; FIQStack=0x33FF_8000

	bic	r0,r0,#MODEMASK|NOINT
	orr	r1,r0,#SVCMODE
	msr	cpsr_cxsf,r1		;SVCMode
	ldr	sp,=SVCStack		; SVCStack=0x33FF_5800

	;USER mode has not be initialized.

	mov	pc,lr
	;The LR register will not be valid if the current mode is not SVC mode.

	
InitMEM
				;DDR2
				
				;Set GPK port when using x32 bus width.
				ldr		r0,=GPKCON
				ldr		r1,=0xaaaaaaaa	; set Sdata[31:16] 
				str		r1, [r0]
				
				;Set DDR2 Memory parameter control registers
				ldr		r0,=BANKCFG
				ldr		r1,=BANKCFGVAL 	; set Sdata[31:16]  
				str		r1, [r0]

				ldr		r0,=BANKCON1
				ldr		r1,=BANKCON1VAL  	; set Sdata[31:16]
				str		r1, [r0]


				ldr		r0,=BANKCON2
				ldr		r1,=BANKCON2VAL   	; set Sdata[31:16]
				str		r1, [r0]
				
					

				ldr		r2,=BANKCON1			;	4nd	:	Issue a PALL command
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x1<<0)			
				str		r1,[r2]	

				ldr		r2,=BANKCON3			;	5th	:	Issue a EMRS2 command
				ldr		r3,=0xffff0000
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,#(BA_EMRS2<<30)
				str		r1,[r2]	

				ldr		r2,=BANKCON1
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x3<<0)			
				str		r1,[r2]

				ldr		r2,=BANKCON3			;	6th	:	Issue a EMRS3 command
				ldr		r3,=0xffff0000
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,#(BA_EMRS3<<30)
				str		r1,[r2]

				ldr		r2,=BANKCON1
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x3<<0)			
				str		r1,[r2]

				ldr		r2,=BANKCON3			;	7th	:	Issue a EMRS1 command
				ldr		r3,=0xffff0000
				ldr		r4,=((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_EXIT<<23)+(DLL_EN<<16)) 
				                      ; (0x1<<30)|(0x0<<27)|(0x1<<26)|(0x0<<23)|(0x0<<16)
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,r4
				str		r1,[r2]

				ldr		r2,=BANKCON1
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x3<<0)			
				str		r1,[r2]

				ldr		r2,=BANKCON3			;	8th	:	Issue a MRS command
				ldr		r3,=0xffff
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,#((BA_MRS<<14)+(DLL_RESET_HIGH<<8)+(TM<<7)+(CL_MRS<<4))
				str		r1,[r2]
				
				ldr		r2,=BANKCON1				
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x2<<0)			
				str		r1,[r2]

				ldr		r2,=BANKCON1			;	9nd	:	Issue a PALL command
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x1<<0)			
				str		r1,[r2]

				ldr		r4,=REFRESH				;	10th : wait 2 auto - clk
				ldr		r0,=0x20
				str		r0,[r4]					
				
				ldr		r2,=BANKCON3			;	11th	:	Issue a MRS command
				ldr		r3,=0xffff
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,#((BA_MRS<<14)+(DLL_RESET_LOW<<8)+(TM<<7)+(CL_MRS<<4))
				str		r1,[r2]
				
				ldr		r2,=BANKCON1				
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x2<<0)			
				str		r1,[r2]

				mov	r0, #0x100					;	Wait 200 clock
2				subs	r0, r0,#1;
				bne	%B2	
				
				ldr		r2,=BANKCON3			;	12th	:	Issue a EMRS1 command For OCD Mode Set to default
				ldr		r3,=0xffff0000
				ldr		r4,=((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_DEFAULT<<23)+(DLL_EN<<16))
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,r4
				str		r1,[r2]

				ldr		r2,=BANKCON1
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x3<<0)			
				str		r1,[r2]

				ldr		r2,=BANKCON3
				ldr		r3,=0xffff0000
				ldr		r4,=((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_EXIT<<23)+(DLL_EN<<16))
				ldr		r1,[r2]
				bic		r1,r1,r3
				orr		r1,r1,r4
				str		r1,[r2]

				ldr		r2,=BANKCON1
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				orr		r1,r1,#(0x3<<0)			
				str		r1,[r2]
				
				ldr		r4,=REFRESH			;	13fh : refresh  normal
				ldr		r0,=REFCYC
				str		r0,[r4]					

				ldr		r2,=BANKCON1		;	14th	:	Issue a Normal mode
				ldr		r1,[r2]
				bic		r1,r1,#(0x3<<0)
				str		r1,[r2]


InitSSMC

	;Set SSMC Memory parameter control registers : AMD Flash
	ldr		r0,=SMBIDCYR0
	ldr		r1,=IDCY0
	str		r1,[r0]
	
	ldr		r0,=SMBWSTRDR0
	ldr		r1,=WSTRD0
	str		r1,[r0]
	
	ldr		r0,=SMBWSTWRR0
	ldr		r1,=WSTWR0
	str		r1,[r0]
	
	ldr		r0,=SMBWSTOENR0
	ldr		r1,=WSTOEN0
	str		r1,[r0]
	
	ldr		r0,=SMBWSTWENR0
	ldr		r1,=WSTWEN0
	str		r1,[r0]
	
	ldr		r0,=SMBCR0
	ldr		r1,=(SMBCR0_2+SMBCR0_1+SMBCR0_0)
	str		r1,[r0]
	
	ldr		r0,=SMBWSTBRDR0
	ldr		r1,=WSTBRD0
	str		r1,[r0]

	
	ldr		r0,=SMBWSTBRDR0
	ldr		r1,=WSTBRD0
	str		r1,[r0]

	ldr		r0,=SSMCCR
	ldr		r1,=((MemClkRatio<<1)+(SMClockEn<<0))
	str		r1,[r0]
	
	ldr		r0,=SMBWSTRDR5
	ldr		r1,=0xe
	str		r1,[r0]
	
	mov pc, lr
	
	
	
	LTORG	
SMRDATA    DATA
    DCD		((RASBW0<<17)+(RASBW1<<14)+(CASBW0<<11)+(CASBW1<<8)+(ADDRCFG0<<6)+(ADDRCFG1<<4)+(MEMCFG<<1)+(BW<<0))
	DCD		((DQSDelay<<28)+(1<<26)+(BStop<<7)+(WBUF<<6)+(AP<<5)+(PWRDN<<4))
	DCD		((tRAS<<20)+(tARFC<<16)+(CL<<4)+(tRCD<<2)+(tRP<<0))
    	
BaseOfROM	DCD	|Image$$RO$$Base|
TopOfROM	DCD	|Image$$RO$$Limit|
BaseOfBSS	DCD	|Image$$RW$$Base|
BaseOfZero	DCD	|Image$$ZI$$Base|
EndOfBSS	DCD	|Image$$ZI$$Limit|


	ALIGN

	AREA RamData, DATA, READWRITE

	^   _ISR_STARTADDRESS		; _ISR_STARTADDRESS=0x33FF_FF00
HandleReset 	#   4
HandleUndef 	#   4
HandleSWI		#   4
HandlePabort    #   4
HandleDabort    #   4
HandleReserved  #   4
HandleIRQ		#   4
HandleFIQ		#   4


;IntVectorTable  ;@0x33FF_FF20
HandleEINT0		#   4
HandleEINT1		#   4
HandleEINT2		#   4
HandleEINT3		#   4
HandleEINT4_7	#   4
HandleEINT8_23	#   4
HandleCAM		#   4		
HandleBATFLT	#   4
HandleTICK		#   4
HandleWDT		#   4
HandleTIMER0 	#   4
HandleTIMER1 	#   4
HandleTIMER2 	#   4
HandleTIMER3 	#   4
HandleTIMER4 	#   4
HandleUART2  	#   4
;@0x33FF_FF60
HandleLCD 		#   4
HandleDMA0		#   4
HandleDMA1		#   4
HandleDMA2		#   4
HandleDMA3		#   4
HandleMMC		#   4
HandleSPI0		#   4
HandleUART1		#   4
HandleNFCON		#   4		
HandleUSBD		#   4
HandleUSBH		#   4
HandleIIC		#   4
HandleUART0 	#   4
HandleSPI1 		#   4
HandleRTC 		#   4
HandleADC 		#   4
;@0x33FF_FFA0
	END	
	
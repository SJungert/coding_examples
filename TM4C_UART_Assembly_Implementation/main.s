;Lab 3 - Brady Riddle, Faye Mieso, and Sam Jungert
;Edit Date: 10/14/2016
	  THUMB
       AREA    DATA, ALIGN=2
       ALIGN          4
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
		   
;***********************************************************************************
;UART START
RCGCUART 	EQU 0x400FE618 ; Enable/Disable UART Modules 0 - 7
RCGCGPIO	EQU	0x400FE608 ; Enable/Disable Clock on Ports A - F
GPIOAFSEL 	EQU 0x40005420 ; Enable/Disable Alt Function Port B
GPIOPCTL	EQU 0x4000552C ; Specify Alt Functionality for PB0 and PB1 etc.
UARTCTL		EQU 0x4000D000 ; UART 1 Base Address
	
Start
;Initializing UART
;Step 1 - Enable UART module using RCGCUART register
	ldr R0, =RCGCUART
	mov R1, #0x02; Enable UART 1
	str R1, [R0] ;

;Step 2 - Enable the clock to the appropriate GPIO module via RCGCPIO
	ldr R0, =RCGCGPIO
	mov R1, #0x02; Enable Port B
	str R1, [R0] ;

;Step 3 - Set the GPIO AFSEL (Alt Functionality) for appropriate pins
	ldr R0, =GPIOAFSEL
	mov R1, #0x3 ; Enable Port B (PB0 & PB1) Alt Functionality
	str R1, [R0];

;Step 4 - Set the GPIO current level and/or slew rate for specificed mode
	;Left blank, don't know if I need this or not
	
;Step 5 - Configure the PMCn fields in the GPIOCTL register to assign UART signals
;to the appropriate pins
	ldr R0, =GPIOPCTL
	mov R1, #0x11 ; Enable Alt Functionality for PB0 and PB1
	str R1, [R0];

;UART 1 initalization complete
;PB0 -> Recieve
;PB1 -> Transmit

;UART 1 initial parameters
;Step 1 - Disable UART by clearing UARTEN bit in UARTCTL register
	ldr R0, =UARTCTL
	mov R1, #0x0; Disable UART 1
	str R1, [R0, #0x030]
	
;Step 2 - Write the integer portion of the BRD to UARTIBRD
	mov R1, #0x68 ; Write Integer value of 104
	str R1, [R0, #0x24]

;Step 3 - Write the fraction portion of the BRD to UARTFBRD
	mov R1, #0xB ;
	str R1, [R0, #0x28]

;Step 4 - Write the desired serial parameters to UARTLCRH
	mov R1, #0x70 ; Word Length 8, FIFO's Enabled
	str R1, [R0, #0x2C]
;UARTLCRH Must be written to

;Step 5 - Configure the UART clock source by writing to UARTCC
	mov R1, #0x00 ; Main System Clock is UART 1 Clock Source
	str R1, [R0, #0xFC8]

;Step 6 - Optionally conigure the microDMA channel UARTDMACTL
	;Skipping this step as it is optional

;Step 7 - Enable the UART by setting the UARTEN
	mov R1, #0x00000301 ; Enable UART 1
	str R1, [R0, #0x030]

;UART 1 parameters complete

;******************************************************************************************	
	ldr R1, =UARTCTL
	mov R0,#0x50 ;what we?ll TX
	str R0, [R1] ;
	mov R6, #0xd ; crap data
	
TX
	ldr R2,[R1,#0x18]
	ands R2,#0x20 ;0b100000 (set Z=1 if result is 0)
	bne TX ;branch if Z=0 (TXFF==1)
	;str R6, [R1] ;TX first byte of R0
	strb R0,[R1,#0x0] ;TX first byte of R0
	ror R0,#1 ;may as well rotate as
	;TX will take a while
	b TX
	b Start
       ALIGN      
       END  
       
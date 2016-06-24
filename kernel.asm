;kernel.asm
;Michael Black, 2007

;kernel.asm contains assembly functions that you can use in your kernel
	.global _putInMemory
	.global _makeInterrupt21
	.global _printChar
	.global _readChar
	.global	_readSector
	.global _writeSector
	.global _callInterrupt21
	.global _setCursorPosition
	.global _launchProgram
	.global _terminates
	.global _copyToSegment
	.global _clearScreens
	.global _setKernelDataSegment
	.global _restoreDataSegment
	.global _irqInstallHandler
	.global _timerISR
	.global _setTimerPhase
	.extern _handleInterrupt21
	.extern _handleTimerInterrupt

;void putInMemory (int segment, int address, char character)
_putInMemory:
	push bp
	mov bp,sp
	push ds
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
	pop ds
	pop bp
	ret

;void makeInterrupt21()
;this sets up the interrupt 0x21 vector
;when an interrupt 0x21 is called in the future, 
;_interrupt21ServiceRoutine will run

_makeInterrupt21:
	;get the address of the service routine
	mov dx,#_interrupt21ServiceRoutine
	push ds
	mov ax, #0	;interrupts are in lowest memory
	mov ds,ax
	mov si,#0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret

;void printChar(char ch)
_printChar:
	push bp
	mov bp, sp
	;mov bh, #0
	mov bl, #0x2
	mov ah, #0xE
	mov al, [bp+4]
	int #0x10
	pop bp
	ret

;void readChar()
_readChar:
	push bp
	mov bp, sp
	mov ah, #0x0
	int #0x16
	pop bp
	ret

;void readSector(char* buffer, int sector)
_readSector:
	push bp
	mov bp, sp
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #18
	div bx
	add dl, #1
	mov cl, dl
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #36
	div bx
	mov ch, al
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #18
	div bx
	mov dx, #0
	mov bx, #2
	div bx
	mov dh, dl
	mov ah, #2
	mov al, #1
	mov bx, [bp+4]
	mov dl, #0
	int #0x13
	pop bp
	ret
	
;void writeSector(char* buffer, int sector_number);
_writeSector:
	push bp
	mov bp, sp
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #18
	div bx
	add dl, #1
	mov cl, dl
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #36
	div bx
	mov ch, al
	mov dx, #0
	mov ax, [bp+6]
	mov bx, #18
	div bx
	mov dx, #0
	mov bx, #2
	div bx
	mov dh, dl
	mov ah, #3
	mov al, #1
	mov bx, [bp+4]
	mov dl, #0
	int #0x13
	pop bp
	ret

;void callInterrupt21 (int AX, int BX, int CX, int DX)
_callInterrupt21:
	push bp
	mov bp, sp
	mov ax, [bp+4]
	mov bx, [bp+6]
	mov cx, [bp+8]
	mov dx, [bp+10]
	int #0x21
	pop bp
	ret

;vois setCursorPosition (int row, int col)
_setCursorPosition:
	push bp
	mov bp, sp
	mov bh, #0
	mov dh, [bp+4]
	mov dl, [bp+6]
	mov ah, #2
	int #0x10
	pop bp
	ret

;void launchProgram(char* buffer, int segment)
_launchProgram:
	mov bp, sp
	mov ax, [bp+4]	;put the segment into ax
	;mov bx, [bp+2]

	;transfer file into bottom of segment
	;push ax
	;push bx
	;call _copyToSegment
	;pop bx
	;pop ax


	mov si, #jump
	mov [si+3], ax
	;set up the segment registers
	mov ds, ax
	mov ss, ax
	mov es, ax

	;let's have the stack start at ax:fff0
	;mov ax, #0xfff0
	mov ax, #0xff00
	mov sp, ax
	mov bp, ax

	;Switch to program
jump:	jmp #0x0000:#0

_terminates:
	retf

;void copyToSegment(char* buffer, int segment)
_copyToSegment:
	push bp
	mov bp, sp
	mov ax, [bp+6]	;put the segment into ax

	;transfer file into bottom of segment
	mov es, ax
	mov si, [bp+4]
	mov di, #0
	mov cx, #13312
	cld

repeat:
	cmp cx, #0
	je epilogue
	movsb
	dec cx
	jmp repeat

epilogue:
	pop bp
	ret

_clearScreens:
	mov ah, #6
	mov al, #0
	mov bh, #7
	mov ch, #0
	mov cl, #0
	mov dh, #25
	mov dl, #80
	int #0x10
	ret

;void setKernelDataSegment()
;sets the data segment to the kernel, saving the current ds on the stack
_setKernelDataSegment:
    pop bx
    push ds
    push bx
    mov ax,#0x1000
    mov ds,ax
    ret

;void restoreDataSegment()
;restores the data segment
_restoreDataSegment:
    pop bx
    pop ds
    push bx
    ret

; void irqInstallHandler(int irq_number, void (*fn)())
; Install an IRQ handler
_irqInstallHandler:
	cli

	push bp
	mov bp, sp
	push si 
	push ds

	;mov dx, [bp+6] 				;Function pointer
	mov dx, #_timerISR
	xor ax, ax 
	mov ds, ax 					;Interrupt vector is at lowest memory
	mov si, [bp+4]
	shl si, 2					;ax = irq_handler * 4

	mov ax, cs
	mov [si + 2], ax
	mov [si], dx

	pop ds
	pop si
	pop bp

	sti
	ret

;void timerISR()
_timerISR:
	;disable interrupts
	cli

	;save all regs for the old process on the old process's stack
	push bx
	push cx
	push dx
	push si
	push di
	push bp
	push ax
	push ds
	push es

	;reset interrupt controller so it performs more interrupts
	mov al, #0x20
	out #0x20, al

	;switch to kernel data segment
	mov ax, #0x1000
	mov ds, ax

	push sp
	call _handleTimerInterrupt
	pop sp

	mov si, ax
	mov ss, [si+4]
	mov sp, [si+2]

	;reload context
	pop es
	pop ds
	pop ax
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx

	iret

; void setTimerPhase(int hz) 
;Set the timer frequency in Hertz
_setTimerPhase:
	push bp
	mov bp, sp
	mov dx, #0x0012 			;Default frequency of the timer is 1,193,180 Hz
	mov ax, #0x34DC
	mov bx, [bp+4]
	div bx

	mov bx, ax 					;Save quotient

	mov dx, #0x43 
	mov	al, #0x36
	out dx, al					;Set our command byte 0x36

	mov dx, #0x40
	mov al, bl
	out dx, al 					;Set low byte of divisor 
	mov al, bh 
	out	dx, al					;Set high byte of divisor

	pop bp
	ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	call _handleInterrupt21
	mov bx, ax
	pop ax
	mov ax, bx
	pop bx
	pop cx
	pop dx

	iret

;------------------------------------------------------------------------MY FUNCTIONS---------------------------------------------------------------


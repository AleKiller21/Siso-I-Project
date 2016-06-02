;kernel.asm
;Michael Black, 2007

;kernel.asm contains assembly functions that you can use in your kernel
	.global _putInMemory
	.global _interrupt
	.global _makeInterrupt21
	.global _printChar
	.global _readChar
	.global	_readSector
	.global _callInterrupt21
	.global _setCursorPosition
	.global _launchProgram
	.global _terminates
	.extern _handleInterrupt21

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

;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,#intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:	int #0x00	;call the interrupt (00 will be changed above)

	mov ah,#0	;we only want AL returned
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

	;transfer file into bottom of segment
	mov es, ax
	mov si, [bp+2]
	mov di, #0
	mov cx, #13312
	cld

repeat:
	cmp cx, #0
	je body
	movsb
	dec cx
	jmp repeat

body:
	mov si, #jump
	mov [si+3], ax
	;set up the segment registers
	mov ds, ax
	mov ss, ax
	mov es, ax

	;let's have the stack start at ax:fff0
	mov ax, #0xfff0
	mov sp, ax
	mov bp, ax

	;Switch to program
jump:	jmp #0x0000:#0
	;retf

_terminates:
	retf

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx

	iret

;------------------------------------------------------------------------MY FUNCTIONS---------------------------------------------------------------


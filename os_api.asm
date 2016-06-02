;kernel.asm
;Michael Black, 2007

;kernel.asm contains assembly functions that you can use in your kernel
	.global _callInterrupt21
	.global _syscall_printString
	.global _syscall_readString
	;.global _syscall_readSector
	.global _syscall_readFile
	.global _syscall_executeProgram
	.global _syscall_terminate
	.global _syscall_clearScreen
	.global _syscall_setCursorPosition
	.global _launchProgram


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

;void printString(char *buffer);
_syscall_printString:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, [bp+4]
	mov ax, #0
	;push #0
	;push #0
	;push bx
	;push #0
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

;void readString(char* buffer)
_syscall_readString:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, [bp+4]
	mov ax, #1
	;push #0
	;push #0
	;push bx
	;push #1
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

;void readFile(char* file, char* buffer)
_syscall_readFile:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, [bp+6]
	mov bx, [bp+4]
	mov ax, #2
	;push #0
	;push cx
	;push bx
	;push #2
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

;void executeProgram(char* name, int segment)
_syscall_executeProgram:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, [bp+6]
	mov bx, [bp+4]
	mov ax, #3
	;push #0
	;push cx
	;push bx
	;push #3
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

_syscall_terminate:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, #0
	mov ax, #4
	;push #0
	;push cx
	;push bx
	;push #3
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

_syscall_clearScreen:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, #0
	mov ax, #5
	;push #0
	;push cx
	;push bx
	;push #3
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
	pop bp
	ret

;vois setCursorPosition (int row, int col)
_syscall_setCursorPosition:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, [bp+6]
	mov bx, [bp+4]
	mov ax, #6
	;push #0
	;push cx
	;push bx
	;push #6
	;call _callInterrupt21
	int #0x21
	;pop ax
	;pop bx
	;pop cx
	;pop dx
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
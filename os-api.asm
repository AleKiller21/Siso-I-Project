
.global _syscall_printString
.global _syscall_readString
.global _syscall_readFile
.global _syscall_executeProgram
.global _syscall_clearScreen
.global _syscall_terminate
.global _syscall_setCursorPosition
.global _syscall_list_files

;void printString(char *buffer);
_syscall_printString:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, [bp+4]
	mov ax, #0
	int #0x21
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
	int #0x21
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
	int #0x21
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
	int #0x21
	pop bp
	ret

_syscall_clearScreen:
	mov dx, #0
	mov cx, #0
	mov bx, #0
	mov ax, #4
	int #0x21
	ret

_syscall_terminate:
	mov dx, #0
	mov cx, #0
	mov bx, #0
	mov ax, #5
	int #0x21
	ret

;void setCursorPosition (int row, int col)
_syscall_setCursorPosition:
	push bp
	mov bp, sp
	mov ax, #6
	mov bx, [bp+4]
	mov cx, [bp+6]
	mov dx, #0
	int #0x21
	pop bp
	ret

;void list_files(char** buffer);
_syscall_list_files:
	push bp
	mov bp, sp
	mov dx, #0
	mov cx, #0
	mov bx, [bp+4]
	mov ax, #7
	int #0x21
	pop bp
	ret

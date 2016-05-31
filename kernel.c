

void handleInterrupt21 (int ax, int bx, int cx, int dx);

int main()
{
    char buffer[13312];

    makeInterrupt21();
    /*callInterrupt21(4, "tstpr2", 0x2000, 0);*/
    syscall_executeProgram("shell", 0x2000);
    /*syscall_clearScreen();*/
    while(1);
}

void handleInterrupt21 (int ax, int bx, int cx, int dx)
{
    switch(ax)
    {
        case 0:
            syscall_printString("Hola\0");
            break;

        case 1:
            syscall_printString("Enter a line: \0");
            syscall_readString(bx);
            syscall_printString(bx);
            break;

        case 2:
            syscall_readSector(bx, cx);
            break;

        case 3:
            syscall_readFile(bx, cx);
            syscall_printString(cx);
            break;

        case 4:
            syscall_executeProgram(bx, cx);
            break;

        case 5:
            syscall_terminate();
            break;

        default:
            syscall_printString("The value in AX is unrecognized!");
    }
}
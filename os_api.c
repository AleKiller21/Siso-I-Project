
void syscall_printString(char *buffer);
void syscall_printString(char *buffer);
void syscall_readSector(char *buffer, int sector);
void syscall_setCursorPosition(int row, int col);

void syscall_printString(char* buffer)
{
	int i = 0;

    while(buffer[i] != '\0')
    {
        printChar(buffer[i]);
        i += 1;
    }
}

void syscall_readString(char* buffer)
{
	int i = 0;
    char key_pressed;

    while(1)
    {
        key_pressed = readChar();

        if(key_pressed == 0xD || i == 78)
        {
            buffer[i] = '\0';
            return;
        }

        if(key_pressed == 0x8)
        {
            printChar(key_pressed);
            i -= 1;
            printChar(0x0);
            printChar(key_pressed);
            continue;
        }

        buffer[i] = key_pressed;

        printChar(buffer[i]);
        
        i += 1;
    }
}

void syscall_readSector(char* buffer, int sector)
{
	readSector(buffer, sector);
	syscall_printString(buffer);
}

void syscall_setCursorPosition(int row, int col)
{
    setCursorPosition(0, 0);
}
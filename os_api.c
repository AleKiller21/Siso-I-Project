
void syscall_printString(char *buffer);
void syscall_readString(char *buffer);
void syscall_readSector(char *buffer, int sector);
void syscall_readFile(char* file, char* buffer);
void syscall_executeProgram(char* name, int segment);
void syscall_terminate();
void syscall_setCursorPosition(int row, int col);
void syscall_clearScreen();

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
	/*syscall_printString(buffer);*/
}

void syscall_readFile(char* file, char* buffer)
{
    char directory[512];
    int entry;
    int i_name;
    int i_sector;
    int found = 0;

    syscall_readSector(directory, 2);

    for(entry = 0; entry < 512; entry += 32)
    {
        for(i_name = 0; i_name  < 6; i_name += 1)
        {
            if(directory[i_name + entry] != file[i_name])
            {
                found = 0;
                break;
            }

            if(i_name == 5)
                found = 1;            
        }

        if(found == 0)
            continue;

        for(i_sector = 6; i_sector < 32; i_sector += 1)
        {
            if(directory[i_sector + entry] == 0)
            {
                syscall_printString("Name found!\0");
                return;
            }

            syscall_readSector(buffer, directory[i_sector + entry]);
            buffer += 512;
        }

        if(found)
        {
            syscall_printString("Name found!\0");
            return;
        }
    }

    syscall_printString("No such file exists!\0");
    return;
}

void syscall_executeProgram(char* name, int segment)
{
    char buffer[13312];
    syscall_readFile(name, buffer);
    launchProgram(buffer, segment);
}

void syscall_terminate()
{
    syscall_printString("Termino");
    while(1);
}

void syscall_setCursorPosition(int row, int col)
{
    setCursorPosition(0, 0);
}

void syscall_clearScreen()
{
    int row, column;

    for(row = 0; row < 25; row = row + 1)
    {
        for(column = 0; column < 80; column+= 1)
        {
            putInMemory(0xB * 0x1000, calculate_address(row+1, column+1) & 0x0ffff, 0x0);
        }
    }
}
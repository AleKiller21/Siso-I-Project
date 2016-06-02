
void handleInterrupt21 (int ax, int bx, int cx, int dx);
void printString(char *buffer);
void readString(char *buffer);
void executeProgram(char* name, int segment);
/*void terminate();*/
void clearScreen();
void clear_buffer(char* buffer, int size);
void list_files(char* buffer);

int calculate_address(int row, int column);
int search_program(char* file, char* buffer);
int readFile(char* file, char* buffer);

int main()
{
    makeInterrupt21();
    executeProgram("shell\0", 0x2000);
    while(1);
}

void handleInterrupt21 (int ax, int bx, int cx, int dx)
{
    switch(ax)
    {
        case 0:
            printString(bx);
            break;

        case 1:
            readString(bx);
            break;

        case 2:
            readFile(bx, cx);
            break;

        case 3:
            executeProgram(bx, cx);
            break;

        case 4:
            clearScreen();
            break;

        case 5:
            terminates();
            break;

        case 6:
            setCursorPosition(bx, cx);
            break;

        case 7:
            list_files(bx);
            break;

        default:
            printString("The value in AX is unrecognized!\0");
    }
}

void printString(char* buffer)
{
    int i = 0;

    while(buffer[i] != '\0')
    {
        printChar(buffer[i]);
        i += 1;
    }
}

void readString(char* buffer)
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

int search_program(char* file, char* buffer)
{
    if(readFile(file, buffer) == 1)
    {
        /*printString("File not found!\0");*/
        return 1;
    }

    return 0;
}

int readFile(char* file, char* buffer)
{
    char directory[512];
    int entry;
    int i_name;
    int i_sector;
    int found = 1;

    readSector(directory, 2);

    for(entry = 0; entry < 512; entry += 32)
    {
        for(i_name = 0; i_name  < 6; i_name += 1)
        {
            if(directory[i_name + entry] != file[i_name])
            {
                found = 1;
                break;
            }

            if(i_name == 5 && file[i_name + 1] == '\0')
                found = 0;            
        }

        if(found == 1)
            continue;

        for(i_sector = 6; i_sector < 32; i_sector += 1)
        {
            if(directory[i_sector + entry] == 0)
            {
                return 0;
            }

            readSector(buffer, directory[i_sector + entry]);
            buffer += 512;
        }

        if(found == 0)
        {
            return 0;
        }
    }

    return 1;
}

void executeProgram(char* name, int segment)
{
    char buffer[13312];

    clear_buffer(buffer, 13312);

    if(search_program(name, buffer) == 0)
    {
        launchProgram(buffer, segment);
    }
}

/*void terminate()
{
    char* shell = "shell\0";
    printChar(0x21);
    executeProgram(shell, 0x2000);
}*/

void clearScreen()
{
    int row, column;

    for(row = 0; row < 25; row = row + 1)
    {
        for(column = 0; column < 80; column+= 1)
        {
            printChar(0xa);
        }
    }
}

void clear_buffer(char* buffer, int size)
{
    int i;

    for(i = 0; i < size; i+= 1)
    {
        buffer[i] = '\0';
    }
}

void list_files(char* buffer)
{
    char directory[512];
    int entry;
    int i_name; int i;

    int iterator = 0;

    readSector(directory, 2);

    for(entry = 0; entry < 512; entry += 32)
    {
        if(directory[entry] == 0x0 || directory[entry] == 0)
        {
            /*printChar('Z');*/
            /*printString(buffer[0]);*/
            break;
        }

        for(i_name = 0; i_name < 6; i_name += 1)
        {
            

            buffer[iterator] = directory[entry + i_name];
            iterator += 1;
        }
    }

    /*for(i = 0; i < 16; i += 1)
    {
        printString(buffer[i]);
    }*/
}

int calculate_address(int row, int column)
{
    int address;
    int base_address = 0xB8000;

    address = 80 * (row-1);
    address = address + (column-1);
    address = address * 2;
    address = address + base_address;

    return address;
}
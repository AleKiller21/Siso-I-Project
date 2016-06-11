
#define PROGRAM_SIZE    13312
#define SECTOR_SIZE     512


void handleInterrupt21 (int ax, int bx, int cx, int dx);
void printString(char *buffer);
void readString(char *buffer, int os_name_length);
void executeProgram(char* name, int segment);
void clear_buffer(char* buffer, int size);
void list_files(char* buffer);
void delete_file(char* name);
void write_file(char* name, char* buffer, int size);

/*int calculate_address(int row, int column);*/
int search_file(char* file, char* buffer);
int search(char* directory, char* name);
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
            readString(bx, cx);
            break;

        case 2:
            readFile(bx, cx);
            break;

        case 3:
            executeProgram(bx, cx);
            break;

        case 4:
            clearScreens();
            break;

        case 5:
            terminates();
            break;

        case 6:
            writeSector(bx, cx);
            break;

        case 7:
            delete_file(bx);
            break;

        case 8:
            write_file(bx, cx, dx);
            break;

        case 9:
            setCursorPosition(bx, cx);
            break;

        case 10:
            list_files(bx);
            break;
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

void readString(char* buffer, int os_name_length)
{
    char key_pressed;
    
    int i = 0;
    int cursor_x = os_name_length + 2;
    int limit_to_erase = os_name_length + 2;

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
            if(cursor_x == limit_to_erase) continue;

            printChar(key_pressed);
            i -= 1;
            cursor_x -= 1;
            printChar(0x0);
            printChar(key_pressed);
            continue;
        }

        buffer[i] = key_pressed;

        printChar(buffer[i]);
        
        cursor_x += 1;
        i += 1;
    }
}

void executeProgram(char* name, int segment)
{
    char buffer[PROGRAM_SIZE];

    clear_buffer(buffer, PROGRAM_SIZE);

    if(search_file(name, buffer) == 0)
    {
        launchProgram(buffer, segment);
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
    char directory[SECTOR_SIZE];
    int entry;
    int i_name;

    int iterator = 0;

    readSector(directory, 2);

    for(entry = 0; entry < SECTOR_SIZE; entry += 32)
    {
        if(directory[entry] == 0x0 || directory[entry] == 0) continue;

        for(i_name = 0; i_name < 6; i_name += 1)
        {          
            buffer[iterator] = directory[entry + i_name];
            iterator += 1;
        }
    }
}

void delete_file(char* name)
{
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];
    int entry;
    int i_name;
    int i_sector;

    int found = 0;
    int delete = 0;

    readSector(map, 1);
    readSector(directory, 2);

    for(entry = 0; entry < 512; entry += 32)
    {
        if(delete == 1) break;

        for(i_name = 0; i_name < 6; i_name += 1)
        {
            if(directory[entry + i_name] != name[i_name]) break;

            if(i_name == 5 || directory[i_name + 1] == 0x0)
            {
                directory[entry] = 0x0;
                found = 1;
                break;
            }
        }

        if(found == 0) continue;

        for(i_sector = 6; i_sector < 32; i_sector += 1)
        {
            int sector = directory[entry + i_sector];

            if(sector == 0x0)
            {
                delete = 1;
                break;
            }

            map[sector] = 0x0;
        }
    }

    writeSector(map, 1);
    writeSector(directory, 2);
}

void write_file(char* name, char* buffer, int size)
{
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];

    readSector(map, 1);
    readSector(directory, 2);
}

int search_file(char* file, char* buffer)
{
    if(readFile(file, buffer) == 1)
    {
        return 1;
    }

    return 0;
}

int search(char* directory, char* name)
{
    int found = 0;

    for(entry = 0; entry < 512; entry += 32)
    {
        for(i_name = 0; i_name < 6; i_name += 1)
        {
            if(directory[entry + i_name] != name[i_name]) break;

            if(i_name == 5 || directory[i_name + 1] == 0x0)
            {
                found = 1;
                break;
            }
        }

        if(found == 1) return entry;
    }

    return 0x0;
}

int readFile(char* file, char* buffer)
{
    char directory[SECTOR_SIZE];
    int entry;
    int i_name;
    int i_sector;

    int found = 1;

    readSector(directory, 2);

    for(entry = 0; entry < SECTOR_SIZE; entry += 32)
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
            buffer += SECTOR_SIZE;
        }

        if(found == 0)
        {
            return 0;
        }
    }

    return 1;
}



#define PROGRAM_SIZE    13312
#define SECTOR_SIZE     512
#define MAX_SECTORS     26


void handleInterrupt21 (int ax, int bx, int cx, int dx);

void printString(char *buffer);
void readString(char *buffer, int os_name_length);
void list_files(char* buffer);
int executeProgram(char* name, int segment);
int delete_file(char* name);
int get_size_as_sectors(char* name);
int write_file(char* name, char* buffer, int size);
int readFile(char* file, char* buffer);

int search_file(char* file, char* buffer);
int search_directory_entry(char* directory, char* name);
int search_map_sector(char* map);
void clear_buffer(char* buffer, int size);
void mem_copy(char* origin, char* destiny, int bytes);
void set_name_directory_entry(char* directory, char* name);

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
            return executeProgram(bx, cx);

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
            return delete_file(bx);

        case 8:
            return write_file(bx, cx, dx);

        case 9:
            setCursorPosition(bx, cx);
            break;

        case 10:
            list_files(bx);
            break;

        case 11:
            return get_size_as_sectors(bx);
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

int executeProgram(char* name, int segment)
{
    char buffer[PROGRAM_SIZE];

    clear_buffer(buffer, PROGRAM_SIZE);

    if(search_file(name, buffer) == 1)
    {
        launchProgram(buffer, segment);
        return 1;
    }

    else
        return -1;
}

int delete_file(char* name)
{
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];
    int entry;
    int i_name;
    int i_sector;

    readSector(map, 1);
    readSector(directory, 2);

    entry = search_directory_entry(directory, name);

    if(entry == -1) return -1;

    directory[entry] = 0x0;

    for(i_sector = 6; i_sector < 32; i_sector += 1)
    {
        int sector = directory[entry + i_sector];

        if(sector == 0x0)
        {
            break;
        }

        map[sector] = 0x0;
    }

    writeSector(map, 1);
    writeSector(directory, 2);

    return 0;
}

int get_size_as_sectors(char* name)
{
    char directory[SECTOR_SIZE];
    int entry;
    int counter;
    int i_sector;

    readSector(directory, 2);
    counter = 0;

    entry = search_directory_entry(directory, name);

    if(entry == -1) return -1;

    for(i_sector = 6; i_sector < 32; i_sector += 1)
    {
        if(directory[entry + i_sector] == 0x00) break;

        counter += 1;
    }

    return counter;
}

int write_file(char* name, char* file, int size)
{
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];
    char buffer[SECTOR_SIZE];
    char free_sectors[MAX_SECTORS];
    int i_sector;
    int i_buffer;
    int i_map;
    int entry;
    int sector;

    int sectors = size / SECTOR_SIZE;

    if(size % SECTOR_SIZE != 0)
    {
        sectors += 1;
    }

    readSector(map, 1);
    readSector(directory, 2);
    clear_buffer(buffer, SECTOR_SIZE);
    clear_buffer(free_sectors, MAX_SECTORS);

    entry = search_directory_entry(directory, 0x00);

    if(entry == -1) return -1;

    for(i_sector = 0; i_sector < 6; i_sector += 1)
    {
        if(name[i_sector] == '\0') break;

        directory[entry + i_sector] = name[i_sector];
    }

    for(; i_sector < 6; i_sector += 1)
        directory[entry + i_sector] = 0x00;

    for(i_map = 0; i_map < sectors; i_map += 1)
    {
        sector = search_map_sector(map);

        if(sector == -1) return -1;

        map[sector] = 0xFF;

        free_sectors[i_map] = sector;
    }

    i_map = 0;

    for(i_buffer = 0; i_buffer < size; i_buffer += 512)
    {
        /*if(file[i_buffer] == '\0' || file[i_buffer] == 0x00) break;*/
        
        directory[entry + i_sector] = free_sectors[i_map];
        mem_copy(file + i_buffer, buffer, SECTOR_SIZE);
        writeSector(buffer, free_sectors[i_map]);
        clear_buffer(buffer, SECTOR_SIZE);

        i_sector += 1;
        i_map += 1;
    }

    for(; i_sector < 32; i_sector += 1)
        directory[entry + i_sector] = 0x00;

    writeSector(map, 1);
    writeSector(directory, 2);
}

void set_name_directory_entry(char* directory, char* name)
{
    int i_sector = 0;
}

int readFile(char* file, char* buffer)
{
    char directory[SECTOR_SIZE];
    int entry;
    int i_sector;

    readSector(directory, 2);

    entry = search_directory_entry(directory, file);

    if(entry == -1) return -1;

    for(i_sector = 6; i_sector < 32; i_sector += 1)
    {
        if(directory[i_sector + entry] == 0x0)
        {
            return 1;
        }

        readSector(buffer, directory[i_sector + entry]);
        buffer += SECTOR_SIZE;
    }

    return 1;
}

int search_file(char* file, char* buffer)
{
    if(readFile(file, buffer) == 1)
    {
        return 1;
    }

    return -1;
}

int search_directory_entry(char* directory, char* name)
{
    int entry;
    int i_name;

    int found = 0;

    for(entry = 0; entry < 512; entry += 32)
    {
        for(i_name = 0; i_name < 6; i_name += 1)
        {
            if(name == 0x00 && directory[entry] == 0x00)
            {
                found = 1;
                break;
            }

            if(name == 0x00) break;

            if(directory[entry + i_name] != name[i_name]) break;

            if((i_name == 5 && name[i_name + 1] == '\0') || directory[i_name + 1] == 0x00)
            {
                found = 1;
                break;
            }
        }

        if(found == 1) return entry;
    }

    return -1;
}

int search_map_sector(char* map)
{
    int sector;

    for(sector = 0; sector < 512; sector += 1)
    {
        if(map[sector] == 0x00)
            return sector;
    }

    return -1;
}

void clear_buffer(char* buffer, int size)
{
    int i;

    for(i = 0; i < size; i+= 1)
    {
        buffer[i] = '\0';
    }
}

void mem_copy(char* origin, char* destiny, int bytes)
{
    int i;

    for(i = 0; i < bytes; i += 1)
    {
        /*if(origin[i] == 0x40) break;*/
        destiny[i] = origin[i];
    }
}




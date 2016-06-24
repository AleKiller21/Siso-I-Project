
#define PROGRAM_SIZE    13312
#define SECTOR_SIZE     512
#define MAX_SECTORS     26
#define PROCESS_ENTRIES 8

int search_file(char* file, char* buffer);
int search_directory_entry(char* directory, char* name);
int search_map_sector(char* map);
int search_free_sectors(int sectors, char* map, char* free_sectors);
void write_file_to_disk(char* directory, char* free_sectors, char* original_file, char* buffer, int entry, int size);
void set_name_directory_entry(char* directory, char* name, int entry);
void clear_buffer(char* buffer, int size);
void mem_copy(char* origin, char* destiny, int bytes);

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

int search_free_sectors(int sectors, char* map, char* free_sectors)
{
    int i_map;
    int sector;

    for(i_map = 0; i_map < sectors; i_map += 1)
    {
        sector = search_map_sector(map);

        if(sector == -1) return -1;

        map[sector] = 0xFF;

        free_sectors[i_map] = sector;
    }

    return 1;
}

void write_file_to_disk(char* directory, char* free_sectors, char* original_file, char* buffer, int entry, int size)
{
    int i_buffer;
    int i_sector = 6;
    int i_map = 0;

    for(i_buffer = 0; i_buffer < size; i_buffer += 512)
    {        
        directory[entry + i_sector] = free_sectors[i_map];
        mem_copy(original_file + i_buffer, buffer, SECTOR_SIZE);
        writeSector(buffer, free_sectors[i_map]);
        clear_buffer(buffer, SECTOR_SIZE);

        i_sector += 1;
        i_map += 1;
    }

    for(; i_sector < 32; i_sector += 1)
        directory[entry + i_sector] = 0x00;
}

void set_name_directory_entry(char* directory, char* name, int entry)
{
    int i_sector = 0;

    for(i_sector = 0; i_sector < 6; i_sector += 1)
    {
        if(name[i_sector] == '\0') break;

        directory[entry + i_sector] = name[i_sector];
    }

    for(; i_sector < 6; i_sector += 1)
        directory[entry + i_sector] = 0x00;
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
        destiny[i] = origin[i];
    }
}


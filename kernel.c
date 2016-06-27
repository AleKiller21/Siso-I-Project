
#define PROGRAM_SIZE    13312
#define SECTOR_SIZE     512
#define MAX_SECTORS     26
#define PROCESS_ENTRIES 8
#define PROCESS_READY   1
#define PROCESS_WAITING 2
#define PROCESS_RUNNING 3
#define PROCESS_DEAD    4


void initialize_process_queue();
void initialize_program(int segment);
void terminate_process();
void kill_process(int process_id);
void activate_waiting_process();
void execute_w(char* name);
void list_process(int** processes, int* total_process);
int modulo(int numerator, int denominator);
void get_digits(int sectors, int* digits);
void print_numbers(int* digits, int size_array);

int schedule_process();

struct process_found* handleTimerInterrupt(int sp);
void handleInterrupt21 (int ax, int bx, int cx, int dx);

void printString(char *buffer);
void readString(char *buffer, int os_name_length);
void list_files(char* buffer);
int executeProgram(char* name);
int delete_file(char* name);
int get_size_as_sectors(char* name);
int write_file(char* name, char* buffer, int size);
int readFile(char* file, char* buffer);

struct PCB
{
    unsigned int status;
    unsigned int sp;
    unsigned int segment;
    struct PCB* waiter;
};

struct Regs {
    unsigned int es;
    unsigned int ds;
    unsigned int ax;
    unsigned int bp;
    unsigned int di;
    unsigned int si;
    unsigned int dx;
    unsigned int cx;
    unsigned int bx;
    unsigned int ip;
    unsigned int cs;
    unsigned int flags;
};

struct process_found
{
    int process_available;
    unsigned int sp;
    unsigned int segment;
};

int counter;
int i_current_process;
int new_process_entry;
struct PCB process_queue[PROCESS_ENTRIES];
struct PCB* current_process;
struct process_found process;

int main()
{
    counter = 0;
    i_current_process = -1;

    makeInterrupt21();
    initialize_process_queue();
    executeProgram("shell\0");
    irqInstallHandler(0x8);
    setTimerPhase(100);
    /*executeProgram("shell\0");*/
    while(1);
}

void initialize_process_queue()
{
    int i;

    int segment = 0x2000;
    current_process = 0;

    for(i = 0; i < PROCESS_ENTRIES; i += 1)
    {
        process_queue[i].status = PROCESS_DEAD;
        process_queue[i].sp = 0xff00;
        process_queue[i].segment = segment;
        process_queue[i].waiter = 0;

        segment += 0x1000;
    }
}

void initialize_program(int segment)
{
    struct Regs context;

    context.es = segment;
    context.ds = segment;
    context.ax = 0;
    context.bp = 0;
    context.di = 0;
    context.si = 0;
    context.dx = 0;
    context.cx = 0;
    context.bx = 0;
    context.ip = 0;
    context.cs = segment;
    context.flags = 0x0200;

    copyToSegment(&context, segment, 0xff00, 24);
    /*copyToSegment(&context, ((segment * 0x10) + 0xff00) - 0x18); /*Save at the top of the stack*/
    /*copyToSegment(&context, segment);*/
}

void terminate_process()
{
    setKernelDataSegment();
    current_process->status = PROCESS_DEAD;
    activate_waiting_process();
    restoreDataSegment();
    enableInterrupts();
    while(1);
}

void kill_process(int process_id)
{
    setKernelDataSegment();
    
    if(process_queue[process_id - 1].status == PROCESS_DEAD)
    {
        printString("That process is already dead \0");
        return;
    }
    
    process_queue[process_id - 1].status = PROCESS_DEAD;
    activate_waiting_process();

    restoreDataSegment();
    /*enableInterrupts();*/
    /*while(1); printChar('Z');*/
}

void activate_waiting_process()
{
    int i;

    for(i = 0; i < PROCESS_ENTRIES; i += 1)
    {
        if(process_queue[i].waiter != current_process) continue;

        process_queue[i].waiter = 0;
        process_queue[i].status = PROCESS_READY;
        /*printChar(process_queue[i].status + 48);*/
        break;
    }
}

void execute_w(char* name)
{
    executeProgram(name);

    setKernelDataSegment();
    if(current_process != 0)
    {
        current_process->status = PROCESS_WAITING;
        current_process->waiter = &process_queue[new_process_entry];
    }
    restoreDataSegment();
}

void list_process(int** processes, int row)
{
    int i;
    int x;

    setKernelDataSegment();
    for(i = 0; i < PROCESS_ENTRIES; i += 1)
    {
        int digits[5];

        for(x = 0; x < 5; x += 1)
            digits[x] = 0;

        get_digits(process_queue[i].segment, digits);
        printString("\r\nProceso - \0");
        printChar(i + 48);
        printString(" State - \0");
        printChar(process_queue[i].status + 48);
        printString(" Segment - \0");
        print_numbers(digits, 5);
    }

    restoreDataSegment();
}

void get_digits(int sectors, int* digits)
{
    int i;
    int size;

    size = sectors;

    for(i = 4; i >= 0; i -= 1)
    {
        digits[i] = modulo(size, 10);
        size /= 10;
    }
}

int modulo(int numerator, int denominator)
{
    int residuo = numerator;

    while(residuo - denominator >= 0)
    {
        residuo -= denominator;
    }

    return residuo;
}

void print_numbers(int* digits, int size_array)
{
    char num[2];
    int i_digits;

    num[1] = '\0';

    for(i_digits = 0; i_digits < size_array; i_digits += 1)
    {           
        num[0] = (char)digits[i_digits];
        num[0] += 48;
        printString(num);
    }
}

struct process_found* handleTimerInterrupt(unsigned int sp)
{
    counter += 1;
    if(current_process != 0) current_process->sp = sp;
    /*printString("Tic\0");*/

    if(counter == 1000)
    {        
        counter = 0;
    }

    schedule_process();
    /*printString("Salio\0");*/

    process.process_available = 1;
    process.sp = current_process->sp;
    process.segment = current_process->segment;

    
    return &process;
}

int schedule_process()
{
    int i;

    for(i = i_current_process + 1; i < PROCESS_ENTRIES + 1; i += 1)
    {
        if(i == 8)
        {
            i = -1;
            continue;
        }

        if(current_process != 0 && current_process->status == PROCESS_RUNNING) current_process->status = PROCESS_READY;
        /*printChar(i_current_process + 48);*/

        if(process_queue[i].status != PROCESS_READY) continue;

        process_queue[i].status = PROCESS_RUNNING;
        current_process = &process_queue[i];
        i_current_process = i;
        /*printChar(i_current_process + 48);*/

        return 1;
    }
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
            return executeProgram(bx);

        case 4:
            clearScreens();
            break;

        case 5:
            terminate_process();
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

        case 12:
            kill_process(bx);
            break;

        case 13:
            execute_w(bx);
            break;

        case 14:
            list_process(bx, cx);
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

int executeProgram(char* name)
{
    char buffer[PROGRAM_SIZE];
    struct PCB running_process;
    int i;

    int free_segment = 0;

    clear_buffer(buffer, PROGRAM_SIZE);

    if(search_file(name, buffer) == -1) return -1;

    setKernelDataSegment();

    for(i = 0; i < PROCESS_ENTRIES; i += 1)
    {
        if(process_queue[i].status == PROCESS_DEAD) free_segment = 1;

        if(free_segment != 1) continue;

        /*printChar(i + 48);*/
        /*process_queue[i].status = PROCESS_READY;*/
        running_process = process_queue[i];
        new_process_entry = i;
        
        break;
    }

    restoreDataSegment();

    if(free_segment == 0) return;

    copyToSegment(buffer, running_process.segment, 0, 13312);
    initialize_program(running_process.segment);

    setKernelDataSegment();
    process_queue[new_process_entry].status = PROCESS_READY;
    restoreDataSegment();
    
    return 1;
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

    set_name_directory_entry(directory, name, entry);

    if(search_free_sectors(sectors, map, free_sectors) == -1) return -1;

    write_file_to_disk(directory, free_sectors, file, buffer, entry, size);
    writeSector(map, 1);
    writeSector(directory, 2);

    return 1;
}

int readFile(char* file, char* buffer)
{
    char directory[SECTOR_SIZE];
    int entry;
    int i_sector;

    int c = 1;

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


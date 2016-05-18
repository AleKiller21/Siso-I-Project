#define BLUE            0x1
#define GREEN           0x2
#define CYAN            0x3
#define RED             0x4
#define MAGENTA         0x5
#define BROWN           0x6
#define LIGHT_GRAY      0x7
#define DARK_GRAY       0x8
#define LIGHT_BLUE      0x9
#define LIGHT_GREEN     0xA
#define LIGHT_CYAN      0xB
#define LIGHT_RED       0xC
#define LIGHT_MAGENTA   0xD
#define YELLOW          0xE
#define WHITE           0xF

#define COLUMNS         80
#define ROWS            25


void print_text(char* word, int row, int column, char foreground_color);
void clear_screen();
void print_details();
void print_introduction();
void print_hello_world();
void set_infinite_loop();
void init_kernel();
int calculate_address(int row, int column);

void handleInterrupt21 (int ax, int bx, int cx, int dx);

int main()
{
    init_kernel();
}

void init_kernel()
{
    char buffer[80];

    clear_screen();
    syscall_setCursorPosition(0, 0);
    makeInterrupt21();
    callInterrupt21(1, buffer, 0, 0);
    set_infinite_loop();
}

void clear_screen()
{
    int row, column;

    for(row = 0; row < ROWS; row = row + 1)
    {
        for(column = 0; column < COLUMNS; column+= 1)
        {
            putInMemory(0xB * 0x1000, calculate_address(row+1, column+1) & 0x0ffff, 0x0);
        }
    }
}

void set_infinite_loop()
{
    while(1)
    {

    }
}

void print_details()
{
    print_introduction();
    print_hello_world();
}

void print_introduction()
{
    print_text("SISTEMAS OPERATIVOS I\0", 1, 27, 0x7);
    print_text("UNIVERSIDAD TECNOLOGICA CENTROAMERICANA (UNITEC)\0", 2, 13, 0x7);
    print_text("21351064 - Alejandro Ferrera", 3, 26, 0x7);
    print_text("MI PRIMER SISTEMA OPERATIVO", 4, 21, 0x7);
}

void print_hello_world()
{
    print_text("--------------------------------------------------------------------------------\0", 5, 1, LIGHT_GRAY);
    print_text("******************************** Hola Mundo!************************************\0", 6, 1, BLUE);            /*Spanish*/
    print_text("******************************** Hello World!***********************************\0", 7, 1, 0x1F);           /*English*/
    print_text("******************************** Kaixo Mundua!**********************************\0", 8, 1, CYAN);            /*Basque*/
    print_text("******************************** Moni Dziko Lapansi!****************************\0", 9, 1, MAGENTA);         /*Chichewa*/
    print_text("******************************** Bonghjornu Monde!******************************\0", 10, 1, BROWN);          /*Corsican*/
    print_text("******************************** Bok svijete!***********************************\0", 11, 1, LIGHT_GRAY);     /*Croatian*/
    print_text("******************************** Hej Verden!************************************\0", 12, 1, DARK_GRAY);      /*Danish*/
    print_text("******************************** Hallo Wereld!**********************************\0", 13, 1, LIGHT_BLUE);     /*Dutch*/
    print_text("******************************** Saluton mondo!*********************************\0", 14, 1, LIGHT_GREEN);    /*Esperanto*/
    print_text("******************************** Tere, Maailm!**********************************\0", 15, 1, LIGHT_CYAN);     /*Estonian*/
    print_text("******************************** Hei maailma!***********************************\0", 16, 1, LIGHT_RED);      /*Finnish*/
    print_text("******************************** Bonjour le monde!******************************\0", 17, 1, LIGHT_MAGENTA);  /*French*/
    print_text("******************************** Ola mundo!*************************************\0", 18, 1, YELLOW);         /*Galician*/
    print_text("******************************** Hallo Welt!************************************\0", 19, 1, WHITE);          /*German*/
    print_text("******************************** Alo Mondyal!***********************************\0", 20, 1, RED);            /*Haitian Creole*/
    print_text("******************************** Sannu Duniya!**********************************\0", 21, 1, MAGENTA);        /*Hausa*/
    print_text("******************************** Nyob zoo lub ntiaj teb no!*********************\0", 22, 1, GREEN);          /*Hmong*/
    print_text("******************************** Halo Dunia!************************************\0", 23, 1, BROWN);          /*Indonesian*/
    print_text("******************************** Ciao mondo!************************************\0", 24, 1, LIGHT_BLUE);     /*Italian*/
    print_text("******************************** Salve mundi!***********************************\0", 25, 1, LIGHT_RED);      /*Latin*/
}

void print_text(char* word, int row, int column, char foreground_color)
{
    int index = 0;

    for(; column < COLUMNS; column+= 1)
    {
        if(word[index] == '\0')
            break;

        putInMemory(0xB * 0x1000, calculate_address(row, column) & 0x0ffff, word[index]);
        putInMemory(0xB * 0x1000, (calculate_address(row, column) & 0x0ffff) + 1, foreground_color);
        index+= 1;
    }
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

void handleInterrupt21 (int ax, int bx, int cx, int dx)
{
    switch(ax)
    {
        case 0:
            syscall_printString(bx);
            break;

        case 1:
            syscall_printString("Enter a line: \0");
            syscall_readString(bx);
            syscall_printString(bx);
            break;

        case 2:
            syscall_readSector(bx, cx);
            break;

        default:
            syscall_printString("The value in AX is unrecognized!");
    }
}
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


void printText(char* word, int row, int column, char foreground_color);
void clearScreen();
void printDetails();
void printIntroduction();
void printHelloWorld();
void setInfiniteLoop();
void initKernel();
int calculateAddress(int row, int column);

main()
{
    initKernel();
}

void initKernel()
{
    clearScreen();
    printDetails();
    setInfiniteLoop();
}

void clearScreen()
{
    int row, column;

    for(row = 0; row < ROWS; row = row + 1)
    {
        for(column = 0; column < COLUMNS; column+= 1)
        {
            putInMemory(0xB * 0x1000, calculateAddress(row+1, column+1) & 0x0ffff, 0x0);
        }
    }
}

void setInfiniteLoop()
{
    while(1)
    {

    }
}

void printDetails()
{
    printIntroduction();
    printHelloWorld();
}

void printIntroduction()
{
    printText("SISTEMAS OPERATIVOS I\0", 1, 27, 0x7);
    printText("UNIVERSIDAD TECNOLOGICA CENTROAMERICANA (UNITEC)\0", 2, 13, 0x7);
    printText("21351064 - Alejandro Ferrera", 3, 26, 0x7);
    printText("MI PRIMER SISTEMA OPERATIVO", 4, 21, 0x7);
}

void printHelloWorld()
{
    printText("--------------------------------------------------------------------------------\0", 5, 1, LIGHT_GRAY);
    printText("******************************** Hola Mundo!************************************\0", 6, 1, BLUE);            /*Spanish*/
    printText("******************************** Hello World!***********************************\0", 7, 1, GREEN);           /*English*/
    printText("******************************** Kaixo Mundua!**********************************\0", 8, 1, CYAN);            /*Basque*/
    printText("******************************** Moni Dziko Lapansi!****************************\0", 9, 1, MAGENTA);         /*Chichewa*/
    printText("******************************** Bonghjornu Monde!******************************\0", 10, 1, BROWN);          /*Corsican*/
    printText("******************************** Bok svijete!***********************************\0", 11, 1, LIGHT_GRAY);     /*Croatian*/
    printText("******************************** Hej Verden!************************************\0", 12, 1, DARK_GRAY);      /*Danish*/
    printText("******************************** Hallo Wereld!**********************************\0", 13, 1, LIGHT_BLUE);     /*Dutch*/
    printText("******************************** Saluton mondo!*********************************\0", 14, 1, LIGHT_GREEN);    /*Esperanto*/
    printText("******************************** Tere, Maailm!**********************************\0", 15, 1, LIGHT_CYAN);     /*Estonian*/
    printText("******************************** Hei maailma!***********************************\0", 16, 1, LIGHT_RED);      /*Finnish*/
    printText("******************************** Bonjour le monde!******************************\0", 17, 1, LIGHT_MAGENTA);  /*French*/
    printText("******************************** Ola mundo!*************************************\0", 18, 1, YELLOW);         /*Galician*/
    printText("******************************** Hallo Welt!************************************\0", 19, 1, WHITE);          /*German*/
    printText("******************************** Alo Mondyal!***********************************\0", 20, 1, RED);            /*Haitian Creole*/
    printText("******************************** Sannu Duniya!**********************************\0", 21, 1, MAGENTA);        /*Hausa*/
    printText("******************************** Nyob zoo lub ntiaj teb no!*********************\0", 22, 1, GREEN);          /*Hmong*/
    printText("******************************** Halo Dunia!************************************\0", 23, 1, BROWN);          /*Indonesian*/
    printText("******************************** Ciao mondo!************************************\0", 24, 1, LIGHT_BLUE);     /*Italian*/
    printText("******************************** Salve mundi!***********************************\0", 25, 1, LIGHT_RED);      /*Latin*/
}

void printText(char* word, int row, int column, char foreground_color)
{
    int index = 0;

    for(; column < COLUMNS; column+= 1)
    {
        if(word[index] == '\0')
            break;

        putInMemory(0xB * 0x1000, calculateAddress(row, column) & 0x0ffff, word[index]);
        putInMemory(0xB * 0x1000, (calculateAddress(row, column) & 0x0ffff) + 1, foreground_color);
        index+= 1;
    }
}

int calculateAddress(int row, int column)
{
    int address;
    int base_address = 0xB8000;

    address = 80 * (row-1);
    address = address + (column-1);
    address = address * 2;
    address = address + base_address;

    return address;
}
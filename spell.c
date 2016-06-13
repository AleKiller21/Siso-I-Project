
#define FILE_NAME_SIZE 		6
#define BUFFER_SIZE 		80
#define INPUT_SIZE 			100
#define PROGRAM_SIZE 		13312

void show_title();
void create_text_file();
void create_new_file();
void clear_buffer(char* buffer, int size);
void set_name(char* name);
int get_last_character_written_pos();
int get_file_size();

char buffer[PROGRAM_SIZE];
int cursor_y;

main()
{
	syscall_clearScreen();
	show_title();
	clear_buffer(buffer, PROGRAM_SIZE);
	create_text_file();
	syscall_terminate();
}

void show_title()
{
	syscall_setCursorPosition(0, 33);
	syscall_printString("New Spell\0");
}

void create_text_file()
{
	int pos = 0;

	cursor_y = 2;

	while(1)
	{
		syscall_setCursorPosition(cursor_y, 0);
		syscall_readString(buffer + pos, -2);

		if(buffer[pos] == '\0')
		{
			create_new_file();
			return;
		}

		pos = get_last_character_written_pos();
		buffer[pos] = 0x20;
		pos += 1;
		cursor_y += 1;
	}
}

void create_new_file()
{
	char name[7];
	int size;

	clear_buffer(name, 7);

	set_name(name);
	size = get_file_size();

	if(size == 0)
	{
		syscall_printString("The file was empty!\0");
		return;
	}

	syscall_writeFile(name, buffer, size);
}

void clear_buffer(char* buffer, int size)
{
	int i;

	for(i = 0; i < size; i += 1)
	{
		buffer[i] = '\0';
	}
}

void set_name(char* name)
{
	cursor_y += 2;
	syscall_setCursorPosition(cursor_y, 0);
	syscall_printString("File name to write: \0");
	syscall_readString(name, 19);
}

int get_last_character_written_pos()
{
	int i;

	for(i = 0; i < PROGRAM_SIZE; i += 1)
	{
		if(buffer[i] == '\0') return i;
	}

	return -1;
}

int get_file_size()
{
	int size;

	for(size = 0; size < PROGRAM_SIZE; size += 1)
	{
		if(buffer[size] == '\0') return size;
	}

	return size;
}


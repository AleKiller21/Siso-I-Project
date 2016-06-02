
#define COMMANDS 			6
#define FILE_NAME_SIZE 		6
#define COMMAND_LENGTH 		6
#define ENTRIES_DIR			16 
#define BUFFER_SIZE 		80
#define INPUT_SIZE 			100
#define PROGRAM_SIZE 		13312


void receive_input(char** commands);
void evaluate_command(char* input, char** commands);
void run_command(char* input, int cmd_sel, int letter_offset);
void set_valid_commands(char** commands);
void clear_screen(int letter_offset, char* input);
void type_file(int letter_offset, char* input);
void exec_program(int letter_offset, char* input);
void list_files(int letter_offset, char* input);
void help(int letter_offset, char* input, char** commands);
void version(int letter_offset, char* input);
void clear_buffer(char* buffer, int size);
int validate_file_name(char* input, , int letter_offset);
int validate_command(char* input, , int letter_offset);

int cursor_y;
char commands[COMMANDS][COMMAND_LENGTH];

main()
{
	
	int i;

	for(i = 0; i < COMMANDS; i += 1)
	{
		clear_buffer(commands[i], COMMAND_LENGTH);
	}

	syscall_clearScreen();
	syscall_setCursorPosition(1, 0);
	set_valid_commands(commands);
	receive_input(commands);
}

void receive_input(char** commands)
{
	char input[INPUT_SIZE];
	clear_buffer(input, INPUT_SIZE);
	cursor_y = 1;

	while(1)
	{
		syscall_printString("FairyOS>\0");
	    syscall_readString(input);
	    evaluate_command(input, commands);
	    cursor_y += 2;
	    syscall_setCursorPosition(cursor_y, 0);
	    clear_buffer(input, INPUT_SIZE);
	}
}

void evaluate_command(char* input, char** commands)
{
	int cmd;
	int letter;
	int match;

	for(cmd = 0; cmd < COMMANDS; cmd += 1)
	{
		letter = 0;
		match = 1;

		if(cmd == 0 && input[0] == '\0')
			return;

		while(commands[cmd][letter] != '\0')
		{
			if(commands[cmd][letter] != input[letter])
			{
				match = 0;
				break;
			}

			letter += 1;
		}

		if(match == 1)
		{
			run_command(input, cmd, letter);
			return;	
		}
	}

	syscall_printString(" --> Command invalid\0");
}

void run_command(char* input, int cmd_sel, int letter_offset)
{
	switch(cmd_sel)
	{
		case 0:
			clear_screen(letter_offset, input);
			break;

		case 1:
			type_file(letter_offset, input);
			break;

		case 2:
			exec_program(letter_offset, input);
			break;

		case 3:
			list_files(letter_offset, input);
			break;

		case 4:
			help(letter_offset, input, commands);
			break;

		case 5:
			version(letter_offset, input);
			break;
	}
}

void set_valid_commands(char** commands)
{
	commands[0] = "clear\0";
	commands[1] = "type\0";
	commands[2] = "exec\0";
	commands[3] = "ls\0";
	commands[4] = "help\0";
	commands[5] = "fairy -v\0";
}

void clear_screen(int letter_offset, char* input)
{
	if(input[letter_offset] != '\0')
	{
		syscall_printString(" --> Command invalid\0");
		return;
	}

	syscall_clearScreen();
	cursor_y = 0;
}

void type_file(int letter_offset, char* input)
{
	char buffer[PROGRAM_SIZE];
	letter_offset += 1;
	cursor_y += 1;

	if(validate_file_name(input, letter_offset) == 1) return;

	clear_buffer(buffer, PROGRAM_SIZE);

	syscall_setCursorPosition(cursor_y, 0);
	syscall_readFile(input + letter_offset, buffer);

	if(buffer[0] == '\0')
	{
		syscall_printString("No such file exists!\0");
		return;
	}
	
	syscall_printString(buffer);
}

void exec_program(int letter_offset, char* input)
{
	/*char buffer[PROGRAM_SIZE];*/
	letter_offset += 1;
	cursor_y += 2;

	if(validate_file_name(input, letter_offset) == 1) return;

	/*clear_buffer(buffer, PROGRAM_SIZE);*/

	syscall_setCursorPosition(cursor_y, 0);
	syscall_executeProgram(input + letter_offset, 0x2000);
}

void list_files(int letter_offset, char* input)
{
	int size = ENTRIES_DIR * FILE_NAME_SIZE;
	int i;
	int i_letter;
	char buffer[ENTRIES_DIR][FILE_NAME_SIZE];
	char names[ENTRIES_DIR * FILE_NAME_SIZE];

	int iterator = 0;

	for(i = 0; i < ENTRIES_DIR; i += 1)
	{
		clear_buffer(buffer[i], FILE_NAME_SIZE);
	}

	clear_buffer(names, size);

	if(validate_command(input, letter_offset) == 1) return;

	syscall_list_files(names);

	for(i = 0; i < ENTRIES_DIR; i += 1)
	{
		if(names[iterator] == 0)
			break;

		for(i_letter = 0; i_letter < FILE_NAME_SIZE; i_letter += 1)
		{
			buffer[i][i_letter] = names[iterator];
			iterator += 1;
		}

		cursor_y += 1;

		syscall_setCursorPosition(cursor_y, 0);
		syscall_printString("- ");
		syscall_printString(buffer[i]);
	}
}

void help(int letter_offset, char* input, char** commands)
{
	int i;

	if(validate_command(input, letter_offset) == 1) return;

	for(i = 0; i < COMMANDS; i += 1)
	{
		cursor_y += 1;

		syscall_setCursorPosition(cursor_y, 0);
		syscall_printString("- ");
		syscall_printString(commands[i]);
	}
}

void version(int letter_offset, char* input)
{
	if(validate_command(input, letter_offset) == 1) return;

	cursor_y += 2;

	syscall_setCursorPosition(cursor_y, 0);
	syscall_printString("Fairy shell v. 0.1. Made by Alejandro Ferrera");
}

void clear_buffer(char* buffer, int size)
{
	int i;

	for(i = 0; i < size; i += 1)
	{
		buffer[i] = '\0';
	}
}

int validate_file_name(char* input, int letter_offset)
{
	if(*(input + (letter_offset - 1)) == '\0' || *(input + letter_offset) == '\0' || *(input + letter_offset) == 0x20)
	{
		syscall_printString(" --> Wrong input of name");

		return 1;
	}

	return 0;
}

int validate_command(char* input, int letter_offset)
{
	if(input[letter_offset] != '\0')
	{
		syscall_printString(" --> Command invalid\0");
		return 1;
	}

	return 0;
}
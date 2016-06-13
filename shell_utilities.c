
#define COMMANDS 			8
#define FILE_NAME_SIZE 		6
#define COMMAND_LENGTH 		7
#define OS_NAME_LENGTH		7
#define ENTRIES_DIR			16 
#define BUFFER_SIZE 		80
#define INPUT_SIZE 			100
#define PROGRAM_SIZE 		13312

int validate_file_name(char* input, , int letter_offset);
int validate_command(char* input, , int letter_offset);
int get_file_size(char* file);
int parse_name_copy_command(char* input, int* iterator, char delimiter, char* buffer);
void clear_buffer(char* buffer, int size);

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

int get_file_size(char* file)
{
	int i;

	for(i = 0; i < PROGRAM_SIZE; i += 1)
	{
		if(file[i] =='\0') break;
	}

	return i;
}

int parse_name_copy_command(char* input, int* iterator, char delimiter, char* buffer)
{
	int i = 0;

	/*while(input[*iterator] != delimiter)
	{
		buffer[i] = input[*iterator];
		i += 1;
		(*iterator) += 1;
	}*/

	for(; *iterator < INPUT_SIZE; *iterator += 1)
	{
		if(input[*iterator] == delimiter) return 1;
		if(input[*iterator] == '\0') return -1;

		buffer[i] = input[*iterator];
		i += 1;
	}

	return -1;
}
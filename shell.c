
#define COMMANDS 3
#define COMMAND_LENGTH 6 


void receive_input(char** commands);
void evaluate_command(char* input, char** commands);
void run_command(char* input, int cmd_sel);
void set_valid_commands(char** commands);
/*void handleInterrupt21 (int ax, int bx, int cx, int dx);*/

main()
{
	char commands[COMMANDS][COMMAND_LENGTH];

	syscall_clearScreen();
	syscall_setCursorPosition(1, 0);
	set_valid_commands(commands);
	receive_input(commands);
}

void receive_input(char** commands)
{
	char input[100];
	int cursor_y = 1;

	while(1)
	{
		syscall_printString("SHELL>\0");
	    syscall_readString(input);
	    evaluate_command(input, commands);
	    cursor_y += 1;
	    syscall_setCursorPosition(cursor_y, 0);
	}
}

void evaluate_command(char* input, char** commands)
{
	int cmd;
	int letter;

	for(cmd = 0; cmd < COMMANDS; cmd += 1)
	{
		if(cmd == 0 && input[0] == '\0')
			return;

		for(letter = 0; letter < COMMAND_LENGTH; letter += 1)
		{
			if(commands[cmd][letter] != input[letter])
				break;

			if(commands[cmd][letter] == '\0')
			{
				syscall_printString("Command valid\0");
				run_command(input, cmd);
				return;				
			}

			if(letter == COMMAND_LENGTH - 1)
			{
				syscall_printString("Command valid\0");
				run_command(input, cmd);
				return;
			}
		}
	}

	syscall_printString(" --> Command invalid\0");
	/*callInterrupt21(4, "tstpr2", 0x2000, 0);*/
}

void run_command(char* input, int cmd_sel)
{
	switch(cmd_sel)
	{
		case 0:
			callInterrupt21(0, 0, 0, 0);
			break;
	}
}

void set_valid_commands(char** commands)
{
	commands[0] = "clear\0";
	commands[1] = "type\0";
	commands[2] = "exec\0";
}



int main()
{
	int i;

	for(i = 0; i < 10000; i += 1)
	{
		if(i == 9999)
		{
			syscall_printString("Hello World \0");
			i = 0;
		}
	}

	/*syscall_printString("Hello World \0");
	while(1);*/
}
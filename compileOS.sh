bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o kernel_utilities.o kernel_utilities.c
bcc -ansi -c -o shell.o shell.c
bcc -ansi -c -o shell_utilities.o shell_utilities.c
bcc -ansi -c -o spell.o spell.c
as86 kernel.asm -o kernel_asm.o
as86 os-api.asm -o os-api.o
ld86 -o kernel -d kernel.o kernel_asm.o kernel_utilities.o
ld86 -o shell -d shell.o os-api.o shell_utilities.o
ld86 -o spell -d spell.o os-api.o
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
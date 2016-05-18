bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o os_api.o os_api.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o os_api.o
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
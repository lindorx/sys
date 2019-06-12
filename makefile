CC=gcc
LD=ld
CREATE_BIN=objcopy

CCLIST=-O1 -nostdinc -fno-stack-protector -fno-tree-ch -Wall -Wno-format -Wno-unused -Werror -gstabs -m32 -fno-omit-frame-pointer -DJOS_KERNEL -gstabs -c -o
LDLIST=-m elf_i386 -N -Ttext 0 -e main -o
OBJCOPY=-S -O binary -j .text -j .data

img=lindorx.img
KERNEL=kernel.bin
SOURCE_CODE=test.c memory.c sysio.c


$(img):boot.bin head.bin $(KERNEL)
	dd if=/dev/zero of=$(img) bs=512 count=256
	dd if=boot.bin of=$(img) bs=512 count=1 skip=0 seek=0 conv=notrunc
	dd if=head.bin of=$(img) bs=512 count=15 skip=0 seek=1 conv=notrunc
	dd if=$(KERNEL) of=$(img) bs=512 count=1 skip=0 seek=32 conv=notrunc

boot.bin:boot.asm
	fasm boot.asm

head.bin:head.asm
	fasm head.asm

$(KERNEL):$(SOURCE)
	$(CC) $(CCLIST) test.o test.c
	$(CC) $(CCLIST) memory.o memory.c
	$(CC) $(CCLIST) sysio.o sysio.c
	$(LD) $(LDLIST) kernel.out test.o memory.o sysio.o
	$(CREATE_BIN) $(OBJCOPY) kernel.out $(KERNEL)

rm:
	make cle
	make $(img)

dbg:$(img)
	bochsdbg -f bochsrc.txt

dump:$(KERNEL)
	objdump -b binary -D -m i386 $(KERNEL) > kernel.txt

run:$(img)
	bochs -f bochsrc.txt

cle:
	rm -f boot.bin head.bin $(img) $(KERNEL)

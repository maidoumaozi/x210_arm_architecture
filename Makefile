CROSS_COMPILE = arm-linux-
INCLUDE = $(shell pwd)/include

CC = $(CROSS_COMPILE)gcc  
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
AR = $(CROSS_COMPILE)ar

CFLAGS = -I $(INCLUDE) -nostdlib -Wall -nostdinc -fno-builtin -c -O0 -g -o 
LDFLAGS = -L /home/fs/workDirectory/architecture_zhu/toolchain/arm-2009q3/lib/gcc/arm-none-linux-gnueabi/4.4.1 -lgcc

export CC LD OBJCOPY OBJDUMP AR CFLAGS

#################################################################
obj_init = init/start.o init/sdram_init.o init/clock.o init/interrupt.o init/movi.o init/mmu.o common/main.o
obj_other = driver/dev.a lib/libc.a
     
pro_name = cc_bootloader
#################################################################

all : $(obj_init) $(obj_other)
	$(LD) -Tlink.lds -o $(pro_name).elf $^ $(LDFLAGS)
	$(OBJCOPY) -O binary $(pro_name).elf $(pro_name).bin
	$(OBJDUMP) -DS  $(pro_name).elf > $(pro_name).dis
	#gcc mkv210_image.c -o mkx210
	./mkx210 $(pro_name).bin $(pro_name).210.bin

driver/dev.a :
	make -C driver dev.a

lib/libc.a :
	make -C lib libc.a

%.o : %.S
	$(CC) $(CFLAGS) $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $@ $<

clean:
	rm -rf $(obj_init) *.elf *.bin *.dis
	make -C driver clean
	make -C lib clean

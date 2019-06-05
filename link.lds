OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
	. = 0x20000000;
	. = ALIGN(4);
	
	.text : {
		init/start.o
		init/clock.o
		init/sdram_init.o
		init/movi.o
		* (.text)
	}
    	
	. = ALIGN(4);
	
	.data : {
		* (.data)
	}
	
	. = ALIGN(4);
	
	.mmudata : {
		*(.mmudata) 
	}
	
	. = ALIGN(4);
	
	bss_start = .; 
	.bss : {
		* (.bss)
	}
	bss_end  = .;	
}

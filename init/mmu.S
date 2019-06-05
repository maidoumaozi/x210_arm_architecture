.macro FL_SECTION_ENTRY base,ap,d,c,b
	.word (\base << 20) | (\ap << 10) | \
		  (\d << 5) | (1<<4) | (\c << 3) | (\b << 2) | (1<<1)
.endm

.section .mmudata, "a"
	.align 14
	.global mmu_table
mmu_table:
	.set __base,0
	.rept 0x100
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	.rept 0x200 - 0x100
	.word 0x00000000
	.endr

	.set __base, 0x200
	.rept 0x600 - 0x200
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	.rept 0x800 - 0x600
	.word 0x00000000
	.endr

	.set __base,0x800
	.rept 0x1000 - 0x800
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr


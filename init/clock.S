//时钟控制寄存器基地址
#define ELFIN_CLOCK_POWER_BASE		0xE0100000	

//时钟控制寄存器偏移地址
#define APLL_LOCK_OFFSET		0x00		
#define MPLL_LOCK_OFFSET		0x08

#define APLL_CON0_OFFSET		0x100
#define APLL_CON1_OFFSET		0x104
#define MPLL_CON_OFFSET			0x108

#define CLK_SRC0_OFFSET			0x200
#define CLK_SRC1_OFFSET			0x204
#define CLK_SRC2_OFFSET			0x208
#define CLK_SRC3_OFFSET			0x20c
#define CLK_SRC4_OFFSET			0x210
#define CLK_SRC5_OFFSET			0x214
#define CLK_SRC6_OFFSET			0x218
#define CLK_SRC_MASK0_OFFSET	0x280
#define CLK_SRC_MASK1_OFFSET	0x284

#define CLK_DIV0_OFFSET			0x300
#define CLK_DIV1_OFFSET			0x304
#define CLK_DIV2_OFFSET			0x308
#define CLK_DIV3_OFFSET			0x30c
#define CLK_DIV4_OFFSET			0x310
#define CLK_DIV5_OFFSET			0x314
#define CLK_DIV6_OFFSET			0x318
#define CLK_DIV7_OFFSET			0x31c

#define CLK_DIV0_MASK			0x7fffffff

//此处的M/P/S value均来自data sheet中的典型值，这些典型值由三星推荐，工作最为稳定
#define APLL_MDIV      	 		0x7d		// 125
#define APLL_PDIV       		0x3
#define APLL_SDIV       		0x1

#define MPLL_MDIV				0x29b		// 667
#define MPLL_PDIV				0xc
#define MPLL_SDIV				0x1

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)
#define APLL_VAL			set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_VAL			set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)


.global clock_init
clock_init:
	ldr	r0, =ELFIN_CLOCK_POWER_BASE
	
	//step 1：暂时关闭PLL输出，使用外部晶振时钟
	//其实根据xPLL_CON0寄存器的初始值，系统启动后各PLL实际并未工作
	ldr	r1, =0x0
	str	r1, [r0, #CLK_SRC0_OFFSET] // CLK_SRC0寄存器的默认值就是0	

	//step 2：设置锁定时间
	//此处设置为寄存器允许的最大值。设置PLL后，时钟从Fin提升到目标频率需要一定时间，即锁定时间
	ldr	r1,	=0x0000FFFF					
	str	r1,	[r0, #APLL_LOCK_OFFSET]				
	str r1, [r0, #MPLL_LOCK_OFFSET]	 				

	//step 3：设置分频系数
	ldr r1, [r0, #CLK_DIV0_OFFSET]					
	ldr	r2, =CLK_DIV0_MASK					
	bic	r1, r1, r2
	ldr	r2, =0x14131440						
	orr	r1, r1, r2
	str	r1, [r0, #CLK_DIV0_OFFSET]

	//step 4：设置PLL
	//FOUT = MDIV*FIN/(PDIV*2^(SDIV-1))=0x7d*24/(0x3*2^(1-1))=1000 MHz
	ldr	r1, =APLL_VAL						
	str	r1, [r0, #APLL_CON0_OFFSET]
	// FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz
	ldr	r1, =MPLL_VAL						
	str	r1, [r0, #MPLL_CON_OFFSET]

	//step 5：使能PLL
	ldr	r1, [r0, #CLK_SRC0_OFFSET]
	ldr	r2, =0x10001111
	orr	r1, r1, r2
	str	r1, [r0, #CLK_SRC0_OFFSET]

	mov	pc, lr

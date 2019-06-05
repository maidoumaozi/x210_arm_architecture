#include <stdio.h>
#include <interrupt.h>

#define		WTCON		(0xE2700000)
#define		WTDAT		(0xE2700004)
#define		WTCNT		(0xE2700008)
#define 	WTCLRINT	(0xE270000C)

#define 	rWTCON		(*(volatile unsigned int *)WTCON)
#define 	rWTDAT		(*(volatile unsigned int *)WTDAT)
#define 	rWTCNT		(*(volatile unsigned int *)WTCNT)
#define 	rWTCLRINT	(*(volatile unsigned int *)WTCLRINT)

void wdt_isr(void)
{
	//中断业务流程
	printf("time out\r\n");

	//中断源端清pend
	rWTCLRINT = 1;
}

void wdt_irq_init(void)
{
	//设置分频系数
	//一级分频后，时钟为1MHz
	rWTCON &= ~(0xff << 8);
	rWTCON |= (65 << 8);

	//二级分频后，时钟为1/128 MHz，即周期为128us
	rWTCON &= ~(0x3 << 3);
	rWTCON |= (0x3 << 3);
	
	//设置计数值
	rWTDAT = 10000; //1.28s
	rWTCNT = 10000;

	//注册ISR
	intc_setvectaddr(NUM_WDT, wdt_isr);

	//中断源端使能中断(禁用reset)
	rWTCON |= (0x1 << 2);
	rWTCON &= ~(0x1 << 0);

	//VIC端使能中断
	intc_enable(NUM_WDT);

	//使能看门狗
	rWTCON |= (0x1 << 5);
}

void feed_dog(void)
{
	rWTCNT = 10000;
}


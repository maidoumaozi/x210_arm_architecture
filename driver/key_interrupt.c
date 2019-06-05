#include <stdio.h>
#include <led.h>
#include <interrupt.h>
#include <key_interrupt.h>
#include <pwm.h>

#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04
#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44

#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)

#define EXT_INT_0_CON	0xE0200E00
#define EXT_INT_2_CON	0xE0200E08
#define EXT_INT_0_PEND	0xE0200F40
#define EXT_INT_2_PEND	0xE0200F48
#define EXT_INT_0_MASK	0xE0200F00
#define EXT_INT_2_MASK	0xE0200F08

#define rEXT_INT_0_CON	(*(volatile unsigned int *)EXT_INT_0_CON)
#define rEXT_INT_2_CON	(*(volatile unsigned int *)EXT_INT_2_CON)
#define rEXT_INT_0_PEND	(*(volatile unsigned int *)EXT_INT_0_PEND)
#define rEXT_INT_2_PEND	(*(volatile unsigned int *)EXT_INT_2_PEND)
#define rEXT_INT_0_MASK	(*(volatile unsigned int *)EXT_INT_0_MASK)
#define rEXT_INT_2_MASK	(*(volatile unsigned int *)EXT_INT_2_MASK)


void key_init_interrupt(void)
{
	//设置外部中断对应管脚的GPIO模式
	rGPH0CON |= 0xFF << 8;
	rGPH2CON |= 0xFFFF << 0;
	
	//设置外部中断触发模式
	//均设置为下降沿触发
	rEXT_INT_0_CON &= ~(0xFF << 8);
	rEXT_INT_0_CON |= ((2 << 8) | (2 << 12));
	rEXT_INT_2_CON &= ~(0xFFFF << 0);
	rEXT_INT_2_CON |= ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12));	
	
	//使能外部中断（中断源端）
	rEXT_INT_0_MASK &= ~(3 << 2);
	rEXT_INT_2_MASK &= ~(0x0f << 0);

#if 0
	//原代码此处清pend，但在按键外部中断初始化阶段没有必要
	rEXT_INT_0_PEND |= (3 << 2);
	rEXT_INT_2_PEND |= (0x0F << 0);
#endif

	//注册ISR
	intc_setvectaddr(NUM_EINT2, isr_eint2);
	//intc_setvectaddr(NUM_EINT3, isr_eint3);
	//intc_setvectaddr(NUM_EINT16_31, isr_eint16171819);
	
	//在VIC中使能对应中断源
	intc_enable(NUM_EINT2);
	//intc_enable(NUM_EINT3);
	//intc_enable(NUM_EINT16_31);
}

void isr_eint2(void)
{
	//中断实际工作内容
	//printf("isr_eint2_LEFT.\r\n");
	update_timer();

	//在中断源端清pend
	rEXT_INT_0_PEND |= (1 << 2);
#if 0
	//原代码在ISR中清VIC端pend
	//个人并不支持，应该将中断源和VIC端的操作尽量区分开来
	intc_clearvectaddr();
#endif
}

void isr_eint3(void)
{
	printf("isr_eint3_DOWN.\n");

	rEXT_INT_0_PEND |= (1 << 3);
}

void isr_eint16171819(void)
{
	//EINT16-31共用中断，需要在ISR中根据pend寄存器确定具体按下的按键
	if (rEXT_INT_2_PEND & (1 << 0))
	{
		printf("eint16\n");
	}
	if (rEXT_INT_2_PEND & (1 << 1))
	{
		printf("eint17\n");
	}
	if (rEXT_INT_2_PEND & (1 << 2))
	{
		printf("eint18\n");
	}
	if (rEXT_INT_2_PEND & (1 << 3))
	{
		printf("eint19\n");
	}

	//在中断源端清pend
	rEXT_INT_2_PEND |= (0x0f<<0);
}


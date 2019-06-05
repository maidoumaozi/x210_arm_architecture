#include <pwm.h>
#include <stdio.h>
#include <interrupt.h>
#include <wdt.h>

#define 	GPD0CON		(0xE02000A0)
#define 	TCFG0		(0xE2500000)
#define 	TCFG1		(0xE2500004)
#define 	CON			(0xE2500008)
#define 	TCNTB2		(0xE2500024)
#define 	TCMPB2		(0xE2500028)
#define 	TCNTB4		(0xE250003C)
#define 	TINT_CSTAT	(0xE2500044)


#define 	rGPD0CON	(*(volatile unsigned int *)GPD0CON)
#define 	rTCFG0		(*(volatile unsigned int *)TCFG0)
#define 	rTCFG1		(*(volatile unsigned int *)TCFG1)
#define 	rCON		(*(volatile unsigned int *)CON)
#define 	rTCNTB2		(*(volatile unsigned int *)TCNTB2)
#define 	rTCMPB2		(*(volatile unsigned int *)TCMPB2)
#define 	rTCNTB4		(*(volatile unsigned int *)TCNTB4)
#define 	rTINT_CSTAT	(*(volatile unsigned int *)TINT_CSTAT)



#define FIN (1000000ul) //1MHz

void pwm_init(void)
{
	//设置GPD0_2引脚，将其配置为XpwmTOUT_2
	rGPD0CON &= ~(0xf << 8);
	rGPD0CON |= (2 << 8);
	
	//一级分频，prescaler1 = 65, 预分频后频率为1MHz
	rTCFG0 &= ~(0xff << 8);
	rTCFG0 |= (65 << 8);

	//二级分频，选择1/1，所以timer2最终频率为1MHz
	rTCFG1 &= ~(0xf << 8);

	//设置Inverter on，即闲时低电平
	rCON |= (1 << 14);
	
	//使能auto-reload
	rCON |= (1 << 15);
}

void set_frequency(unsigned int fre)
{
	//将PWM频率设为fre，占空比设置为50%
	//由于FIN设置为1MHz，所以设置的频率fre必须<= 1MHz
	//还需要注意的是，此处除法为取整运算
	rTCNTB2 = FIN / fre;
	rTCMPB2 = FIN / (fre + fre);

	//第一次需要手工将TCNTB和TCMPB的值更新到TCNT和TCMP中
	rCON |= (1 << 13);
	rCON &= ~(1 << 13);
}

void start_beep(void)
{
	rCON |= (1 << 12);
}

void stop_beep(void)
{
	//特别注意：可以用禁用auto-reload的方式安全关闭PWM
	//让该周期自然结束，这点在控制蜂鸣器时很重要
	//如果只是当作一般定时器使用，直接stop定时器即可
	//此处的关键是PWM终止时的电平状态
	rCON &= ~(1 << 15);
}

void timer_isr(void)
{
	//中断业务流程
	printf("press key\r\n");

	//中断源端清pend
	rTINT_CSTAT |= 0x1 << 9;
}

void timer_feed_dog_isr(void)
{
	//中断业务流程
	feed_dog();
	printf("feed dog\r\n");

	//中断源端清pend
	rTINT_CSTAT |= 0x1 << 9;
}

void timer_init(void)
{
	//一级分频，prescaler1 = 65, 预分频后频率为1MHz
	rTCFG0 &= ~(0xff << 8);
	rTCFG0 |= (65 << 8);

	//二级分频，选择1/1，所以timer 4最终频率为1MHz
	rTCFG1 &= ~(0xf << 16);

	//注册ISR
	intc_setvectaddr(NUM_TIMER4, timer_isr);

	//中断源端使能中断
	rTINT_CSTAT |= (0x1 << 4);

	//VIC端使能中断
	intc_enable(NUM_TIMER4);
}

void timer_feed_dog_init(void)
{
	//一级分频，prescaler1 = 65, 预分频后频率为1MHz
	rTCFG0 &= ~(0xff << 8);
	rTCFG0 |= (65 << 8);

	//二级分频，选择1/1，所以timer 4最终频率为1MHz
	rTCFG1 &= ~(0xf << 16);

	//注册ISR
	intc_setvectaddr(NUM_TIMER4, timer_feed_dog_isr);

	//中断源端使能中断
	rTINT_CSTAT |= (0x1 << 4);

	//VIC端使能中断
	intc_enable(NUM_TIMER4);

	//使能auto-reload
	rCON |= (0x1 << 22);

	//定时1s(目前wdt设置为1.28s超时)
	rTCNTB4 = 1000000;
	rCON |= (1 << 21);
	rCON &= ~(1 << 21);

	//开启定时器
	rCON |= (0x1 << 20);
}


void update_timer(void)
{
	//停止计时器
	rCON &= ~(0x1 << 20);

	//定时10ms
	rTCNTB4 = 10000;
	rCON |= (1 << 21);
	rCON &= ~(1 << 21);

	//开启计时器
	rCON |= (0x1 << 20);
}

int raise(int signum)
{
	printf("raise: Signal # %d caught\n", signum);
	return 0;
}


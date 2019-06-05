#include <stdio.h>
#include <rtc.h>
#include <interrupt.h>

#define 	RTC_BASE	    (0xE2800000)
#define		rINTP      	    (*((volatile unsigned long *)(RTC_BASE + 0x30)))
#define		rRTCCON    	    (*((volatile unsigned long *)(RTC_BASE + 0x40)))
#define		rTICCNT    	    (*((volatile unsigned long *)(RTC_BASE + 0x44)))
#define		rRTCALM    	    (*((volatile unsigned long *)(RTC_BASE + 0x50)))
#define		rALMSEC    	    (*((volatile unsigned long *)(RTC_BASE + 0x54)))
#define		rALMMIN    	    (*((volatile unsigned long *)(RTC_BASE + 0x58)))
#define		rALMHOUR  	    (*((volatile unsigned long *)(RTC_BASE + 0x5c)))
#define		rALMDATE        (*((volatile unsigned long *)(RTC_BASE + 0x60)))
#define		rALMMON    	    (*((volatile unsigned long *)(RTC_BASE + 0x64)))
#define		rALMYEAR  	    (*((volatile unsigned long *)(RTC_BASE + 0x68)))
#define		rBCDSEC    	    (*((volatile unsigned long *)(RTC_BASE + 0x70)))
#define		rBCDMIN   	    (*((volatile unsigned long *)(RTC_BASE + 0x74)))
#define		rBCDHOUR        (*((volatile unsigned long *)(RTC_BASE + 0x78)))
#define		rBCDDAYWEEK     (*((volatile unsigned long *)(RTC_BASE + 0x7c)))
#define		rBCDDAY         (*((volatile unsigned long *)(RTC_BASE + 0x80)))
#define		rBCDMON         (*((volatile unsigned long *)(RTC_BASE + 0x84)))
#define		rBCDYEAR        (*((volatile unsigned long *)(RTC_BASE + 0x88)))
#define		rCURTICCNT      (*((volatile unsigned long *)(RTC_BASE + 0x90)))

void rtc_set_time(const struct rtc_time_s *rtc_time)
{
	//打开RTC控制开关
	rRTCCON |= (0x1 << 0);

	//复位clock divider
	rRTCCON |= (0x1 << 3);
	rRTCCON &= ~(0x1 << 3);

	//设置RTC实时时间
	rBCDYEAR = rtc_time->year - 0x2000;
	rBCDMON = rtc_time->month;
	rBCDDAY = rtc_time->day;
	rBCDDAYWEEK = rtc_time->dayweek;
	rBCDHOUR = rtc_time->hour;
	rBCDMIN = rtc_time->minute;
	rBCDSEC = rtc_time->second;

	//关闭RTC控制开关
	rRTCCON &= ~(0x1 << 0);
}

void rtc_get_time(struct rtc_time_s *rtc_time)
{
	//打开RTC控制开关
	rRTCCON |= (0x1 << 0);

	//获取RTC实时时间
	rtc_time->year = rBCDYEAR + 0x2000;
	rtc_time->month = rBCDMON;
	rtc_time->day = rBCDDAY;
	rtc_time->dayweek = rBCDDAYWEEK;
	rtc_time->hour = rBCDHOUR;
	rtc_time->minute = rBCDMIN;
	rtc_time->second = rBCDSEC;

	//关闭RTC控制开关
	rRTCCON &= ~(0x1 << 0);
}

void rtc_alarm_isr(void)
{
	//中断业务流程
	printf("rtc_alarm\r\n");

	//中断源端清pend
	rINTP |= (0x1 << 1);
}

void rtc_alarm_init(void)
{
	//打开RTC控制开关
	rRTCCON |= (0x1 << 0);

	//注册ISR
	intc_setvectaddr(NUM_RTC_ALARM, rtc_alarm_isr);

	//中断源端使能中断
	//设置为每到第30秒触发中断
	rALMSEC = 0x30;
	rRTCALM |= (0x1 << 6);
	rRTCALM |= (0x1 << 0);

	//VIC端使能中断
	intc_enable(NUM_RTC_ALARM);
	
	//关闭RTC控制开关
	rRTCCON &= ~(0x1 << 0);
}

void rtc_time_tick_isr(void)
{
	//中断业务流程
	printf("rtc_time_tick\r\n");

	//中断源端清pend
	rINTP |= (0x1 << 0);
}

void rtc_time_tick_init(void)
{
	//打开RTC控制开关
	rRTCCON |= (0x1 << 0);

	//注册ISR
	intc_setvectaddr(NUM_RTC_TICK, rtc_time_tick_isr);

	//复位clock divider
	rRTCCON |= (0x1 << 3);
	rRTCCON &= ~(0x1 << 3);

	//中断源端使能中断
	//设置为每2秒触发一次Time Tick中断
	//Time Tick时钟频率为1Hz，TICNT为2
	rRTCCON &= ~(0xf << 4);
	rRTCCON |= (0xf << 4);
	rTICCNT = 2;
	rRTCCON |= (0x1 << 8);

	//VIC端使能中断
	intc_enable(NUM_RTC_TICK);
	
	//关闭RTC控制开关
	rRTCCON &= ~(0x1 << 0);
}

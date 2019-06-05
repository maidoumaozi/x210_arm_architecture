#include <led.h>
#include <uart.h>
#include <stdio.h>
#include <key_polling.h>
#include <key_interrupt.h>
#include <interrupt.h>
#include <pwm.h>
#include <rtc.h>

#if 0
//验证重定位对.data段的影响
static int data_global = 5;
static int bss_global;
#endif

#if 1
//测试链接器脚本中变量在C代码中的使用
extern unsigned int bss_start;
extern unsigned int _bss_start;
#endif

void start_main(void)
{
	int i = 0;
	struct rtc_time_s rtc_time;

	led_init();
	uart_init();
#if 0
	//验证重定位对.data段的影响
	led_on(1);

	putc((data_global + 0x30) & 0xff);
	putc('\r');
	putc('\n');
	putc((bss_global + 0x30) & 0xff);
	putc('\r');
	putc('\n');
	while (1);
#endif

	//printf("*(unsigned int *)0xD0037F98 = %p\r\n", *((unsigned int *)0xD0037F98));
	
	intc_init();
	//uart_irq_init();
	//key_init_interrupt();
#if 0
	//测试LED
	while (1){
		led_on(1);
		delay();
		led_off(1);
		delay();
	}
#endif
#if 0
	//验证iRAM中Global Variables的值
	printf("globalBlockSize = %x\r\n", *((unsigned int *)0xD0037480));
	printf("globalSDHCInfoBit = %x\r\n", *((unsigned int *)0xD0037484));
	printf("V210_SDMMC_BASE = %x\r\n", *((unsigned int *)0xD0037488))0
	while (1);
#endif
#if 0
	//测试UART Rx中断
	while (1)
	{
		printf("A\r\n");
		delay();
	}
#endif
#if 0
	//测试按键中断
	printf("-------------key interrypt test--------------\r\n");
	
	// 在这里加个心跳
	while (1)
	{
		printf("A ");
		delay();
	}
#endif
#if 0
	//测试PWM控制蜂鸣器
	pwm_init();
	set_frequency(100);
	start_beep();

	for (i = 0; i < 10; ++i){
		delay();
	}

	//调用stop_beep会禁用auto-reload
	stop_beep();

	pwm_init();
	set_frequency(10000);
	start_beep();
	for (i = 0; i < 10; ++i){
		delay();
	}
	stop_beep();

	while (1);
#endif
#if 0
	//测试基于定时器的按键去抖动
	key_init_interrupt();
	timer_init();
	while (1);
#endif
#if 0
	//测试看门狗
	wdt_irq_init();
	timer_feed_dog_init();
	while (1);
#endif
#if 0
	//测试RTC实时时钟
	rtc_time.year = 0x2018;
	rtc_time.month = 0x1;
	rtc_time.day = 0x7;
	rtc_time.dayweek = 0x7;
	rtc_time.hour = 0x16;
	rtc_time.minute = 0x08;
	rtc_time.second = 0x01;

	rtc_set_time(&rtc_time);

	while (1){
		delay();
		rtc_get_time(&rtc_time);
		printf("%x-%x-%x-%x-%x-%x-%x\r\n", rtc_time.year, rtc_time.month, rtc_time.day, rtc_time.dayweek,
			rtc_time.hour, rtc_time.minute, rtc_time.second);
	}
#endif
#if 0
	//测试RTC Alarm
	rtc_time.year = 0x2018;
	rtc_time.month = 0x1;
	rtc_time.day = 0x7;
	rtc_time.dayweek = 0x7;
	rtc_time.hour = 0x16;
	rtc_time.minute = 0x08;
	rtc_time.second = 0x25;

	rtc_set_time(&rtc_time);
	rtc_alarm_init();
	while (1);
#endif
#if 0
	//测试RTC Time Tick
	rtc_time_tick_init();
	while (1);
#endif
#if 0
	//测试链接器脚本中变量在C代码中的使用
	printf("bss_start = %x\r\n", bss_start);
	printf("&bss_start = %x\r\n", &bss_start);
	printf("\r\n");
	printf("_bss_start = %x\r\n", _bss_start);
	printf("&_bss_start = %x\r\n", &_bss_start);
	printf("\r\n");
	while (1);
#endif
#if 0
	// 测试I2C通信
	printf("I2C read test\r\n");
	gsensor_init();
	gsensor_read_ct_resp();

	printf("I2C read write test\r\n");
	gsensor_read_write_ctrl_reg();
	while (1);
#endif
#if 0
	// ADC测试
	printf("ADC test\r\n");
	adc_init();
	adc_test();
#endif
#if 1
	// 测试iRAM & DDR实现中异常向量表的位置
	printf("iram_vector = %x\r\n", (*(unsigned int *)0xd0037580));
	printf("ddr_vector = %x\r\n", (*(unsigned int *)0xd0037584));
#endif
}

#include <stdio.h>
#include <led.h>

#define GPH0CON		0xE0200C00
#define GPH0DAT		0xE0200C04

#define GPH2CON		0xE0200C40
#define GPH2DAT		0xE0200C44

#define rGPH0CON	(*(volatile unsigned int *)GPH0CON)
#define rGPH0DAT	(*(volatile unsigned int *)GPH0DAT)
#define rGPH2CON	(*(volatile unsigned int *)GPH2CON)
#define rGPH2DAT	(*(volatile unsigned int *)GPH2DAT)

/*
* 按键对应GPIO：
* SW5：GPH0_2
* SW6：GPH0_3
* SW7：GPH2_0
* SW8：GPH2_1
* SW9：GPH2_2
* SW10：GPH2_3
*/

void key_init_polling(void)
{
	//将按键对应GPIO初始化为Input模式
	rGPH0CON &= ~(0xFF<<8);
	rGPH2CON &= ~(0xFFFF<<0);
}

void key_polling(void)
{
	//循环读取按键对应GPIO的电平值，判断按键是否按下
	//如果按下，则打印按键编号并操作LED
	while (1){
		if (!(rGPH0DAT & (1 << 2))){
			printf("SW 5 is pressed\r\n");
			led_on(1);
		}
		else if (!(rGPH0DAT & (1 << 3))){
			printf("SW 6 is pressed\r\n");
			led_off(1);
		}
		else if (!(rGPH2DAT & (1 << 0))){
			printf("SW 7 is pressed\r\n");
			led_on(2);
		}
		else if (!(rGPH2DAT & (1 << 1))){
			printf("SW 8 is pressed\r\n");
			led_off(2);
		}
		else if (!(rGPH2DAT & (1 << 2))){
			printf("SW 9 is pressed\r\n");
			led_on(3);
		}
		else if(!(rGPH2DAT & (1 << 3))){
			printf("SW 10 is pressed\r\n");
			led_off(3);
		}
	}
}

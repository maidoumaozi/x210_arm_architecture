#include <led.h>

#define GPJ0CON		0xE0200240
#define GPJ0DAT		0xE0200244

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

void delay(void)
{
	volatile unsigned int i = 999999;
	while (i--);
}

void led_init(void)
{
	rGPJ0CON = rGPJ0CON & ~(0xfff << 12) | (0x111 << 12);
	rGPJ0DAT = rGPJ0DAT | (0x7 << 3);
}

void led_on(int index)
{
	int real_index = index + 2;
	
	rGPJ0DAT = rGPJ0DAT & ~(0x1 << real_index);
}

void led_off(int index)
{
	int real_index = index + 2;
	
	rGPJ0DAT = rGPJ0DAT | (0x1 << real_index);
}

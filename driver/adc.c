#include <stdio.h>
#include <interrupt.h>
#include <led.h>
#include <adc.h>

#define TSADCCON0		0xE1700000
#define TSDATX0			0xE170000C
#define ADCMUX			0xE170001C

#define rTSADCCON0		(*(volatile unsigned int *)TSADCCON0)
#define rTSDATX0		(*(volatile unsigned int *)TSDATX0)
#define rADCMUX			(*(volatile unsigned int *)ADCMUX)

void adc_init(void)
{
	rTSADCCON0	|= (1 << 16);		// resolution set to 12bit
	rTSADCCON0	|= (1 << 14);		// enable clock prescaler
	rTSADCCON0 	&= ~(0xFF << 6);
	rTSADCCON0	|= (65 << 6);		// convertor clock = 66/66M=1MHz, MSPS=200KHz
	rTSADCCON0 	&= ~(1 << 2);		// normal operation mode
	rTSADCCON0 	|= (1 << 1);		// enable start by read mode
	
	rADCMUX		&= ~(0x0F << 0);	// MUX选择ADCIN0
}

void adc_test(void)
{
	unsigned int val = 0;

	val = rTSDATX0; // start by read

	while (1) {
		while (!(rTSADCCON0 & (1<<15)));

		val = rTSDATX0;
		printf("adc value = %d\r\n", val & 0xFFF);

		delay();
		delay();
		delay();
	}
}

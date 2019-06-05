#include <uart.h>
#include <interrupt.h>
#include <uart.h>

#define GPA0CON		0xE0200000
#define UCON0 		0xE2900004
#define ULCON0 		0xE2900000
#define UMCON0 		0xE290000C
#define UFCON0 		0xE2900008
#define UBRDIV0 	0xE2900028
#define UDIVSLOT0	0xE290002C
#define UTRSTAT0	0xE2900010
#define UTXH0		0xE2900020	
#define URXH0		0xE2900024
#define UINTP0      0xE2900030
#define UINTSP0     0xE2900034
#define UINTM0      0xE2900038

#define rGPA0CON	(*(volatile unsigned int *)GPA0CON)
#define rUCON0		(*(volatile unsigned int *)UCON0)
#define rULCON0		(*(volatile unsigned int *)ULCON0)
#define rUMCON0		(*(volatile unsigned int *)UMCON0)
#define rUFCON0		(*(volatile unsigned int *)UFCON0)
#define rUBRDIV0	(*(volatile unsigned int *)UBRDIV0)
#define rUDIVSLOT0	(*(volatile unsigned int *)UDIVSLOT0)
#define rUTRSTAT0	(*(volatile unsigned int *)UTRSTAT0)
#define rUTXH0		(*(volatile unsigned int *)UTXH0)
#define rURXH0		(*(volatile unsigned int *)URXH0)
#define rUINTP0		(*(volatile unsigned int *)UINTP0)
#define rUINTSP0	(*(volatile unsigned int *)UINTSP0)
#define rUINTM0		(*(volatile unsigned int *)UINTM0)

//UART0 ISR
void uart0_irq_isr(void)
{
	//中断业务流程
	//目前为回显数据
	//注意：此处要将URXH0寄存器中的数据读出，
	//否则后续串口输入会导致overrun错误（该结论已经过注释中代码的验证）
	putc(rURXH0);
#if 0
	unsigned char c = 0;
	c = rURXH0;
	printf("uart\r\n");
#endif

	//中断源端清pend
	//rUINTP0 = 0xf; //清除所有位
	rUINTP0 |= 0x1 << 0; //仅清除RXD对应位
}

//串口初始化
void uart_init(void)
{
	//初始化UART Tx/Rx对应的GPIO引脚
	rGPA0CON = rGPA0CON & ~(0xff) | (0x22);
	
	rULCON0 = 0x3;
	rUCON0 = 0x5;

	//设置波特率
	//PCLK_PSYS = 66.7MHz
	//DIV_VAL = (66700000/(115200*16)-1) = 35.18
	//16 * 0.18 = 2.88
	rUBRDIV0 = 35;
	rUDIVSLOT0 = 0x0888;
}

//串口中断初始化
void uart_irq_init(void)
{
	//注册ISR
	intc_setvectaddr(NUM_UART0, uart0_irq_isr);

	//中断源端使能中断
	//目前仅使能RXD中断，屏蔽其他中断
	rUINTM0 = 0xe;

	//VIC端使能中断
	intc_enable(NUM_UART0);
}


//串口发送，发送一个字节
void putc(unsigned char c)
{                  	
	//先判断发送缓冲区是否为空
	while (!(rUTRSTAT0 & (1<<1)));
	rUTXH0 = c;
}

//串口接收，轮询方式接收一个字节
unsigned char getc(void)
{
	//先判断接收缓冲区中是否有数据
	while (!(rUTRSTAT0 & (1<<0)));
	return (rURXH0 & 0xff);
}


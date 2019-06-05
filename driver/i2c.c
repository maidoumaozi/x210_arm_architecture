#include <stdio.h>
#include <interrupt.h>
#include <i2c.h>

#define 	GPD0CON		(0xE02000A0)
#define 	GPD0DAT		(0xE02000A4)
#define     I2CCON0     (0xE1800000)
#define     I2CSTAT0    (0xE1800004)
#define     I2CADD0     (0xE1800008)
#define     I2CDS0      (0xE180000C)
#define 	GPD1CON		(0xE02000C0)
#define 	GPD1PUD		(0xE02000C8)

#define 	rGPD0CON	(*(volatile unsigned int *)GPD0CON)
#define     rGPD0DAT    (*(volatile unsigned int *)GPD0DAT)
#define     rI2CCON0    (*(volatile unsigned char *)I2CCON0)
#define     rI2CSTAT0   (*(volatile unsigned char *)I2CSTAT0)
#define     rI2CADD0    (*(volatile unsigned char *)I2CADD0)
#define     rI2CDS0     (*(volatile unsigned char *)I2CDS0)
#define 	rGPD1CON	(*(volatile unsigned int *)GPD1CON)
#define 	rGPD1PUD	(*(volatile unsigned int *)GPD1PUD)

#define GSENSOR_WRITE_ADDR   (0x1E)
#define GSENSOR_READ_ADDR    (0x1F)
#define CT_RESP_ADDR         (0x0C)
#define CTRL_REG2_ADDR       (0x1C)

static sleep_loop(int i)
{
	int j = 0;
	while (i--)
		for (j = 0; j < 100; ++j)
			;
}

static void gsensor_power_on(void)
{
	//设置GPD0_3引脚，将其配置为output
	rGPD0CON &= ~(0xf << 12);
	rGPD0CON |= (1 << 12);

	// 设置GPD0_3引脚输出高电平
	rGPD0DAT |= (1 << 3);
}

static void i2c_init(void)
{
	// GPIO设置
	// 使能上拉
	rGPD1CON &= ~(0xff << 0);
	rGPD1CON |= (0x22 << 0); // 使能上拉

	// 中断源端使能中断
	rI2CCON0 |= (1 << 5);

	// 时钟设置
	// PCLK_PSYS = 65MHz
	// SCL = 65000 / 512 / 2 ≈ 64KHz
	rI2CCON0 &= ~(0xF << 0);
	rI2CCON0 |= (1 << 6);
	rI2CCON0 |= (1 << 0);

	// 使能serial output
	rI2CSTAT0 |= (1 << 4);
}

void gsensor_init(void)
{
	gsensor_power_on();
	i2c_init();
}

/*
* 功能：发出start signal + slave address
* direction: i2c传输方向
* 0: transmit
* 1: receive
*/
static void i2c_start_transfer(int direction)
{
	// 使能ACK响应
	rI2CCON0 |= (1 << 7);

	// 设置通信角色并发出start信号
	//rI2CSTAT0 &= ~(0x3 << 6); // 会发出stop信号
	if (direction == 0) {
		//rI2CSTAT0 |= (0x3 << 6); // 会发出stop信号
		rI2CDS0 = GSENSOR_WRITE_ADDR;
		rI2CSTAT0 = 0xF0;
	} else {
		//rI2CSTAT0 |= (0x2 << 6); // 会发出stop信号
		rI2CDS0 = GSENSOR_READ_ADDR;
		rI2CSTAT0 = 0xB0;
	}

	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);
}

static void i2c_restart_transfer(int direction)
{
	if (direction == 0) {
		rI2CDS0 = GSENSOR_WRITE_ADDR;
		rI2CSTAT0 = 0xF0;
	} else {
		rI2CDS0 = GSENSOR_READ_ADDR;
		rI2CSTAT0 = 0xB0;
	}

	// 清中断挂起（恢复发送）
	rI2CCON0 &= ~(1 << 4);

	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);
}

/*
* 功能：发出stop signal
*/
static void i2c_stop_transfer(int direction)
{
	// 发出stop信号
	if (direction == 0) {
		rI2CSTAT0 = 0xD0;
	} else {
		rI2CSTAT0 = 0x90;
	}

	// 清中断挂起
	rI2CCON0 &= ~(1 << 4);
#if 1
	// 等待stop信号生效
	sleep_loop(100);
#else
	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);
#endif
}

static void i2c_write_byte(unsigned char data)
{
	// 写入要发送的数据
	rI2CDS0 = data;

	// 清中断挂起（恢复发送）
	rI2CCON0 &= ~(1 << 4);

	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);
}

static unsigned char i2c_read_byte(void)
{
	unsigned char ret = 0;

	// 清中断挂起（恢复传输）
	rI2CCON0 &= ~(1 << 4);
	
	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);

	// 读取接收到的数据
	ret = rI2CDS0;
	
	return ret;
}

static unsigned char i2c_read_last_byte(void)
{
	unsigned char ret = 0;

	// 禁用ACK响应
	rI2CCON0 &= ~(1 << 7);

	// ret = rI2CDS0; // get wrong data

	// 清中断挂起（恢复传输）
	rI2CCON0 &= ~(1 << 4);

	// 轮询中断挂起
	while ((rI2CCON0 & (1 << 4)) == 0)
		sleep_loop(100);

	// 读取接收到的数据
	ret = rI2CDS0;
	
	return ret;
}

void gsensor_read_ct_resp(void)
{
	unsigned char ret = 0;
	
	i2c_start_transfer(0);
	i2c_write_byte(CT_RESP_ADDR);
	i2c_restart_transfer(1);
	ret = i2c_read_last_byte();
	i2c_stop_transfer(1);
	printf("ct_resp = 0x%x\r\n", ret);
}

void gsensor_read_write_ctrl_reg(void)
{
	unsigned char ret = 0;

	i2c_start_transfer(0);
	i2c_write_byte(CTRL_REG2_ADDR);
	i2c_restart_transfer(1);
	ret = i2c_read_last_byte();
	i2c_stop_transfer(1);
	printf("ctrl_reg2_before = 0x%x\r\n", ret);

	i2c_start_transfer(0);
	i2c_write_byte(CTRL_REG2_ADDR);
	i2c_write_byte(0xaa);
	i2c_stop_transfer(0);

	
	i2c_start_transfer(0);
	i2c_write_byte(CTRL_REG2_ADDR);
	i2c_restart_transfer(1);
	ret = i2c_read_last_byte();
	i2c_stop_transfer(1);
	printf("ctrl_reg2_after = 0x%x\r\n", ret);
}


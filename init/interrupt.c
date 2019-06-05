#include <interrupt.h>
#include <stdio.h>


void reset_exception(void)
{
	printf("reset_exception.\n");
}

void undef_exception(void)
{
	printf("undef_exception.\n");
}

void sotf_int_exception(void)
{
	printf("sotf_int_exception.\n");
}

void prefetch_exception(void)
{
	printf("prefetch_exception.\n");
}

void data_exception(void)
{
	printf("data_exception.\n");
}

void system_init_exception(void)
{
	//绑定iRAM中的异常向量表
	r_exception_reset = (unsigned int)reset_exception;
	r_exception_undef = (unsigned int)undef_exception;
	r_exception_sotf_int = (unsigned int)sotf_int_exception;
	r_exception_prefetch = (unsigned int)prefetch_exception;
	r_exception_data = (unsigned int)data_exception;
	r_exception_irq = (unsigned int)IRQ_handle;
	r_exception_fiq = (unsigned int)IRQ_handle;
	
	//初始化VIC
	intc_init();
}

void intc_clearvectaddr(void)
{
	//VIC端清pend
    VIC0ADDR = 0;
    VIC1ADDR = 0;
    VIC2ADDR = 0;
    VIC3ADDR = 0;
}

void intc_init(void)
{
	//禁用所有中断
	//一般的流程是先禁用所有中断，然后再打开配置好的中断
	//以此避免中断误触发导致程序跑飞
    VIC0INTENCLEAR = 0xffffffff;
    VIC1INTENCLEAR = 0xffffffff;
    VIC2INTENCLEAR = 0xffffffff;
    VIC3INTENCLEAR = 0xffffffff;

    //配置所有中断类型为IRQ
    VIC0INTSELECT = 0x0;
    VIC1INTSELECT = 0x0;
    VIC2INTSELECT = 0x0;
    VIC3INTSELECT = 0x0;

    //原代码中在此处写VICADDRESS寄存器
    //但是根据VIC手册，只在ISR清中断时才写VICADDRESS寄存器
    //因此此处不建议清VICADDRESS寄存器
    //intc_clearvectaddr();
}

//设置VICVECTADDR寄存器
void intc_setvectaddr(unsigned long intnum, void (*handler)(void))
{
    //VIC0
    if(intnum<32)
    {
        *( (volatile unsigned long *)(VIC0VECTADDR + 4*(intnum - 0)) ) = (unsigned)handler;
    }
    //VIC1
    else if(intnum<64)
    {
        *( (volatile unsigned long *)(VIC1VECTADDR + 4*(intnum - 32)) ) = (unsigned)handler;
    }
    //VIC2
    else if(intnum<96)
    {
        *( (volatile unsigned long *)(VIC2VECTADDR + 4*(intnum - 64)) ) = (unsigned)handler;
    }
    //VIC3
    else
    {
        *( (volatile unsigned long *)(VIC3VECTADDR + 4*(intnum - 96)) ) = (unsigned)handler;
    }
}

void intc_enable(unsigned long intnum)
{
    unsigned long temp = 0;

    if(intnum < 32)
    {
    	//目前代码中遵循读出-修改-写回的步骤
    	//VIC0INTENABLE |= 1 << intnum
        temp = VIC0INTENABLE;
        temp |= (1 << intnum);
        VIC0INTENABLE = temp;
    }
    else if(intnum < 64)
    {
        temp = VIC1INTENABLE;
        temp |= (1 << (intnum - 32));
        VIC1INTENABLE = temp;
    }
    else if(intnum < 96)
    {
        temp = VIC2INTENABLE;
        temp |= (1 << (intnum - 64));
        VIC2INTENABLE = temp;
    }
    else if(intnum < NUM_ALL)
    {
        temp = VIC3INTENABLE;
        temp |= (1 << (intnum - 96));
        VIC3INTENABLE = temp;
    }
    // NUM_ALL : enable all interrupt
    else
    {
        VIC0INTENABLE = 0xFFFFFFFF;
        VIC1INTENABLE = 0xFFFFFFFF;
        VIC2INTENABLE = 0xFFFFFFFF;
        VIC3INTENABLE = 0xFFFFFFFF;
    }

}

void intc_disable(unsigned long intnum)
{
    unsigned long temp = 0;

    if(intnum < 32)
    {
        temp = VIC0INTENCLEAR;
        temp |= (1 << intnum);
        VIC0INTENCLEAR = temp;
    }
    else if(intnum < 64)
    {
        temp = VIC1INTENCLEAR;
        temp |= (1 << (intnum - 32));
        VIC1INTENCLEAR = temp;
    }
    else if(intnum < 96)
    {
        temp = VIC2INTENCLEAR;
        temp |= (1 << (intnum - 64));
        VIC2INTENCLEAR = temp;
    }
    else if(intnum < NUM_ALL)
    {
        temp = VIC3INTENCLEAR;
        temp |= (1 << (intnum - 96));
        VIC3INTENCLEAR = temp;
    }
    // NUM_ALL : disable all interrupt
    else
    {
        VIC0INTENCLEAR = 0xFFFFFFFF;
        VIC1INTENCLEAR = 0xFFFFFFFF;
        VIC2INTENCLEAR = 0xFFFFFFFF;
        VIC3INTENCLEAR = 0xFFFFFFFF;
    }

    return;
}

unsigned long intc_getvicirqstatus(unsigned long ucontroller)
{
    if(ucontroller == 0)
        return	VIC0IRQSTATUS;
    else if(ucontroller == 1)
        return 	VIC1IRQSTATUS;
    else if(ucontroller == 2)
        return 	VIC2IRQSTATUS;
    else if(ucontroller == 3)
        return 	VIC3IRQSTATUS;
    else
    {
		printf("error parameter\r\n");
		return 0;
	}
}


void irq_handler(void)
{
#if 0
	unsigned long vicaddr[4] = {VIC0ADDR,VIC1ADDR,VIC2ADDR,VIC3ADDR};
    int i = 0;
    void (*isr)(void) = NULL;

    for(i = 0; i < 4; i++)
    {
		//根据VICIRQSTATUS寄存器确定哪个VIC发生中断
        if(intc_getvicirqstatus(i) != 0)
        {
            isr = (void (*)(void)) vicaddr[i];
            break;
        }
    }
#endif
	//由于采用菊花链模式，直接读取VIC0ADDRESS寄存器的值即可
	void (*isr)(void) = (void (*)(void))VIC0ADDR;

	//执行ISR
    (*isr)();

	//清VIC端pend
	intc_clearvectaddr();
}


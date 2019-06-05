.global _start
_start:
	//异常向量表
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq
	
_undefined_instruction: .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq

undefined_instruction:
	nop
	
software_interrupt:
	nop
	
prefetch_abort:
	nop
	
data_abort:
	nop
	
not_used:
	nop
	
irq:
	b IRQ_handle
	
fiq:
	nop

.global _bss_start
_bss_start:
	.word bss_start

.global _bss_end
_bss_end:
	.word bss_end

reset:
	bl lock_ps_hold
	bl disable_watchdog
	bl enable_icache
	bl clock_init
	bl sdram_asm_init
#if 0
	//这种处理其实有逻辑漏洞
	adr r0, _start
	ldr r1, =_start 
	cmp r0, r1 //当运行地址和链接地址匹配时，无需代码重定位
	blne sdram_asm_init
#endif
	//bl code_relocate
	bl set_exception_vector_addr
	//bl enable_mmu
	bl init_stack
	bl clean_bss
	//bl start_main //短调转，仍在iRAM中
	bl movi_bl2_copy
	ldr pc, =start_main //长跳转，进入DDR中运行

stop:	
	b stop

//关闭看门狗
#define WTCON 0xE2700000	
disable_watchdog:
	ldr r0, =WTCON
	ldr r1, =0x0
	str r1, [r0]
	mov pc, lr

//设置SVC模式栈
//注意：在设置栈时并未关闭中断；最后设置SVC模式栈，确保init_stack完成后，CPU处于SVC模式
//#define SVC_STACK   0xD0037D80
#define SVC_STACK	0x30000000 //栈地址也要4字节对齐/满减栈/DMC0高端地址
//#define IQR_STACK   0xD0037F80
#define IRQ_STACK   0x2FF00000
//注意一个问题，在iRAM的内存布局中，IRQ栈在高端，SVC栈在低端，其实这样才是合理的
//因为一般情况下，IRQ栈的使用量远在SVC栈之下，所以给SVC栈更大的空间是合理的
//目前相当于SVC栈被限制在了1MB，虽然够用，但安排上还是应该调换2个栈的位置
init_stack:
	msr cpsr_c, #0x12
	ldr sp, =IRQ_STACK
	msr cpsr_c, #0x13
	ldr sp, =SVC_STACK
	mov pc, lr

enable_icache:
	mrc p15, 0, r0, c1, c0, 0         //读出cp15_c1到r0
	//bic r0, r0, #(1<<12)    // bit12清0，disable icache
	orr r0, r0, #(1<<12)	  // bit12置1，enable icache
	mcr p15, 0, r0, c1, c0, 0
	mov pc, lr

code_relocate:
	//代码重定位(iRAM --> DDR)
	//adr伪指令获取运行地址；ldr伪指令获取链接地址
	//r0：运行地址，即代码重定位源地址
	//r1：链接地址，即代码重定位目的地址
	//r2：bss段起始地址，即代码重定位的结束地址
	adr r0, _start
	ldr r1, =_start 
	ldr r2, =bss_start
	cmp r0, r1 //当运行地址和链接地址匹配时，无需代码重定位
	moveq pc, lr
	//bl sdram_asm_init //不能在此处再次调用bl，会覆盖原LR中的数据（非叶子函数）
copy_loop:
	ldr r3, [r0], #4
	str r3, [r1], #4
	cmp r1, r2
	bne copy_loop
	mov pc, lr

set_exception_vector_addr:
	mrc p15, 0, r0, c12, c0, 0
	ldr r1, =0xd0037580
	str r0, [r1]
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c12, c0, 0
	ldr r1, =0xd0037584
	str r0, [r1]
	mov pc, lr

clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	cmp r0, r1
	moveq pc, lr
	mov r2, #0
clear_loop:
	str r2, [r0], #4
	cmp r0, r1
	bne clear_loop
	mov pc, lr

#define PS_HOLD_CONTROL 0xE010E81C
lock_ps_hold:
	ldr r0, =PS_HOLD_CONTROL
	ldr r1, [r0]
	ldr r2, =0x301
	orr r1, r1, r2
	str r1, [r0]
	mov pc, lr

enable_mmu:
	//设置domain寄存器
	ldr r0, =0x0000ffff
	mcr p15, 0, r0, c3, c0, 0

	//设置TTB寄存器
	ldr r0, =mmu_table
	mcr p15, 0, r0, c2, c0, 0

	//设置控制寄存器
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #1
	mcr p15, 0, r0, c1, c0, 0

	mov pc, lr

.global IRQ_handle
IRQ_handle:
	// 保存LR
	// 因为ARM有流水线，所以PC的值为真正执行的代码+8，
	sub lr, lr, #4
	// 保存r0-r12和lr到irq模式下的栈上面
	stmfd sp!, {r0-r12, lr}
	// 在此调用C语言阶段的中断处理函数
	bl irq_handler
	// 处理完成开始恢复现场,其实就是做中断返回，关键是将r0-r12，pc，cpsr一起恢复
	ldmfd sp!, {r0-r12, pc}^
	

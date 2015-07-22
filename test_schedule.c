#include "schedule.h"
#include "stm32f10x.h"
#include "core_cmFunc.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define BitP(addr, n) (*(volatile unsigned int *)(0x42000000 + (((unsigned)addr - 0x40000000) * 8 + n) * 4))
#define BitM(addr, n) (*(volatile unsigned int *)(0x22000000 + (((unsigned)addr - 0x20000000) * 8 + n) * 4))

#define STACK_SIZE 64
uint32_t stack1[STACK_SIZE];
void process1()
{
	int i;
	while (1)
	{
		BitP(&GPIOA->ODR, 2) = 1;	/* 0xff有效 0xfe无效, 只有lsb为1才有效 */
		for (i = 0; i < 0x100000; ++i);
		BitP(&GPIOA->ODR, 2) = 0;
		for (i = 0; i < 0x100000; ++i);
	}
}
uint32_t stack2[STACK_SIZE];
void process2()
{
	int i;
	while (1)
	{
		BitP(&GPIOA->ODR, 2) = 0;	/* 0xff有效 0xfe无效, 只有lsb为1才有效 */
		/*for (i = 0; i < 0x100000; ++i);
		BitP(&GPIOA->ODR, 2) = 0;
		for (i = 0; i < 0x100000; ++i);*/
	}
}

void test_schedule()
{
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	
	__disable_irq();
	
	create_process(&stack1[STACK_SIZE], process1);
	create_process(&stack2[STACK_SIZE], process2);
	
	schedule();
	__enable_irq();
}

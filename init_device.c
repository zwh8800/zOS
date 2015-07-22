#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <misc.h>

#include "kdebug.h"

void init_device()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	NVIC_InitTypeDef nvic;

	/* 初始化APB2外设时钟 */
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* 初始化gpio端口 */
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	/* 初始化NVIC中断控制器 */
	nvic.NVIC_IRQChannel = USART1_IRQn;	/* USART1所有中断或到一根IRQ上 */
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	/* 初始化USART */
	usart.USART_BaudRate = 115200;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 开启接收区非空(RXNE)中断 */
}

void USART1_IRQHandler()
{
	/*
	 * TODO:
	 * 以后用DMA + 循环队列
	 */
	uint16_t data;
	if ((USART1->SR & USART_FLAG_RXNE) != 0)	/* 寄存器读取清零 */
	{
		data = USART_ReceiveData(USART1);
		if (data == '\r')
		{
			USART_SendData(USART1, '\r');
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			USART_SendData(USART1, '\n');
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
		else
		{
			USART_SendData(USART1, data);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
	}
}

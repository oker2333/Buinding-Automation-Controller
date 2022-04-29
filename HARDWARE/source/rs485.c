#include "rs485.h"
#include "dma.h"
#include "debug.h"
#include <stdio.h>

void RS485_Init(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	//GPIO引脚初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART初始化
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	//USART中断向量初始化
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	
	USART_Cmd(USART2, ENABLE);	//USART串口使能
}


extern OS_SEM	SYNC_Uart_Recv;
extern OS_SEM	SYNC_Uart_Send;


void USART2_IRQHandler(void)
{
	OSIntEnter();
	
	OS_ERR err;
	uint32_t temp;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		OSSemPost(&SYNC_Uart_Recv,OS_OPT_POST_1,&err);	
		temp = USART2->SR;
		temp = USART2->DR;
		(void)temp;

	}
	
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		//发送完成
		Log_Print("transmit complete");
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		USART_ClearFlag(USART2, USART_FLAG_TC);
		
		OSSemPost(&SYNC_Uart_Send,OS_OPT_POST_1,&err);	
	}
	OSIntExit();
}

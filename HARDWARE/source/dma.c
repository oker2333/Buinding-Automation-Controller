#include "dma.h"
#include <stdio.h>

uint8_t OutputBuffer[OutputBufferSize] = {"hello world\n"};

void Usart_Tx_Config(void)		//DMA����
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	DMA_DeInit(DMA1_Stream6);
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}
		
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&OutputBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = OutputBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);
}

void DMA1_Stream6_Send(uint16_t Counter)
{
	DMA_Cmd(DMA1_Stream6, DISABLE); 
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);
	
	DMA_SetCurrDataCounter(DMA1_Stream6,Counter);
	
	DMA_Cmd(DMA1_Stream6, ENABLE); 
}

void DMA1_Stream6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6) != RESET){
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
		
		USART_ClearFlag(USART2, USART_FLAG_TC);
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
}

uint8_t InputBuffer[InputBufferSize];

void Usart_Rx_Config(void)		//DMA����
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

	DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}
		
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&InputBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = InputBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

void DMA1_Stream5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5) != RESET){
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
		
		DMA_Cmd(DMA1_Stream5, DISABLE);
		uint16_t data_len = InputBufferSize - DMA_GetCurrDataCounter(DMA1_Stream5);
		Log_Info("data_len = %d",data_len);
		DMA_SetCurrDataCounter(DMA1_Stream5, InputBufferSize);
		DMA_Cmd(DMA1_Stream5, ENABLE);
	}
}

/**********************************************************************/

#if(LOG_LEVEL >= TRACE) 

uint8_t LogBuffer[LogBufferSize];

void Debug_Tx_Config(void)		//DMA����
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

	DMA_DeInit(DMA2_Stream7);
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE){}
		
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&LogBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = LogBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);
}

void DMA2_Stream7_Send(uint16_t Counter)
{
	DMA_Cmd(DMA2_Stream7, DISABLE); 
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
	
	DMA_SetCurrDataCounter(DMA2_Stream7,Counter);
	
	DMA_Cmd(DMA2_Stream7, ENABLE); 
}

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET){
		DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
		
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	}
}

#endif

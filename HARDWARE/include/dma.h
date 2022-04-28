#ifndef DMA_H_
#define DMA_H_

#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "rs485.h"
#include "debug.h"

#define OutputBufferSize 512
#define InputBufferSize 512

void Usart_Tx_Config(void);
void DMA1_Stream6_Send(uint16_t Counter);

#if (LOG_LEVEL >= TRACE)

#define LogBufferSize 128
extern uint8_t LogBuffer[LogBufferSize];

extern void log_Q_post(uint8_t* pbuf,uint32_t len);

void Debug_Tx_Config(void);
void Usart_Rx_Config(void);

void DMA2_Stream7_Send(uint16_t Counter);

#endif

#endif

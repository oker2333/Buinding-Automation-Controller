#ifndef RS485_H_
#define RS485_H_

#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#define InputBufferSize 512

#define RS485_RX() GPIO_ResetBits(GPIOG,GPIO_Pin_8)
#define RS485_TX() GPIO_SetBits(GPIOG,GPIO_Pin_8)

void RS485_Init(uint32_t BaudRate);

#endif

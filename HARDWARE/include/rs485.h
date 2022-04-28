#ifndef RS485_H_
#define RS485_H_

#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

void RS485_Init(uint32_t BaudRate);

#endif

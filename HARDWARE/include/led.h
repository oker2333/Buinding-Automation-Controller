#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define Red_Led GPIO_Pin_9 
#define Yellow_Led GPIO_Pin_10

void LED_Init(void);
void LED_Invert(uint16_t led);

#endif

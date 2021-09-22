#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
}

void LED_Invert(uint16_t led)
{
	uint16_t DataRegister = 0;
	DataRegister = GPIO_ReadOutputData(GPIOF);
	
		if(DataRegister & led)
			GPIO_ResetBits(GPIOF,led);
		else
			GPIO_SetBits(GPIOF,led);
}

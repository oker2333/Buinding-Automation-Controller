#include "lan8720.h"
#include "stm32f4xx_conf.h"
#include "stm32f4x7_eth.h"
#include  "lib_mem.h"
#include <stdlib.h>
#include <string.h>

#define PHY_ADDR 0

/*
RMII�ӿ�
PA1				RMII_REF_CLK
PA2				ETH_MDIO
PA7				RMII_CRS_DV

PC1				ETH_MDC
PC4				RMII_RXD0
PC5				RMII_RXD1

PD3				ETH_RESET

PG11     	RMII_TX_EN
PG13			RMII_TXD0
PG14			RMII_TXD1

*/

void ETH_PHY_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
	
	/*PA1��PA2��PA7*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	/*PC1��PC4��PC5*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
	
	/*PG11��PG13��PG14*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
	
	/*PD3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	ETH_Delay(100000);
	GPIO_SetBits(GPIOD,GPIO_Pin_3);
	ETH_Delay(100000);
}

void ETH_MAC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

	ETH_SoftwareReset();
	while (ETH_GetSoftwareResetStatus() == SET);
	
	ETH_InitTypeDef ETH_InitStructure; 
	ETH_StructInit(&ETH_InitStructure);
	
	int ret = ETH_Init(&ETH_InitStructure,PHY_ADDR);
	if(ret == ETH_SUCCESS){
		ETH_DMADescsChainInit();
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);
	}
}
uint8_t* ETH_Frame_Received(void)
{  
	uint8_t *ptr = NULL;;
	
	FrameTypeDef frame = ETH_Get_Received_Frame_interrupt();

	ptr = malloc(frame.length * sizeof(uint8_t));
	if(ptr == NULL)
		return NULL;

	Mem_Copy(ptr,(uint8_t*)frame.buffer,frame.length);
	
	frame.descriptor->Status = ETH_DMARxDesc_OWN;
	if((ETH->DMASR&ETH_DMASR_RBUS) != (uint32_t)RESET)
	{
		ETH->DMASR=ETH_DMASR_RBUS;
		ETH->DMARPDR=0;
	}
	return ptr;
}


void ETH_Frame_Send(uint8_t *Buffer_Load,uint32_t framelength)
{
  uint32_t bufferoffset = 0;
	uint32_t byteslefttocopy = framelength;
	
  uint8_t *DMA_Buffer =  (uint8_t *)(DMATxDescToSet->Buffer1Addr);
  __IO ETH_DMADESCTypeDef *DmaTxDesc = DMATxDescToSet;
	
	while(byteslefttocopy > 0){	
		if((DmaTxDesc->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
		{
			goto error;
		}
		if(byteslefttocopy >= ETH_TX_BUF_SIZE){
			Mem_Copy(DMA_Buffer,&Buffer_Load[bufferoffset],ETH_TX_BUF_SIZE);
			
			byteslefttocopy = byteslefttocopy - ETH_TX_BUF_SIZE;
			bufferoffset = bufferoffset + ETH_TX_BUF_SIZE;
			
			DmaTxDesc = (ETH_DMADESCTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
			DMA_Buffer =  (uint8_t *)(DmaTxDesc->Buffer1Addr);
			
		}else{
			Mem_Copy(DMA_Buffer,&Buffer_Load[bufferoffset],byteslefttocopy);
			byteslefttocopy = 0;
		}
	}
  ETH_Prepare_Transmit_Descriptors(framelength);

error:
  if ((ETH->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
  {
    ETH->DMASR = ETH_DMASR_TUS;
    ETH->DMATPDR = 0;
  }
}


void ETH_IRQHandler(void)
{
	uint8_t* FrameBuffer = NULL;
	while(ETH_CheckFrameReceived() != 0)
	{
		FrameBuffer = ETH_Frame_Received();
		if(FrameBuffer != NULL){
			free(FrameBuffer);
			FrameBuffer = NULL;
		}
	}
	
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}  

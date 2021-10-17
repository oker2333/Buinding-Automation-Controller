#include "at24cxx.h"

#define IIC_PageSize 8
#define IIC_SlaveAddress 0xA0
#define Delay(N) delay_us(N);

/*写周期时间是指从一个写时序的有效停止信号到内部编程/擦除周期结束的这一段时间 在写周期期
间 总线接口电路禁能 SDA 保持为高电平 器件不响应外部操作.故需要延時*/
void delay_us(u32 us)
{
		u8 i = 0;
		while(us--)
		{
			i = 42;
			while(i--);
		};
}

void I2C1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//必须设置为开漏输出，实现iic的线与逻辑
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
}

void I2C1_Configuration(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0XA0;//主机的地址        
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;//400KHZ
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);                                             
}

void I2C1_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   
    I2C1_GPIO_Configuration();
    I2C1_Configuration();
}

uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress,uint8_t ByteAddress)
{
    if(NumByteToWrite==0)
        return 1;
		
    /* 1.开始*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)){};

    /* 2.设备地址·/写 */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){};

		/* 3.字节地址*/
		I2C_SendData(I2Cx, ByteAddress);
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){};
		
    /* 4.连续写数据 */
    while(NumByteToWrite--)
    {
      I2C_SendData(I2Cx, *pBuffer);
      while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){};
      pBuffer++;
    }

    /* 5.停止 */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while ((I2Cx->CR1&0x200) == 0x200);
    return 0;
}

uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress)
{
    if(NumByteToRead==0)
        return 1;

    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));  
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
		
    /* 1.开始*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
		
    /* 2.设备地址·/写 */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.开始*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
		
    /* 4.设备地址·/读 */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 5.连续写数据 */
    while (NumByteToRead)
    {
        if(NumByteToRead==1)
        {
            I2C_AcknowledgeConfig(I2Cx, DISABLE);
            I2C_GenerateSTOP(I2Cx, ENABLE);//6.停止，非应答
        }
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
        *pBuffer++ = I2C_ReceiveData(I2Cx);
        NumByteToRead--;
    }
		
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    return 0;
}
 
void I2C_BufferWrite(u8* pBuffer, u8 WriteAddr,
						u32 NumByteToWrite)
{
	u8 NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,temp = 0;
	
	Addr = WriteAddr % IIC_PageSize;
	count = IIC_PageSize - Addr;
	NumOfPage = NumByteToWrite / IIC_PageSize;
 	NumOfSingle = NumByteToWrite % IIC_PageSize;
	
	if (Addr == 0){
		if (NumOfPage == 0) {
			I2C_Master_BufferWrite(I2C1, pBuffer, NumByteToWrite, IIC_SlaveAddress,WriteAddr);
			Delay(1000);
		}
		else {
			while (NumOfPage--) {
				I2C_Master_BufferWrite(I2C1, pBuffer, IIC_PageSize, IIC_SlaveAddress,WriteAddr);
				Delay(1000);
				WriteAddr += IIC_PageSize;
				pBuffer += IIC_PageSize;
			}
			if (NumOfSingle!=0) {
				I2C_Master_BufferWrite(I2C1, pBuffer, NumOfSingle, IIC_SlaveAddress,WriteAddr);
				Delay(1000);
			}
		}
	}
	else {
		if (NumOfPage== 0) {
				if (NumOfSingle > count) {
					temp = NumOfSingle - count;
					I2C_Master_BufferWrite(I2C1, pBuffer, count, IIC_SlaveAddress,WriteAddr);
					Delay(1000);
					WriteAddr += count;
					pBuffer += count;

					I2C_Master_BufferWrite(I2C1, pBuffer, temp, IIC_SlaveAddress,WriteAddr);
					Delay(1000);
				}
				else {
					I2C_Master_BufferWrite(I2C1, pBuffer, NumByteToWrite, IIC_SlaveAddress,WriteAddr);
					Delay(1000);
				}
		}
		else {
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / IIC_PageSize;
			NumOfSingle = NumByteToWrite % IIC_PageSize;
			
			if (count != 0) {
				I2C_Master_BufferWrite(I2C1, pBuffer, count, IIC_SlaveAddress,WriteAddr);
				Delay(1000);
				WriteAddr += count;
				pBuffer += count;
			}
			while (NumOfPage--) {
				I2C_Master_BufferWrite(I2C1, pBuffer, IIC_PageSize, IIC_SlaveAddress,WriteAddr);
				Delay(1000);
				WriteAddr += IIC_PageSize;
				pBuffer += IIC_PageSize;
			}
			if (NumOfSingle != 0) {
				I2C_Master_BufferWrite(I2C1, pBuffer, NumOfSingle, IIC_SlaveAddress,WriteAddr);
				Delay(1000);
			}
		}
	}
}




































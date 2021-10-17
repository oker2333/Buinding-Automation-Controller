#ifndef AT24CXX_H_
#define AT24CXX_H_

#include <stdint.h>
#include "stm32f4xx_conf.h"

void I2C1_Init(void);
uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress,uint8_t ByteAddress);
uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress);

#endif
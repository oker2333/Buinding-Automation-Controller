#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include "stm32f4xx_conf.h"
#include "dma.h"

#define DEBUG_ENABLE 1

#if DEBUG_ENABLE

#define print_info(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[INFO]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)
#else

#define print_info(format,...)
		
#endif

void debug_init(uint32_t BaudRate);

#endif
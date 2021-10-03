#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include "stm32f4xx_conf.h"
#include "dma.h"

typedef enum{
	OFF = 7,
	FATAL = 6,
	Error = 5,
	WARN = 4,
	INFO = 3,
	DEBUG = 2,
	TRACE = 1,
	ALL = 0
}LOG_LEVEL_T;

#define DEBUG_ENABLE 1

#define LOG_LEVEL ALL

#define CCMRAM  __attribute__((section("CCMRAM")))

#if DEBUG_ENABLE

/*
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */

#define CSI_START                      "\033["
#define CSI_END                        "\033[0m"
/* output log front color */
#define F_BLACK                        "30;"
#define F_RED                          "31;"
#define F_GREEN                        "32;"
#define F_YELLOW                       "33;"
#define F_BLUE                         "34;"
#define F_MAGENTA                      "35;"
#define F_CYAN                         "36;"
#define F_WHITE                        "37;"
/* output log background color */
#define B_NULL
#define B_BLACK                        "40;"
#define B_RED                          "41;"
#define B_GREEN                        "42;"
#define B_YELLOW                       "43;"
#define B_BLUE                         "44;"
#define B_MAGENTA                      "45;"
#define B_CYAN                         "46;"
#define B_WHITE                        "47;"
/* output log fonts style */
#define S_BOLD                         "1m"
#define S_UNDERLINE                    "4m"
#define S_BLINK                        "5m"
#define S_NORMAL                       "22m"

/*********************
��־���� 	| ���� 		---|--- 
OFF		| �رգ���߼��𣬲���ӡ��־��
FATAL	| ������ָ���ǳ����صĿ��ܻᵼ��Ӧ����ִֹ�д����¼��� 
ERROR | ����ָ�������¼�����Ӧ�ÿ��ܻ��ܼ������С� 
WARN	| ���棺ָ������Ǳ�ڵ�Σ��״���� 
INFO	| ��Ϣ��ָ��������Ϣ���Ӵ�������������Ӧ�����й��̡� 
DEBUG	| ���ԣ�ָ��ϸ�µ��¼���Ϣ���Ե���Ӧ�������á� 
TRACE	| ���٣�ָ���������й켣����DEBUG��������ȸ�ϸ�� 
ALL		| ���У�������־���𣬰������Ƽ���
**********************/
#if (LOG_LEVEL >= FATAL)
#define Log_Fatal(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[FATAL]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)
#if(LOG_LEVEL >= ERROR)
#define Log_Error(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[ERROR]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)
#if(LOG_LEVEL >= WARN)
#define Log_Warn(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[WARN]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)
#if(LOG_LEVEL >= INFO)
#define Log_Info(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[INFO]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)														
#if(LOG_LEVEL >= DEBUG)
#define Log_Debug(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[DEBUG]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)
#if(LOG_LEVEL >= TRACE)
#define Log_Trace(format,...)	do{	\
																uint8_t StringSize = \
																sprintf(DebugBuffer,"[TRACE]FileName:%s; Line:%d; Function:%s."\
																format"\r\n",__FILE__,__LINE__,__func__,##__VA_ARGS__);	\
																DMA2_Stream7_Send(StringSize);	\
														}while(0)


#endif	//TRACE
#endif	//DEBUG
#endif	//INFO
#endif	//WARN
#endif	//ERROR
#endif	//FATAL
#endif	//DEBUG_ENABLE

void debug_init(uint32_t BaudRate);

#endif
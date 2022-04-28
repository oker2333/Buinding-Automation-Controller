/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include "main.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_TASK_EQ_0_ITERATION_NBR              16u
#define DEBUG_LOG_MSG_MAX                         7u


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
                                                                /* ------------ FLOATING POINT TEST TASK -------------- */
static  OS_TCB       App_TaskEq0FpTCB;
static  CPU_STK      App_TaskEq0FpStk[APP_CFG_TASK_EQ_STK_SIZE];

static  OS_TCB       App_TaskLogTCB;
static  CPU_STK      App_TaskLogStk[APP_CFG_TASK_LOG_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);

static  void  App_TaskEq0Fp         (void  *p_arg);             /* Floating Point Equation 0 task.                      */
static  void  App_TaskLogPrint      (void  *p_arg);
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;
		
    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    BSP_Init();
		BSP_Tick_Init();                                            /* Initialize Tick Services.                            */
	
    CPU_Init();                                                 /* Initialize the uC/CPU Services                       */
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    AppTaskCreate();                                            /* Create Application tasks                             */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
				LED_Invert(GPIO_Pin_9);
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, 0u, &err);
				LED_Invert(GPIO_Pin_10);
				OSTimeDlyHMSM(0u, 0u, 0u, 500u, 0u, &err);

    }
}


/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  os_err;
    
                                                                /* ------------- CREATE FLOATING POINT TASK ----------- */
    OSTaskCreate((OS_TCB      *)&App_TaskEq0FpTCB,
                 (CPU_CHAR    *)"FP Equation",
                 (OS_TASK_PTR  ) App_TaskEq0Fp, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_EQ_PRIO,
                 (CPU_STK     *)&App_TaskEq0FpStk[0],
                 (CPU_STK_SIZE ) App_TaskEq0FpStk[APP_CFG_TASK_EQ_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_EQ_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);

    OSTaskCreate((OS_TCB      *) &App_TaskLogTCB,
                 (CPU_CHAR    *) "Log Print",
                 (OS_TASK_PTR  ) App_TaskLogPrint, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_LOG_PRIO,
                 (CPU_STK     *) &App_TaskLogStk[0],
                 (CPU_STK_SIZE ) App_TaskLogStk[APP_CFG_TASK_LOG_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_LOG_STK_SIZE,
                 (OS_MSG_QTY   ) DEBUG_LOG_MSG_MAX,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       ) OS_OPT_TASK_NONE,
                 (OS_ERR      *) &os_err);								 
}

/*
*********************************************************************************************************
*                                             App_TaskEq0Fp()
*
* Description : This task finds the root of the following equation.
*               f(x) =  e^-x(3.2 sin(x) - 0.5 cos(x)) using the bisection mehtod
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEq0Fp' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  App_TaskEq0Fp (void  *p_arg)
{
		OS_ERR err;
    while (DEF_TRUE) {
			DMA1_Stream6_Send(11);
			OSTimeDlyHMSM(0u, 0u, 1u, 0u, 0u, &err);
			DMA1_Stream6_Send(5);
			OSTimeDlyHMSM(0u, 0u, 1u, 0u, 0u, &err);
    }
}

static  void  App_TaskLogPrint(void *p_arg)
{
		OS_ERR err;
		OS_MSG_SIZE msg_size;
		uint8_t* pbuf = NULL;
		uint32_t Transmit_Time = 0;

		while(DEF_TRUE){
				pbuf =  OSTaskQPend ((OS_TICK)       0,
									(OS_OPT)        OS_OPT_PEND_BLOCKING,
									(OS_MSG_SIZE*)  &msg_size,
									(CPU_TS*)       0,
									(OS_ERR*)       &err);
			
				if(err == OS_ERR_NONE){
					Mem_Copy(LogBuffer,pbuf,msg_size);
					DMA2_Stream7_Send(msg_size);
				}
				
				if(pbuf != NULL){
					free(pbuf);
					pbuf = NULL;
				}
				Transmit_Time = 1 + 1000*msg_size*10/115200;
				OSTimeDlyHMSM(0u, 0u, 0u, Transmit_Time, 0u, &err);
		}
}

void log_Q_post(uint8_t* pbuf,uint32_t len){
		OS_ERR err;
		OSTaskQPost ((OS_TCB*)       &App_TaskLogTCB,
				 (void*)				 pbuf,
				 (OS_MSG_SIZE)   len,
				 (OS_OPT)        (OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED),
				 (OS_ERR*)       &err);
		if(err != OS_ERR_NONE){
			free(pbuf);
			pbuf = NULL;
		}
}

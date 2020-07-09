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

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static  OS_TCB   AppTaskLed1TCB;
static  OS_TCB   AppTaskLed2TCB;
static  OS_TCB   AppTaskLed3TCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];
static  CPU_STK  AppTaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];
static  CPU_STK  AppTaskLed3Stk [ APP_TASK_LED3_STK_SIZE ];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);

static  void  AppTaskLed1  ( void * p_arg );
static  void  AppTaskLed2  ( void * p_arg );
static  void  AppTaskLed3  ( void * p_arg );

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

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
		if (err != OS_ERR_NONE)
		{
			while(1);
		}
		
    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
		
		if (err != OS_ERR_NONE)
		{
			while(1);
		}
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
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;
	
    CPU_Init();
    BSP_Init();                                                 /* Initialize BSP functions                             */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                /* Create the Led1 task                                */
                 (CPU_CHAR   *)"App Task Led1",
                 (OS_TASK_PTR ) AppTaskLed1,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,
                 (CPU_STK    *)&AppTaskLed1Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
    OSTaskCreate((OS_TCB     *)&AppTaskLed2TCB,                /* Create the Led2 task                                */
                 (CPU_CHAR   *)"App Task Led2",
                 (OS_TASK_PTR ) AppTaskLed2,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,
                 (CPU_STK    *)&AppTaskLed2Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&AppTaskLed3TCB,                /* Create the Led3 task                                */
                 (CPU_CHAR   *)"App Task Led3",
                 (OS_TASK_PTR ) AppTaskLed3,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,
                 (CPU_STK    *)&AppTaskLed3Stk[0],
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
		
		
		OSTaskDel ( & AppTaskStartTCB, & err );
		
}
/*
*********************************************************************************************************
*                                          LED1 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed1 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;	

	
	(void)p_arg;


	while (DEF_TRUE) {                                   //任务体，通常写成一个死循环
		LED1_TOGGLE;                                 //切换 LED1 的亮灭状态
		
		value = OSTaskRegGet ( 0, 0, & err );              //获取自身任务寄存器值
		
		if ( value < 10 )                                  //如果任务寄存器值<10
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );          //继续累加任务寄存器值
		}		
		else                                               //如果累加到10
		{
			OSTaskRegSet ( 0, 0, 0, & err );                 //将任务寄存器值归0
			
			OSTaskResume ( & AppTaskLed2TCB, & err );        //恢复 LED2 任务
      printf("恢复LED2任务！\n");
			 
			OSTaskResume ( & AppTaskLed3TCB, & err );        //恢复 LED3 任务
			printf("恢复LED3任务！\n");
		}
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );        //相对性延时1000个时钟节拍（1s）
		
	}
		
		
}


/*
*********************************************************************************************************
*                                          LED2 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed2 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (DEF_TRUE) {                                 //任务体，通常写成一个死循环
		LED2_TOGGLE;                               //切换 LED2 的亮灭状态
		
		value = OSTaskRegGet ( 0, 0, & err );            //获取自身任务寄存器值
		
		if ( value < 5 )                                 //如果任务寄存器值<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //继续累加任务寄存器值
		}
		else                                             //如果累加到5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //将任务寄存器值归0
			
			OSTaskSuspend ( 0, & err );                    //挂起自身
			printf("挂起LED2任务（自身）！\n");
		}
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		
	}
		
		
}


/*
*********************************************************************************************************
*                                          LED3 TASK
*********************************************************************************************************
*/

static  void  AppTaskLed3 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (DEF_TRUE) {                                 //任务体，通常写成一个死循环
		LED3_TOGGLE;                               //切换 LED3 的亮灭状态
		
		value = OSTaskRegGet ( 0, 0, & err );            //获取自身任务寄存器值
		
		if ( value < 5 )                                 //如果任务寄存器值<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //继续累加任务寄存器值
		}
		else                                             //如果累加到5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //将任务寄存器值归零
			
			OSTaskSuspend ( 0, & err );                    //挂起自身
			printf("挂起LED3任务（自身）！\n");
		}

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		
	}

		
}

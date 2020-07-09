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

OS_MUTEX TestMutex;          //标志KEY1是否被单击的多值互斥量

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static  OS_TCB   AppTaskKey1TCB;
static  OS_TCB   AppTaskKey2TCB;
static  OS_TCB   AppTaskKey3TCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  AppTaskKey1Stk [ APP_TASK_KEY1_STK_SIZE ];
static  CPU_STK  AppTaskKey2Stk [ APP_TASK_KEY2_STK_SIZE ];
static  CPU_STK  AppTaskKey3Stk [ APP_TASK_KEY2_STK_SIZE ];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);

static  void  AppTaskKey1  ( void * p_arg );
static  void  AppTaskKey2  ( void * p_arg );
static  void  AppTaskKey3  ( void * p_arg );

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
	
    CPU_Init();																									//初始化 CPU 组件（时间戳、关中断时间测量和主机名）
    BSP_Init();                                                 //板级初始化

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               //如果使能（默认使能）了统计任务,计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif																													//容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 使用率使用）。

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间
    printf("\r\n欢迎使用野火骄阳开发板.\r\n");
		/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX  *)&TestMutex,           //指向信号量变量的指针
                   (CPU_CHAR  *)"Mutex For Test", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型
							 

		/* 创建 AppTaskKey1 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskKey1TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Key1",                             //任务名称
                 (OS_TASK_PTR ) AppTaskKey1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_KEY1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskKey1Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

		/* 创建 AppTaskKey2 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskKey2TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Key2",                             //任务名称
                 (OS_TASK_PTR ) AppTaskKey2,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_KEY2_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskKey2Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

		/* 创建 AppTaskKey3 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskKey3TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Key3",                             //任务名称
                 (OS_TASK_PTR ) AppTaskKey3,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_KEY3_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskKey3Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_KEY3_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_KEY3_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
								 
		OSTaskDel ( & AppTaskStartTCB, & err );                     //删除起始任务本身，该任务不再运行
		
		
}

/*
*********************************************************************************************************
*                                          KEY1 TASK
*********************************************************************************************************
*/
static  void  AppTaskKey1 ( void * p_arg )
{
    OS_ERR      err;
    static uint32_t i;
  
   (void)p_arg;
  

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			
      printf("AppTaskLed1 获取互斥量\n");
      //获取 互斥量 ,没获取到则一直等待
		OSMutexPend ((OS_MUTEX  *)&TestMutex,                  //申请互斥信号量 mutex
								 (OS_TICK    )0,                       //无期限等待
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果不能申请到信号量就堵塞任务
								 (CPU_TS    *)0,                       //不想获得时间戳
								 (OS_ERR    *)&err);                   //返回错误类型		
		
      
      for(i=0;i<500000;i++)      //模拟低优先级任务占用互斥量
      {
//        ;
        OSSched();//发起任务调度
      }

      printf("AppTaskLed1 释放互斥量\n");
		OSMutexPost ((OS_MUTEX  *)&TestMutex,                  //释放互斥信号量 mutex
								 (OS_OPT     )OS_OPT_POST_NONE,        //进行任务调度
								 (OS_ERR    *)&err);                   //返回错误类型		
		
      
      
      LED1_TOGGLE;
			OSTimeDlyHMSM (0,0,1,0,OS_OPT_TIME_PERIODIC,&err);
    }
		
		
}


/*
*********************************************************************************************************
*                                          KEY2 TASK
*********************************************************************************************************
*/
static  void  AppTaskKey2 ( void * p_arg )
{
    OS_ERR      err;

  
   (void)p_arg;


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			printf("AppTaskLed2 Running\n");
      
			OSTimeDlyHMSM (0,0,0,200,OS_OPT_TIME_PERIODIC,&err);
    }
	
}

/*
*********************************************************************************************************
*                                          KEY3 TASK
*********************************************************************************************************
*/
static  void  AppTaskKey3 ( void * p_arg )
{
	    OS_ERR      err;

   (void)p_arg;


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			
      printf("AppTaskLed3 获取互斥量\n");	
      //获取 互斥量 ,没获取到则一直等待
		OSMutexPend ((OS_MUTEX  *)&TestMutex,                  //申请互斥信号量 mutex
								 (OS_TICK    )0,                       //无期限等待
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果不能申请到信号量就堵塞任务
								 (CPU_TS    *)0,                       //不想获得时间戳
								 (OS_ERR    *)&err);                   //返回错误类型		
		
      LED3_TOGGLE;
      
      printf("AppTaskLed3 释放互斥量\n");
      OSMutexPost ((OS_MUTEX  *)&TestMutex,                  //释放互斥信号量 mutex
								  (OS_OPT     )OS_OPT_POST_NONE,        //进行任务调度
								  (OS_ERR    *)&err);                   //返回错误类型		
		
      
			OSTimeDlyHMSM (0,0,1,0,OS_OPT_TIME_PERIODIC,&err);
      
    }
		
		
}


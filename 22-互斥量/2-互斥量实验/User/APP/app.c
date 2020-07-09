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

OS_MUTEX TestMutex;          //��־KEY1�Ƿ񱻵����Ķ�ֵ������

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
	
    CPU_Init();																									//��ʼ�� CPU �����ʱ��������ж�ʱ���������������
    BSP_Init();                                                 //�弶��ʼ��

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������,����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif																													//���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU ʹ����ʹ�ã���

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��
    printf("\r\n��ӭʹ��Ұ����������.\r\n");
		/* ���������ź��� mutex */
    OSMutexCreate ((OS_MUTEX  *)&TestMutex,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"Mutex For Test", //�ź���������
                   (OS_ERR    *)&err);            //��������
							 

		/* ���� AppTaskKey1 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKey1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key1",                             //��������
                 (OS_TASK_PTR ) AppTaskKey1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskKey1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

		/* ���� AppTaskKey2 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKey2TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key2",                             //��������
                 (OS_TASK_PTR ) AppTaskKey2,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY2_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskKey2Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

		/* ���� AppTaskKey3 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKey3TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key3",                             //��������
                 (OS_TASK_PTR ) AppTaskKey3,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY3_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskKey3Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY3_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY3_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
								 
		OSTaskDel ( & AppTaskStartTCB, & err );                     //ɾ����ʼ������������������
		
		
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
			
      printf("AppTaskLed1 ��ȡ������\n");
      //��ȡ ������ ,û��ȡ����һֱ�ȴ�
		OSMutexPend ((OS_MUTEX  *)&TestMutex,                  //���뻥���ź��� mutex
								 (OS_TICK    )0,                       //�����޵ȴ�
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //����������뵽�ź����Ͷ�������
								 (CPU_TS    *)0,                       //������ʱ���
								 (OS_ERR    *)&err);                   //���ش�������		
		
      
      for(i=0;i<500000;i++)      //ģ������ȼ�����ռ�û�����
      {
//        ;
        OSSched();//�����������
      }

      printf("AppTaskLed1 �ͷŻ�����\n");
		OSMutexPost ((OS_MUTEX  *)&TestMutex,                  //�ͷŻ����ź��� mutex
								 (OS_OPT     )OS_OPT_POST_NONE,        //�����������
								 (OS_ERR    *)&err);                   //���ش�������		
		
      
      
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
			
      printf("AppTaskLed3 ��ȡ������\n");	
      //��ȡ ������ ,û��ȡ����һֱ�ȴ�
		OSMutexPend ((OS_MUTEX  *)&TestMutex,                  //���뻥���ź��� mutex
								 (OS_TICK    )0,                       //�����޵ȴ�
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //����������뵽�ź����Ͷ�������
								 (CPU_TS    *)0,                       //������ʱ���
								 (OS_ERR    *)&err);                   //���ش�������		
		
      LED3_TOGGLE;
      
      printf("AppTaskLed3 �ͷŻ�����\n");
      OSMutexPost ((OS_MUTEX  *)&TestMutex,                  //�ͷŻ����ź��� mutex
								  (OS_OPT     )OS_OPT_POST_NONE,        //�����������
								  (OS_ERR    *)&err);                   //���ش�������		
		
      
			OSTimeDlyHMSM (0,0,1,0,OS_OPT_TIME_PERIODIC,&err);
      
    }
		
		
}


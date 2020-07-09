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

OS_TCB           AppTaskUsartTCB;
OS_TCB           AppTaskKeyTCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  AppTaskUsartStk [ APP_TASK_USART_STK_SIZE ];
static  CPU_STK  AppTaskKeyStk   [ APP_TASK_KEY_STK_SIZE ];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);

static  void  AppTaskUsart  ( void * p_arg );
static  void  AppTaskKey    ( void * p_arg );
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
    /* ����ʱ��Ƭ��ת���� */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //ʹ��ʱ��Ƭ��ת����
		                     (OS_TICK       )0,                    //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
												 (OS_ERR       *)&err );               //���ش�������

							 
							 
		/* ���� AppTaskUsart ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskUsartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Usart",                            //��������
                 (OS_TASK_PTR ) AppTaskUsart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
								 
								 
		/* ���� AppTaskKey ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                              //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key",                              //��������
                 (OS_TASK_PTR ) AppTaskKey,                                 //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,                          //��������ȼ�
                 (CPU_STK    *)&AppTaskKeyStk[0],                           //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,                 //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,                      //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

								 
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������
		
		
}

/*
*********************************************************************************************************
*                                          USART TASK
*********************************************************************************************************
*/
static  void  AppTaskUsart ( void * p_arg )
{
	OS_ERR         err;
  
	CPU_SR_ALLOC();
	

	(void)p_arg;

					 
	while (DEF_TRUE) {                                           //������

		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)0,                     //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������

		OS_CRITICAL_ENTER();                                       //�����ٽ�Σ����⴮�ڴ�ӡ�����
    
    printf("�յ�����:%s\n",Usart_Rx_Buf);

    memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */

		OS_CRITICAL_EXIT();                                        //�˳��ٽ��
		
	
		
	}
	
}


/*
*********************************************************************************************************
*                                          KEY TASK
*********************************************************************************************************
*/
static  void  AppTaskKey ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();

	
	(void)p_arg;

					 
  cpu_clk_freq = BSP_CPU_ClkFreq();                   //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���

	
	while (DEF_TRUE) {                                  //������
		/* ��������ֱ��KEY1������ */
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)0,                     //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
		
		ts_int = CPU_IntDisMeasMaxGet ();                 //��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����

		printf ( "���������ж�,����ж�ʱ����%dus\r\n", 
						ts_int / ( cpu_clk_freq / 1000000 ) );        		
		
		OS_CRITICAL_EXIT();                               //�˳��ٽ��
		
	}
	
}

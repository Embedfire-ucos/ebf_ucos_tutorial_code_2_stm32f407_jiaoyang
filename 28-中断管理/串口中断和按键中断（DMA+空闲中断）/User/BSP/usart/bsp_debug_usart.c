/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重现c库printf函数到usart端口,使用DMA模式发送数据
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"
#include "includes.h"

DMA_HandleTypeDef  DMA_Handle;      //DMA句柄
UART_HandleTypeDef UartHandle;      //UART句柄

uint8_t Usart_Rx_Buf[USART_RBUFF_SIZE];

 /**
  * @brief  USART 中断配置
  * @param  无
  * @retval 无
  */
void USART_NVIC_Config(void)
{
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn,7,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
	
	HAL_NVIC_SetPriority(USART1_IRQn,8,0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

 /**
  * @brief  USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
      
  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  /* 使能 UART 时钟 */
  DEBUG_USART_CLK_ENABLE();
  
   /* 配置Tx引脚为复用功能  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置Rx引脚为复用功能 */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);
  
  
  UartHandle.Instance          = DEBUG_USART;
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle); 
	
	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_IDLE);
}

///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

/**
  * @brief  USART1 TX DMA 配置，外设到内存(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART_DMA_Config(void)
{
  DEBUG_USART_DMA_CLK_ENABLE();  
   
  //Tx DMA配置
  DMA_Handle.Instance=DEBUG_USART_DMA_STREAM;                            //数据流选择
  DMA_Handle.Init.Channel=DEBUG_USART_DMA_CHANNEL;                                //通道选择
  DMA_Handle.Init.Direction=DMA_PERIPH_TO_MEMORY;             //外设到存储器
  DMA_Handle.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
  DMA_Handle.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
  DMA_Handle.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
  DMA_Handle.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
  DMA_Handle.Init.Mode=DMA_CIRCULAR;                            //外设普通模式
  DMA_Handle.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
  DMA_Handle.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              //禁用FIFO
  DMA_Handle.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
  DMA_Handle.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
  DMA_Handle.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
//	DMA_Handle.XferCpltCallback = callback;
  HAL_DMA_Init(&DMA_Handle);
  /* Associate the DMA handle */
  __HAL_LINKDMA(&UartHandle, hdmarx, DMA_Handle); 
	__HAL_DMA_ENABLE_IT(UartHandle.hdmarx, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE);//关闭DMA 错误 传输一半 全部完成 中断
	HAL_UART_Receive_DMA(&UartHandle,Usart_Rx_Buf,USART_RBUFF_SIZE);

}

void USART_Init(void)
{
	Debug_USART_Config();
	USART_DMA_Config();
	USART_NVIC_Config();
}

/* 外部定义信号量控制块 */
extern OS_TCB	 AppTaskUsartTCB;

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
	OS_ERR   err;
	if(huart == &UartHandle)
	{
    /* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   //目标任务
									(OS_OPT   )OS_OPT_POST_NONE, //没选项要求
									(OS_ERR  *)&err);            //返回错误类型
		//重新开始DMA接收
		HAL_UART_Receive_DMA(&UartHandle,Usart_Rx_Buf,USART_RBUFF_SIZE);
	}
}


/*********************************************END OF FILE**********************/

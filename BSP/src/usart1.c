#include "usart1.h"
//C库
#include <stdarg.h>
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

ALTER_INFO alterInfo;



/**
  * @brief  USART1初始化函数
**/
void USART1_Init(uint32_t baud)
{
	GPIO_InitTypeDef  		GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	//打开所用GPIO及串口1的时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO , ENABLE); 

	//配置的IO是PA9，串口1的TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置的IO是PA10，串口1的RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置串口1的硬件参数
	USART_DeInit(USART1);                                            //将外设USART1寄存器重设为缺省值
	USART_InitStructure.USART_BaudRate = baud;                      //设置串口1波特率为19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART1, &USART_InitStructure);    

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
	USART_ClockInit(USART1, &USART_ClockInitStructure); 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                   //使能USART1接收中断

	USART_Cmd(USART1, ENABLE);                                       //使能USART1外设
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	               //配置为串口1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;          //先占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //使能中断通道
	NVIC_Init(&NVIC_InitStructure);
}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	unsigned char UsartPrintfBuf[256],len;
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	len = vsprintf((char *)UsartPrintfBuf, fmt, ap);							//格式化
	va_end(ap);
	
	while(len--)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
}


/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USART1_IRQHandler(void)
{
	
	rt_interrupt_enter();

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
	{
        if(alterInfo.alterCount > strlen(alterInfo.alterBuf))
            alterInfo.alterCount = 0;
        alterInfo.alterBuf[alterInfo.alterCount++] = USART1->DR;
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
	
	rt_interrupt_leave();

}

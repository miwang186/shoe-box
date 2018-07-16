#include "net_io.h"

#include "delay.h"

#include <string.h>




NET_IO_INFO netIOInfo;




void NET_IO_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = 115200;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收和发送
	usartInitStruct.USART_Parity = USART_Parity_No; //无校验
	usartInitStruct.USART_StopBits = USART_StopBits_1; //1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b; //8位数据位
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE); //使能串口
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //使能接收中断
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
	
	NET_IO_ClearRecive();

}

void NET_IO_Send(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(NET_IO, *str++);
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
	}

}

//返回0代表ok		返回1代表未完成
_Bool NET_IO_WaitRecive(void)
{

	if(netIOInfo.dataLen == 0) //如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre) //如果上一次的值和这次相同，则说明接收完毕
	{
		netIOInfo.dataLen = 0; //清0接收计数
			
		return REV_OK; //跳出
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen; //置为相同
	
	return REV_WAIT;

}

void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));

}

void USART2_IRQHandler(void)
{
	
	OSIntEnter();

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //防止串口被刷爆
		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
	
	OSIntExit();

}

/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	net_IO.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		网络设备数据IO层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f10x.h"  //单片机头文件

#include "net_io.h"		//网络设备数据IO

#include "delay.h"		//硬件驱动

#include <string.h>		//C库


NET_IO_INFO netIOInfo;


/**************************************************************************************
 * 描  述 : 初始化ESP8266模块用到的引脚
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void esp8266_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//打开所用GPIO
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE); 

	//配置的IO是PB10，串口3的TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//配置的IO是PB11，串口3的RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 
	//配置的IO是PE0，ESP8266复位引脚RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	//  //初始状态将ESP8266复位引脚置高
	NET_DEVICE_RST_OFF;
}

/*
************************************************************
*	函数名称：	NET_IO_Init
*
*	函数功能：	初始化网络设备IO驱动层
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		底层的数据收发驱动
************************************************************
*/
void NET_IO_Init(void)
{
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;

	//打开串口3的时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE); 

	esp8266_init();

	//配置串口3的硬件参数
	USART_DeInit(USART3);                                            //将外设USART3寄存器重设为缺省值
	USART_InitStructure.USART_BaudRate = 115200;                      //设置串口2波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART3, &USART_InitStructure);    

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
	USART_ClockInit(USART3, &USART_ClockInitStructure); 

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                   //使能USART3接收中断
	USART_Cmd(USART3, ENABLE);                                       //使能USART3外设

	//配置串口3中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //配置为串口3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;          //先占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //使能中断通道
	NVIC_Init(&NVIC_InitStructure);

	
	NET_IO_ClearRecive();

}

/*
************************************************************
*	函数名称：	NET_IO_Send
*
*	函数功能：	发送数据
*
*	入口参数：	str：需要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		底层的数据发送驱动
*
************************************************************
*/
void NET_IO_Send(unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)											//发送一帧数据
	{
		USART_SendData(NET_IO, *str++);
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
	}

}

/*
************************************************************
*	函数名称：	NET_IO_WaitRecive
*
*	函数功能：	等待接收完成
*
*	入口参数：	无
*
*	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
*
*	说明：		循环调用检测是否接收完成
************************************************************
*/
_Bool NET_IO_WaitRecive(void)
{

	if(netIOInfo.dataLen == 0) 						//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//如果上一次的值和这次相同，则说明接收完毕
	{
		netIOInfo.dataLen = 0;						//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

/*
************************************************************
*	函数名称：	NET_IO_ClearRecive
*
*	函数功能：	清空缓存
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));

}

/*
************************************************************
*	函数名称：	USART3_IRQHandler
*
*	函数功能：	接收中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USART3_IRQHandler(void)
{	
	rt_interrupt_enter();

	if(USART_GetITStatus(NET_IO, USART_IT_RXNE) != RESET) //接收中断
	{
		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //防止串口被刷爆
		netIOInfo.buf[netIOInfo.dataLen++] = NET_IO->DR;
		
		USART_ClearFlag(NET_IO, USART_FLAG_RXNE);
	}
	
	rt_interrupt_leave();
}

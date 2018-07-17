#include "stm32f10x.h"
#include "iic.h"	
#include "delay.h"

/**************************************************************************************
 * 描  述 : 初始化EEPROM用IIC所用到的IO口
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void IIC_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //打开所用GPIO的时钟
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 
  
  //配置的IO是PB6 PB7，SPI的SCL SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;           //开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************************
 * 描  述 : 模拟IIC总线产生启动信号
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void IIC_Start(void)
{
	IIC_SDA_1;	  	  
	IIC_SCL_1;
	sw_delay_us(4);
 	IIC_SDA_0;                        //当SCL高电平时，SDA出现一个下跳沿表示IIC总线启动信号
	sw_delay_us(4);
	IIC_SCL_0;      
  sw_delay_us(4);  
}

/**************************************************************************************
 * 描  述 : 模拟IIC总线产生停止信号
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void IIC_Stop(void)
{
	IIC_SDA_0; 	
	IIC_SCL_1; 
  sw_delay_us(4);
	IIC_SDA_1;                        //当SCL高电平时，SDA出现一个上跳沿表示IIC总线停止信号
	sw_delay_us(4);
}

/**************************************************************************************
 * 描  述 : 产生ACK应答信号
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void IIC_Ack(void)
{
	IIC_SDA_0;
	sw_delay_us(4);
	IIC_SCL_1;
	sw_delay_us(4);
	IIC_SCL_0;
  sw_delay_us(4);
  IIC_SDA_1;
}

/**************************************************************************************
 * 描  述 : 产生NACK应答信号
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void IIC_NAck(void)
{
	IIC_SDA_1;
	sw_delay_us(4);
	IIC_SCL_1;
	sw_delay_us(4);
	IIC_SCL_0;
  sw_delay_us(4);
}	

/************************************************************************************
功能描述：等待应答信号到来
入口参数：无
返回值：1，接收应答失败；0，接收应答成功
*************************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t temp;

	IIC_SDA_1;	                  //释放SDA总线
	sw_delay_us(4);
	IIC_SCL_1;	                  //驱动SCL为高电平, 此时器件会返回ACK应答
	sw_delay_us(4);
	if (IIC_SDA_READ)	            //读取SDA口线状态
	{
		temp = 1;
	}
	else
	{
		temp = 0;
	}
	IIC_SCL_0;
	sw_delay_us(4);
	return temp;
}

/************************************************************************************
功能描述：字节数据发送函数
入口参数：Data，可以是地址，也可以是数据
返回值：无 
*************************************************************************************/
void IIC_Send_Byte(uint8_t Data)
{
	uint8_t BitCnt;

	// 先发送字节的高位bit7 
	for (BitCnt = 0; BitCnt < 8; BitCnt++)
	{		
		if (Data & 0x80)
		{
			IIC_SDA_1;
		}
		else
		{
			IIC_SDA_0;
		}
		sw_delay_us(4);
		IIC_SCL_1;
		sw_delay_us(4);	
		IIC_SCL_0;
		if (BitCnt == 7)
		{
			IIC_SDA_1;          // 释放IIC总线
		}
		Data <<= 1;	          // 左移一个bit
		sw_delay_us(4);
	}
}

/************************************************************************************
功能描述：用来接收从器件传来的数据，并判断总线错误（发应答信号）
入口参数：ack=1:发送应答信号  ack=0:不发送应答信号
返回值：读1个字节的数据 
*************************************************************************************/
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,temp=0;

  for(i=0;i<8;i++ )
	{
     IIC_SCL_0; 
     sw_delay_us(4);
		 IIC_SCL_1;
     temp<<=1;
     if(IIC_SDA_READ)	            //读取SDA口线状态
		 {
			 temp++;
		 }  
		 sw_delay_us(4); 
   }					 
    if (!ack)
      IIC_NAck();                //发送nACK
    else
      IIC_Ack();                 //发送ACK   
    return temp;
}

/*********************************END FILE********************************************/	


#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI1的初始化

SPI_InitTypeDef  SPI_InitStructure;

void SPI2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
    
	// Enable SPI2 and GPIO clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //SPI2时钟使能
	//打开所用GPIO的时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 

	//配置的IO是PB12，SPI的CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//配置的IO是PB13 PB15，SPI的CLK MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//配置的IO是PB14，SPI的MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_12);                          //该步不可少！！！

	SPI_Cmd(SPI2, DISABLE); 
	/* SPI2 configuration */                                              //初始化SPI结构体
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI设置为双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPI时钟空闲时为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //第一个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //SPI2波特率预分频值为32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式


	SPI_Init(SPI2, &SPI_InitStructure);      //根据SPI_InitStruct中指定的参数初始化外设SPI2寄存器
	SPI_Cmd(SPI2, ENABLE);                   //使能SPI2外设
	SPI2_ReadWriteByte(0xff);                //启动传输	 
}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)
  
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE);
} 

/***************************************************************************
 * 描  述 : SPI2读写数据函数，读写一个字节
 * 入  参 : Dat：写入的数据
 * 返回值 : 读取的数据
 **************************************************************************/
uint8_t SPI2_ReadWriteByte(uint8_t Dat)                                       
{		
	uint8_t retry=0;				 	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)      //发送缓存标志位为空
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, Dat);                                        //通过外设SPI2发送一个数据
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)     //接收缓存标志位不为空
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);                                   //通过SPI2返回接收数据				    
}
































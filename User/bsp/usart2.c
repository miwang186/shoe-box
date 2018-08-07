#include "main.h"

volatile unsigned char  rcv_cmd_start=0;
volatile unsigned char  rcv_cmd_flag=0;

unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
volatile unsigned int   usart2_rcv_len=0;

unsigned char  usart2_cmd_buf[MAX_CMD_LEN];
volatile unsigned int   usart2_cmd_len=0;

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
	ESP8266_RST_H;
}

/**************************************************************************************
 * 描  述 : 初始化USART3并配置USART3中断优先级
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void USART3_Init(void)
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  //NVIC_Group:先占优先级2位，从优先级2位
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //配置为串口3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;          //先占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //使能中断通道
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  USART3发送一个字符串
**/
void usart2_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len)
{
    uint32_t i;
	
	USART_ClearFlag(USARTx,USART_FLAG_TC); 
    for(i=0; i<len; i++)
    {                                         
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

void USART3_IRQHandler(void)
{  		
	unsigned int data;

    if(USART3->SR & 0x0F)
    {
        // See if we have some kind of error
        // Clear interrupt (do nothing about it!)
        data = USART3->DR;
    }
		else if(USART3->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
    {		
        data = USART3->DR;
				usart2_rcv_buf[usart2_rcv_len++]=data;
				
				if(data=='{') //约定平台下发的控制命令以'{'为开始符，‘}’为控制命令结束符，读者可以自定义自己的开始符合结束符
				{
						rcv_cmd_start=1;
				}
				if(rcv_cmd_start==1)
				{
						usart2_cmd_buf[usart2_cmd_len++]=data;
						if((data=='}')||(usart2_cmd_len>=MAX_CMD_LEN-1))
						{
								rcv_cmd_start=0;
								LED_CmdCtl();
								memset(usart2_cmd_buf,0,usart2_cmd_len);
        				usart2_cmd_len=0;
						}
				}	  
    }
		else
		{
				;
		}
}	


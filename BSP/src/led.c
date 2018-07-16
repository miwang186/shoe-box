#include "led.h"

/**
  * @brief  LED指示灯初始化函数
**/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Pb6--对应开发板led2指示灯;Pb7--对应开发板led3指示灯;Pb8--对应开发板led4指示灯;Pb9--对应开发板led5指示灯;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//配置的IO是PG6
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;		
	//配置为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//IO口速度为10MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//配置IO
	GPIO_Init(GPIOG, &GPIO_InitStructure);	

	//配置的IO是PA8
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;	
	//配置IO
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}









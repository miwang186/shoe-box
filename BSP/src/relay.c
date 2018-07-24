#include "relay.h"

void Relay_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	RELAY1_OFF();
	RELAY2_OFF();
	
}

uint8_t Read_Relay_Status(uint8_t index)
{
	if(index == 1)
	{
		return !GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_14);
	}
	else
	{
		return !GPIO_ReadOutputDataBit(GPIOG,GPIO_Pin_9);		
	}
}



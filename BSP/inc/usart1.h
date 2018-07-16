#ifndef USART1_H_H
#define USART1_H_H
#include "stm32f10x.h"
#include "rtthread.h"
#define USART_DEBUG		USART1		//调试打印所使用的串口组

typedef struct
{

	char alterBuf[150];
	unsigned char alterCount;

} ALTER_INFO;

extern ALTER_INFO alterInfo;


void USART1_Init(uint32_t buad);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);



#endif


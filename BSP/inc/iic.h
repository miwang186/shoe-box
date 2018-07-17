#ifndef _IIC_H_
#define _IIC_H_

#include "stm32f10x.h"

#define  IIC_SDA_0     GPIO_ResetBits(GPIOB, GPIO_Pin_7) 
#define  IIC_SDA_1     GPIO_SetBits(GPIOB, GPIO_Pin_7) 
#define  IIC_SCL_0     GPIO_ResetBits(GPIOB, GPIO_Pin_6) 
#define  IIC_SCL_1     GPIO_SetBits(GPIOB, GPIO_Pin_6) 

#define  IIC_SDA_READ  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)	

void IIC_GPIO_Init(void);  
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t Data);
uint8_t IIC_Read_Byte(uint8_t ack);

#endif 


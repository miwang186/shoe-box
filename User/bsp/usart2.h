#ifndef USART2_H_H
#define USART2_H_H
#include "stm32f10x.h"

#define MAX_RCV_LEN  1024
#define MAX_CMD_LEN  256

#define ESP8266_RST_H   GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define ESP8266_RST_L   GPIO_ResetBits(GPIOE,GPIO_Pin_0)

extern void USART3_Init(void);
extern void usart2_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len);

extern volatile unsigned char  rcv_cmd_start;
extern volatile unsigned char  rcv_cmd_flag;

extern unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
extern volatile unsigned int   usart2_rcv_len;

extern unsigned char  usart2_cmd_buf[MAX_CMD_LEN];
extern volatile unsigned int   usart2_cmd_len;



#endif


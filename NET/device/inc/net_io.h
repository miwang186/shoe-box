#ifndef _NET_IO_H_
#define _NET_IO_H_

typedef struct
{
	
	unsigned short dataLen;			//接收数据长度
	unsigned short dataLenPre;		//上一次的长度数据，用于比较
	
	unsigned char buf[200];			//接收缓存

} NET_IO_INFO;

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

#define NET_IO			USART3		//ESP8266 串口

#define NET_DEVICE_RST_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define NET_DEVICE_RST_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_0)

extern NET_IO_INFO netIOInfo;


void NET_IO_Init(void);

void NET_IO_Send(unsigned char *str, unsigned short len);

_Bool NET_IO_WaitRecive(void);

void NET_IO_ClearRecive(void);


#endif

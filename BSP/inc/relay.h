#ifndef _RELAY_H_
#define _RELAY_H_

#include "stm32f10x.h"
#include "sys.h"
#define  RELAY1_OFF()    GPIO_SetBits(GPIOF,GPIO_Pin_14)
#define  RELAY1_ON()     GPIO_ResetBits(GPIOF,GPIO_Pin_14)

#define  RELAY2_OFF()    GPIO_SetBits(GPIOG,GPIO_Pin_9)
#define  RELAY2_ON()     GPIO_ResetBits(GPIOG,GPIO_Pin_9)


#define  RELAY1_KEY    PFout(14)
#define  RELAY2_KEY    PGout(9)

typedef struct
{
	uint32_t timing_sec;
	uint16_t motor_rate;
	_Bool 	 motor_key;		/*¼ÌµçÆ÷ 2*/
	_Bool 	 relay_key;		/*¼ÌµçÆ÷ 1*/
	_Bool 	 buzzer_key;		
	
}DEVICE_STATUS;

void Relay_Init(void);
uint8_t Read_Relay_Status(uint8_t index);
#endif 


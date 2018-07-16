#ifndef 	_BUZZER_H_
#define 	_BUZZER_H_
#include "stm32f10x.h"

#define  BUZZER_ON()    GPIO_SetBits(GPIOG,GPIO_Pin_7)
#define  BUZZER_OFF()   GPIO_ResetBits(GPIOG,GPIO_Pin_7)
void Buzzer_Init(void);

#endif 


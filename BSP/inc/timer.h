#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f10x.h"
#define OS_TIMER	TIM7

void TIME2_5PWM_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);
uint16_t Read_PWM_Arr(void);
void SET_PWM_Arr(uint16_t arr);

void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void RTOS_TimerInit(void);
#endif

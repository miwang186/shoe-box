#include "timer.h"
#include "led.h"
#include "GUI.h"
#include <rtthread.h>

void PWM_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);//使能GPIO外设和AFIO复用功能模块时钟
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //选择Timer3部分重映像    

	//选择定时器3的通道2作为PWM的输出引脚TIM3_CH2->PB5    GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 						//TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  				//复用推挽功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化引脚
}

void TIME2_5PWM_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;							//定义结构体	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;					//定义初始化结构体
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 			//使能定时器3时钟
	PWM_GPIO_Init();
	//初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; 						//自动重装载寄存器的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 						//TIMX预分频的值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					//时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); 				//根据以上功能对定时器进行初始化
	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;				//选择定时器模式，TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出比较极性低
	TIM_OC2Init(TIMx, &TIM_OCInitStructure);						//根据结构体信息进行初始化
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);  				//使能定时器TIM2在CCR2上的预装载值	
	TIM_Cmd(TIMx, ENABLE);  //使能定时器TIMx
}

uint16_t Read_PWM_Arr(void)
{
	return TIM_GetCapture2(TIM3);
}
void SET_PWM_Arr(uint16_t arr)
{
	TIM_SetCompare2(TIM3,arr);//得到占空比为50%的pwm波形
}

/*
************************************************************
*	函数名称：	Timer1_8_Init
*
*	函数功能：	Timer1或8的PWM配置
*
*	入口参数：	TIMx：TIM1 或者 TIM8
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	
	GPIO_InitTypeDef gpioInitStruct;
	TIM_TimeBaseInitTypeDef timerInitStruct;
	TIM_OCInitTypeDef timerOCInitStruct;

	if(TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	}
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStruct);	
	
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;				//选择定时器模式:TIM脉冲宽度调制模式2
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	timerOCInitStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIMx, &timerOCInitStruct);
	TIM_OC3Init(TIMx, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE 主输出使能	
	
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPE使能
	
	TIM_Cmd(TIMx, ENABLE);										//使能TIMx
}

/*
************************************************************
*	函数名称：	Timer6_7_Init
*
*	函数功能：	Timer6或7的定时配置
*
*	入口参数：	TIMx：TIM6 或者 TIM7
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer6和timer7只具有更新中断功能
************************************************************
*/
void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{

	TIM_TimeBaseInitTypeDef timerInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	if(TIMx == TIM6)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM6_IRQn;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM7_IRQn;
	}
	
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//使能更新中断
	
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvicInitStruct);
	
	TIM_Cmd(TIMx, ENABLE); //使能定时器

}

/*
************************************************************
*	函数名称：	RTOS_TimerInit
*
*	函数功能：	RTOS的心跳定时初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		APB1--36MHz，APB1总线分频不为1，则定时器时钟要乘以2
*				定时5ms
************************************************************
*/
void RTOS_TimerInit(void)
{
	Timer6_7_Init(TIM7,10000/RT_TICK_PER_SECOND, 7199);	//72MHz，7200分频-100us，50重载值。则中断周期为100us * 50 = 5ms
}

/*
************************************************************
*	函数名称：	TIM7_IRQHandler
*
*	函数功能：	Timer7更新中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM7_IRQHandler(void)
{
	//do something...
	rt_interrupt_enter();
	
	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		rt_tick_increase();
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}
	
	/* leave interrupt */
	rt_interrupt_leave();	
}


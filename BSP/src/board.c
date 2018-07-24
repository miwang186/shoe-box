/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2013-07-12     aozima       update for auto initial.
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f10x.h"
#include "board.h"

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

/**
 * @addtogroup STM32
 */
 
  //硬件驱动
#include "hw_includes.h"
 
 //协议
#include "onenet.h"
 
  //网络设备
#include "net_device.h"
#include "fontupd.h"
#include "GUI.h"
/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//分组配置为2:2
	
}
/*
************************************************************
*	函数名称：	rt_hw_board_init
*
*	函数功能：	目标板相关外设初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void rt_hw_board_init(void)
{
	NVIC_Configuration();												//中断控制器分组设置

	LCD_GPIO_Init();													//初始化TFTLCD2.8寸屏
	
	LCD_Init();

	DHT11_GPIO_Config(); 												//DH11 温湿度
	
	SMBus_Init();														/*GY 906 红外测温*/
	
	LED_Init();    														//LED指示灯初始化函数
	
	xpt2046_Init();														//初始化xpt2046用IO口（模拟SPI、中断IO）	

	SPI_Flash_Init();
	
	font_init();
	
	IIC_Init();					//与DHT11共用引脚
	
	Buzzer_Init();														//蜂鸣器初始化
	
	Relay_Init();														//初始化继电器
	
	Check_PowerOn(); 													//上电自检
	
	USART1_Init(115200); 												//USART1串口初始化函数	
	
//	TIME2_5PWM_Init(TIM3,9999,143);//频率为：72*10^6/(9999+1)/(143+1)=50Hz
//	TIM_SetCompare2(TIM3,4999);//得到占空比为50%的pwm波形
	TIME2_5PWM_Init(TIM3,99,35);//频率为：72*10^6/(99+1)/(35+1)=20KHz
	TIM_SetCompare2(TIM3,10);//得到占空比为10%的pwm波形
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) 								//如果是看门狗复位则提示
	{
		UsartPrintf(USART_DEBUG, "WARN:	IWDG Reboot\r\n");
		
		RCC_ClearFlag();														//清除看门狗复位标志位
		
		faultTypeReport = faultType = FAULT_REBOOT; 							//标记为重启错误
		
		if(!Info_Check() && checkInfo.EEPROM_OK)								//如果EEPROM里有信息
			Info_Read();
	}
	else
	{
		//先读出ssid、pswd、devid、apikey
		if(!Info_Check())														//如果EEPROM里有信息
		{
			//AT24C02_Clear(0, 255, 256);
			UsartPrintf(USART_DEBUG, " 1.ssid_pswd in EEPROM\r\n");
			Info_Read();
		}
		else //没有数据
		{
			UsartPrintf(USART_DEBUG, "1.ssid_pswd in ROM\r\n");
		}
		
		UsartPrintf(USART_DEBUG, "2.SSID: %s,   PSWD: %s\r\n"
								"DEVID: %s,     APIKEY: %s\r\n"
								, netDeviceInfo.staName, netDeviceInfo.staPass
								, oneNetInfo.devID, oneNetInfo.apiKey);
	}
	
	Iwdg_Init(4, 1250); 	//64分频，每秒625次，重载1250次，2s

	RTOS_TimerInit();
	
	UsartPrintf(USART_DEBUG, "3.Hardware init OK\r\n");		//提示初始化完成
	mem_init(); 											//初始化内部内存池	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);		//使能CRC时钟，否则STemWin不能使用 	
	LCD_Clear(0, 0, 320, 240, BLACK);
}

/*@}*/

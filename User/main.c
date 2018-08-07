/****************************************Copyright (c)******************************************
**                                     艾克姆科技(IKMSIK)   
**                                 淘宝店铺: acmemcu.taobao.com
**                                 技术论坛: 930ebbs.com
**
**--------------File Info-----------------------------------------------------------------------
** File name:		 main.c
** Last modified Date:          
** Last Version:		   
** Descriptions:		   			
**----------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2018-1-20
** Version:			    1.0
** Descriptions:		配置TFTLCD屏，可以触摸校准电阻触摸屏
**--------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "user_ili9341_lcd.h"
#include "user_xpt2046.h"
#include "dht11.h"
#include "gy_906.h"
#include <stdio.h>
//delay这里报错的原因是：delay函数用汇编实现的，导致了MDK误报。
#include "delay.h"
#include "main.h"

extern volatile uint8_t touch_flag;

void display_temp()
{
	DHT11_Data_TypeDef DHT11_Data;
	
	Read_DHT11(&DHT11_Data);
	
	LCD_DispStr(32,32,WHITE,"Temp:%d.%d'C Humi:%d.%d%%", DHT11_Data.temp_int,DHT11_Data.temp_deci, \
				DHT11_Data.humi_int,DHT11_Data.humi_deci);
	
	LCD_DispStr(32,44,WHITE,"Temp_TA:%0.2f", SMBus_ReadTemp(RAM_TA));
	LCD_DispStr(32,56,WHITE,"Temp_To1:%0.2f", SMBus_ReadTemp(RAM_TOBJ1));
}
/***************************************************************************
 * 描  述 : 主函数
 * 参  数 : 无
 * 返回值 : 无
 **************************************************************************/
int main(void)
{ 
	//初始化TFTLCD2.8寸屏
	LCD_Init();	
	DHT11_GPIO_Config(); 
	SMBus_Init();
	LED_Init();    //LED指示灯初始化函数
	USART1_Init(); //USART1串口初始化函数
	USART3_Init(); //USART2串口初始化函数	
//	//指定位置显示一串红色字体的字符串
	Lcd_GramScan(2);
	//清屏为黑色背景
	LCD_Clear(0, 0, 320, 240, BACKGROUND);	 

	
//  //初始化xpt2046用IO口（模拟SPI、中断IO）
//  xpt2046_Init();
//  //初始化xpt2046用IO口（模拟SPI、中断IO）
//  while(Touch_Calibrate() !=0);
 

  //主循环	
	while(1)	
	{
		while(1)
		{		
			LED_Switch(LED_ON,LED_R|LED_G|LED_Y|LED_B);	   //点亮开发板四个指示灯，表示程序进入主流程
			ESP8266_Init();    //ESP8266初始化
			ESP8266_DevLink(DEVICEID,APIKEY,20);    //和平台建立设备连接
			LED_Switch(LED_OFF,LED_R|LED_G|LED_Y|LED_B); //熄灭开发板四个指示灯，表示程序完成设备连接，进入数据发送循环
		
			while(1)
			{								
					if(!(ESP8266_CheckStatus(30)))    //检测ESP8266模块连接状态
					{
							ESP8266_SendDat();     //向平台发送数据
					}
					else
					{
							break;
					}
					display_temp();
			}
		}			

	}
}
/********************************************END FILE********************/



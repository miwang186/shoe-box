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
#include "ili9341_lcd.h"
#include "xpt2046.h"
#include "dht11.h"
#include "gy_906.h"
#include <stdio.h>
//delay这里报错的原因是：delay函数用汇编实现的，导致了MDK误报。
#include "delay.h"
#include "main.h"
#include "timer.h"
#include "malloc.h"	  
#include "GUI.h"
#include "GUIDemo.h"

//协议层
#include "onenet.h"

//网络设备
#include "net_device.h"

//数据流
DATA_STREAM dataStream[11];
unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);
							

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
	SMBus_Init();	/*GY 906 红外测温*/
	LED_Init();    //LED指示灯初始化函数
	USART1_Init(); //USART1串口初始化函数
	Lcd_GramScan(2);

	LCD_Clear(0, 0, 320, 240, BLUE);	 	//清屏为黑色背景
	xpt2046_Init();			//初始化xpt2046用IO口（模拟SPI、中断IO）	
	mem_init(); 			//初始化内部内存池
	
	TIM3_Int_Init(999,71);	//1KHZ 定时器1ms 
	TIM6_Int_Init(999,719);	//10ms中断
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//使能CRC时钟，否则STemWin不能使用 
	
	//	while(Touch_Calibrate() !=0);
// 	WM_SetCreateFlags(WM_CF_MEMDEV);
//	GUI_Init();
////	Touch_MainTask();
//	MainTask();

//主循环	
	while(1)	
	{			

	}
}
/********************************************END FILE********************/



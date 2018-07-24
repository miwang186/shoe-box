//硬件驱动
#include "hw_includes.h"

//协议层
#include "onenet.h"

//网络设备
#include "net_device.h"

#include "main_dispaly.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define IWDG_TASK_PRIO		2	//看门狗任务
#define TOUCH_TASK_PRIO		3	//触摸任务
#define USART_TASK_PRIO		5	//串口任务
#define HEART_TASK_PRIO		7	//心跳任务
#define TIMER_TASK_PRIO		9 	//定时器任务	
#define FAULT_TASK_PRIO		11	//故障处理任务
#define SENSOR_TASK_PRIO	13	//传感器任务
#define SEND_TASK_PRIO		15	//数据发送任务
#define NET_TASK_PRIO		17	//网络初始化任务
#define DISPLAY_TASK_PRIO	19	//显示任务
#define ALTER_TASK_PRIO		20  //


#define IWDG_TASK_STK_SIZE		256	//运行栈大小
#define ALTER_TASK_STK_SIZE		1024
#define TOUCH_TASK_STK_SIZE		1024 
#define USART_TASK_STK_SIZE		1024 
#define HEART_TASK_STK_SIZE		1024
#define TIMER_TASK_STK_SIZE		512
#define FAULT_TASK_STK_SIZE		1024
#define SENSOR_TASK_STK_SIZE	1024
#define SEND_TASK_STK_SIZE		2048
#define DISPLAY_TASK_STK_SIZE	2048
#define NET_TASK_STK_SIZE		1024

//看门狗任务
ALIGN(RT_ALIGN_SIZE) unsigned char IWDG_TASK_STK[IWDG_TASK_STK_SIZE];
struct rt_thread IWDG_Task_Handle;	//任务句柄

//串口任务
ALIGN(RT_ALIGN_SIZE) unsigned char TOUCH_TASK_STK[TOUCH_TASK_STK_SIZE]; //
struct rt_thread TOUCH_Task_Handle;	//任务句柄

//串口任务
ALIGN(RT_ALIGN_SIZE) unsigned char USART_TASK_STK[USART_TASK_STK_SIZE]; //
struct rt_thread USART_Task_Handle;	//任务句柄

//心跳任务
ALIGN(RT_ALIGN_SIZE) unsigned char HEART_TASK_STK[HEART_TASK_STK_SIZE]; //
struct rt_thread HEART_Task_Handle;	//任务句柄

//定时器任务
ALIGN(RT_ALIGN_SIZE) unsigned char TIMER_TASK_STK[TIMER_TASK_STK_SIZE]; //
struct rt_thread TIMER_Task_Handle;	//任务句柄

//故障处理任务
ALIGN(RT_ALIGN_SIZE) unsigned char  FAULT_TASK_STK[FAULT_TASK_STK_SIZE]; //
struct rt_thread FAULT_Task_Handle;	//任务句柄

//传感器任务
ALIGN(RT_ALIGN_SIZE) unsigned char SENSOR_TASK_STK[SENSOR_TASK_STK_SIZE]; //
struct rt_thread SENSOR_Task_Handle;	//任务句柄

//数据发送任务
ALIGN(RT_ALIGN_SIZE) unsigned char SEND_TASK_STK[SEND_TASK_STK_SIZE]; //
struct rt_thread SEND_Task_Handle;	//任务句柄

//显示任务
ALIGN(RT_ALIGN_SIZE) unsigned char DISPLAY_TASK_STK[DISPLAY_TASK_STK_SIZE]; //
struct rt_thread DISPLAY_Task_Handle;//任务句柄

//网络初始化任务
ALIGN(RT_ALIGN_SIZE) unsigned char NET_TASK_STK[NET_TASK_STK_SIZE]; //
struct rt_thread NET_Task_Handle;	//任务句柄

//信息更改任务
ALIGN(RT_ALIGN_SIZE) unsigned char ALTER_TASK_STK[ALTER_TASK_STK_SIZE]; //
struct rt_thread ALTER_Task_Handle;	//任务句柄

DHT11_INFO 		dht11Info = {30,40,30,40};	/*DTH11*/							
GY_906_INFO 	gy906Info = {30,30};		/*gy906*/	
LED_STATUS 		ledStatus = {0,0,0,0};
DEVICE_STATUS	deviceStatus = {18000,10,0,0};


Warning_Info warning_info =
{
	0x0f,
	5,
	60,
	60,
	20,
	0,
	0,	
	145
};
//数据流
DATA_STREAM dataStream[] ={
								{"led1", &ledStatus.Led1Sta, TYPE_BOOL, 1},
								{"led2", &ledStatus.Led2Sta, TYPE_BOOL, 1},
								{"led3", &ledStatus.Led3Sta, TYPE_BOOL, 1},
								{"led4", &ledStatus.Led4Sta, TYPE_BOOL, 1},
								{"fanner_key",&deviceStatus.motor_key, TYPE_BOOL, 1},
								{"relay_key",&deviceStatus.relay_key, TYPE_BOOL, 1},								
								{"fanner_rate",&deviceStatus.motor_rate, TYPE_USHORT, 1},
								{"timing_sec",&deviceStatus.timing_sec, TYPE_USHORT, 1},
								{"temp_max", &warning_info.max_temp, TYPE_CHAR, 1},
								{"temp_min", &warning_info.min_temp, TYPE_CHAR, 1},
								{"humi_max", &warning_info.max_humi, TYPE_UCHAR, 1},
								
																
								{"dth11-1_temp", &dht11Info.tempreture_1, TYPE_FLOAT, 1},
								{"dth11-1_humi", &dht11Info.humidity_1, TYPE_FLOAT, 1},
								{"dth11-2_temp", &dht11Info.tempreture_2, TYPE_FLOAT, 1},
								{"dth11-2_humi", &dht11Info.humidity_2, TYPE_FLOAT, 1},
								
								{"gy-906_Ta", &gy906Info.temp_ta, TYPE_FLOAT, 1},
								{"gy-906_To", &gy906Info.temp_to, TYPE_FLOAT, 1},			
							};

unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);

						
							
#define NET_TIME	60			//设定时间--单位秒
unsigned short timerCount = 0;	//时间计数--单位秒

							
//void prinf_task_stk_size()
//{
//	uint16_t size,i,j,index;
//	for(j = 0; j < 11; j++)
//	{
//		size = sizeof(IWDG_TASK_STK);
//		for(i = 0;i<size;i++)
//		{
//		
//		}
//	}
//}	
							
/*
************************************************************
*	函数名称：	IWDG_Task
*
*	函数功能：	清除看门狗
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		看门狗任务
************************************************************
*/
void IWDG_Task(void *pdata)
{

	while(1)
	{
	
		Iwdg_Feed(); 		//喂狗
		
		RTOS_TimeDly(50); 	//挂起任务250ms
	
	}

}

/*
************************************************************
*	函数名称：	NET_Task
*
*	函数功能：	网络连接、平台接入
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		网络连接任务任务。会在心跳检测里边检测网络连接状态，如果有错，会标记状态，然后在这里进行重连
************************************************************
*/
void NET_Task(void *pdata)
{
	
	NET_DEVICE_IO_Init();													//网络设备IO初始化
	NET_DEVICE_Reset();														//网络设备复位
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);								//设置为命令收发模式(例如ESP8266要区分AT的返回还是平台下发数据的返回)

	while(1)
	{
		
		if(!oneNetInfo.netWork && (checkInfo.NET_DEVICE_OK == DEV_OK))		//当没有网络 且 网络模块检测到时
		{
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//设置为命令收发模式
			
			if(!NET_DEVICE_Init())											//初始化网络设备，能连入网络
			{
				OneNet_DevLink(oneNetInfo.devID, oneNetInfo.apiKey);		//接入平台
				
				if(oneNetInfo.netWork)
				{
					RTOS_TimeDly(40);
				}
				else
				{
					RTOS_TimeDly(100);
				}
			}
		}
		
		if(checkInfo.NET_DEVICE_OK == DEV_ERR) 								//当网络设备未做检测
		{
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//设置为命令收发模式
			
			if(timerCount >= NET_TIME) 										//如果网络连接超时
			{
				NET_DEVICE_Reset();											//复位网络设备
				timerCount = 0;												//清零连接超时计数
				faultType = FAULT_NONE;										//清除错误标志
			}
			
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK"))							//网络设备检测
			{
//				LCD_DispStr(32,68,WHITE,"NET Device :Ok");

				UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
				checkInfo.NET_DEVICE_OK = DEV_OK;							//检测到网络设备，标记
				NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);					//设置为数据收发模式
			}
			else
				UsartPrintf(USART_DEBUG, "NET Device :Error\r\n");
		}
		
		RTOS_TimeDly(5);													//挂起任务25ms
	
	}

}
static void auto_adjust_temp()
{
	uint16_t temp_Average = 0;
	uint16_t humi_Average = 0;	
	
	if(fabs(dht11Info.tempreture_1 - dht11Info.tempreture_2) <= 20)
	{
		temp_Average = (dht11Info.tempreture_1 +dht11Info.tempreture_2)/2;
		if(warning_info.enable_flag & TEMP_MAX_SWITCH)
		{	
			if(warning_info.max_temp < temp_Average || warning_info.max_temp < gy906Info.temp_to)
			{
				RELAY1_OFF();
				RELAY2_OFF();
				SET_PWM_Arr(1);		
				deviceStatus.relay_key = 0;
				deviceStatus.motor_key = 0;
				deviceStatus.motor_rate = 1;
			}
		}
		
		if(warning_info.enable_flag & TEMP_MIN_SWITCH)
		{	
			if(warning_info.min_temp > temp_Average || warning_info.min_temp > gy906Info.temp_to)
			{
				RELAY1_ON();
				RELAY2_ON();
				SET_PWM_Arr(40);
				deviceStatus.relay_key = 1;
				deviceStatus.motor_key = 1;
				deviceStatus.motor_rate = 40;				
			}
		}		
	}
	
	if(fabs(dht11Info.humidity_1 - dht11Info.humidity_2) <= 30)
	{
		humi_Average = (dht11Info.humidity_1 + dht11Info.humidity_2)/2;		
		if(warning_info.enable_flag & HUMI_MAX_SWITCH)
		{
			if(warning_info.max_humi < humi_Average)
			{		
				RELAY2_ON();
				SET_PWM_Arr(60);	
				deviceStatus.motor_key = 1;
				deviceStatus.motor_rate = 60;				
			}				
		}
	}		
}
/*
************************************************************
*	函数名称：	TIMER_Task
*
*	函数功能：	定时检查网络状态标志位
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		定时器任务。定时检查网络状态，若持续超过设定时间无网络连接，则进行平台重连
************************************************************
*/
void TIMER_Task(void *pdata)
{

	while(1)
	{
	
		if(oneNetInfo.netWork == 0)											//如果网络断开
		{
			if(++timerCount >= NET_TIME) 									//如果网络断开超时
			{
				UsartPrintf(USART_DEBUG, "Tips:		Timer Check Err\r\n");	
				
				checkInfo.NET_DEVICE_OK = 0;								//置设备未检测标志
				
				NET_DEVICE_ReConfig(0);										//设备初始化步骤设置为开始状态
				
				oneNetInfo.netWork = 0;
			}
		}
		else
		{
			timerCount = 0;													//清除计数
		}
		
		if(deviceStatus.timing_sec != 0)
		{
			deviceStatus.timing_sec--;
		}
		else 
		{
			RELAY1_OFF();
			RELAY2_OFF();
			SET_PWM_Arr(1);
			warning_info.enable_flag = 0;
		}
		auto_adjust_temp();
		RTOS_TimeDly(200);													//挂起任务1s
	
	}

}

/*
************************************************************
*	函数名称：	FAULT_Task
*
*	函数功能：	网络状态错误处理
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		故障处理任务。当发生网络错误、设备错误时，会标记对应标志位，然后集中进行处理
************************************************************
*/
void FAULT_Task(void *pdata)
{

	while(1)
	{
		
		if(faultType != FAULT_NONE)									//如果错误标志被设置
		{
			UsartPrintf(USART_DEBUG, "WARN:	Fault Process\r\n");
			Fault_Process();										//进入错误处理函数
		}
		
		RTOS_TimeDly(10);											//挂起任务50ms
	
	}

}
/*
************************************************************
*	函数名称：	SEND_Task
*
*	函数功能：	上传传感器数据
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		数据发送任务
************************************************************
*/
void SEND_Task(void *parameter)
{
	uint8_t send_time = 0;
	while(1)
	{
		if(oneNetInfo.sendData == 1)		//如果数据反馈标志位被设置
		{
			oneNetInfo.sendData = OneNet_SendData(kTypeSimpleJsonWithoutTime, dataStreamLen);	//上传数据到平台
			if(oneNetInfo.sendData == 0)
			{
				send_time = 0;
			}
		}
		send_time++;
		
		if(send_time >= 250)  //25s一次
		{
			send_time = 0;
			OneNet_SendData(kTypeSimpleJsonWithoutTime, dataStreamLen);	//上传数据到平台
		}
		RTOS_TimeDly(20);	//100ms			
		
	}

}

/*
************************************************************
*	函数名称：	USART_Task
*
*	函数功能：	处理平台下发的命令
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		串口接收任务。在数据模式下时，等待平台下发的命令并解析、处理
************************************************************
*/
void USART_Task(void *pdata)
{
	
	unsigned char *dataPtr;														//数据指针

	while(1)
	{
		if(oneNetInfo.netWork && NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)	//当有网络 且 在命令接收模式时
		{
			dataPtr = NET_DEVICE_GetIPD(0);										//不等待，获取平台下发的数据
			if(dataPtr != NULL)													//如果数据指针不为空，则代表收到了数据
			{
				OneNet_App(dataPtr);											//集中处理
			}
		}
		
		RTOS_TimeDly(2);														//挂起任务10ms
	}
}
/*
************************************************************
*	函数名称：	HEART_Task
*
*	函数功能：	心跳检测
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		心跳任务。发送心跳请求并等待心跳响应，若在设定时间内没有响应则会进行平台重连
************************************************************
*/
void HEART_Task(void *pdata)
{

	while(1)
	{
		
		OneNet_HeartBeat();				//心跳检测
		
		RTOS_TimeDly(4000);				//挂起任务20s
	
	}

}


/*
************************************************************
*	函数名称：	ALTER_Task
*
*	函数功能：	通过串口更改SSID、PSWD、DEVID、APIKEY
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		更改后会保存到EEPROM里
************************************************************
*/
void ALTER_Task(void *pdata)
{
    
    unsigned char usart1Count = 0;

    while(1)
    {
    
        rt_memset(alterInfo.alterBuf, 0, sizeof(alterInfo.alterBuf));
		alterInfo.alterCount = 0;
		usart1Count = 0;
        while((rt_strlen(alterInfo.alterBuf) != usart1Count) || (usart1Count == 0))	//等待接收完成
        {
            usart1Count = rt_strlen(alterInfo.alterBuf);							//计算长度
            RTOS_TimeDly(20);														//每100ms检查一次
        }
        UsartPrintf(USART_DEBUG, "\r\nusart1Buf Len: %d, usart1Count = %d\r\n",
									rt_strlen(alterInfo.alterBuf), usart1Count);
        
		if(checkInfo.EEPROM_OK == DEV_OK)											//如果EEPROM存在
		{
			if(Info_Alter(alterInfo.alterBuf))										//更改信息
			{
				oneNetInfo.netWork = 0;												//重连网络和平台
				NET_DEVICE_ReConfig(0);
			}
		}
    
    }

}

/*
************************************************************
*	函数名称：	TOUCH_Task
*
*	函数功能：	触摸
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		触摸
************************************************************
*/
void TOUCH_Task(void *pdata)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		rt_thread_delay(2);
	}
}
void display_sensor_info()
{
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHZ24);
	GUI_DispStringAt("皮皮鞋盒",112,6);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt("温度1:",10,40+16);
	GUI_DispFloat(dht11Info.tempreture_1,5);
	GUI_DispString("'C");
	GUI_DispString("        湿度1:");
	GUI_DispDec(dht11Info.humidity_1,2);
	GUI_DispString("%");

	GUI_DispStringAt("温度2:",10,50+16*2);
	GUI_DispFloat(dht11Info.tempreture_2,5);
	GUI_DispString("'C");
	GUI_DispString("        湿度2:");
	GUI_DispDec(dht11Info.humidity_2,2);	
	GUI_DispString("%");

	GUI_DispStringAt("环境温度:",10,60 + 16*3);
	GUI_DispFloat(gy906Info.temp_ta,5);
	GUI_DispString("'C");

	GUI_DispString("     物体温度:");
	GUI_DispFloat(gy906Info.temp_to,5);	
	GUI_DispString("'C");

	GUI_DispStringAt("电机速度:",10,70+16*4);
	GUI_DispDec(deviceStatus.motor_rate,2);
	GUI_DispString("%");

	GUI_DispString("        继电器:");
	if(deviceStatus.relay_key)
	{
		GUI_DispString("开");
	}
	else
	{
		GUI_DispString("关");
	}
}
/*
************************************************************
*	函数名称：	DISPLAY_Task
*
*	函数功能：	显示
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		显示
************************************************************
*/
void DISPLAY_Task(void *pdata)
{
	GUI_Init();	
//	GUI_CURSOR_Show();
//	//更换皮肤
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);	
	
//	WM_SetDesktopColor(GUI_WHITE); /* Automacally update desktop window */
	WM_SetCreateFlags(WM_CF_MEMDEV); /* Use memory devices on all windows to avoid flicker */	
	Main_Display_Create();
	
	while(1)
	{
		GUI_Delay(500); 

	}
}
/*
************************************************************
*	函数名称：	SENSOR_Task
*
*	函数功能：	传感器数据采集、显示
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		传感器数据采集任务。进行外接传感器的数据采集、读取、显示
************************************************************
*/
void SENSOR_Task(void *pdata) //
{
	DHT11_Data_TypeDef DHT11_Data;
	while(1)
	{
		deviceStatus.motor_rate = Read_PWM_Arr();
		deviceStatus.relay_key = Read_Relay_Status(1);	
		deviceStatus.motor_key = Read_Relay_Status(2);
		deviceStatus.buzzer_key =Read_Buzzer_Status();
		
		ledStatus.Led1Sta = Read_LED_Status(1);
		ledStatus.Led2Sta = Read_LED_Status(2);
		ledStatus.Led3Sta = Read_LED_Status(3);
		ledStatus.Led4Sta = Read_LED_Status(4);
		
		if(Read_DHT11(&DHT11_Data,1))
		{
			dht11Info.humidity_1 = DHT11_Data.humi_int + DHT11_Data.humi_deci/100.0;
			dht11Info.tempreture_1 = DHT11_Data.temp_int + DHT11_Data.temp_deci/100.0;	
		}
		
		if(Read_DHT11(&DHT11_Data,2))
		{
			dht11Info.humidity_2 = DHT11_Data.humi_int + DHT11_Data.humi_deci/100.0;
			dht11Info.tempreture_2 = DHT11_Data.temp_int + DHT11_Data.temp_deci/100.0;	
		}
		gy906Info.temp_ta = SMBus_ReadTemp(RAM_TA);
		gy906Info.temp_to = SMBus_ReadTemp(RAM_TOBJ1);
		
		RTOS_TimeDly(200); 										//挂起任务1000ms	
	}
	
}

int rt_application_init(void)
{
	if(rt_thread_init(&IWDG_Task_Handle, "IWDG", IWDG_Task, RT_NULL,
						(unsigned char *)&IWDG_TASK_STK[0], IWDG_TASK_STK_SIZE, IWDG_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&IWDG_Task_Handle);
						
	if(rt_thread_init(&NET_Task_Handle, "NET", NET_Task, RT_NULL,
						(unsigned char *)&NET_TASK_STK[0], NET_TASK_STK_SIZE, NET_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&NET_Task_Handle);
						
	if(rt_thread_init(&HEART_Task_Handle, "HEART", HEART_Task, RT_NULL,
						(unsigned char *)&HEART_TASK_STK[0], HEART_TASK_STK_SIZE, HEART_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&HEART_Task_Handle);
						
	if(rt_thread_init(&FAULT_Task_Handle, "FAULT", FAULT_Task, RT_NULL,
						(unsigned char *)&FAULT_TASK_STK[0], FAULT_TASK_STK_SIZE, FAULT_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&FAULT_Task_Handle);						
						
	if(rt_thread_init(&SENSOR_Task_Handle, "SENSOR", SENSOR_Task, RT_NULL,
						(unsigned char *)&SENSOR_TASK_STK[0], SENSOR_TASK_STK_SIZE, SENSOR_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&SENSOR_Task_Handle);	

	if(rt_thread_init(&TIMER_Task_Handle, "TIMER", TIMER_Task, RT_NULL,
						(unsigned char *)&TIMER_TASK_STK[0], TIMER_TASK_STK_SIZE, TIMER_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&TIMER_Task_Handle);						
						
	if(rt_thread_init(&SEND_Task_Handle, "SEND", SEND_Task, RT_NULL,
						(unsigned char *)&SEND_TASK_STK[0], SEND_TASK_STK_SIZE, SEND_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&SEND_Task_Handle);

	if(rt_thread_init(&DISPLAY_Task_Handle, "DISPLAY", DISPLAY_Task, RT_NULL,
						(unsigned char *)&DISPLAY_TASK_STK[0], DISPLAY_TASK_STK_SIZE, DISPLAY_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&DISPLAY_Task_Handle);
						
	if(rt_thread_init(&USART_Task_Handle, "USART", USART_Task, RT_NULL,
						(unsigned char *)&USART_TASK_STK[0], USART_TASK_STK_SIZE, USART_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&USART_Task_Handle);

	if(rt_thread_init(&TOUCH_Task_Handle, "TOUCH", TOUCH_Task, RT_NULL,
						(unsigned char *)&TOUCH_TASK_STK[0], TOUCH_TASK_STK_SIZE, TOUCH_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&TOUCH_Task_Handle);	

	if(rt_thread_init(&ALTER_Task_Handle, "ALTER", ALTER_Task, RT_NULL,
						(unsigned char *)&ALTER_TASK_STK[0], ALTER_TASK_STK_SIZE, ALTER_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&ALTER_Task_Handle);						
	return 0;
}


//硬件驱动
#include "hw_includes.h"

//协议层
#include "onenet.h"

//网络设备
#include "net_device.h"

#include "GUI.h"
#include "GUIDemo.h"

#define TOUCH_TASK_PRIO		3	//触摸任务
#define USART_TASK_PRIO		5	//串口任务
#define HEART_TASK_PRIO		7	//心跳任务
#define TIMER_TASK_PRIO		9 	//定时器任务	
#define FAULT_TASK_PRIO		11	//故障处理任务
#define SENSOR_TASK_PRIO	13	//传感器任务
#define SEND_TASK_PRIO		15	//数据发送任务
#define NET_TASK_PRIO		17	//网络初始化任务
#define DISPLAY_TASK_PRIO	19	//显示任务

#define TOUCH_TASK_STK_SIZE		1024 
#define USART_TASK_STK_SIZE		2048 
#define HEART_TASK_STK_SIZE		2048
#define TIMER_TASK_STK_SIZE		1024
#define FAULT_TASK_STK_SIZE		2048
#define SENSOR_TASK_STK_SIZE	1024
#define SEND_TASK_STK_SIZE		3072
#define DISPLAY_TASK_STK_SIZE	2048
#define NET_TASK_STK_SIZE		2048

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


DHT11_INFO 		dht11Info;	/*DTH11*/							
GY_906_INFO 	gy906Info;	/*gy906*/	
LED_STATUS 		ledStatus = {0,0,0,0};
DEVICE_STATUS	deviceStatus = {10,1};
//数据流
DATA_STREAM dataStream[] ={
								{"led1", &ledStatus.Led1Sta, TYPE_BOOL, 1},
								{"led2", &ledStatus.Led2Sta, TYPE_BOOL, 1},
								{"led3", &ledStatus.Led3Sta, TYPE_BOOL, 1},
								{"led4", &ledStatus.Led4Sta, TYPE_BOOL, 1},
								{"fanner_key", &deviceStatus.motor_key, TYPE_BOOL, 1},
								{"relay_key", &deviceStatus.relay_key, TYPE_BOOL, 1},								
								{"fanner_rate", &deviceStatus.motor_rate, TYPE_USHORT, 1},
																
								{"dth11-1_temp", &dht11Info.tempreture, TYPE_FLOAT, 1},
								{"dth11-1_humi", &dht11Info.humidity, TYPE_FLOAT, 1},
								{"gy-906_Ta", &gy906Info.temp_ta, TYPE_FLOAT, 1},
								{"gy-906_To", &gy906Info.temp_to, TYPE_FLOAT, 1},			
							};

unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);

						
							
#define NET_TIME	60			//设定时间--单位秒
unsigned short timerCount = 0;	//时间计数--单位秒

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
		
		if(send_time >= 150)  //15s一次
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
 	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();		
	while(1)
	{
		GUIDEMO_Main(); 
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
		
		Read_DHT11(&DHT11_Data);
		
		dht11Info.humidity = DHT11_Data.humi_int + DHT11_Data.humi_deci/100.0;
		dht11Info.tempreture = DHT11_Data.temp_int + DHT11_Data.temp_deci/100.0;	
				
		gy906Info.temp_ta = SMBus_ReadTemp(RAM_TA);
		gy906Info.temp_to = SMBus_ReadTemp(RAM_TOBJ1);

		LCD_DispStr(32,32,WHITE,"Temp:%d.%d'C Humi:%d.%d%%", DHT11_Data.temp_int,DHT11_Data.temp_deci, \
					DHT11_Data.humi_int,DHT11_Data.humi_deci);
					
		LCD_DispStr(32,44,WHITE,"Temp_TA:%0.2f", gy906Info.temp_ta);
		LCD_DispStr(32,56,WHITE,"Temp_To1:%0.2f",gy906Info.temp_to);
		
		RTOS_TimeDly(200); 										//挂起任务1000ms	
	}
	
}

int rt_application_init(void)
{	
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

//	if(rt_thread_init(&SEND_Task_Handle, "DISPLAY", DISPLAY_Task, RT_NULL,
//						(unsigned char *)&DISPLAY_TASK_STK[0], DISPLAY_TASK_STK_SIZE, DISPLAY_TASK_PRIO, 1) == RT_EOK)
//		rt_thread_startup(&SEND_Task_Handle);
						
	if(rt_thread_init(&USART_Task_Handle, "USART", USART_Task, RT_NULL,
						(unsigned char *)&USART_TASK_STK[0], USART_TASK_STK_SIZE, USART_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&USART_Task_Handle);

	if(rt_thread_init(&TOUCH_Task_Handle, "TOUCH", TOUCH_Task, RT_NULL,
						(unsigned char *)&TOUCH_TASK_STK[0], TOUCH_TASK_STK_SIZE, TOUCH_TASK_PRIO, 1) == RT_EOK)
		rt_thread_startup(&TOUCH_Task_Handle);							
	return 0;
}


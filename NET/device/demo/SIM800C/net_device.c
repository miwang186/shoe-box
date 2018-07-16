#include "net_device.h"
#include "net_io.h"

#include "delay.h"
#include "led.h"
#include "usart.h"

#include "stm32f10x.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {"", "", 0, 0, 7, 0, 0}; //netWork ssid pswd 保留


//在开机下调用一次则关机，再关机下调用则开机
void NET_DEVICE_PowerCtl(void)
{
	
	RTOS_TimeDly(200);
	NET_DEVICE_PWRK_OFF;
    RTOS_TimeDly(240);
    NET_DEVICE_PWRK_ON;
    RTOS_TimeDly(140);

}

void NET_DEVICE_IO_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
    
	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    
	//PowerKey
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStruct.GPIO_Pin =  GPIO_Pin_4;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStruct);
	
	//Status
	gpioInitStruct.GPIO_Pin =  GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	if(netDeviceInfo.reboot == 0)
	{
		NET_DEVICE_PWRK_ON;
		NET_DEVICE_PowerCtl();
	}
	
	netDeviceInfo.reboot = 0;
	
	NET_IO_Init();

}

/***************************************************************************************

							以下是网络设备层需要提供的功能

****************************************************************************************/

//初始化
_Bool NET_DEVICE_Init(void)
{
	
	switch(netDeviceInfo.initStep)
	{
		case 0:
			
			UsartPrintf(USART_DEBUG, "AT+CPIN?\r\n");
			if(!NET_DEVICE_SendCmd("AT+CPIN?\r\n", "OK"))						//SIM卡是否解锁
				netDeviceInfo.initStep++;
		
		break;
			
		case 1:																	//自动判断卡类型
		{
			char resBuf[5] = {0, 0, 0, 0, 0};
			char text[2] = {0, 0};
			
			strcpy(resBuf, "0,");
			sprintf(text, "%d", netDeviceInfo.cardType);
			strcat(resBuf, text);
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CREG?  %d\r\n",
										netDeviceInfo.cardType);
			if(!NET_DEVICE_SendCmd("AT+CREG?\r\n", resBuf)) 					//确认网络搜索成功,OK
				netDeviceInfo.initStep++;
			else 																//如果失败则检测返回的内容
			{
				if(netIOInfo.buf[11] != 48)
					netDeviceInfo.cardType = netIOInfo.buf[11] - 48;
				
				NET_DEVICE_ClrData();
			}
		}
		break;
			
		case 2:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CSQ\r\n");
			if(!NET_DEVICE_SendCmd("AT+CSQ\r\n","OK"))							//查询信号强度,OK
				netDeviceInfo.initStep++;
		break;
			
		case 3:
				UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGREG?\r\n");			//检查网络注册状态
				if(!NET_DEVICE_SendCmd("AT+CGREG?\r\n","OK"))
					netDeviceInfo.initStep++;
		break;
			
		case 4:
			
			UsartPrintf(USART_DEBUG, "AT+CIPHEAD=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPHEAD=1\r\n", "OK"))					//显示报文头
				netDeviceInfo.initStep++;
		
		break;
			
		case 5:
			
			UsartPrintf(USART_DEBUG, "AT+CGCLASS=\"B\"\r\n");
			if(!NET_DEVICE_SendCmd("AT+CGCLASS=\"B\"\r\n", "OK"))				//设置GPRS移动台类别为B,支持包交换和数据交换
				netDeviceInfo.initStep++;
		
		break;
			
		case 6:
			
			UsartPrintf(USART_DEBUG, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
			if(!NET_DEVICE_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK")) //设置PDP上下文,互联网接协议,接入点等信息
				netDeviceInfo.initStep++;
		
		break;
			
		case 7:
			
			UsartPrintf(USART_DEBUG, "AT+CGATT=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CGATT=1\r\n", "OK"))						//附着GPRS业务
				netDeviceInfo.initStep++;
		
		break;
			
		case 8:
			
			UsartPrintf(USART_DEBUG, "AT+CIPCSGP=1,\"CMNET\"\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r\n", "OK"))			//设置为GPRS连接模式
				netDeviceInfo.initStep++;
		
		break;
			
		case 9:
			
			UsartPrintf(USART_DEBUG, "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n", "CONNECT")) //连接平台
				netDeviceInfo.initStep++;
		
		break;
			
		case 10:
			
			UsartPrintf(USART_DEBUG, "AT+CIPSTATUS\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK"))			//连接平台
				netDeviceInfo.initStep++;
			else
			{
				NET_DEVICE_SendCmd("AT+CIPCLOSE=1\r\n", "CLOSE OK");			//关闭连接
				NET_DEVICE_SendCmd("AT+CIPSHUT\r\n", "SHUT OK");				//关闭移动场景
				RTOS_TimeDly(20);
				NET_DEVICE_ReConfig(4);
			}
		
		break;
			
		default:
			break;
	}
    
	if(netDeviceInfo.initStep == 11)
		return 0;
	else
		return 1;

}

//复位
void NET_DEVICE_Reset(void)
{
	


}

//重连
_Bool NET_DEVICE_ReLink(void)
{
	
	_Bool status = 0;
	
	NET_DEVICE_SendCmd("AT+CIPCLOSE=1\r\n", "CLOSE OK"); //关闭连接
	RTOS_TimeDly(20);
	
	UsartPrintf(USART_DEBUG, "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n");
	status = NET_DEVICE_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n", "CONNECT OK");	//连接平台
	
	return status;

}

//发送命令
_Bool NET_DEVICE_SendCmd(char *cmd, char *res)
{
	
	unsigned short timeOut = 300;
	
	NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)netIOInfo.buf, res) != NULL)
			{
				NET_IO_ClearRecive();
				
				return 0;
			}
		}
		
		RTOS_TimeDly(2);
	}
	
	return 1;

}

//数据发送
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	
#if(NET_DEVICE_TRANS == 1)
	NET_IO_Send(data, len);  //发送设备连接请求数据
#else
	char cmdBuf[30];

	RTOS_TimeDly(10);
	
	NET_IO_ClearRecive();
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);
	if(!NET_DEVICE_SendCmd(cmdBuf, ">"))
	{
		NET_IO_Send(data, len);  //发送设备连接请求数据
	}
#endif

}

//把IPD的内容复制到串口buffer里边
unsigned char *NET_DEVICE_GetIPD(unsigned short timeOut)
{

#if(NET_DEVICE_TRANS == 0)
	unsigned char byte = 0, count = 0;
	char sByte[5];
	char *ptrIPD;
#endif
	
	do
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			//UsartPrintf(USART_DEBUG, "\r\n%s\r\n", netIOInfo.buf);
			ptrIPD = strstr((char *)netIOInfo.buf, "IPD,");
			if(ptrIPD == NULL)
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strstr(ptrIPD, ",");ptrIPD++;
				
				while(*ptrIPD != ':')
				{
					sByte[count++] = *ptrIPD++;
				}
				byte = (unsigned char)atoi(sByte);
				
				ptrIPD++;
				for(count = 0; count < byte; count++)
				{
					netIOInfo.buf[count] = *ptrIPD++;
				}
				
				return netIOInfo.buf;
			}
		}
		
		RTOS_TimeDly(2);
	} while(timeOut--);
	
	return NULL;

}

void NET_DEVICE_ClrData(void)
{

	NET_IO_ClearRecive();

}

//状态检查
unsigned char NET_DEVICE_Check(void)
{
	
	unsigned char status = 0;
	unsigned char timeOut = 200;

	NET_IO_ClearRecive();
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n",  14);
	
	while(--timeOut)
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)netIOInfo.buf, "TCP CLOSED"))			//连接关闭
			{
				status = 1;
				UsartPrintf(USART_DEBUG, "SIM800C TCP CLOSED\r\n");
			}
			
			break;
		}
		
		RTOS_TimeDly(2);
	}
	
	if(timeOut == 0)
	{
		status = 1;
		UsartPrintf(USART_DEBUG, "Check TimeOut\r\n");
	}
	
	return status;

}

void NET_DEVICE_ReConfig(unsigned char step)
{

	netDeviceInfo.initStep = step;

}

void NET_DEVICE_Set_DataMode(unsigned char mode)
{

	netDeviceInfo.dataType = mode;

}

unsigned char NET_DEVICE_Get_DataMode(void)
{

	return netDeviceInfo.dataType;

}


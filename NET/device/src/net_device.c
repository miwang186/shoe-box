/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_device.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		�����豸Ӧ�ò�
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f10x.h"	//��Ƭ��ͷ�ļ�

#include "net_device.h"	//�����豸Ӧ�ò�
#include "net_io.h"		//�����豸����IO��

//Ӳ������
#include "delay.h"
#include "led.h"
#include "usart1.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {"ChinaNet-CMCC", "ygww1616", 0, 0, 0, 0, 0}; //





/*
************************************************************
*	�������ƣ�	NET_DEVICE_IO_Init
*
*	�������ܣ�	��ʼ�������豸IO��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ�������豸�Ŀ������š������շ����ܵ�
************************************************************
*/
void NET_DEVICE_IO_Init(void)
{
	NET_IO_Init();											//�����豸����IO���ʼ��	
	netDeviceInfo.reboot = 0;
}

#if(NET_DEVICE_TRANS == 1)
/*
************************************************************
*	�������ƣ�	ESP8266_QuitTrans
*
*	�������ܣ�	�˳�͸��ģʽ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��������������+����Ȼ��ر�͸��ģʽ
************************************************************
*/
void ESP8266_QuitTrans(void)
{

	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';
	RTOS_TimeDly(3); 					//���ڴ�����֡ʱ��(10ms)
	
	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';        
	RTOS_TimeDly(3); 					//���ڴ�����֡ʱ��(10ms)
	
	while((NET_IO->SR & 0X40) == 0);	//�ȴ����Ϳ�
	NET_IO->DR = '+';        
	RTOS_TimeDly(20);					//�ȴ�100ms
	
	NET_DEVICE_SendCmd("AT+CIPMODE=0\r\n", "OK"); //�ر�͸��ģʽ

}

/*
************************************************************
*	�������ƣ�	ESP8266_EnterTrans
*
*	�������ܣ�	����͸��ģʽ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void ESP8266_EnterTrans(void)
{
	
	NET_DEVICE_SendCmd("AT+CIPMUX=0\r\n", "OK");	//������ģʽ

	NET_DEVICE_SendCmd("AT+CIPMODE=1\r\n", "OK");	//ʹ��͸��
	
	NET_DEVICE_SendCmd("AT+CIPSEND\r\n", ">");		//��������
	
	RTOS_TimeDly(20);								//�ȴ�100ms

}

#endif

/*
************************************************************
*	�������ƣ�	ESP8266_StaInit
*
*	�������ܣ�	����ESP8266Ϊ staģʽ
*
*	��ڲ�����	��
*
*	���ز�����	���س�ʼ�����
*
*	˵����		1.wifi��Ϣ����	2.�豸id����apikey����	3.��ʼ���ɹ�
************************************************************
*/
unsigned char ESP8266_StaInit(void)
{
	
	unsigned char errCount = 0, errType = 0;
	char cfgBuffer[70];

	switch(netDeviceInfo.initStep)
	{
		case 0:
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CWMODE=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+CWMODE=1\r\n", "OK")) 										//stationģʽ
				netDeviceInfo.initStep++;
		
		break;
		
		case 1:
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CIPMODE=0\r\n");
			if(!NET_DEVICE_SendCmd("AT+CIPMODE=0\r\n", "OK")) 										//�ر�͸��ģʽ---ָ��ģʽ
				netDeviceInfo.initStep++;
		
		break;
		
		case 2:
			
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
		
			strcpy(cfgBuffer, "AT+CWJAP=\"");														//��дssid��pswd
			strcat(cfgBuffer, netDeviceInfo.staName);
			strcat(cfgBuffer, "\",\"");
			strcat(cfgBuffer, netDeviceInfo.staPass);
			strcat(cfgBuffer, "\"\r\n");
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);
		
			while(NET_DEVICE_SendCmd(cfgBuffer, "GOT IP")) 											//����·������������GOT IP�������ʧ�ܻ����ѭ����
			{
				LED_1_ON();																//������ʾ
				RTOS_TimeDly(100);
				
				LED_1_OFF();																		//�����ʾ
				RTOS_TimeDly(100);
				
				if(++errCount >= 5)																	//�������5��û�����ϣ���Ҫ�������״̬����ssid��pswd
				{
					UsartPrintf(USART_DEBUG, "WARN:	AT+CWJAP Err\r\n");
					errType = 1; //wifi��Ϣ����
					break;
				}
			}
			
			if(!errType)
				netDeviceInfo.initStep++;
		
		break;
		
		case 3:
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n");
		
			while(NET_DEVICE_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n", "CONNECT"))	//����ƽ̨��������CONNECT�������ʧ�ܻ����ѭ����
			{
				LED_2_ON();		
				RTOS_TimeDly(100);
				
				LED_2_OFF();
				RTOS_TimeDly(100);
				
				if(++errCount >= 10)
				{
					UsartPrintf(USART_DEBUG, "WARN:	AT+CIPSTART Err\r\n");
					errType = 2; //�豸id����apikey����
					break;
				}
			}
			
			if(!errType)
			{
				netDeviceInfo.initStep++;
			}
		
		break;
			
#if(NET_DEVICE_TRANS == 1)
			
		case 4:

			ESP8266_EnterTrans();																//����͸��ģʽ
			UsartPrintf(USART_DEBUG, "Tips:	EnterTrans\r\n");
			
			UsartPrintf(USART_DEBUG, "Tips:	ESP8266 STA_Mode OK\r\n");
			netDeviceInfo.initStep++;

		break;
			
#endif
		
		default:
			errType = 3; //������ʼ�����
		break;
	}
	
	return errType;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_Init
*
*	�������ܣ�	�����豸��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	���س�ʼ�����
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool NET_DEVICE_Init(void)
{
	
	netDeviceInfo.err = ESP8266_StaInit(); 									//��ȡ���ӽ��	0-�ɹ�
	
	if(netDeviceInfo.err == 1) 												//���·����Ϣ����������apģʽ ͨ���ֻ���ȡssid��password
	{
		UsartPrintf(USART_DEBUG, "Wifi info Error,Use USART1 -> 8266\r\n");
        
        return 1;
	}
	else if(netDeviceInfo.err == 2) 										//�����ƽ̨��Ϣ����
	{
		UsartPrintf(USART_DEBUG, "PT info Error,Use APP -> 8266\r\n");
		
		return 1;
	}
	else if(netDeviceInfo.err == 3)
	{
		UsartPrintf(USART_DEBUG, "Tips:	ESP8266 STA OK\r\n");
		
		RTOS_TimeDly(100); //��ʱ��ʾ
		
		return 0;
	}
	else
		return 1;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_Reset
*
*	�������ܣ�	�����豸��λ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void NET_DEVICE_Reset(void)
{
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();	//�˳�͸��ģʽ
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

	UsartPrintf(USART_DEBUG, "Tips:	ESP8266_Reset\r\n");
	
	NET_DEVICE_RST_ON;		//��λ
	RTOS_TimeDly(50);
	
	NET_DEVICE_RST_OFF;		//������λ
	RTOS_TimeDly(200);

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_ReLink
*
*	�������ܣ�	����ƽ̨
*
*	��ڲ�����	��
*
*	���ز�����	�������ӽ��
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool NET_DEVICE_ReLink(void)
{
	
	_Bool status = 0;
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif
	
	NET_DEVICE_SendCmd("AT+CIPCLOSE\r\n", "OK");												//����ǰ�ȹر�һ��
	UsartPrintf(USART_DEBUG, "Tips:	CIPCLOSE\r\n");
	RTOS_TimeDly(100);																			//�ȴ�
	
	UsartPrintf(USART_DEBUG, "ReLink Tips:	AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n");
	
	status = NET_DEVICE_SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n", "CONNECT");	//��������
	
#if(NET_DEVICE_TRANS == 1)
		ESP8266_EnterTrans();
		UsartPrintf(USART_DEBUG, "Tips:	EnterTrans\r\n");
#endif
	
	return status;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_SendCmd
*
*	�������ܣ�	�������豸����һ��������ȴ���ȷ����Ӧ
*
*	��ڲ�����	cmd����Ҫ���͵�����		res����Ҫ��������Ӧ
*
*	���ز�����	�������ӽ��
*
*	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool NET_DEVICE_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	
	NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));	//д��������豸
	
	while(timeOut--)												//�ȴ�
	{
		if(NET_IO_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)netIOInfo.buf, res) != NULL)	//����������ؼ���
			{
				NET_IO_ClearRecive();								//��ջ���
				
				return 0;
			}
		}
		
		RTOS_TimeDly(2);											//����ȴ�
	}
	
	return 1;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_SendData
*
*	�������ܣ�	ʹ�����豸�������ݵ�ƽ̨
*
*	��ڲ�����	data����Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	
#if(NET_DEVICE_TRANS == 1)
	NET_IO_Send(data, len);  						//�����豸������������
#else
	char cmdBuf[30];

	RTOS_TimeDly(10);								//�ȴ�һ��
	
	NET_IO_ClearRecive();							//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	//if(!NET_DEVICE_SendCmd(cmdBuf, ">"))			//�յ���>��ʱ���Է�������
	NET_DEVICE_SendCmd(cmdBuf, ">");
	{
		NET_IO_Send(data, len);  					//�����豸������������
	}
#endif

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_GetIPD
*
*	�������ܣ�	��ȡƽ̨���ص�����
*
*	��ڲ�����	�ȴ���ʱ��(����10ms)
*
*	���ز�����	ƽ̨���ص�ԭʼ����
*
*	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
************************************************************
*/
unsigned char *NET_DEVICE_GetIPD(unsigned short timeOut)
{

#if(NET_DEVICE_TRANS == 0)
	unsigned char byte = 0, count = 0;
	char sByte[5];
	char *ptrIPD;
#endif
	
	do
	{
		if(NET_IO_WaitRecive() == REV_OK)								//����������
		{
#if(NET_DEVICE_TRANS == 0)
			ptrIPD = strstr((char *)netIOInfo.buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strstr(ptrIPD, ",");ptrIPD++;					//�ҵ�','  ��Ȼ��ƫ�Ƶ���һ���ַ����������ݳ��ȵĵ�һ������
				
				while(*ptrIPD != ':')									//��','��':' ֮��Ķ������ݳ��ȵ�����
				{
					sByte[count++] = *ptrIPD++;
				}
				byte = (unsigned char)atoi(sByte);						//���ַ�תΪ��ֵ��ʽ
				
				ptrIPD++;												//��ʱptrIPDָ�뻹ָ��':'��������Ҫƫ�Ƶ��¸�����
				for(count = 0; count < byte; count++)					//��������
				{
					netIOInfo.buf[count] = *ptrIPD++;
				}
				memset(netIOInfo.buf + byte,
						0, sizeof(netIOInfo.buf) - byte);				//����ߵ��������
				netDeviceInfo.ipdBytes = byte;
				
				return netIOInfo.buf;									//������ɣ���������ָ��
			}
#else
			netDeviceInfo.ipdBytes = byte;
			return netIOInfo.buf;
#endif
		}
		
		RTOS_TimeDly(2);												//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_ClrData
*
*	�������ܣ�	��������豸���ݽ��ջ���
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void NET_DEVICE_ClrData(void)
{

	NET_IO_ClearRecive();	//��ջ���

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_Check
*
*	�������ܣ�	��������豸����״̬
*
*	��ڲ�����	��
*
*	���ز�����	����״̬
*
*	˵����		
************************************************************
*/
unsigned char NET_DEVICE_Check(void)
{
	
	unsigned char status = 0;
	unsigned char timeOut = 200;
	
#if(NET_DEVICE_TRANS == 1)
	ESP8266_QuitTrans();
	UsartPrintf(USART_DEBUG, "Tips:	QuitTrans\r\n");
#endif

	NET_IO_ClearRecive();												//��ջ���
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n",  14);				//����״̬���
	
	while(--timeOut)
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)netIOInfo.buf, "STATUS:2"))			//���IP
			{
				status = 2;
				UsartPrintf(USART_DEBUG, "ESP8266 Got IP\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:3"))	//��������
			{
				status = 0;
				UsartPrintf(USART_DEBUG, "ESP8266 Connect OK\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:4"))	//ʧȥ����
			{
				status = 1;
				UsartPrintf(USART_DEBUG, "ESP8266 Lost Connect\r\n");
			}
			else if(strstr((const char *)netIOInfo.buf, "STATUS:5"))	//��������
			{
				status = 3;
				UsartPrintf(USART_DEBUG, "ESP8266 Lost\r\n");			//�豸��ʧ
			}
			
			break;
		}
		
		RTOS_TimeDly(2);
	}
	
	if(timeOut == 0)
	{
		status = 3;
		UsartPrintf(USART_DEBUG, "ESP8266 TimeOut\r\n");
	}
	
	NET_IO_ClearRecive();												//��ջ���
	
	return status;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_ReConfig
*
*	�������ܣ�	�豸�����豸��ʼ���Ĳ���
*
*	��ڲ�����	����ֵ
*
*	���ز�����	��
*
*	˵����		�ú������õĲ����������豸��ʼ������õ�
************************************************************
*/
void NET_DEVICE_ReConfig(unsigned char step)
{

	netDeviceInfo.initStep = step;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_Set_DataMode
*
*	�������ܣ�	�����豸�����շ�ģʽ
*
*	��ڲ�����	ģʽ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void NET_DEVICE_Set_DataMode(unsigned char mode)
{

	netDeviceInfo.dataType = mode;

}

/*
************************************************************
*	�������ƣ�	NET_DEVICE_Get_DataMode
*
*	�������ܣ�	��ȡ�豸�����շ�ģʽ
*
*	��ڲ�����	��
*
*	���ز�����	ģʽ
*
*	˵����		
************************************************************
*/
unsigned char NET_DEVICE_Get_DataMode(void)
{

	return netDeviceInfo.dataType;

}

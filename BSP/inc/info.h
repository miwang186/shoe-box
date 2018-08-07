#ifndef __INFO_H_
#define __INFO_H_
#include "stdint.h"

#define SSID_ADDRESS		0		//起始地址，第一个数据表示长度。最大59字节。
#define PSWD_ADDRESS		60		//起始地址，第一个数据表示长度。最大59字节。

#define DEVID_ADDRESS       120		//起始地址，第一个数据表示长度。最大19字节。
#define AKEY_ADDRESS        140		//起始地址，第一个数据表示长度。最大59字节。

#define WARNING_INFO_ADDR	200		

typedef struct
{
	float tempreture_1;
	float humidity_1;
	float tempreture_2;
	float humidity_2;

}DHT11_INFO;	

typedef struct
{
	float temp_ta;
	float temp_to;

}GY_906_INFO;

typedef enum
{
	TIMING_MAX_SWITCH= 0x01,
	HUMI_MAX_SWITCH = 0x02,
	TEMP_MAX_SWITCH = 0x04,
	TEMP_MIN_SWITCH = 0x08,
	
}SWITCH_FLAG;

typedef struct
{
	/*
	bit 0 定时 开关
	bit 1 湿度上限 开关
	bit 2 温度上限 开关
	bit 3 温度下限 开关
	bit 4-7保留
	*/
	uint8_t enable_flag;
	uint8_t timing;
	uint8_t max_humi;
	int8_t  max_temp;
	int8_t  min_temp;
	uint8_t reserve[2];
	uint8_t sum_crc;
}Warning_Info;

unsigned char Info_Check(void);

unsigned char Info_WifiLen(unsigned char sp);

_Bool Info_Read(void);

_Bool Info_Alter(char *info);

uint8_t Get_Warning_info(Warning_Info *info);
uint8_t Save_Warning_info(Warning_Info *info);
#endif 


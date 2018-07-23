#ifndef __INFO_H_
#define __INFO_H_


#define SSID_ADDRESS		0		//起始地址，第一个数据表示长度。最大59字节。
#define PSWD_ADDRESS		60		//起始地址，第一个数据表示长度。最大59字节。

#define DEVID_ADDRESS       120		//起始地址，第一个数据表示长度。最大19字节。
#define AKEY_ADDRESS        140		//起始地址，第一个数据表示长度。最大59字节。

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



unsigned char Info_Check(void);

unsigned char Info_WifiLen(unsigned char sp);

_Bool Info_Read(void);

_Bool Info_Alter(char *info);

#endif 


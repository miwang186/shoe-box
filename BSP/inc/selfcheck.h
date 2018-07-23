#ifndef _SELFCHECK_H_
#define _SELFCHECK_H_







typedef struct
{

	unsigned short DHT11_OK : 1;		//温湿度传感器正常标志位
	unsigned short EEPROM_OK : 1;		//存储器正常标志位
	unsigned short GY906_OK : 1;		//
	unsigned short LCD_OK : 1;			//LCD正常标志
	unsigned short NET_DEVICE_OK : 1;	//网络设备正常标志位

} CHECK_INFO;

#define DEV_OK		1
#define DEV_ERR		0

extern CHECK_INFO checkInfo;



void Check_PowerOn(void);


#endif

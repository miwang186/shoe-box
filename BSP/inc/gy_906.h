#ifndef	 _GY906_H_
#define  _GY906_H_

#include <stm32f10x.h>

#define RAM_TA			0x06 //TA  address in the eeprom
#define RAM_TOBJ1		0x07 //To1 address in the eeprom
#define RAM_TOBJ2		0x08 //To2 address in the eeprom

void SMBus_Init(void);
float SMBus_ReadTemp(uint8_t addr); //获取温度值

#endif 


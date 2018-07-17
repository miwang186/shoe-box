#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "stm32f10x.h"

#define  AT24CXX_SIZE				  256			  // 宏定义AT24C02总容量 
   
uint8_t AT24CXX_RcvOneByte(uint8_t Addr);
void AT24CXX_SendOneByte(uint8_t Addr,uint8_t Data);              
void AT24CXX_SendLenByte(uint8_t Addr,uint8_t *Data,uint8_t Len);
void AT24CXX_RcvLenByte(uint8_t Addr,uint8_t *Data,uint8_t Len);
void AT24CXX_EraseOneByte(uint8_t Addr);
void AT24CXX_EraseAll(void);

#endif 

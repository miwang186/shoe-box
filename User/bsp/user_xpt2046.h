#include "stm32f10x.h"


#define  xpt2046_SPI_CS_0     GPIO_ResetBits(GPIOD, GPIO_Pin_12) 
#define  xpt2046_SPI_CS_1     GPIO_SetBits(GPIOD, GPIO_Pin_12) 
#define  xpt2046_SPI_SCK_0    GPIO_ResetBits(GPIOG, GPIO_Pin_14) 
#define  xpt2046_SPI_SCK_1    GPIO_SetBits(GPIOG, GPIO_Pin_14) 
#define  xpt2046_SPI_MOSI_0   GPIO_ResetBits(GPIOG, GPIO_Pin_13) 
#define  xpt2046_SPI_MOSI_1   GPIO_SetBits(GPIOG, GPIO_Pin_13)

#define INT_IN_2046   GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)

/* 液晶坐标结构体 */
typedef	struct POINT 
{
   uint16_t x;		
   uint16_t y;
}Coordinate;

/*  校正系数结构体 */
typedef struct Parameter 
{						
long double An,  			 //注:sizeof(long double) = 8
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
}Parameter ;

extern Coordinate ScreenSample[4];
extern Coordinate DisplaySample[4];
extern Parameter touch_para ;
extern Coordinate  display ;

#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字


void xpt2046_Init(void);
int Touch_Calibrate(void);
void Palette_Init(void);
FunctionalState Get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para );
Coordinate *Read_2046_2(void);
void Palette_draw_point(uint16_t x, uint16_t y);



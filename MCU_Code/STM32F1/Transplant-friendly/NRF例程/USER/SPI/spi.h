#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"

//SPI1
#define RCC_SPI1	RCC_APB2Periph_GPIOA
#define SPI1_PORT	GPIOA
//SCK
#define SPI1_SCK_PIN	GPIO_Pin_5
//MISO
#define SPI1_MISO_PIN	GPIO_Pin_6
//MOSI
#define SPI1_MOSI_PIN	GPIO_Pin_7

// SPI总线速度设置  			  
void SPI1_Init(void);			 //初始化SPI口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif


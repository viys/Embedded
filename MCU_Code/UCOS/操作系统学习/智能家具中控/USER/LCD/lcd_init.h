#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stm32f10x.h"

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 80
#endif



//-----------------LCD端口定义---------------- 
//RES
#define TFT_RCC_RES		RCC_APB2Periph_GPIOA
#define TFT_PORT_RES	GPIOA
#define TFT_MODE_RES	GPIO_Mode_Out_PP
#define TFT_PIN_RES 	GPIO_Pin_1
//DC
#define TFT_RCC_DC		RCC_APB2Periph_GPIOA
#define TFT_PORT_DC		GPIOA
#define TFT_MODE_DC		GPIO_Mode_Out_PP
#define TFT_PIN_DC 		GPIO_Pin_2
//CS
#define TFT_RCC_CS		RCC_APB2Periph_GPIOC
#define TFT_PORT_CS		GPIOC
#define TFT_MODE_CS		GPIO_Mode_Out_PP
#define TFT_PIN_CS 		GPIO_Pin_4
//BLK
#define TFT_RCC_BLK		RCC_APB2Periph_GPIOC
#define TFT_PORT_BLK	GPIOC
#define TFT_MODE_BLK	GPIO_Mode_Out_PP
#define TFT_PIN_BLK 	GPIO_Pin_5
//SW
#define TFT_RCC_SW		RCC_APB2Periph_GPIOB
#define TFT_PORT_SW		GPIOB
#define TFT_MODE_SW		GPIO_Mode_Out_PP
#define TFT_PIN_SW 		GPIO_Pin_0

#define LCD_RES_Clr()  GPIO_ResetBits(TFT_PORT_RES,TFT_PIN_RES)//RES
#define LCD_RES_Set()  GPIO_SetBits(TFT_PORT_RES,TFT_PIN_RES)

#define LCD_DC_Clr()   GPIO_ResetBits(TFT_PORT_DC,TFT_PIN_DC)//DC
#define LCD_DC_Set()   GPIO_SetBits(TFT_PORT_DC,TFT_PIN_DC)
 		     
#define LCD_CS_Clr()   GPIO_ResetBits(TFT_PORT_CS,TFT_PIN_CS)//CS
#define LCD_CS_Set()   GPIO_SetBits(TFT_PORT_CS,TFT_PIN_CS)

#define LCD_BLK_Clr()  GPIO_ResetBits(TFT_PORT_BLK,TFT_PIN_BLK)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(TFT_PORT_BLK,TFT_PIN_BLK)


void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
void LCD_Off(void);
#endif





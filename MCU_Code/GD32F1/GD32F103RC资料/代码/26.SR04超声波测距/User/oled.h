
#ifndef __OLED_H
#define __OLED_H			  	 
#include "gd32f10x.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    


//-----------------OLED�˿ڶ���----------------  					   


#define OLED_SCLK_Clr() gpio_bit_write(GPIOB, GPIO_PIN_8, RESET)//CLK 
#define OLED_SCLK_Set() gpio_bit_write(GPIOB, GPIO_PIN_8, SET)

#define OLED_SDIN_Clr() gpio_bit_write(GPIOB, GPIO_PIN_9, RESET)//DIN
#define OLED_SDIN_Set() gpio_bit_write(GPIOB, GPIO_PIN_9, SET)


#define OLED_DC_Clr() gpio_bit_write(GPIOB, GPIO_PIN_12, RESET)//DC
#define OLED_DC_Set() gpio_bit_write(GPIOB, GPIO_PIN_12, SET)
 		     
#define OLED_CS_Clr()  gpio_bit_write(GPIOB, GPIO_PIN_13, RESET)//CS
#define OLED_CS_Set()  gpio_bit_write(GPIOB, GPIO_PIN_13, SET)

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);


#endif  
	 




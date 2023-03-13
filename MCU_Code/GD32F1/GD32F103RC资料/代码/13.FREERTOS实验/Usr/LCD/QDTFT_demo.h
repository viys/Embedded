#ifndef __QDTFT_DEMO_H
#define __QDTFT_DEMO_H 
//---------------------- include files -----------------------
#include "lcdPort.h"
//--------------------- macro define ----------------------
#define		LCD_LED_SET		lcd_backlight_control(1)	
#define		LCD_LED_CLR		lcd_backlight_control(0)		//IO¿ØÖÆ±³¹âÃð	

//--------------------- function declare ------------------
void Redraw_Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void showimage(const unsigned char *p); 
void QDTFT_Test_Demo(void);
void drawIcon_20_20(unsigned short int xStart, unsigned short int yStart, unsigned short int  fc, unsigned short int  bc,const unsigned char *p);
void drawIcon_32_14(unsigned short int xStart, unsigned short int yStart, unsigned short int fc, unsigned short int bc,const unsigned char *p);
void drawIcon_16_8(unsigned short int xStart, unsigned short int yStart, unsigned short int fc, unsigned short int bc,const unsigned char *p);
void drawIcon_16x16(unsigned short int xStart, unsigned short int yStart, unsigned short int fc, unsigned short int bc,const unsigned char *p);


#endif

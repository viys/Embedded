#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
#include "key.h"
#include "relays.h"
#include "remote.h"
int main(){
	
	USART_Config(USART_1,USART_MODE_COMMON,115200);
	Relays_GPIO_Config(RELAYS1);
	Relays_GPIO_Config(RELAYS2);
	Remote_Init();
	// zzEXTI_Key_Config(WKUP,2,2);
	float t=0;
	LCD_Init();//LCD≥ı ºªØ
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	//LCD_Off();
	printf("HELLO\r\n");
	while(1){
		LCD_ShowChinese(40,0,"≤‚ ‘∞Â",RED,WHITE,16,0);
		LCD_ShowString(10,20,"LCD_W:",RED,WHITE,16,0);
		LCD_ShowIntNum(58,20,LCD_W,3,RED,WHITE,16);
		LCD_ShowString(10,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowIntNum(58,40,LCD_H,3,RED,WHITE,16);
		LCD_ShowFloatNum1(10,60,t,4,RED,WHITE,16);
		t+=0.1;
		LCD_ShowPicture(100,20,40,40,gImage_1);
		
	}
}

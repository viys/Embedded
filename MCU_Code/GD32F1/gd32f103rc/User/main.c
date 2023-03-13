//#include "gd32f10x.h"
//#include "gd32f10x_libopt.h"
//#include "systick.h"
//#include <stdio.h>
//#include "led.h"
//#include "key.h"
//#include "usart.h"

//int main(void)
//{
//	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
//	delay_1ms(1000);
//	
//	led_init();
//	key_init(KEY_USER, KEY_MODE_EXTI);
//	usart_init(USART0);

//	
//	while(1)
//	{		
//		delay_1ms(1000);
//		printf("dddd");
//	}
//}
/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "oled.h"
#include <stdio.h>
#include "led.h"
#include "key.h"
#include "usart.h"
#include "adc.h"

uint16_t adc_value;//ADC采样值
uint16_t Vol_Value;//ADC采样值转换成电压值
uint8_t voltage;

//void intTchar(uint8_t *mystring, uint8_t myint);

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能 
	delay_1ms(1000);//等待1秒
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能
	usart_init(USART0);
	demo();
	
	OLED_Init();//OLED初始化  
	OLED_Clear();//OLED清屏 
	while(1)
	{
//		OLED_Clear();//OLED清屏 
//		OLED_ShowCHinese(18,0,0);//显示中文(光)
//		OLED_ShowCHinese(36,0,1);//显示中文(子)
//		OLED_ShowCHinese(54,0,2);//显示中文(物)
//		OLED_ShowCHinese(72,0,3);//显示中文(联)
		//OLED_ShowCHinese(90,0,4);//显示中文(网)
		 adc_flag_clear(ADC1,ADC_FLAG_EOC);//清除结束标志
        while(SET != adc_flag_get(ADC1,ADC_FLAG_EOC)){//获取转换结束标志
        }

        adc_value = ADC_RDATA(ADC1);//读取ADC数据
		Vol_Value=adc_value*3300/4095;//转换成电压值
		voltage = Vol_Value/100;
        delay_1ms(50);//等待500ms
		OLED_ShowString(6,3,(u8 *)"GD32F103 TEST");//显示字符
		OLED_ShowString(0,6,(u8 *)"voltage");//显示字符  
//		OLED_ShowNum(63,6,voltage,2,2);
		OLED_ShowChar(63,6,(char)(voltage/10 + '0'));
		OLED_ShowChar(70,6,'.');
		OLED_ShowChar(74,6,(char)(voltage%10 + '0'));
		//printf("voltage: %d\r\n",voltage);
		//OLED_ShowString(63,6,(u8 *)"666");//显示字符  
		delay_1ms(50);
	}
}

//void intTchar(uint8_t *mystring, uint8_t myint)
//{
//	myint / 10 + '0';
//	myint % 10;
//}

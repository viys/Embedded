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

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能 
	delay_1ms(1000);//等待1秒
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能

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
		OLED_ShowString(6,3,(u8 *)"GD32F103 TEST");//显示字符
		OLED_ShowString(0,6,(u8 *)"GAOCHAO");//显示字符  
		OLED_ShowString(63,6,(u8 *)"666");//显示字符  
		HAL_Delay(5000);//等待5秒
	}
   
}

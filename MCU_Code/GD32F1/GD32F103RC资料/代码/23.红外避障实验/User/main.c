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

	rcu_periph_clock_enable(RCU_GPIOA);
  gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);//PA0配置成输入，上拉
	

	OLED_Init(); //初始化OLED  
	OLED_Clear();//清屏

	while(1) 
	{		
		OLED_Clear();
    OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
		OLED_ShowCHinese(10,3,5);//障
		OLED_ShowCHinese(28,3,6);//碍
		OLED_ShowCHinese(46,3,7);//物
	  OLED_ShowString(80,3,"TEST");


		if(gpio_input_bit_get(GPIOA,GPIO_PIN_0)==0)//读取PA0引脚电平，如果是低电平，说明有障碍物
		{
				OLED_ShowCHinese(46,6,8);//有
		}else
		{
		
				OLED_ShowCHinese(46,6,9);//无
		}
		HAL_Delay(1000);//等待1秒
	}	  
 
}

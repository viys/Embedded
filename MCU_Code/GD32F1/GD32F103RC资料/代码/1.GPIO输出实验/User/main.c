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



int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
//	delay_1ms(1000);//等待1秒
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4管脚默认是NJTRST，要当GPIO，需要重映射
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4配置成输出
	
    while(1)
    {
        
        gpio_bit_set(GPIOB, GPIO_PIN_4);//输出高电平
        delay_1ms(1000);//等待1秒
        gpio_bit_reset(GPIOB, GPIO_PIN_4);//输出低电平
			delay_1ms(1000);//等待1秒
    }
}


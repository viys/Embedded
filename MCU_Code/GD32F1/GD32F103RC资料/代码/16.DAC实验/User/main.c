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
#include "gd32f10x_adc.h"

#define DAC_OUT_VAL0 0x7FF0//0x7ff*3.3v/4096=1.65V
#define DAC_OUT_VAL1 0x1FF0


void dac_config(void)
{
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_disable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);
    
    /* configure the DAC1 */
    dac_trigger_disable(DAC1);
    dac_wave_mode_config(DAC1, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC1);
    
    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    dac_concurrent_data_set(DAC_ALIGN_12B_L, DAC_OUT_VAL0, DAC_OUT_VAL1);
}

extern uint8_t IntFlag;
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
	delay_1ms(1000);//等待1秒
	
		//配置PA4 PA5 DAC引脚
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOA时钟使能
	rcu_periph_clock_enable(RCU_DAC);//DAC时钟使能
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
	dac_config();//DAC配置
	while(1)
	{
		    
		
	}
		
}


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
#include "usart.h"
#include <stdio.h>

uint16_t adc_value;//ADC采样值
uint16_t Vol_Value;//ADC采样值转换成电压值

//ADC初始化配置
void adc_config(void)
{
   /* reset ADC */
    adc_deinit(ADC1);
 
//    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC continous function enable */
    adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, ENABLE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
      /* ADC resolusion 6B */
    // adc_resolution_config(ADC1, ADC_RESOLUTION_6B); 
    /* ADC channel length config */
    adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 1);

  
    /* ADC regular channel config */
    adc_regular_channel_config(ADC1, 0, ADC_CHANNEL_13, ADC_SAMPLETIME_55POINT5);



    /* ADC trigger config */
     adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
     adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);


    /* enable ADC interface */
    adc_enable(ADC1);  
    delay_1ms(1);//等待1ms
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
    /* enable ADC interface */    
	//	adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    
}

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
	delay_1ms(1000);//等待1秒
	
		//配置PC3 ADC引脚
	 rcu_periph_clock_enable(RCU_GPIOC);//GPIOC时钟使能
	 gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);//PC3配置成模拟输入
	
		//配置ADC
	
	rcu_periph_clock_enable(RCU_ADC1);//使能ADC时钟
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);//ADC时钟8分频
	

  adc_config();//ADC配置
	adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);//规则采样软件触发
	usart_init(USART0);
	while(1)
	{
		    adc_flag_clear(ADC1,ADC_FLAG_EOC);//清除结束标志
        while(SET != adc_flag_get(ADC1,ADC_FLAG_EOC)){//获取转换结束标志
        }

        adc_value = ADC_RDATA(ADC1);//读取ADC数据
				Vol_Value=adc_value*3300/4095;//转换成电压值
				printf("adc:%d",Vol_Value);
        delay_1ms(500);//等待500ms
		
	}
		
}

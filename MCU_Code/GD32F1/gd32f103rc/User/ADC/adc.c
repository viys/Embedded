#include "adc.h"


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
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
    /* enable ADC interface */    
	//	adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    
}
void demo(void)
{
	 rcu_periph_clock_enable(RCU_GPIOC);//GPIOC时钟使能
	 gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);//PC3配置成模拟输入
	
		//配置ADC
	
	rcu_periph_clock_enable(RCU_ADC1);//使能ADC时钟
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);//ADC时钟8分频
	

	adc_config();//ADC配置
	adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);//规则采样软件触发
}

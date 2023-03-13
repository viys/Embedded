
/*****************************************************************************
  �ļ�: main.c
  ����: Zhengyu https://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20210401
	ƽ̨:MINI-GD32F103RCT6������
	΢�ź�:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "gd32f10x_adc.h"
#include "usart.h"
#include <stdio.h>

uint16_t adc_value;//ADC����ֵ
uint16_t Vol_Value;//ADC����ֵת���ɵ�ѹֵ

//ADC��ʼ������
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
    delay_1ms(1);//�ȴ�1ms
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC1);
    /* enable ADC interface */    
	//	adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    
}

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);//�ȴ�1��
	
		//����PC3 ADC����
	 rcu_periph_clock_enable(RCU_GPIOC);//GPIOCʱ��ʹ��
	 gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);//PC3���ó�ģ������
	
		//����ADC
	
	rcu_periph_clock_enable(RCU_ADC1);//ʹ��ADCʱ��
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);//ADCʱ��8��Ƶ
	

  adc_config();//ADC����
	adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);//��������������
	usart_init(USART0);
	while(1)
	{
		    adc_flag_clear(ADC1,ADC_FLAG_EOC);//���������־
        while(SET != adc_flag_get(ADC1,ADC_FLAG_EOC)){//��ȡת��������־
        }

        adc_value = ADC_RDATA(ADC1);//��ȡADC����
				Vol_Value=adc_value*3300/4095;//ת���ɵ�ѹֵ
				printf("adc:%d",Vol_Value);
        delay_1ms(500);//�ȴ�500ms
		
	}
		
}

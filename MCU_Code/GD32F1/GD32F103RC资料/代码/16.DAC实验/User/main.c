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
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);//�ȴ�1��
	
		//����PA4 PA5 DAC����
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOAʱ��ʹ��
	rcu_periph_clock_enable(RCU_DAC);//DACʱ��ʹ��
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
	dac_config();//DAC����
	while(1)
	{
		    
		
	}
		
}

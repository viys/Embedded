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



int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
//	delay_1ms(1000);//�ȴ�1��
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4�ܽ�Ĭ����NJTRST��Ҫ��GPIO����Ҫ��ӳ��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4���ó����
	
    while(1)
    {
        
        gpio_bit_set(GPIOB, GPIO_PIN_4);//����ߵ�ƽ
        delay_1ms(1000);//�ȴ�1��
        gpio_bit_reset(GPIOB, GPIO_PIN_4);//����͵�ƽ
			delay_1ms(1000);//�ȴ�1��
    }
}


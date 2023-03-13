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


extern uint8_t IntFlag;//�����жϷ�������־����EXTI1_IRQHandler()������λ
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);//�ȴ�1��
	
  //����PB4 GPIO���
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4�ܽ�Ĭ����NJTRST��Ҫ��GPIO����Ҫ��ӳ��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4���ó����
	
	//����PA1�ⲿ�ж�
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOAʱ��ʹ��
  gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_1);//PA1���ó���������
	nvic_irq_enable(EXTI1_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
  gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_1);//connect key EXTI line to key GPIO pin
	exti_init(EXTI_1, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//�½����ж�
	exti_interrupt_flag_clear(EXTI_1);//���жϱ�־
    while(1)
    {
        if(gpio_input_bit_get(GPIOA,GPIO_PIN_1)==RESET && IntFlag==0x55)//��ȡPA1���ŵ�ƽ����ǵ͵�ƽ�������ⲿ�ж��Ѿ�����
				{
					delay_1ms(100);//�ȴ�100ms
					gpio_bit_set(GPIOB, GPIO_PIN_4);//PB4����ߵ�ƽ
					delay_1ms(100);
					gpio_bit_reset(GPIOB, GPIO_PIN_4);//PB4����͵�ƽ
				}
    }
  
		
}

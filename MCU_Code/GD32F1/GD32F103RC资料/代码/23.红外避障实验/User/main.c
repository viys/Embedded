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
#include "oled.h"

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ�� 
	delay_1ms(1000);//�ȴ�1��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��

	rcu_periph_clock_enable(RCU_GPIOA);
  gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);//PA0���ó����룬����
	

	OLED_Init(); //��ʼ��OLED  
	OLED_Clear();//����

	while(1) 
	{		
		OLED_Clear();
    OLED_ShowCHinese(18,0,0);//��
		OLED_ShowCHinese(36,0,1);//��
		OLED_ShowCHinese(54,0,2);//��
		OLED_ShowCHinese(72,0,3);//��
		OLED_ShowCHinese(90,0,4);//��
		OLED_ShowCHinese(10,3,5);//��
		OLED_ShowCHinese(28,3,6);//��
		OLED_ShowCHinese(46,3,7);//��
	  OLED_ShowString(80,3,"TEST");


		if(gpio_input_bit_get(GPIOA,GPIO_PIN_0)==0)//��ȡPA0���ŵ�ƽ������ǵ͵�ƽ��˵�����ϰ���
		{
				OLED_ShowCHinese(46,6,8);//��
		}else
		{
		
				OLED_ShowCHinese(46,6,9);//��
		}
		HAL_Delay(1000);//�ȴ�1��
	}	  
 
}

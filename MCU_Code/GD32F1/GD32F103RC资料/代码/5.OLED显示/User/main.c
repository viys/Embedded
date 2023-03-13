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

	OLED_Init();//OLED��ʼ��  
	OLED_Clear();//OLED���� 
	while(1)
	{
//		OLED_Clear();//OLED���� 
//		OLED_ShowCHinese(18,0,0);//��ʾ����(��)
//		OLED_ShowCHinese(36,0,1);//��ʾ����(��)
//		OLED_ShowCHinese(54,0,2);//��ʾ����(��)
//		OLED_ShowCHinese(72,0,3);//��ʾ����(��)
		//OLED_ShowCHinese(90,0,4);//��ʾ����(��)
		OLED_ShowString(6,3,(u8 *)"GD32F103 TEST");//��ʾ�ַ�
		OLED_ShowString(0,6,(u8 *)"GAOCHAO");//��ʾ�ַ�  
		OLED_ShowString(63,6,(u8 *)"666");//��ʾ�ַ�  
		HAL_Delay(5000);//�ȴ�5��
	}
   
}

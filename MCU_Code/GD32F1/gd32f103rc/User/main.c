//#include "gd32f10x.h"
//#include "gd32f10x_libopt.h"
//#include "systick.h"
//#include <stdio.h>
//#include "led.h"
//#include "key.h"
//#include "usart.h"

//int main(void)
//{
//	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
//	delay_1ms(1000);
//	
//	led_init();
//	key_init(KEY_USER, KEY_MODE_EXTI);
//	usart_init(USART0);

//	
//	while(1)
//	{		
//		delay_1ms(1000);
//		printf("dddd");
//	}
//}
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
#include <stdio.h>
#include "led.h"
#include "key.h"
#include "usart.h"
#include "adc.h"

uint16_t adc_value;//ADC����ֵ
uint16_t Vol_Value;//ADC����ֵת���ɵ�ѹֵ
uint8_t voltage;

//void intTchar(uint8_t *mystring, uint8_t myint);

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ�� 
	delay_1ms(1000);//�ȴ�1��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	usart_init(USART0);
	demo();
	
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
		 adc_flag_clear(ADC1,ADC_FLAG_EOC);//���������־
        while(SET != adc_flag_get(ADC1,ADC_FLAG_EOC)){//��ȡת��������־
        }

        adc_value = ADC_RDATA(ADC1);//��ȡADC����
		Vol_Value=adc_value*3300/4095;//ת���ɵ�ѹֵ
		voltage = Vol_Value/100;
        delay_1ms(50);//�ȴ�500ms
		OLED_ShowString(6,3,(u8 *)"GD32F103 TEST");//��ʾ�ַ�
		OLED_ShowString(0,6,(u8 *)"voltage");//��ʾ�ַ�  
//		OLED_ShowNum(63,6,voltage,2,2);
		OLED_ShowChar(63,6,(char)(voltage/10 + '0'));
		OLED_ShowChar(70,6,'.');
		OLED_ShowChar(74,6,(char)(voltage%10 + '0'));
		//printf("voltage: %d\r\n",voltage);
		//OLED_ShowString(63,6,(u8 *)"666");//��ʾ�ַ�  
		delay_1ms(50);
	}
}

//void intTchar(uint8_t *mystring, uint8_t myint)
//{
//	myint / 10 + '0';
//	myint % 10;
//}

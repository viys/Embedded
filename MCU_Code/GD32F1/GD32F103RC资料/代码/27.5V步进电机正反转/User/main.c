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

#define uchar unsigned char
#define uint  unsigned int

uchar phasecw[4] ={GPIO_PIN_5,GPIO_PIN_2,GPIO_PIN_1,GPIO_PIN_0};//��ת�����ͨ����D-C-B-A
uchar phaseccw[4]={GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_5};//��ת�����ͨ����A-B-C-D

//��ת
void MotorCW(void)
{
 uchar i;
 for(i=0;i<4;i++)
  {

		gpio_bit_write(GPIOB,phasecw[i],SET);//�����
    delay_1ms(10);//�ȴ�5ms
		gpio_bit_write(GPIOB,phasecw[i],RESET);//�����
  }
}
//��ת
void MotorCCW(void)
{
 uchar i;
	
 for(i=0;i<4;i++)
  {

		gpio_bit_write(GPIOB,phaseccw[i],SET);//�����
    delay_1ms(10);//�ȴ�5ms
		gpio_bit_write(GPIOB,phaseccw[i],RESET);//�����
  }
}
int main(void)
{
	int i;
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);//������Ƶ108M(#define __SYSTEM_CLOCK_108M_PLL_HXTAL         (uint32_t)(108000000)),8M�ⲿ����  (#define HXTAL_VALUE    ((uint32_t)8000000))
  systick_config();//����1ms SysTick
	rcu_periph_clock_enable(RCU_AF);//AFʱ��ʹ�� 
	
  
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ�Ӵ�
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5);//PB0,PB1,PB2,PB5���ó����
	
    while(1)
    {
			
				delay_1ms(50);//�ȴ�50ms			
				for(i=0;i<500;i++)
				{
				MotorCW();   //��ת
				} 
				delay_1ms(500);
				for(i=0;i<500;i++)
				{
				MotorCCW();  //��ת
				} 
			delay_1ms(500); //�ȴ�500ms
    }
 
}

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

uint32_t HalTime1,HalTime2;
//�ȴ�ʱ�䣬us����
void Delay_us(unsigned long i)
{
	unsigned long j;
	for(;i>0;i--)
	{
			for(j=24;j>0;j--);
	}
}
uint32_t Distance_Calculate(uint32_t count)//����ʱ�䵥λ100us
{
    uint32_t Distance = 0;
    Distance = (uint32_t)(((float)count *17)/10);//���뵥λcm,����340M/S��ʱ��*�ٶ�/2=����
    return Distance;
}

uint32_t Distance;
int16_t Data;
uint32_t TimeCounter;
//��ʱ������
void timer_config(void)
{
    /* -----------------------------------------------------------------------
	  ϵͳ��Ƶ108MHZ,timer_initpara.prescalerΪ107��timer_initpara.periodΪ999��Ƶ�ʾ�Ϊ1KHZ
    ----------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);
	  nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
		nvic_irq_enable(TIMER1_IRQn, 0, 1);
		timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ�� 
	delay_1ms(1000);//�ȴ�1��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOAʱ��ʹ��
  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);//���ó����
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);//���ó�����
	OLED_Init();			       //��ʼ��OLED  
	OLED_Clear();            //����
	timer_config();//��ʱ����ʼ������
	while(1) 
	{		
       gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);//Ԥ������Trig����
		    HAL_Delay(5);
        gpio_bit_write(GPIOA, GPIO_PIN_0,SET);//����Trig����
        Delay_us(20);
        gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);//����Trig����
       Delay_us(20);
        while(gpio_input_bit_get(GPIOA, GPIO_PIN_1) == 0);//����ǵ͵�ƽ��һֱ��
       HalTime1= TimeCounter;
        while(gpio_input_bit_get(GPIOA, GPIO_PIN_1) == 1);//����Ǹߵ�ƽһֱ��
        if(TimeCounter>HalTime1)
				{
        HalTime2 = TimeCounter-HalTime1;
					if(HalTime2<0x300)
					{
        Distance = Distance_Calculate(HalTime2);//�������ֵ
					}
				}
	  OLED_Clear();
    OLED_ShowCHinese(18,0,0);//��
		OLED_ShowCHinese(36,0,1);//��
		OLED_ShowCHinese(54,0,2);//��
		OLED_ShowCHinese(72,0,3);//��
		OLED_ShowCHinese(90,0,4);//��
		OLED_ShowString(24,3,"SR04 TEST");
		OLED_ShowString(0,6,"Dis: ");  
		OLED_ShowNum(30,6,Distance,3,16);//��ʾ���룬��λcm  
		OLED_ShowString(60,6,"cm"); 
    HAL_Delay(200);		
	}
 
}
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
    timer_initpara.period            = 999;
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
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4�ܽ�Ĭ����NJTRST��Ҫ��GPIO����Ҫ��ӳ��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4���ó����
	timer_config();//��ʱ����ʼ�����ã�1ms����һ���жϣ�TIMER1_IRQHandler()������ʵ��LED��˸
    while(1)
    {

    }
}

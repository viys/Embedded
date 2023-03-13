#include "led.h"

/*!
    \brief      congfig LED1
    \Note	      PB4 -> LED1			
    \param[out] none
    \retval     none
*/
void led_init(void)
{
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��

	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4�ܽ�Ĭ����NJTRST��Ҫ��GPIO����Ҫ��ӳ��
	
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4���ó����
}

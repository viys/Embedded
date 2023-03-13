#include "led.h"

/*!
    \brief      congfig LED1
    \Note	      PB4 -> LED1			
    \param[out] none
    \retval     none
*/
void led_init(void)
{
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能

	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4管脚默认是NJTRST，要当GPIO，需要重映射
	
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4配置成输出
}

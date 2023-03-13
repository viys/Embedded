/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"


extern uint8_t IntFlag;//进入中断服务程序标志，在EXTI1_IRQHandler()函数置位
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
	delay_1ms(1000);//等待1秒
	
  //配置PB4 GPIO输出
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4管脚默认是NJTRST，要当GPIO，需要重映射
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//PB4配置成输出
	
	//配置PA1外部中断
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOA时钟使能
  gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_1);//PA1配置成上拉输入
	nvic_irq_enable(EXTI1_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
  gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_1);//connect key EXTI line to key GPIO pin
	exti_init(EXTI_1, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_1);//清中断标志
    while(1)
    {
        if(gpio_input_bit_get(GPIOA,GPIO_PIN_1)==RESET && IntFlag==0x55)//读取PA1引脚电平如果是低电平，而且外部中断已经进入
				{
					delay_1ms(100);//等待100ms
					gpio_bit_set(GPIOB, GPIO_PIN_4);//PB4输出高电平
					delay_1ms(100);
					gpio_bit_reset(GPIOB, GPIO_PIN_4);//PB4输出低电平
				}
    }
  
		
}

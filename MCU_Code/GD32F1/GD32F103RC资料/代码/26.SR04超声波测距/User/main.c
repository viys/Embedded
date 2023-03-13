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
#include "oled.h"

uint32_t HalTime1,HalTime2;
//等待时间，us级别
void Delay_us(unsigned long i)
{
	unsigned long j;
	for(;i>0;i--)
	{
			for(j=24;j>0;j--);
	}
}
uint32_t Distance_Calculate(uint32_t count)//传入时间单位100us
{
    uint32_t Distance = 0;
    Distance = (uint32_t)(((float)count *17)/10);//距离单位cm,声速340M/S，时间*速度/2=距离
    return Distance;
}

uint32_t Distance;
int16_t Data;
uint32_t TimeCounter;
//定时器配置
void timer_config(void)
{
    /* -----------------------------------------------------------------------
	  系统主频108MHZ,timer_initpara.prescaler为107，timer_initpara.period为999，频率就为1KHZ
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
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能 
	delay_1ms(1000);//等待1秒
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能
	rcu_periph_clock_enable(RCU_GPIOA);//GPIOA时钟使能
  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);//配置成输出
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);//配置成输入
	OLED_Init();			       //初始化OLED  
	OLED_Clear();            //清屏
	timer_config();//定时器初始化配置
	while(1) 
	{		
       gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);//预先拉低Trig引脚
		    HAL_Delay(5);
        gpio_bit_write(GPIOA, GPIO_PIN_0,SET);//拉高Trig引脚
        Delay_us(20);
        gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);//拉低Trig引脚
       Delay_us(20);
        while(gpio_input_bit_get(GPIOA, GPIO_PIN_1) == 0);//如果是低电平，一直等
       HalTime1= TimeCounter;
        while(gpio_input_bit_get(GPIOA, GPIO_PIN_1) == 1);//如果是高电平一直等
        if(TimeCounter>HalTime1)
				{
        HalTime2 = TimeCounter-HalTime1;
					if(HalTime2<0x300)
					{
        Distance = Distance_Calculate(HalTime2);//计算距离值
					}
				}
	  OLED_Clear();
    OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
		OLED_ShowString(24,3,"SR04 TEST");
		OLED_ShowString(0,6,"Dis: ");  
		OLED_ShowNum(30,6,Distance,3,16);//显示距离，单位cm  
		OLED_ShowString(60,6,"cm"); 
    HAL_Delay(200);		
	}
 
}
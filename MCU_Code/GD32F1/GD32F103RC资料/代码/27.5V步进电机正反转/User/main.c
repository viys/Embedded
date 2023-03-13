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

#define uchar unsigned char
#define uint  unsigned int

uchar phasecw[4] ={GPIO_PIN_5,GPIO_PIN_2,GPIO_PIN_1,GPIO_PIN_0};//正转电机导通相序D-C-B-A
uchar phaseccw[4]={GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_5};//反转电机导通相序A-B-C-D

//正转
void MotorCW(void)
{
 uchar i;
 for(i=0;i<4;i++)
  {

		gpio_bit_write(GPIOB,phasecw[i],SET);//输出高
    delay_1ms(10);//等待5ms
		gpio_bit_write(GPIOB,phasecw[i],RESET);//输出低
  }
}
//反转
void MotorCCW(void)
{
 uchar i;
	
 for(i=0;i<4;i++)
  {

		gpio_bit_write(GPIOB,phaseccw[i],SET);//输出高
    delay_1ms(10);//等待5ms
		gpio_bit_write(GPIOB,phaseccw[i],RESET);//输出低
  }
}
int main(void)
{
	int i;
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);//设置主频108M(#define __SYSTEM_CLOCK_108M_PLL_HXTAL         (uint32_t)(108000000)),8M外部晶振  (#define HXTAL_VALUE    ((uint32_t)8000000))
  systick_config();//配置1ms SysTick
	rcu_periph_clock_enable(RCU_AF);//AF时钟使能 
	
  
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟打开
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5);//PB0,PB1,PB2,PB5配置成输出
	
    while(1)
    {
			
				delay_1ms(50);//等待50ms			
				for(i=0;i<500;i++)
				{
				MotorCW();   //正转
				} 
				delay_1ms(500);
				for(i=0;i<500;i++)
				{
				MotorCCW();  //反转
				} 
			delay_1ms(500); //等待500ms
    }
 
}

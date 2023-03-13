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
 unsigned char  fseg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
 unsigned char  segbit[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char  disbuf[4]={0,0,0,0};
void LED4_Display (void);			// LED显示
void LED_OUT(uchar X);				// LED单字节串行移位函数
unsigned char  LED_0F[17] = //低电平显示
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0x7F,0x7F
};
	unsigned char Counter=0;
	unsigned long TimeCounter=0;

#define SCLK_PIN GPIO_PIN_4
#define RCLK_PIN GPIO_PIN_5
#define DIO_PIN  GPIO_PIN_6
uchar LED[8];	//用于LED的8位显示缓存
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void LED4_Display (void)
{
	unsigned char  *led_table;          // 查表指针
	uchar i;
	//显示第1位
	led_table = LED_0F + LED[0];
	i = *led_table;

	LED_OUT(i);			
	LED_OUT(0x01);		

	
	gpio_bit_write(GPIOA, RCLK_PIN, RESET);//管脚输出低
  delay_1ms(1);
	gpio_bit_write(GPIOA, RCLK_PIN, SET);//管脚输出高
	//显示第2位
	led_table = LED_0F + LED[1];
	i = *led_table;

	LED_OUT(i);		
	LED_OUT(0x02);		

	gpio_bit_write(GPIOA, RCLK_PIN, RESET);
  delay_1ms(1);
	gpio_bit_write(GPIOA, RCLK_PIN, SET);
	//显示第3位
	led_table = LED_0F + LED[2];
	i = *led_table&0X7F;

	LED_OUT(i);			
	LED_OUT(0x04);	

	gpio_bit_write(GPIOA, RCLK_PIN, RESET);
  delay_1ms(1);
	gpio_bit_write(GPIOA, RCLK_PIN, SET);
	//显示第4位
	led_table = LED_0F + LED[3];
	i = *led_table;

	LED_OUT(i);			
	LED_OUT(0x08);		

	gpio_bit_write(GPIOA, RCLK_PIN, RESET);
	delay_1ms(1);
	gpio_bit_write(GPIOA, RCLK_PIN, SET);
}

void LED_OUT(uchar X)
{
	uchar i;
	for(i=8;i>=1;i--)
	{
		if (X&0x80) 
		{
			gpio_bit_write(GPIOA, DIO_PIN, SET);
		}else 
		{
			gpio_bit_write(GPIOA, DIO_PIN, RESET);
		}
		X<<=1;
		gpio_bit_write(GPIOA, SCLK_PIN, RESET);
		gpio_bit_write(GPIOA, SCLK_PIN, SET);
	}
}


int main(void)
{
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);//设置主频108M(#define __SYSTEM_CLOCK_108M_PLL_HXTAL         (uint32_t)(108000000)),8M外部晶振  (#define HXTAL_VALUE    ((uint32_t)8000000))
  systick_config();//配置1ms SysTick
	rcu_periph_clock_enable(RCU_AF);//AF时钟使能 
	

	rcu_periph_clock_enable(RCU_GPIOA);//GPIOA时钟打开
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCLK_PIN|RCLK_PIN|DIO_PIN);//PA4,PA5,PA6配置成输出

  LED[0]=1;//数字偏移量
	LED[1]=2;
	LED[2]=3;
	LED[3]=4;
	LED[4]=5;
	LED[5]=6;
	LED[6]=7;
	LED[7]=8;

	while(1) 
	{		
			LED4_Display();//数码管显示

	}	  
 
}

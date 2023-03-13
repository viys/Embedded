#include "stm32f10x.h"

#define SCLK_PIN	GPIO_Pin_10
#define RCLK_PIN	GPIO_Pin_11
#define DIO_PIN 	GPIO_Pin_12

#define SCLK_LOW	GPIO_ResetBits(GPIOC, SCLK_PIN)
#define SCLK_HIGH	GPIO_SetBits(GPIOC, SCLK_PIN)

#define RCLK_LOW	GPIO_ResetBits(GPIOC, RCLK_PIN)
#define RCLK_HIGH	GPIO_SetBits(GPIOC, RCLK_PIN)

#define DATA_0		GPIO_ResetBits(GPIOC, DIO_PIN)
#define DATA_1		GPIO_SetBits(GPIOC, DIO_PIN)

void SysTick_Init(void)
{

	if (SysTick_Config(72000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

//系统滴答定时器
void SysTick_Delay(uint32_t ms)
{
	SysTick_Config(72000);
	
	for(uint32_t i = 0; i < ms; i++)
	{
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// 关闭SysTick定时器
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructrue.GPIO_Pin = SCLK_PIN;//数据输入时钟线
	GPIO_Init(GPIOC, &GPIO_InitStructrue);
	
	GPIO_InitStructrue.GPIO_Pin = RCLK_PIN;//数据输出锁存线
	GPIO_Init(GPIOC, &GPIO_InitStructrue);
	
	GPIO_InitStructrue.GPIO_Pin = DIO_PIN;//数据输入线
	GPIO_Init(GPIOC, &GPIO_InitStructrue);
	
	GPIO_ResetBits(GPIOC, SCLK_PIN);
	GPIO_ResetBits(GPIOC, RCLK_PIN);
	GPIO_ResetBits(GPIOC, DIO_PIN);
}

void LED_OUT(uint8_t x)
{
	uint8_t i;
	for(i=8;i>=1;i--)
	{
		if (x&0x80) 
		{
			DATA_1;
		}else 
		{
			DATA_0;
		}
		x<<=1;
		SCLK_LOW;
		SCLK_HIGH;
	}
}

//管脚由15->1->7
uint8_t LED_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -	   .
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf,0x7f
};
/********************* 数码管显示函数 *******************///

void Digital_Tube(uint8_t num,uint8_t whichbit)
{
	
	LED_OUT(LED_0F[num]);
	LED_OUT(whichbit);
	RCLK_LOW;
	RCLK_HIGH;
}


int main()
{
	GPIO_Config();
	Digital_Tube(9,1);//暂时无法动态显示

	while(1)
	{

	}
}

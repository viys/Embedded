#include "systick.h"

static __IO u32 TimingDelay;


void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

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

void SysTick_Handler(void)
{
	//用到中断必须要有中断函数
	//TimingDelay_Decrement();	
}

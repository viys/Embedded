#include "bsp_systick.h"

__IO u32 TimingDelay;

void SysTick_Init(void)
{
	if(SysTick_Config (SystemCoreClock / 1000))
	{
		while(1);
	}
}

void Delay_ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	
	while(TimingDelay != 0);
}

void Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;
	SysTick_Config (SystemCoreClock/1000 );
	
	for( i=0; i<ms; i++)
	{
		//当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		//当置1时，读取该位会清0
		while(!((SysTick->CTRL)&(1<<16)));//等待延迟1ms
	}
	//关闭SysTick定时器
	SysTick ->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk ;
}

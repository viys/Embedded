#include "sysclick.h"
void Sysclick_us(u32 us)
{
	SysTick ->CTRL &= ~(1<<2);		//选择外部时钟源
	SysTick ->LOAD = us * (SYSCLK /1000) ; 	//写入重载值
	SysTick ->VAL = 0;
	SysTick ->CTRL |=1<<0;			//使能寄存器
	while(!(SysTick ->CTRL & (1<<16)))
	{
	//等待时间
	}
	SysTick ->CTRL &= ~(1<<0);		//关掉计数器
	
}


void Sysclick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1<<2);		//选择外部时钟源
	SysTick ->LOAD = ms  * SYSCLK  ; 	//写入重载值
	SysTick ->VAL = 0;
	SysTick ->CTRL |=1<<0;			//使能寄存器
	while(!(SysTick ->CTRL & (1<<16)))
	{
	//等待时间
	}
	SysTick ->CTRL &= ~(1<<0);		//关掉时钟
	
}

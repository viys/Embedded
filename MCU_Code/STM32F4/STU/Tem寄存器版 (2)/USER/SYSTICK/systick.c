#include "systick.h"
/*******************系统滴答定时器*******************/
void SysTick_us(u32 us)
{
	SysTick ->CTRL &= ~(1 << 2);	//选择外部时钟源
	
	SysTick ->LOAD = SYSCLK * us;	//写入重载值
	SysTick ->VAL = 0;						//清计数器
	
	SysTick ->CTRL  |= 1 << 0;		//使能计数器
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//等待时间到
  }
		SysTick ->CTRL  &= ~(1 << 0);		//关闭计数器
}

void SysTick_inter_ms(u32 ms)//延时时间中断函数
{
	SysTick ->CTRL &= ~(1 << 2);//选择外部时钟源
	
	SysTick ->LOAD = SYSCLK * ms * 1000;//写入重载值
	
	SysTick ->VAL = 0;//计数器清零
	SysTick ->CTRL |= 1 << 0;//使能计数器
	
	SysTick ->CTRL |= 1 << 1;//使能中断

}


void SysTick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2);	//选择外部时钟源
	
	SysTick ->LOAD = SYSCLK * ms * 1000;	//写入重载值
	SysTick ->VAL = 0;						//清计数器
	
	SysTick ->CTRL  |= 1 << 0;		//使能计数器
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//等待时间到
  }
		SysTick ->CTRL  &= ~(1 << 0);		//关闭计数器
}




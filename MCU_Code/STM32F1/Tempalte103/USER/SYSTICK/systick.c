#include "systick.h"

/**
@brief	系统滴答毫秒计时
@note   168*10^6 / 8 HZ
				18750 为校准值
@param	
@retval	
*/
void SysTick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2);					//选择外部时钟源
	
	SysTick ->LOAD = SYSCLK * ms  ;				//写入重载值
	SysTick ->VAL = 0;										//清计数器
	
	SysTick ->CTRL  |= 1 << 0;						//使能计数器
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//等待时间到
  }
		SysTick ->CTRL  &= ~(1 << 0);//关闭计数器
}

/**
@brief	延时时间中断函数
@Note		
@param	ms
@retval	None
*/
void SysTick_inter_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2); //选择外部时钟源
	
	SysTick ->LOAD = 18750 * ms; //写入重载值
	SysTick ->VAL = 0; 			 		 //计数器清零
	SysTick ->CTRL |= 1 << 0;	   //使能计数器
	
	SysTick ->CTRL |= 1 << 1;	   //使能中断
}

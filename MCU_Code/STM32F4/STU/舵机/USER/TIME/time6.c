#include "time6.h"

/*******************基本定时器*******************/
u16 PSC = 8400;

/*
@brief	Time6初始化
@note   None
@param	None
@retval	None
*/
void Time6_Init(void)
{
	RCC ->APB1ENR |= 1 << 4;//使能time6的时钟
	
	/*配置定时器的工作方式*/
	TIM6 ->CR1 |= 1 << 7;		//使用影子寄存器
	
	TIM6 ->CR1 |= 1 << 3;		//单次脉冲
	
	TIM6 ->CR1 &= ~(1 << 2);//更新请求源  p489
	TIM6 ->CR1 &= ~(1 << 1);//更新使能
}

void Time6_delay(u16 psc,u16 arr)//84MHZ  10ms -- 100
{
	TIM6 ->PSC = psc - 1;		//设定的预分频值
	TIM6 ->ARR = arr;				//设定的计数次数
	
	TIM6 ->EGR |= 1 << 0;		//把设定的值更新到影子寄存器
	
	TIM6 ->SR &= ~(1 << 0);	//重新初始化CNT
	
	TIM6 ->CR1 |= 1 << 0;		//使能计数器
	
	while(!(TIM6 ->SR) & (1 << 0))
	{
		/*等待时间到*/
	}
	TIM6 ->CR1 &= ~(1 << 0);//标志位清零
	TIM6 ->CR1 &= ~(1 << 0);//关闭计数器
}

/*************Time6中断初始化*************/
void Time6_inter_Init(u16 arr)
{
	u32 Priority;
	
	RCC->APB1ENR |= 1 << 4; //使能time6
	
	//配置寄存器工作方式
	TIM6 ->CR1 |= 1 << 7;   //使用影子寄存器
	
	TIM6 ->CR1 &= ~(1 << 3);//循环脉冲
	
	TIM6 ->CR1 &= ~(1 << 2);//更新请求源
	TIM6 ->CR1 &= ~(1 << 1);//更新使能
	
	//设置预分频值
	TIM6 ->PSC = PSC - 1;		//设定的预分频值
	TIM6 ->ARR = arr;  			//设定的计数次数
	TIM6 ->EGR |= 1 << 0;		//把设定的值更新到影子寄存器
	
	//设置中断
	TIM6 ->DIER |= 1 << 0;
	
	//配置NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (TIM6_DAC_IRQn,Priority );
	NVIC_EnableIRQ (TIM6_DAC_IRQn);
	
	
	//使能计数器
	TIM6 ->CR1 |= 1 << 0;
}

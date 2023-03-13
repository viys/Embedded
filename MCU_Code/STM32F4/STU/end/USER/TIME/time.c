#include "time.h"

/**
@brief	time初始化
@Note		TIME3 -- PC6
@param	预分频 重装载值 比较值
@retval	None
*/
void TIM3_CH1_Init(u16 psc, u16 arr, u16 ccr)
{
	/*配置定时器对应的GPIO*/
	RCC->AHB1ENR |= 1 << 2;  //使能GPIOC时钟
	
	GPIOC->MODER &= ~(3 << (2 * 6));
	GPIOC->MODER |= 2 << (2 * 6);  //复用模式
	
	GPIOC->AFR[0] &= ~(15 << (4 * 6));
	GPIOC->AFR[0] |= 2 << (4 * 6);  //复用为TIM3
	
	/*配置计数器*/
	RCC->APB1ENR |= 1 << 1;  //使能TIM3时钟
	TIM3->CR1 |= 1 << 7;  //使用影子寄存器
	TIM3->CR1 &= ~(1 << 3);  //循环计数

	TIM3->CR1 &= ~(1 << 2);  //更新请求源
	TIM3->CR1 &= ~(1 << 1);  //更新使能
	
	/* 设定预分频值*/
	TIM3->PSC = psc - 1;  //设定的预分频值
	TIM3->ARR = arr;  //设定的计数次数
	TIM3->CCR1 = ccr;  //设定占空比
	TIM3->EGR |= 1 << 0;  //把设定的值更新到影子寄存器中
	
	/*配置PWM工作方式*/
	TIM3->CCMR1 &= ~(1 << 7);  //不受外部时钟影响
	TIM3->CCMR1 |= 7 << 4;  //PWM模式2
	TIM3->CCMR1 &= ~(3 << 0);  //通道1配置为输出模式
  TIM3->CCER &= ~(1 << 0);  //高电平有效
	TIM3->CCER |= 1 << 0;  //使能输出通道
	
	/*使能计数器*/
	TIM3->CR1 |= 1 << 0;
}
	


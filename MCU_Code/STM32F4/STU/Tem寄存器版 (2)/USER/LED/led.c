#include "led.h"

void LED_Init(void)
{
	RCC->AHB1ENR |= 1 << 2;//打开时钟
	GPIOC->MODER &= ~(3 << (2 * 4));
	GPIOC->MODER |= 1 << (2 * 4);//配置为输出模式
	
	GPIOC->OTYPER &= ~(1 << 4);//推挽输出
	
	GPIOC->OSPEEDR &= ~(3 << (2 * 4));
	GPIOC->OSPEEDR |= 2 << (2 * 4);//50MHZ
	
	GPIOC->PUPDR &= ~(3 << (2 * 4));//无上下拉
	GPIOC->ODR |= 1 << 4;//灯灭
	
}




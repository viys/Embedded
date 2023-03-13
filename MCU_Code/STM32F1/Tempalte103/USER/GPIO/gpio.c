#include "gpio.h"

void GPIOBx_Init(u8 pin)
{
	// 开启GPIOB 端口时钟
	RCC->APB2ENR |= (1 << 3);

	//清空控制PB0的端口位
	GPIOB->CRL &= ~( 0x0F << (4 * pin));	
	
	// 配置PB0为通用推挽输出，速度为10M
	GPIOB->CRL |= (1 << (4 * pin));

	// PB0 输出 低电平
	GPIOB->ODR |= (1 << pin);
}	
void GPIOA12_Init(void )
{
	// 开启GPIOB 端口时钟
	RCC->APB2ENR |= (1 << 3);

	//清空控制PB0的端口位
	GPIOA->CRH &= ~( 0x0F << (4 * 4));	
	
	// 配置PB0为通用推挽输出，速度为10M
	GPIOA->CRH |= (1 << (4 * 4));

	// PB0 输出 低电平
	GPIOA->ODR |= (1 << 12);
}	




#include "led.h"

void LED_Init(void)
{
	RCC->AHB1ENR |= 1 << 2;//��ʱ��
	GPIOC->MODER &= ~(3 << (2 * 4));
	GPIOC->MODER |= 1 << (2 * 4);//����Ϊ���ģʽ
	
	GPIOC->OTYPER &= ~(1 << 4);//�������
	
	GPIOC->OSPEEDR &= ~(3 << (2 * 4));
	GPIOC->OSPEEDR |= 2 << (2 * 4);//50MHZ
	
	GPIOC->PUPDR &= ~(3 << (2 * 4));//��������
	GPIOC->ODR |= 1 << 4;//����
	
}




#include "gpio.h"

void GPIOBx_Init(u8 pin)
{
	// ����GPIOB �˿�ʱ��
	RCC->APB2ENR |= (1 << 3);

	//��տ���PB0�Ķ˿�λ
	GPIOB->CRL &= ~( 0x0F << (4 * pin));	
	
	// ����PB0Ϊͨ������������ٶ�Ϊ10M
	GPIOB->CRL |= (1 << (4 * pin));

	// PB0 ��� �͵�ƽ
	GPIOB->ODR |= (1 << pin);
}	
void GPIOA12_Init(void )
{
	// ����GPIOB �˿�ʱ��
	RCC->APB2ENR |= (1 << 3);

	//��տ���PB0�Ķ˿�λ
	GPIOA->CRH &= ~( 0x0F << (4 * 4));	
	
	// ����PB0Ϊͨ������������ٶ�Ϊ10M
	GPIOA->CRH |= (1 << (4 * 4));

	// PB0 ��� �͵�ƽ
	GPIOA->ODR |= (1 << 12);
}	




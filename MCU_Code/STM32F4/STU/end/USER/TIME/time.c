#include "time.h"

/**
@brief	time��ʼ��
@Note		TIME3 -- PC6
@param	Ԥ��Ƶ ��װ��ֵ �Ƚ�ֵ
@retval	None
*/
void TIM3_CH1_Init(u16 psc, u16 arr, u16 ccr)
{
	/*���ö�ʱ����Ӧ��GPIO*/
	RCC->AHB1ENR |= 1 << 2;  //ʹ��GPIOCʱ��
	
	GPIOC->MODER &= ~(3 << (2 * 6));
	GPIOC->MODER |= 2 << (2 * 6);  //����ģʽ
	
	GPIOC->AFR[0] &= ~(15 << (4 * 6));
	GPIOC->AFR[0] |= 2 << (4 * 6);  //����ΪTIM3
	
	/*���ü�����*/
	RCC->APB1ENR |= 1 << 1;  //ʹ��TIM3ʱ��
	TIM3->CR1 |= 1 << 7;  //ʹ��Ӱ�ӼĴ���
	TIM3->CR1 &= ~(1 << 3);  //ѭ������

	TIM3->CR1 &= ~(1 << 2);  //��������Դ
	TIM3->CR1 &= ~(1 << 1);  //����ʹ��
	
	/* �趨Ԥ��Ƶֵ*/
	TIM3->PSC = psc - 1;  //�趨��Ԥ��Ƶֵ
	TIM3->ARR = arr;  //�趨�ļ�������
	TIM3->CCR1 = ccr;  //�趨ռ�ձ�
	TIM3->EGR |= 1 << 0;  //���趨��ֵ���µ�Ӱ�ӼĴ�����
	
	/*����PWM������ʽ*/
	TIM3->CCMR1 &= ~(1 << 7);  //�����ⲿʱ��Ӱ��
	TIM3->CCMR1 |= 7 << 4;  //PWMģʽ2
	TIM3->CCMR1 &= ~(3 << 0);  //ͨ��1����Ϊ���ģʽ
  TIM3->CCER &= ~(1 << 0);  //�ߵ�ƽ��Ч
	TIM3->CCER |= 1 << 0;  //ʹ�����ͨ��
	
	/*ʹ�ܼ�����*/
	TIM3->CR1 |= 1 << 0;
}
	


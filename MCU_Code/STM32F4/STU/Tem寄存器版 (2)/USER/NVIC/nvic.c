#include "nvic.h"
#include "systick.h"

char rec[20] = {0};
u8 i =0;
u8 flag = 0;//��ʾ����״̬  1���������
//�жϷ�����
/*************************�ڲ��ж�*************************/
void SysTick_Handler(void )
{
	if(SysTick ->CTRL & (1 << 16))
	{
		GPIOC->ODR ^= 1 << 4;
	}
}

/*************************�ⲿ�ж�*************************/

void USART1_IRQHandler(void)//����1�жϷ�����
{
	if(USART1 ->SR & (1 << 5))
	{
		USART1 ->SR &= ~(1 << 5);//�����жϱ�־λ����
		rec[i] = USART1 ->DR ;
		
		if(rec[i] == '#' && flag == 0)
		{
			i = 0;
			flag = 1;
		}
		else
		{
			i++;
		}
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if(TIM6 ->SR & (1 << 0))
	{
		TIM6 ->SR &= ~(1 << 0);//�����־λ
		GPIOC ->ODR ^= 1 << 4;
	}
}

u8 width;//������

void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 CNT1;
	u16 CNT2;
	if(TIM9 ->SR & (1 << 1))//�����ز���
	{
		TIM9 ->SR &= ~(1 << 1);//�����־λ
		CNT2 = TIM9 ->CCR1 ;
		if(CNT2 < CNT1 )
		{
			width = 0xffff - CNT1 + CNT2 ;
		}
		else
		{
			width = CNT2 - CNT1 ;
		}
		CNT1 = CNT2 ;
	}
	if(TIM9 ->SR & (1 << 0))//�����ж�
	{
		TIM9 ->SR &= ~(1 << 0);
	}
}

//�жϺ�������ò�Ҫ���ӳٺ�ѭ�����

#include "time6.h"

/*******************������ʱ��*******************/
u16 PSC = 8400;

/*
@brief	Time6��ʼ��
@note   None
@param	None
@retval	None
*/
void Time6_Init(void)
{
	RCC ->APB1ENR |= 1 << 4;//ʹ��time6��ʱ��
	
	/*���ö�ʱ���Ĺ�����ʽ*/
	TIM6 ->CR1 |= 1 << 7;		//ʹ��Ӱ�ӼĴ���
	
	TIM6 ->CR1 |= 1 << 3;		//��������
	
	TIM6 ->CR1 &= ~(1 << 2);//��������Դ  p489
	TIM6 ->CR1 &= ~(1 << 1);//����ʹ��
}

void Time6_delay(u16 psc,u16 arr)//84MHZ  10ms -- 100
{
	TIM6 ->PSC = psc - 1;		//�趨��Ԥ��Ƶֵ
	TIM6 ->ARR = arr;				//�趨�ļ�������
	
	TIM6 ->EGR |= 1 << 0;		//���趨��ֵ���µ�Ӱ�ӼĴ���
	
	TIM6 ->SR &= ~(1 << 0);	//���³�ʼ��CNT
	
	TIM6 ->CR1 |= 1 << 0;		//ʹ�ܼ�����
	
	while(!(TIM6 ->SR) & (1 << 0))
	{
		/*�ȴ�ʱ�䵽*/
	}
	TIM6 ->CR1 &= ~(1 << 0);//��־λ����
	TIM6 ->CR1 &= ~(1 << 0);//�رռ�����
}

/*************Time6�жϳ�ʼ��*************/
void Time6_inter_Init(u16 arr)
{
	u32 Priority;
	
	RCC->APB1ENR |= 1 << 4; //ʹ��time6
	
	//���üĴ���������ʽ
	TIM6 ->CR1 |= 1 << 7;   //ʹ��Ӱ�ӼĴ���
	
	TIM6 ->CR1 &= ~(1 << 3);//ѭ������
	
	TIM6 ->CR1 &= ~(1 << 2);//��������Դ
	TIM6 ->CR1 &= ~(1 << 1);//����ʹ��
	
	//����Ԥ��Ƶֵ
	TIM6 ->PSC = PSC - 1;		//�趨��Ԥ��Ƶֵ
	TIM6 ->ARR = arr;  			//�趨�ļ�������
	TIM6 ->EGR |= 1 << 0;		//���趨��ֵ���µ�Ӱ�ӼĴ���
	
	//�����ж�
	TIM6 ->DIER |= 1 << 0;
	
	//����NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (TIM6_DAC_IRQn,Priority );
	NVIC_EnableIRQ (TIM6_DAC_IRQn);
	
	
	//ʹ�ܼ�����
	TIM6 ->CR1 |= 1 << 0;
}

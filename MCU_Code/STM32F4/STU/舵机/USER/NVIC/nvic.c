#include "nvic.h"
#include "systick.h"
#include "led.h"
#include "usart.h"
#include "key.h"
#include "adc.h"
#include "time_gen.h"

extern u8 keystate;
char rec[20] = {0};
u8 i =0;
u8 flag = 0;//��ʾ����״̬  1���������
//�жϷ�����
/*************************�ڲ��ж�*************************/
void SysTick_Handler(void)
{
	float Vx = 0;
	u16 temp_light = 0;
	if(SysTick ->CTRL & (1 << 16))
	{	
		temp_light = ADC_GETD();
		Vx = (3.3 * temp_light) / 4096;
		printf ("��ǿ=%.1f\r\n",3.3-Vx);
		printf ("low = %d\r\n",low);
		TIM10_CH1_Init(168,330,((3.3-Vx)*100));
	}
}

/*************************�ⲿ�ж�*************************/

void USART1_IRQHandler(void)	 //����1�жϷ�����
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
		LED4_TOGGLE;
	}
}

//u8 width;//������

//void TIM8_TRG_COM_TIM14_IRQHandler(void)
//{
//	static u16 CNT1;
//	u16 CNT2;
//	
//	if(TIM14 ->SR & (1 << 1))	//�����ز���
//	{
//		TIM14 ->SR &= ~(1 << 1); //�����־λ
//		CNT2 = TIM14 ->CCR1;
//		if(CNT2 < CNT1 )
//		{
//			width = 10000 - CNT1 + CNT2 ;
//		}
//		else
//		{
//			width = CNT2 - CNT1 ;
//		}
//		CNT1 = CNT2 ;
//	}
//	if(TIM14 ->SR & (1 << 0))	//�����ж�
//	{
//		TIM14 ->SR &= ~(1 << 0);
//	}
//}
u16 high =0;
u16 low = 0;
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	static u8 tim14_flag = 1;
	static u8 update_flag = 0;
	if(TIM14 ->SR & (1 << 1))	
	{
		TIM14 ->CNT = 0;//��֤��0��ʼ����
		TIM14 ->SR &= ~(1 << 1);//��־λtim		if(TIM14_flag)//����������
		if(tim14_flag )	
		{
			tim14_flag = 0;
			low = update_flag * 2000 + TIM14 ->CCR1;//����͵�ƽ��ʱ��
		}
		else
		{
			tim14_flag = 1;
			high = update_flag * 2000 + TIM14 ->CCR1;
		}
			update_flag = 0;
			TIM14 ->CCER ^= 1 << 1;//�����л�
		}
		if(TIM14 ->SR & (1 << 0))
		{
			update_flag ++;
			TIM14 ->SR &= ~(1 << 0);
		}
}
/************�жϺ�������ò�Ҫ���ӳٺ�ѭ�����*************/


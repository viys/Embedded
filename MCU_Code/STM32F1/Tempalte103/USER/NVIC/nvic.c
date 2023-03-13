#include "nvic.h"
#include "systick.h"


/**
@brief	�δ�ʱ���жϷ�����
@Note		����ADC�����������ռ�ձ�
@param	None
@retval	None
*/
void SysTick_Handler(void)
{
	if(SysTick ->CTRL & (1 << 16))
	{	
		/* code */
	}
}

char rec[20] = {0};
u8 i =0;
u8 flag = 0;//��ʾ����״̬  1���������

/**
@brief	����1�жϷ�����
@Note		
@param	None
@retval	None
*/
void USART1_IRQHandler(void)
{
	if(USART1 ->SR & (1 << 5))
	{
		USART1 ->SR &= ~(1 << 5);//�����жϱ�־λ����
		rec[i] = USART1 ->DR ;
		
		if(rec[i] == '#' && flag == 0)
		{
			i = 0;
			flag = 1;
			/* code */
		}
		else
		{
			i++;
		}
	}
}

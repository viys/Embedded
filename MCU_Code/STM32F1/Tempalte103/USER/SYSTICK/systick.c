#include "systick.h"

/**
@brief	ϵͳ�δ�����ʱ
@note   168*10^6 / 8 HZ
				18750 ΪУ׼ֵ
@param	
@retval	
*/
void SysTick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2);					//ѡ���ⲿʱ��Դ
	
	SysTick ->LOAD = SYSCLK * ms  ;				//д������ֵ
	SysTick ->VAL = 0;										//�������
	
	SysTick ->CTRL  |= 1 << 0;						//ʹ�ܼ�����
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//�ȴ�ʱ�䵽
  }
		SysTick ->CTRL  &= ~(1 << 0);//�رռ�����
}

/**
@brief	��ʱʱ���жϺ���
@Note		
@param	ms
@retval	None
*/
void SysTick_inter_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2); //ѡ���ⲿʱ��Դ
	
	SysTick ->LOAD = 18750 * ms; //д������ֵ
	SysTick ->VAL = 0; 			 		 //����������
	SysTick ->CTRL |= 1 << 0;	   //ʹ�ܼ�����
	
	SysTick ->CTRL |= 1 << 1;	   //ʹ���ж�
}

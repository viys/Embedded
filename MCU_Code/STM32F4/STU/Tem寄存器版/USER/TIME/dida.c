#include "sysclick.h"
void Sysclick_us(u32 us)
{
	SysTick ->CTRL &= ~(1<<2);		//ѡ���ⲿʱ��Դ
	SysTick ->LOAD = us * (SYSCLK /1000) ; 	//д������ֵ
	SysTick ->VAL = 0;
	SysTick ->CTRL |=1<<0;			//ʹ�ܼĴ���
	while(!(SysTick ->CTRL & (1<<16)))
	{
	//�ȴ�ʱ��
	}
	SysTick ->CTRL &= ~(1<<0);		//�ص�������
	
}


void Sysclick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1<<2);		//ѡ���ⲿʱ��Դ
	SysTick ->LOAD = ms  * SYSCLK  ; 	//д������ֵ
	SysTick ->VAL = 0;
	SysTick ->CTRL |=1<<0;			//ʹ�ܼĴ���
	while(!(SysTick ->CTRL & (1<<16)))
	{
	//�ȴ�ʱ��
	}
	SysTick ->CTRL &= ~(1<<0);		//�ص�ʱ��
	
}

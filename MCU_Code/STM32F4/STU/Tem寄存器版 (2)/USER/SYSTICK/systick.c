#include "systick.h"
/*******************ϵͳ�δ�ʱ��*******************/
void SysTick_us(u32 us)
{
	SysTick ->CTRL &= ~(1 << 2);	//ѡ���ⲿʱ��Դ
	
	SysTick ->LOAD = SYSCLK * us;	//д������ֵ
	SysTick ->VAL = 0;						//�������
	
	SysTick ->CTRL  |= 1 << 0;		//ʹ�ܼ�����
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//�ȴ�ʱ�䵽
  }
		SysTick ->CTRL  &= ~(1 << 0);		//�رռ�����
}

void SysTick_inter_ms(u32 ms)//��ʱʱ���жϺ���
{
	SysTick ->CTRL &= ~(1 << 2);//ѡ���ⲿʱ��Դ
	
	SysTick ->LOAD = SYSCLK * ms * 1000;//д������ֵ
	
	SysTick ->VAL = 0;//����������
	SysTick ->CTRL |= 1 << 0;//ʹ�ܼ�����
	
	SysTick ->CTRL |= 1 << 1;//ʹ���ж�

}


void SysTick_ms(u32 ms)
{
	SysTick ->CTRL &= ~(1 << 2);	//ѡ���ⲿʱ��Դ
	
	SysTick ->LOAD = SYSCLK * ms * 1000;	//д������ֵ
	SysTick ->VAL = 0;						//�������
	
	SysTick ->CTRL  |= 1 << 0;		//ʹ�ܼ�����
	
	while (!(SysTick ->CTRL & (1 << 16)))
  {
		//�ȴ�ʱ�䵽
  }
		SysTick ->CTRL  &= ~(1 << 0);		//�رռ�����
}




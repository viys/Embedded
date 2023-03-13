#include "bsp_systick.h"

__IO u32 TimingDelay;

void SysTick_Init(void)
{
	if(SysTick_Config (SystemCoreClock / 1000))
	{
		while(1);
	}
}

void Delay_ms(__IO u32 nTime)
{
	TimingDelay = nTime;
	
	while(TimingDelay != 0);
}

void Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;
	SysTick_Config (SystemCoreClock/1000 );
	
	for( i=0; i<ms; i++)
	{
		//����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
		//����1ʱ����ȡ��λ����0
		while(!((SysTick->CTRL)&(1<<16)));//�ȴ��ӳ�1ms
	}
	//�ر�SysTick��ʱ��
	SysTick ->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk ;
}

#include "systick.h"

static __IO u32 TimingDelay;


void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

void SysTick_Init(void)
{

	if (SysTick_Config(72000))
	{ 
		/* Capture error */ 
		while (1);
	}
}


//ϵͳ�δ�ʱ��
void SysTick_Delay(uint32_t ms)
{
	SysTick_Config(72000);
	
	for(uint32_t i = 0; i < ms; i++)
	{
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// �ر�SysTick��ʱ��
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}

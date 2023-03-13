#include "wdg.h"

void IWDG_Init(uint8_t prer, uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prer);
	IWDG_SetReload(rlr);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

//��ʼ�����ڿ��Ź�
//tr : T[6:0],������ֵ wr: W[6:0],����ֵ fprer:��Ƶϵ��
uint8_t WWDG_CNT = 0x7f;
//WWDG_Init(0x7f,0x5f,WWDG_Prescaler_8);
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);//wwdgʹ��
	WWDG_CNT = tr & WWDG_CNT;//��ʼ��WWDG_CNT
	WWDG_SetPrescaler(fprer);//����Ԥ��Ƶֵ
	WWDG_SetWindowValue(wr);//���ô���ֵ
	WWDG_Enable(WWDG_CNT);//ʹ��WWDG,����counter
	WWDG_ClearFlag();//�����ǰ�����жϱ�־λ
	WWDG_NVIC_Init();//��ʼ��WWDG�ж�
	WWDG_EnableIT();//�����ж�
}

void WWDG_Set_Counter(uint8_t cnt)
{
	WWDG_Enable(cnt);//ʹ�ܿ��Ź�,����counter
}

void WWDG_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

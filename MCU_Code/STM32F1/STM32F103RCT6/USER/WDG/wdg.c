#include "wdg.h"

void IWDG_Init(uint8_t prer, uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prer);
	IWDG_SetReload(rlr);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

//初始化窗口看门狗
//tr : T[6:0],计数器值 wr: W[6:0],窗口值 fprer:分频系数
uint8_t WWDG_CNT = 0x7f;
//WWDG_Init(0x7f,0x5f,WWDG_Prescaler_8);
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);//wwdg使能
	WWDG_CNT = tr & WWDG_CNT;//初始化WWDG_CNT
	WWDG_SetPrescaler(fprer);//设置预分频值
	WWDG_SetWindowValue(wr);//设置窗口值
	WWDG_Enable(WWDG_CNT);//使能WWDG,设置counter
	WWDG_ClearFlag();//清楚提前唤醒中断标志位
	WWDG_NVIC_Init();//初始化WWDG中断
	WWDG_EnableIT();//开启中断
}

void WWDG_Set_Counter(uint8_t cnt)
{
	WWDG_Enable(cnt);//使能看门狗,设置counter
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

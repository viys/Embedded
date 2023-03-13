#ifndef __NVIC_H
#define __NVIC_H

#include "stm32f4xx.h"


extern char rec[20];
extern u8 flag;
extern u8 width;
extern u16 high;
extern u16 low;
void USART1_IRQHandler(void);
//void SysTick_inter_us(u32 ms);
void TIM1_BRK_TIM9_IRQHandler(void);
void TIM8_TRG_COM_TIM14_IRQHandler(void);

#endif

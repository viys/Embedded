#ifndef __NVIC_H
#define __NVIC_H

#include "stm32f4xx.h"

extern char rec[20];
extern u8 flag;
extern u8 width;
void USART1_IRQHandler(void);
//void SysTick_inter_us(u32 ms);
void TIM8_TRG_COM_TIM14_IRQHandler(void);

#endif

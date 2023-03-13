#ifndef _STM32F10X_IT_H
#define _STM32F10X_IT_H

#include "stm32f10x.h"

void EXTI0_IRQHandler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);
void HardFault_Handler(void);
void TIM4_IRQHandler(void);

#endif

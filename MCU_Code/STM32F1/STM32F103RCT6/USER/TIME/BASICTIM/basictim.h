#ifndef _BASICTIM_H
#define _BASICTIM_H

#include "stm32f10x.h"

#define            BASIC_TIM                   TIM6
#define            BASIC_TIM_APBxClockCmd      RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler

void BASIC_TIM_Init(void);

#endif

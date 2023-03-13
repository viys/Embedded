#ifndef _TIM_H
#define _TIM_H

#include "stm32f10x.h"
#include "usart.h"

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_Period            60000-1
#define 		   CCR1_Val 					 10     //占空比

// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

void Tim4_Init(void);
/**************************函数声明********************************/
void GENERAL_TIM_Init(void);

#endif

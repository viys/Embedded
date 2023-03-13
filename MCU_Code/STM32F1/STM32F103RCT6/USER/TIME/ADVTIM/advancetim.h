#ifndef _ADVANCETIM_H
#define _ADVANCETIM_H

#include "stm32f10x.h"

#define            ADVANCE_TIM                   TIM1
#define            ADVANCE_TIM_APBxClockCmd      RCC_APB2PeriphClockCmd
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1
// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            ADVANCE_TIM_PERIOD            (20*1000-1)
#define            ADVANCE_TIM_PSC               (72-1)
#define            ADVANCE_TIM_PULSE             400

#define            ADVANCE_TIM_IRQ               TIM1_UP_IRQn
#define            ADVANCE_TIM_IRQHandler        TIM1_UP_IRQHandler

// TIM1 输出比较通道
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            ADVANCE_TIM_CH1_PORT          GPIOA
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_8

// TIM1 输出比较通道的互补通道
#define            ADVANCE_TIM_CH1N_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            ADVANCE_TIM_CH1N_PORT          GPIOB
#define            ADVANCE_TIM_CH1N_PIN           GPIO_Pin_13

// TIM1 输出比较通道的刹车通道
#define            ADVANCE_TIM_BKIN_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            ADVANCE_TIM_BKIN_PORT          GPIOB
#define            ADVANCE_TIM_BKIN_PIN           GPIO_Pin_12

//捕获
// 这里我们使用高级控制定时器TIM1

#define            ADVANCE_CAP_TIM                   TIM1
#define            ADVANCE_CAP_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            ADVANCE_CAP_TIM_CLK               RCC_APB2Periph_TIM1

// 输入捕获能捕获到的最小的频率为 72M/{ (ARR+1)*(PSC+1) }
#define            ADVANCE_CAP_TIM_PERIOD            (1000-1)
#define            ADVANCE_CAP_TIM_PSC               (72-1)

// 中断相关宏定义
#define            ADVANCE_CAP_TIM_IRQ               TIM1_CC_IRQn
#define            ADVANCE_CAP_TIM_IRQHandler        TIM1_CC_IRQHandler
						  
// TIM1 输入捕获通道1      
#define            ADVANCE_CAP_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            ADVANCE_CAP_TIM_CH1_PORT          GPIOA
#define            ADVANCE_CAP_TIM_CH1_PIN           GPIO_Pin_8
						  
#define            ADVANCE_CAP_TIM_IC1PWM_CHANNEL    TIM_Channel_1
#define            ADVANCE_CAP_TIM_IC2PWM_CHANNEL    TIM_Channel_2


void ADVANCE_TIM_Init(void);
void ADVANCE_CAP_TIM_Init(void);

#endif

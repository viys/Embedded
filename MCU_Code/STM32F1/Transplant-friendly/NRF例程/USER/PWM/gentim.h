#ifndef _GENTIM_H
#define _GENTIM_H

#include "stm32f10x.h"

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM3

#define            GENERAL_TIM                   TIM2
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM2

// TIM2 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_15


/**************************函数声明********************************/

void GENERAL_TIM2_Init(uint16_t GENERAL_TIM_Prescaler,\
								uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val);

void ADC_Task(void);

#endif	/* __BSP_GENERALTIME_H */



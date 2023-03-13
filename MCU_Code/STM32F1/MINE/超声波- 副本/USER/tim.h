#ifndef _TIM_H
#define _TIM_H

#include "stm32f10x.h"
#include "usart.h"

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_Period            60000-1
#define 		   CCR1_Val 					 17     //占空比

// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

#define            GENERAL_TIME                   TIM5
#define            GENERAL_TIME_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIME_CLK               RCC_APB1Periph_TIM5
#define            GENERAL_TIME_PERIOD            0XFFFF
#define            GENERAL_TIME_PSC              (72-1)

// TIM 输入捕获通道GPIO相关宏定义
#define            GENERAL_TIME_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIME_CH1_PORT          GPIOA
#define            GENERAL_TIME_CH1_PIN           GPIO_Pin_1
#define            GENERAL_TIME_CHANNEL_x         TIM_Channel_2

// 中断相关宏定义
#define            GENERAL_TIME_IT_CCx            TIM_IT_CC2
#define            GENERAL_TIME_IRQ               TIM5_IRQn
#define            GENERAL_TIME_INT_FUN           TIM5_IRQHandler

// 获取捕获寄存器值函数宏定义
#define            GENERAL_TIME_GetCapturex_FUN                 TIM_GetCapture2
// 捕获信号极性函数宏定义
#define            GENERAL_TIME_OCxPolarityConfig_FUN           TIM_OC2PolarityConfig

// 测量的起始边沿
#define            GENERAL_TIME_STRAT_ICPolarity                TIM_ICPolarity_Rising
// 测量的结束边沿
#define            GENERAL_TIME_END_ICPolarity                  TIM_ICPolarity_Falling


// 定时器输入捕获用户自定义变量结构体声明
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // 捕获结束标志位
	uint8_t   Capture_StartFlag;    // 捕获开始标志位
	uint16_t  Capture_CcrValue;     // 捕获寄存器的值
	uint16_t  Capture_Period;       // 自动重装载寄存器更新标志 
}TIM_ICUserValueTypeDef;

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;

/**************************函数声明********************************/
void GENERAL_TIME_Init(void);

void TIM5_Init(void);
/**************************函数声明********************************/
void GENERAL_TIM_Init(void);

#endif

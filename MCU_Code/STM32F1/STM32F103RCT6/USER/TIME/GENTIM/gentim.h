#ifndef _GENTIM_H
#define _GENTIM_H

#include "stm32f10x.h"

typedef enum 
{
    CH1 = 0,
    CH2 = 1,
	CH3 = 2,
	CH4 = 3
} gtim3_typedef_enum;

#define GTIM3	4
/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM3

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3

// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 输出比较通道2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH2_PORT          GPIOA
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 输出比较通道3
#define            GENERAL_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH3_PORT          GPIOB
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_0

// TIM3 输出比较通道4
#define            GENERAL_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH4_PORT          GPIOB
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_1

//舵机PWM参数
#define Servo_Prescaler		7200		//此时pwm时钟频率为10k(100us)
#define Servo_Period		200			//20ms
#define Servo_0				5
#define Servo_45			10				
#define Servo_90			15
#define Servo_135			20
#define Servo_180			25

/***********************捕获**********************/
//这里使用TIM5
#define            GENERAL_CAP_TIM                   TIM5
#define            GENERAL_CAP_TIM_APBxClockCmd      RCC_APB1PeriphClockCmd
#define            GENERAL_CAP_TIM_CLK               RCC_APB1Periph_TIM5
#define            GENERAL_CAP_TIM_PERIOD            0XFFFF
#define            GENERAL_CAP_TIM_PSC              (72-1)

// TIM 输入捕获通道GPIO相关宏定义
#define            GENERAL_CAP_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_CAP_TIM_CH1_PORT          GPIOA
#define            GENERAL_CAP_TIM_CH1_PIN           GPIO_Pin_1
#define            GENERAL_CAP_TIM_CHANNEL_x         TIM_Channel_2

// 中断相关宏定义
#define            GENERAL_CAP_TIM_IT_CCx            TIM_IT_CC2
#define            GENERAL_CAP_TIM_IRQ               TIM5_IRQn
#define            GENERAL_CAP_TIM_INT_FUN           TIM5_IRQHandler

// 获取捕获寄存器值函数宏定义
#define            GENERAL_CAP_TIM_GetCapturexCmd                  TIM_GetCapture2
// 捕获信号极性函数宏定义
#define            GENERAL_CAP_TIM_OCxPolarityConfigCmd            TIM_OC2PolarityConfig

// 测量的起始边沿
#define            GENERAL_CAP_TIM_STRAT_ICPolarity                TIM_ICPolarity_Rising
// 测量的结束边沿
#define            GENERAL_CAP_TIM_END_ICPolarity                  TIM_ICPolarity_Falling

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

void GENERAL_TIM3_Init(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
								uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val);
void GENERAL_TIM5_Init(void);

#endif	/* __BSP_GENERALTIME_H */



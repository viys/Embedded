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
/************ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5************/
// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������Ĭ��ʹ��TIM3

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3

// TIM3 ����Ƚ�ͨ��1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 ����Ƚ�ͨ��2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH2_PORT          GPIOA
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 ����Ƚ�ͨ��3
#define            GENERAL_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH3_PORT          GPIOB
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_0

// TIM3 ����Ƚ�ͨ��4
#define            GENERAL_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH4_PORT          GPIOB
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_1

//���PWM����
#define Servo_Prescaler		7200		//��ʱpwmʱ��Ƶ��Ϊ10k(100us)
#define Servo_Period		200			//20ms
#define Servo_0				5
#define Servo_45			10				
#define Servo_90			15
#define Servo_135			20
#define Servo_180			25

/***********************����**********************/
//����ʹ��TIM5
#define            GENERAL_CAP_TIM                   TIM5
#define            GENERAL_CAP_TIM_APBxClockCmd      RCC_APB1PeriphClockCmd
#define            GENERAL_CAP_TIM_CLK               RCC_APB1Periph_TIM5
#define            GENERAL_CAP_TIM_PERIOD            0XFFFF
#define            GENERAL_CAP_TIM_PSC              (72-1)

// TIM ���벶��ͨ��GPIO��غ궨��
#define            GENERAL_CAP_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_CAP_TIM_CH1_PORT          GPIOA
#define            GENERAL_CAP_TIM_CH1_PIN           GPIO_Pin_1
#define            GENERAL_CAP_TIM_CHANNEL_x         TIM_Channel_2

// �ж���غ궨��
#define            GENERAL_CAP_TIM_IT_CCx            TIM_IT_CC2
#define            GENERAL_CAP_TIM_IRQ               TIM5_IRQn
#define            GENERAL_CAP_TIM_INT_FUN           TIM5_IRQHandler

// ��ȡ����Ĵ���ֵ�����궨��
#define            GENERAL_CAP_TIM_GetCapturexCmd                  TIM_GetCapture2
// �����źż��Ժ����궨��
#define            GENERAL_CAP_TIM_OCxPolarityConfigCmd            TIM_OC2PolarityConfig

// ��������ʼ����
#define            GENERAL_CAP_TIM_STRAT_ICPolarity                TIM_ICPolarity_Rising
// �����Ľ�������
#define            GENERAL_CAP_TIM_END_ICPolarity                  TIM_ICPolarity_Falling

// ��ʱ�����벶���û��Զ�������ṹ������
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // ���������־λ
	uint8_t   Capture_StartFlag;    // ����ʼ��־λ
	uint16_t  Capture_CcrValue;     // ����Ĵ�����ֵ
	uint16_t  Capture_Period;       // �Զ���װ�ؼĴ������±�־ 
}TIM_ICUserValueTypeDef;

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;
/**************************��������********************************/

void GENERAL_TIM3_Init(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
								uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val);
void GENERAL_TIM5_Init(void);

#endif	/* __BSP_GENERALTIME_H */



#ifndef _TIM_H
#define _TIM_H

#include "stm32f10x.h"
#include "usart.h"

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_Period            60000-1
#define 		   CCR1_Val 					 17     //ռ�ձ�

// TIM3 ����Ƚ�ͨ��1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

#define            GENERAL_TIME                   TIM5
#define            GENERAL_TIME_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIME_CLK               RCC_APB1Periph_TIM5
#define            GENERAL_TIME_PERIOD            0XFFFF
#define            GENERAL_TIME_PSC              (72-1)

// TIM ���벶��ͨ��GPIO��غ궨��
#define            GENERAL_TIME_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIME_CH1_PORT          GPIOA
#define            GENERAL_TIME_CH1_PIN           GPIO_Pin_1
#define            GENERAL_TIME_CHANNEL_x         TIM_Channel_2

// �ж���غ궨��
#define            GENERAL_TIME_IT_CCx            TIM_IT_CC2
#define            GENERAL_TIME_IRQ               TIM5_IRQn
#define            GENERAL_TIME_INT_FUN           TIM5_IRQHandler

// ��ȡ����Ĵ���ֵ�����궨��
#define            GENERAL_TIME_GetCapturex_FUN                 TIM_GetCapture2
// �����źż��Ժ����궨��
#define            GENERAL_TIME_OCxPolarityConfig_FUN           TIM_OC2PolarityConfig

// ��������ʼ����
#define            GENERAL_TIME_STRAT_ICPolarity                TIM_ICPolarity_Rising
// �����Ľ�������
#define            GENERAL_TIME_END_ICPolarity                  TIM_ICPolarity_Falling


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
void GENERAL_TIME_Init(void);

void TIM5_Init(void);
/**************************��������********************************/
void GENERAL_TIM_Init(void);

#endif

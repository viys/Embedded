#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"

#define ADCx_APBxClockCmd 		RCC_APB2PeriphClockCmd
#define ADCx 					ADC2
#define ADC_CLK					RCC_APB2Periph_ADC2

#define ADC_GPIO_APBxClockCmd 	RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK			RCC_APB2Periph_GPIOC
#define ADC_GPIO_PORT			GPIOC
#define ADC_GPIO_PIN			GPIO_Pin_1

#define ADC_CHANNEL				ADC_Channel_11

#define ADC_IRQ					ADC1_2_IRQn
#define ADC_IRQHandler			ADC1_2_IRQHandler

void ADCx_GPIO_Config(void);
void ADCx_Mode_Config(void);
void ADCx_NVIC_Config(void);
void ADCx_Init(void);

#endif

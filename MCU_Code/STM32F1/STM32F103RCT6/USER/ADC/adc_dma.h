#ifndef _ADC_DMA_H
#define _ADC_DMA_H

#include "stm32f10x.h"

#define ADCx_DMA_APBxClockCmd 	RCC_APB2PeriphClockCmd
#define ADCx_DMA 				ADC1
#define ADCx_DMA_CLK			RCC_APB2Periph_ADC1

#define ADC_DMA_GPIO_APBxClockCmd 	RCC_APB2PeriphClockCmd
#define ADC_DMA_GPIO_CLK			RCC_APB2Periph_GPIOC
#define ADC_DMA_GPIO_PORT			GPIOC
// 转换通道个数
#define    NOFCHANEL			6

#define    ADC_PIN1             GPIO_Pin_0
#define    ADC_CHANNEL1         ADC_Channel_10

#define    ADC_PIN2             GPIO_Pin_1
#define    ADC_CHANNEL2         ADC_Channel_11

#define    ADC_PIN3             GPIO_Pin_2
#define    ADC_CHANNEL3         ADC_Channel_12

#define    ADC_PIN4             GPIO_Pin_3
#define    ADC_CHANNEL4         ADC_Channel_13

#define    ADC_PIN5             GPIO_Pin_4
#define    ADC_CHANNEL5         ADC_Channel_14

#define    ADC_PIN6             GPIO_Pin_5
#define    ADC_CHANNEL6         ADC_Channel_15

//若在双模式下ADC1对应DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能
#define ADC_DMA_CLK				RCC_AHBPeriph_DMA1
#define ADC_DMA_CHANNEL			DMA1_Channel1

// 双模式时，ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
// ADC1的在低十六位，ADC2的在高十六位
// 双ADC模式的第一个ADC，必须是ADC1

#define    DUAL_NOFCHANEL                        2

#define    ADCx_1                           ADC1
#define    ADCx_1_APBxClockCmd           	RCC_APB2PeriphClockCmd
#define    ADCx_1_CLK                       RCC_APB2Periph_ADC1

#define    ADCx_1_GPIO_APBxClockCmd        	RCC_APB2PeriphClockCmd
#define    ADCx_1_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADCx_1_PORT                      GPIOC
#define    ADCx_1_PIN                       GPIO_Pin_1
#define    ADCx_1_CHANNEL                   ADC_Channel_11

// 双ADC模式的第二个ADC，必须是ADC2
#define    ADCx_2                           ADC2
#define    ADCx_2_APBxClockCmd             	RCC_APB2PeriphClockCmd
#define    ADCx_2_CLK                       RCC_APB2Periph_ADC2

#define    ADCx_2_GPIO_APBxClockCmd        	RCC_APB2PeriphClockCmd
#define    ADCx_2_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADCx_2_PORT                      GPIOC
#define    ADCx_2_PIN                       GPIO_Pin_4
#define    ADCx_2_CHANNEL                 	ADC_Channel_14

//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define V25  0x6EE
//斜率 每摄氏度4.3mV 对应每摄氏度0x05
#define AVG_SLOPE 0x05 

void ADCx_DMA_Init(void);
void ADC_nChannel_Init(void);
void ADCx_Dual_Init(void);
void ADC1_temperature_Init(void);

#endif

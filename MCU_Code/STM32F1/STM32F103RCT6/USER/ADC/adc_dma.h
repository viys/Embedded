#ifndef _ADC_DMA_H
#define _ADC_DMA_H

#include "stm32f10x.h"

#define ADCx_DMA_APBxClockCmd 	RCC_APB2PeriphClockCmd
#define ADCx_DMA 				ADC1
#define ADCx_DMA_CLK			RCC_APB2Periph_ADC1

#define ADC_DMA_GPIO_APBxClockCmd 	RCC_APB2PeriphClockCmd
#define ADC_DMA_GPIO_CLK			RCC_APB2Periph_GPIOC
#define ADC_DMA_GPIO_PORT			GPIOC
// ת��ͨ������
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

//����˫ģʽ��ADC1��ӦDMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA����
#define ADC_DMA_CLK				RCC_AHBPeriph_DMA1
#define ADC_DMA_CHANNEL			DMA1_Channel1

// ˫ģʽʱ��ADC1��ADC2ת�������ݶ������ADC1�����ݼĴ�����
// ADC1���ڵ�ʮ��λ��ADC2���ڸ�ʮ��λ
// ˫ADCģʽ�ĵ�һ��ADC��������ADC1

#define    DUAL_NOFCHANEL                        2

#define    ADCx_1                           ADC1
#define    ADCx_1_APBxClockCmd           	RCC_APB2PeriphClockCmd
#define    ADCx_1_CLK                       RCC_APB2Periph_ADC1

#define    ADCx_1_GPIO_APBxClockCmd        	RCC_APB2PeriphClockCmd
#define    ADCx_1_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADCx_1_PORT                      GPIOC
#define    ADCx_1_PIN                       GPIO_Pin_1
#define    ADCx_1_CHANNEL                   ADC_Channel_11

// ˫ADCģʽ�ĵڶ���ADC��������ADC2
#define    ADCx_2                           ADC2
#define    ADCx_2_APBxClockCmd             	RCC_APB2PeriphClockCmd
#define    ADCx_2_CLK                       RCC_APB2Periph_ADC2

#define    ADCx_2_GPIO_APBxClockCmd        	RCC_APB2PeriphClockCmd
#define    ADCx_2_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADCx_2_PORT                      GPIOC
#define    ADCx_2_PIN                       GPIO_Pin_4
#define    ADCx_2_CHANNEL                 	ADC_Channel_14

//����12λ��ADC��3.3V��ADCֵΪ0xfff,�¶�Ϊ25��ʱ��Ӧ�ĵ�ѹֵΪ1.43V��0x6EE
#define V25  0x6EE
//б�� ÿ���϶�4.3mV ��Ӧÿ���϶�0x05
#define AVG_SLOPE 0x05 

void ADCx_DMA_Init(void);
void ADC_nChannel_Init(void);
void ADCx_Dual_Init(void);
void ADC1_temperature_Init(void);

#endif

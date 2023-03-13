#include "adc.h"

//1.ADC��ӦGPIO��ʼ��
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ADC_GPIO_APBxClockCmd(ADC_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Mode_AIN;
	
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStruct);
}
//2.��ʼ��ADC�ṹ�� 3.����ADCʱ��,����ͨ����ת��˳��Ͳ���ʱ�� 4.ʹ��ADCת������ж�,�����ж����ȼ� 5.ʹ��adc
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_Initstruct;
	
	ADCx_APBxClockCmd(ADC_CLK, ENABLE);
	
	ADC_Initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_Initstruct.ADC_ScanConvMode = DISABLE;
	ADC_Initstruct.ADC_ContinuousConvMode = ENABLE;
	ADC_Initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//
	ADC_Initstruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Initstruct.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADCx, &ADC_Initstruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADCx, ENABLE);
	
	//ADCУ׼
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));
	
	//����û�в����ⲿ����,����ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

static void ADCx_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void ADCx_Init(void)
{
	ADCx_NVIC_Config();
	
	ADCx_GPIO_Config();
	
	ADCx_Mode_Config();
}


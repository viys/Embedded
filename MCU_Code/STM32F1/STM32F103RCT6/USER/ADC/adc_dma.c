#include "adc_dma.h"

extern uint16_t ADC_ConvertedValue;
uint16_t ADC_nChannel_ConvertedValue[NOFCHANEL] = {0, 0, 0, 0, 0, 0};
uint32_t ADC_Dual_ConvertedValue[DUAL_NOFCHANEL] = {0};
uint16_t ADC_Temp_ConvertedValue = 0;

//1.ADC��ӦGPIO��ʼ��
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ADC_DMA_GPIO_APBxClockCmd(ADC_DMA_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = ADC_PIN2;
	GPIO_InitStruct.GPIO_Pin = GPIO_Mode_AIN;
	
	GPIO_Init(ADC_DMA_GPIO_PORT, &GPIO_InitStruct);
}
//2.��ʼ��ADC�ṹ�� 3.����ADCʱ��,����ͨ����ת��˳��Ͳ���ʱ�� 4.ʹ��ADCת������ж�,�����ж����ȼ� 5.ʹ��adc
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	ADCx_DMA_APBxClockCmd(ADCx_DMA_CLK, ENABLE);
	
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_DMA->DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = 1;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADCx_DMA, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL2, 1, ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADCx_DMA, ENABLE);
	
	//ʹ��DMA����
	ADC_DMACmd(ADCx_DMA, ENABLE);
	
	//ADCУ׼
	ADC_StartCalibration(ADCx_DMA);
	while(ADC_GetCalibrationStatus(ADCx_DMA));
	
	//����û�в����ⲿ����,����ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADCx_DMA, ENABLE);
}

void ADCx_DMA_Init(void)
{
	ADCx_GPIO_Config();
	
	ADCx_Mode_Config();
}

static void ADCx_nChannel_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ADC_DMA_GPIO_APBxClockCmd(ADC_DMA_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
	GPIO_InitStruct.GPIO_Pin = GPIO_Mode_AIN;
	
	GPIO_Init(ADC_DMA_GPIO_PORT, &GPIO_InitStruct);
}

static void ADCx_nChannel_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	ADCx_DMA_APBxClockCmd(ADCx_DMA_CLK, ENABLE);
	
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_DMA->DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_nChannel_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = NOFCHANEL;//��
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//��
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//��
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = NOFCHANEL;
	ADC_Init(ADCx_DMA, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL2, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL3, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL4, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL5, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_DMA, ADC_CHANNEL6, 6, ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADCx_DMA, ENABLE);
	
	//ʹ��DMA����
	ADC_DMACmd(ADCx_DMA, ENABLE);
	
	//ADCУ׼
	ADC_StartCalibration(ADCx_DMA);
	while(ADC_GetCalibrationStatus(ADCx_DMA));
	
	//����û�в����ⲿ����,����ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADCx_DMA, ENABLE);
}

void ADC_nChannel_Init(void)
{
	ADCx_nChannel_GPIO_Config();
	ADCx_nChannel_Mode_Config();
}

static void ADCx_Dual_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ADCx_1_GPIO_APBxClockCmd(ADCx_1_GPIO_CLK, ENABLE);
	ADCx_2_GPIO_APBxClockCmd(ADCx_2_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = ADCx_2_PIN|ADCx_1_PIN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Mode_AIN;
	//�˴�ADC1��ADC2��GPIO�˿ڶ�ΪGPIOC
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void ADCx_Dual_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_1->DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_Dual_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = DUAL_NOFCHANEL;//����ͨ��
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//32
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	
	ADCx_1_APBxClockCmd(ADCx_1_CLK, ENABLE);
	ADCx_2_APBxClockCmd(ADCx_2_CLK, ENABLE);
	//ADC1
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ����
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = DUAL_NOFCHANEL;
	ADC_Init(ADCx_1, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_1, ADCx_1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	//ʹ��DMA����
	ADC_DMACmd(ADCx_1, ENABLE);
	
	//ADC2
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�ⲿ����
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = DUAL_NOFCHANEL;
	ADC_Init(ADCx_2, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_2, ADCx_2_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	//ADC1У׼
	ADC_Cmd(ADCx_1, ENABLE);
	ADC_ResetCalibration(ADCx_1);
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	ADC_StartCalibration(ADCx_1);
	while(ADC_GetCalibrationStatus(ADCx_1));
	
	//ADC2У׼
	ADC_Cmd(ADCx_2, ENABLE);
	ADC_ResetCalibration(ADCx_2);
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	ADC_StartCalibration(ADCx_2);
	while(ADC_GetCalibrationStatus(ADCx_2));
	
	//ʹ��ADC2���ⲿ����ת��
	ADC_ExternalTrigConvCmd(ADCx_2, ENABLE);
	//����û�в����ⲿ����,����ʹ���������ADCת��
	ADC_SoftwareStartConvCmd(ADCx_1, ENABLE);
}

void ADCx_Dual_Init(void)
{
	ADCx_Dual_GPIO_Config();
	ADCx_Dual_Mode_Config();
}

static void ADC1_Temperature_GPIO_Config(void)		       
{

	/* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

 }


/* ��������ADC1_Mode_Config
 * ����  ������ADC1�Ĺ���ģʽΪMDAģʽ
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void ADC1_Temperature_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
  
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));	       
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&ADC_Temp_ConvertedValue;	// ADת��ֵ����ŵ��ڴ����ַ	�����Ǹ�����ַ��
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                        // ������Ϊ���ݴ������Դ	
	DMA_InitStruct.DMA_BufferSize = 1;                                     // ����ָ��DMAͨ�� DMA����Ĵ�С
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          // �����ַ�Ĵ�������
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;			        // �ڴ��ַ�Ĵ�������
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // ���ݿ��Ϊ16λ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // HalfWord
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;		                    //������ѭ��ģʽ��
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;	                    //�����ȼ�
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		                        //û������Ϊ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);

	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                        //��������ģʽ
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;			                    //��ͨ��
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;			            //����ת��
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;       //�������������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;					//Right
	ADC_InitStruct.ADC_NbrOfChannel = 1;				                    //��һ��ͨ��ת��
	ADC_Init(ADC1, &ADC_InitStruct);

	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	//���ò���ͨ��IN16, ���ò���ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

	//ʹ���¶ȴ��������ڲ��ο���ѹ   
	ADC_TempSensorVrefintCmd(ENABLE);                                    
 
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);			                              
	while(ADC_GetResetCalibrationStatus(ADC1));                         
	ADC_StartCalibration(ADC1);				                        
	while(ADC_GetCalibrationStatus(ADC1));	  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}												


//ADC1�ڲ��¶ȴ�����
//���㷽��:(u16)Current_Temp=(V25-ADC_Temp_ConvertedValue)/AVG_SLOPE+25
void ADC1_temperature_Init(void)
{
	ADC1_Temperature_GPIO_Config();
	ADC1_Temperature_Mode_Config();
}

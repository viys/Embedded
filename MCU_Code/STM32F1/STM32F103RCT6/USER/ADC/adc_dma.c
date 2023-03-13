#include "adc_dma.h"

extern uint16_t ADC_ConvertedValue;
uint16_t ADC_nChannel_ConvertedValue[NOFCHANEL] = {0, 0, 0, 0, 0, 0};
uint32_t ADC_Dual_ConvertedValue[DUAL_NOFCHANEL] = {0};
uint16_t ADC_Temp_ConvertedValue = 0;

//1.ADC对应GPIO初始化
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	ADC_DMA_GPIO_APBxClockCmd(ADC_DMA_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = ADC_PIN2;
	GPIO_InitStruct.GPIO_Pin = GPIO_Mode_AIN;
	
	GPIO_Init(ADC_DMA_GPIO_PORT, &GPIO_InitStruct);
}
//2.初始化ADC结构体 3.配置ADC时钟,配置通道的转换顺序和采样时间 4.使能ADC转换完成中断,配置中断优先级 5.使能adc
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
	
	//使能DMA请求
	ADC_DMACmd(ADCx_DMA, ENABLE);
	
	//ADC校准
	ADC_StartCalibration(ADCx_DMA);
	while(ADC_GetCalibrationStatus(ADCx_DMA));
	
	//由于没有采用外部出发,所以使用软件触发ADC转换
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
	DMA_InitStruct.DMA_BufferSize = NOFCHANEL;//改
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//改
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//改
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
	
	//使能DMA请求
	ADC_DMACmd(ADCx_DMA, ENABLE);
	
	//ADC校准
	ADC_StartCalibration(ADCx_DMA);
	while(ADC_GetCalibrationStatus(ADCx_DMA));
	
	//由于没有采用外部出发,所以使用软件触发ADC转换
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
	//此处ADC1和ADC2的GPIO端口都为GPIOC
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
	DMA_InitStruct.DMA_BufferSize = DUAL_NOFCHANEL;//几个通道
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
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = DUAL_NOFCHANEL;
	ADC_Init(ADCx_1, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_1, ADCx_1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	//使能DMA请求
	ADC_DMACmd(ADCx_1, ENABLE);
	
	//ADC2
	ADC_InitStruct.ADC_Mode = ADC_Mode_RegSimult;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = DUAL_NOFCHANEL;
	ADC_Init(ADCx_2, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADCx_2, ADCx_2_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
	//ADC1校准
	ADC_Cmd(ADCx_1, ENABLE);
	ADC_ResetCalibration(ADCx_1);
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	ADC_StartCalibration(ADCx_1);
	while(ADC_GetCalibrationStatus(ADCx_1));
	
	//ADC2校准
	ADC_Cmd(ADCx_2, ENABLE);
	ADC_ResetCalibration(ADCx_2);
	while(ADC_GetResetCalibrationStatus(ADCx_2));
	ADC_StartCalibration(ADCx_2);
	while(ADC_GetCalibrationStatus(ADCx_2));
	
	//使能ADC2的外部触发转换
	ADC_ExternalTrigConvCmd(ADCx_2, ENABLE);
	//由于没有采用外部出发,所以使用软件触发ADC转换
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


/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_Temperature_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
  
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));	       
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&ADC_Temp_ConvertedValue;	// AD转换值所存放的内存基地址	（就是给个地址）
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                        // 外设作为数据传输的来源	
	DMA_InitStruct.DMA_BufferSize = 1;                                     // 定义指定DMA通道 DMA缓存的大小
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          // 外设地址寄存器不变
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;			        // 内存地址寄存器不变
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 数据宽度为16位
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // HalfWord
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;		                    //工作在循环模式下
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;	                    //高优先级
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;		                        //没有设置为内存到内存的传输
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);

	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                        //独立工作模式
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;			                    //多通道
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;			            //连续转换
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;       //由软件触发启动
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;					//Right
	ADC_InitStruct.ADC_NbrOfChannel = 1;				                    //仅一个通道转换
	ADC_Init(ADC1, &ADC_InitStruct);

	/*配置ADC时钟，为PCLK2的8分频，即9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	//设置采样通道IN16, 设置采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

	//使能温度传感器和内部参考电压   
	ADC_TempSensorVrefintCmd(ENABLE);                                    
 
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);			                              
	while(ADC_GetResetCalibrationStatus(ADC1));                         
	ADC_StartCalibration(ADC1);				                        
	while(ADC_GetCalibrationStatus(ADC1));	  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}												


//ADC1内部温度传感器
//计算方法:(u16)Current_Temp=(V25-ADC_Temp_ConvertedValue)/AVG_SLOPE+25
void ADC1_temperature_Init(void)
{
	ADC1_Temperature_GPIO_Config();
	ADC1_Temperature_Mode_Config();
}

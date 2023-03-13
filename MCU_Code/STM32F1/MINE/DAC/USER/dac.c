#include "dac.h"

static void DAC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//一定要用模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

static void DAC1_Mode_Config(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//屏蔽幅值设置
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//DAC1输出缓存关闭
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;//不使用触发功能
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//不使用波形发生
	DAC_Init(DAC_Channel_1,&DAC_InitStructure);
	
	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R,0);//12位右对齐数据格式
}

void DAC1_Init(void)
{
	DAC1_GPIO_Config();
	
	DAC1_Mode_Config();
}

void DAC1_Set_Vol(uint16_t vol)
{
	float temp = vol;
	temp /= 1000;
	temp = temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);
}

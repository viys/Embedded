#include "bsp_key.h"

/* KEY�˿ڳ�ʼ�� */
void KEY_GPIOA_Config(void)
{
	/* �����ĸ������ʺ���������ĳ�ʼ�� */
	
	/* ��һ���������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* �ڶ�������һ��GPIO��ʼ���ṹ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ����������GPIO��ʼ���ṹ��ĳ�Ա */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* ���Ĳ�����GPIO��ʼ�������������úõĽṹ��ĳ�Ա����д��Ĵ��� */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}


void KEY_GPIOE_Config(void)
{
	/* �����ĸ������ʺ���������ĳ�ʼ�� */
	
	/* ��һ���������ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* �ڶ�������һ��GPIO��ʼ���ṹ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ����������GPIO��ʼ���ṹ��ĳ�Ա */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* ���Ĳ�����GPIO��ʼ�������������úõĽṹ��ĳ�Ա����д��Ĵ��� */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

/* ����ɨ�� */
uint8_t KEY_Scan(GPIO_TypeDef *GPIOx ,uint16_t GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON )
	{
		//while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_OFF)
			return KEY_ON ;
	}
	else
	{		
		return KEY_OFF ;
	} 
}


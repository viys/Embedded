#include "bsp_key.h"

/* KEY端口初始化 */
void KEY_GPIOA_Config(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}


void KEY_GPIOE_Config(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

/* 按键扫描 */
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


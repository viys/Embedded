//bsp为板级支持包的缩写

#include "bsp_led.h"

void LED_GPIO_Config1(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOC , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void LED_GPIO_Config2(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOC , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void LED_GPIO_Config3(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOC , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void LED_GPIO_Config4(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOC , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void PIN_GPIO_Config1(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void PIN_GPIO_Config2(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}
void PIN_GPIO_Config3(void)
{
	/* 以下四个步骤适合所有外设的初始化 */
	
	/* 第一步开外设的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	/* 第二步定义一个GPIO初始化结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 第三步配置GPIO初始化结构体的成员 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	/* 第四步调用GPIO初始化函数，把配置好的结构体的成员参数写入寄存器 */
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

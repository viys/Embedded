#include "basictim.h"

static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// 设置中断来源
    NVIC_InitStruct.NVIC_IRQChannel = BASIC_TIM_IRQ ;	
	// 设置主优先级为 0
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	 
	// 设置抢占优先级为3
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

//TIME6基本定时器初始化
static void BASIC_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	BASIC_TIM_APBxClockCmd(BASIC_TIM_CLK, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler = BASIC_TIM_Prescaler;//预分频
	TIM_TimeBaseInitStruct.TIM_Period = BASIC_TIM_Period;//自动重装载
	
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseInitStruct);//初始化定时器
	
	TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);//清除计数器中断标志位
	TIM_ITConfig(BASIC_TIM, TIM_IT_Update, ENABLE);//开启中断
	TIM_Cmd(BASIC_TIM, ENABLE);//使能定时器
}

void BASIC_TIM_Init(void)
{
	BASIC_TIM_NVIC_Config();
	
	BASIC_TIM_Config();
}

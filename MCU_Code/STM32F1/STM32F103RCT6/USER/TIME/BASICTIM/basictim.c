#include "basictim.h"

static void BASIC_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
    NVIC_InitStruct.NVIC_IRQChannel = BASIC_TIM_IRQ ;	
	// ���������ȼ�Ϊ 0
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	 
	// ������ռ���ȼ�Ϊ3
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

//TIME6������ʱ����ʼ��
static void BASIC_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	BASIC_TIM_APBxClockCmd(BASIC_TIM_CLK, ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Prescaler = BASIC_TIM_Prescaler;//Ԥ��Ƶ
	TIM_TimeBaseInitStruct.TIM_Period = BASIC_TIM_Period;//�Զ���װ��
	
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseInitStruct);//��ʼ����ʱ��
	
	TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);//����������жϱ�־λ
	TIM_ITConfig(BASIC_TIM, TIM_IT_Update, ENABLE);//�����ж�
	TIM_Cmd(BASIC_TIM, ENABLE);//ʹ�ܶ�ʱ��
}

void BASIC_TIM_Init(void)
{
	BASIC_TIM_NVIC_Config();
	
	BASIC_TIM_Config();
}

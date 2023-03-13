#include "advancetim.h"

static void ADVANCE_TIM_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
    //����Ƚ�ͨ��GPIO��ʼ��
    RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStruct.GPIO_Pin = ADVANCE_TIM_CH1_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStruct);
	
	//����Ƚ�ͨ������ͨ��GPIO��ʼ��
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1N_GPIO_CLK, ENABLE);
    GPIO_InitStruct.GPIO_Pin = ADVANCE_TIM_CH1N_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADVANCE_TIM_CH1N_PORT, &GPIO_InitStruct);
	
	//����Ƚ�ͨ��ɲ��ͨ��GPIO��ʼ��
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_BKIN_GPIO_CLK, ENABLE);
    GPIO_InitStruct.GPIO_Pin = ADVANCE_TIM_BKIN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStruct);
	//BKIN����Ĭ��������͵�ƽ
	GPIO_ResetBits(ADVANCE_TIM_BKIN_PORT,ADVANCE_TIM_BKIN_PIN);	
}

static void ADVANCE_TIM_Mode_Config(void)
{
	ADVANCE_TIM_APBxClockCmd(ADVANCE_TIM_CLK, ENABLE);
	//ʱ���ṹ��ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = ADVANCE_TIM_PERIOD;//�Զ���װ�ؼĴ�����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = ADVANCE_TIM_PSC;//����CNT��������ʱ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ�ӷ�Ƶ����
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//����������ģʽ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//�ظ���������ֵ
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseInitStruct);
	//����ȽϽṹ���ʼ��
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//����PWMΪģʽ1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//���ʹ��
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;//�������ʹ��
	TIM_OCInitStruct.TIM_Pulse = ADVANCE_TIM_PULSE;//����ռ�ձȴ�С
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//���ͨ����ƽ��������
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;//�������ͨ����ƽ��������
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;//���ͨ�����е�ƽ����
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//�������ͨ�����е�ƽ��������
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStruct);
	TIM_OC1PolarityConfig(ADVANCE_TIM, TIM_OCPreload_Enable);
	//ɲ���������ṹ���ʼ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// ����Ƚ��ź�����ʱ�����ã�������μ���ɲο� BDTR:UTG[7:0]������
	// �������õ�����ʱ��Ϊ152ns
	TIM_BDTRInitStruct.TIM_DeadTime = 11;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Enable;
	// ��BKIN���ż�⵽�ߵ�ƽ��ʱ������Ƚ��źű���ֹ���ͺ�����ɲ��һ��
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStruct);
	// ʹ�ܼ�����
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}

void ADVANCE_TIM_Init(void)
{
	ADVANCE_TIM_GPIO_Config();
	
	ADVANCE_TIM_Mode_Config();
	
}

/*************************����*************************/
 /**
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void ADVANCE_CAP_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_CAP_TIM_IRQ; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  �߼���ʱ��PWM�����õ���GPIO��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADVANCE_CAP_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ADVANCE_CAP_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  ADVANCE_CAP_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(ADVANCE_CAP_TIM_CH1_PORT, &GPIO_InitStructure);	
}


///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

/**
  * @brief  �߼���ʱ��PWM�����ʼ�����õ���GPIO��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADVANCE_CAP_TIM_Mode_Config(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	ADVANCE_CAP_TIM_APBxClock_FUN(ADVANCE_CAP_TIM_CLK,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=ADVANCE_CAP_TIM_PERIOD;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= ADVANCE_CAP_TIM_PSC;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(ADVANCE_CAP_TIM, &TIM_TimeBaseStructure);

	/*--------------------���벶��ṹ���ʼ��-------------------*/	
  // ʹ��PWM����ģʽʱ����Ҫռ����������Ĵ�����һ�������ڣ�����һ����ռ�ձ�
	
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	// ����ͨ��IC1����
	// ѡ�񲶻�ͨ��
  TIM_ICInitStructure.TIM_Channel = ADVANCE_CAP_TIM_IC1PWM_CHANNEL;
	// ���ò���ı���
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	// ���ò���ͨ�����ź��������ĸ�����ͨ������ֱ���ͷ�ֱ������
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 1��Ƶ���������źŵ�ÿ����Ч���ض�����
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// ���˲�
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	// ��ʼ��PWM����ģʽ
  TIM_PWMIConfig(ADVANCE_CAP_TIM, &TIM_ICInitStructure);
	
	// ��������PWM����ģʽʱ,ֻ��Ҫ���ô����źŵ���һ·���ɣ����ڲ������ڣ�
	// ����һ·�����ڲ���ռ�ձȣ�����Ӳ���Դ����ã�����Ҫ������
	
	// ����ͨ��IC2����	
//	TIM_ICInitStructure.TIM_Channel = ADVANCE_TIM_IC1PWM_CHANNEL;
//  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
//  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
//  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//  TIM_ICInitStructure.TIM_ICFilter = 0x0;
//  TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);
	
	// ѡ�����벶��Ĵ����ź�
  TIM_SelectInputTrigger(ADVANCE_CAP_TIM, TIM_TS_TI1FP1);		

	// ѡ���ģʽ: ��λģʽ
	// PWM����ģʽʱ,��ģʽ���빤���ڸ�λģʽ��������ʼʱ,������CNT�ᱻ��λ
  TIM_SelectSlaveMode(ADVANCE_CAP_TIM, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(ADVANCE_CAP_TIM,TIM_MasterSlaveMode_Enable); 

  // ʹ�ܲ����ж�,����ж���Ե���������ͨ�������������Ǹ���
  TIM_ITConfig(ADVANCE_CAP_TIM, TIM_IT_CC1, ENABLE);	
	// ����жϱ�־λ
	TIM_ClearITPendingBit(ADVANCE_CAP_TIM, TIM_IT_CC1);
	
	 // ʹ�ܸ߼����ƶ�ʱ������������ʼ����
  TIM_Cmd(ADVANCE_CAP_TIM, ENABLE);
}
/**
  * @brief  �߼���ʱ��PWM�����ʼ�����õ���GPIO��ʼ��
  * @param  ��
  * @retval ��
  */
void ADVANCE_CAP_TIM_Init(void)
{
	ADVANCE_CAP_TIM_GPIO_Config();
	ADVANCE_CAP_TIM_NVIC_Config();
	ADVANCE_CAP_TIM_Mode_Config();		
}


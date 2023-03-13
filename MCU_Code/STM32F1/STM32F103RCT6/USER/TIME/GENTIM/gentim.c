#include "gentim.h" 

TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0,0,0,0};

static uint32_t GTIM3_CLK[GTIM3] = {GENERAL_TIM_CH1_GPIO_CLK,GENERAL_TIM_CH2_GPIO_CLK,\
										GENERAL_TIM_CH3_GPIO_CLK,GENERAL_TIM_CH4_GPIO_CLK};
static GPIO_TypeDef* GTIM3_PORT[GTIM3] = {GENERAL_TIM_CH1_PORT,GENERAL_TIM_CH2_PORT,\
											  GENERAL_TIM_CH3_PORT,GENERAL_TIM_CH4_PORT};
static uint16_t GTIM3_PIN[GTIM3] = {GENERAL_TIM_CH1_PIN,GENERAL_TIM_CH2_PIN,\
										GENERAL_TIM_CH3_PIN,GENERAL_TIM_CH4_PIN,};

static void GENERAL_TIM3_GPIO_Config(gtim3_typedef_enum chx) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GTIM3_CLK[chx], ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GTIM3_PIN[chx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GTIM3_PORT[chx], &GPIO_InitStructure);
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

static void GENERAL_TIM3_Mode_Config(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
				uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	// �������ڣ���������Ϊ100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period-1;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler-1;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// ���ͨ����ƽ��������	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// ����Ƚ�ͨ�� 1
	TIM_OCInitStructure.TIM_Pulse = CCRx_Val;
	switch (chx)
    {
    	case 0:
			TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
    		break;
    	case 1:
			TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
    		break;
		case 2:
			TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
    		break;
		case 3:
			TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
    		break;
    	default:
    		break;
    }
	
	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM3_Init(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
				uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val)
{
	GENERAL_TIM3_GPIO_Config(chx);
	GENERAL_TIM3_Mode_Config(chx,GENERAL_TIM_Prescaler,GENERAL_TIM_Period,CCRx_Val);		
}

/***********************����**********************/
static void GENERAL_TIM5_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ���벶��ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_CAP_TIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GENERAL_CAP_TIM_CH1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GENERAL_CAP_TIM_CH1_PORT, &GPIO_InitStructure);	
}

static void GENERAL_TIM5_Mode_Config(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_CAP_TIM_CLK,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=GENERAL_CAP_TIM_PERIOD;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_CAP_TIM_PSC;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_CAP_TIM, &TIM_TimeBaseStructure);

	/*--------------------���벶��ṹ���ʼ��-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// �������벶���ͨ������Ҫ���ݾ����GPIO������
	TIM_ICInitStructure.TIM_Channel = GENERAL_CAP_TIM_CHANNEL_x;
	// ���벶���źŵļ�������
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_CAP_TIM_STRAT_ICPolarity;
	// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// �������Ҫ��������źŵķ�Ƶϵ��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// �������Ҫ��������źŵ��˲�ϵ��
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// ��ʱ�����벶���ʼ��
	TIM_ICInit(GENERAL_CAP_TIM, &TIM_ICInitStructure);
	
	// ������ºͲ����жϱ�־λ
    TIM_ClearFlag(GENERAL_CAP_TIM, TIM_FLAG_Update|GENERAL_CAP_TIM_IT_CCx);	
    // �������ºͲ����ж�  
	TIM_ITConfig (GENERAL_CAP_TIM, TIM_IT_Update | GENERAL_CAP_TIM_IT_CCx, ENABLE );
	
	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_CAP_TIM, ENABLE);
}

static void GENERAL_TIM5_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_CAP_TIM_IRQ ;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void GENERAL_TIM5_Init(void)
{
	GENERAL_TIM5_GPIO_Config();
	GENERAL_TIM5_NVIC_Config();
	GENERAL_TIM5_Mode_Config();		
}

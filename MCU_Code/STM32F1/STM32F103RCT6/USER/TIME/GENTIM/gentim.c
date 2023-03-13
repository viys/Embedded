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

  // 输出比较通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(GTIM3_CLK[chx], ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GTIM3_PIN[chx];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GTIM3_PORT[chx], &GPIO_InitStructure);
}


///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void GENERAL_TIM3_Mode_Config(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
				uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
	// 配置周期，这里配置为100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period-1;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler-1;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出通道电平极性配置	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// 输出比较通道 1
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
	
	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM3_Init(gtim3_typedef_enum chx,uint16_t GENERAL_TIM_Prescaler,\
				uint16_t GENERAL_TIM_Period,uint16_t CCRx_Val)
{
	GENERAL_TIM3_GPIO_Config(chx);
	GENERAL_TIM3_Mode_Config(chx,GENERAL_TIM_Prescaler,GENERAL_TIM_Period,CCRx_Val);		
}

/***********************捕获**********************/
static void GENERAL_TIM5_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 输入捕获通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_CAP_TIM_CH1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GENERAL_CAP_TIM_CH1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GENERAL_CAP_TIM_CH1_PORT, &GPIO_InitStructure);	
}

static void GENERAL_TIM5_Mode_Config(void)
{
  // 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_CAP_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period=GENERAL_CAP_TIM_PERIOD;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_CAP_TIM_PSC;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_CAP_TIM, &TIM_TimeBaseStructure);

	/*--------------------输入捕获结构体初始化-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = GENERAL_CAP_TIM_CHANNEL_x;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_CAP_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(GENERAL_CAP_TIM, &TIM_ICInitStructure);
	
	// 清除更新和捕获中断标志位
    TIM_ClearFlag(GENERAL_CAP_TIM, TIM_FLAG_Update|GENERAL_CAP_TIM_IT_CCx);	
    // 开启更新和捕获中断  
	TIM_ITConfig (GENERAL_CAP_TIM, TIM_IT_Update | GENERAL_CAP_TIM_IT_CCx, ENABLE );
	
	// 使能计数器
	TIM_Cmd(GENERAL_CAP_TIM, ENABLE);
}

static void GENERAL_TIM5_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_CAP_TIM_IRQ ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为3
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

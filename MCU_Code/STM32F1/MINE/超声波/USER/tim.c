#include "tim.h"
#include <stdio.h>

static void GENERAL_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ����Ƚ�ͨ��1 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);
}


static void GENERAL_TIM_Mode_Config(void)
{
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	// �������ڣ���������Ϊ100K
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_Period;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
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
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config();		
}

/******************************Capture********************************/
static void GPIO_Config(void)
{
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //ʹ��GPIOBʱ�� 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 ����   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	     //��ʼ��GPIOB.9
}

static void Tim4_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	   //TIM4 ʱ��ʹ�� 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000-1; //�趨�������Զ���װֵ ���10ms���
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	 0.01s   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

	TIM_ICInitTypeDef  TIM_ICInitStructure;     //���벶��
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // ѡ������� IC4ӳ�䵽TI4��    ��PB9
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //TIM ���� 4 ѡ���Ӧ���� IC1 �� IC2 ��IC3 �� IC4 ����
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
    TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲���һ����ʱ������Ϊ��72*10000��72Mhz=0.01s(����1m�Ľ�������)
    TIM_ICInit(TIM4, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��
    
    TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4

}
static void NVIC_Config(void)
{

  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);     //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}

void Tim4_Init(void)    			  
{  


    GPIO_Config();  //GPIO��ʼ��
	Tim4_Config();  //TIM4��ʼ��
    NVIC_Config();  //�ж����ȼ�����
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�								 
}

u16 Dval;
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET) 
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) != RESET)
		{
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);	//CC4P=1����Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);

		}else
		{
			Dval=TIM_GetCapture4(TIM4);							//��� TIMx ���벶�� 4 ��ֵ
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);	//CC4P=0����Ϊ�����ز���
			printf("%d cm\r\n",((Dval+3)/58));
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);//��� TIMx ���жϴ�����λ 
	
}

#include "key.h"

static uint32_t KEY_CLK[KEYn] = {WKUP_GPIO_CLK, KEY1_GPIO_CLK, KEY2_GPIO_CLK};
static GPIO_TypeDef * KEY_PORT[KEYn] = {WKUP_GPIO_PORT, KEY1_GPIO_PORT, KEY2_GPIO_PORT};
static uint16_t KEY_PIN[KEYn] = {WKUP_GPIO_PIN, KEY1_GPIO_PIN, KEY2_GPIO_PIN};

/*!
    \brief      ������ʼ��
    \Note	    none			
    \param[in]  KEY1, KEY2
    \retval     none
*/
void KEY_GPIO_Cofig(key_typedef_enum key_num)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(KEY_CLK[key_num], ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = KEY_PIN[key_num];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
                                                	
    GPIO_Init(KEY_PORT[key_num], &GPIO_InitStruct);
}

/*!
    \brief      ������⺯��
    \Note	    none			
    \param[in]  KEY1, KEY2
    \retval     none
*/
uint8_t Key_Scan_HIGH(key_typedef_enum key_num)
{
	if(KEY_HIGH == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]))
	{
		//���ּ��
		while(KEY_LOW == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]));
		
		return KEY_HIGH;
	}
	else
	{
			return KEY_LOW;
	}
	
}

uint8_t Key_Scan_LOW(key_typedef_enum key_num)
{
	if(KEY_LOW == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]))
	{
		//���ּ��
		while(KEY_HIGH == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]));
		
		return KEY_LOW;
	}
	else
	{
		return KEY_HIGH;
	}
	
}

//ʹ�øú���ʱ,����ʹ��switch���
uint8_t Key_Scan(key_typedef_enum key_num, key_state_enum key_state)
{
	if(KEY_HIGH == key_state)
		return Key_Scan_HIGH(key_num);
	else if(KEY_LOW == key_state)
		return Key_Scan_LOW(key_num);
	else 
		return KEY_ERROR;
}


static uint8_t KEY_PortSource[KEYn] = {WKUP_EXTI_GPIO_PORT, KEY1_EXTI_GPIO_PORT, KEY2_EXTI_GPIO_PORT};
static uint8_t KEY_PinSource[KEYn] = {WKUP_EXTI_GPIO_PIN, KEY1_EXTI_GPIO_PIN, KEY2_EXTI_GPIO_PIN};
static uint8_t KEY_NVIC_IRQChannel[KEYn] = {EXTI0_IRQn,EXTI2_IRQn, EXTI3_IRQn};

/* KEY */

//�ж����ȼ����ú���
static void EXTI_NVIC_Config(key_typedef_enum key_num, u8 pre, u8 sub)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//��2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//�����ж�Դ
	NVIC_InitStruct.NVIC_IRQChannel = KEY_NVIC_IRQChannel[key_num];
	
	//������ռ���ȼ� ���������ȼ�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = pre;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = sub;
	
	//ʹ���ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*!
    \brief      GPIO�ж����ú���
    \Note	    KEY1, KEY2
    \param[in]  none
    \retval     none
*/
void EXTI_Key_Config(key_typedef_enum key_num, u8 pre, u8 sub)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	
	//�����ж����ȼ�
	EXTI_NVIC_Config(key_num, pre, sub);
	
	//��ʼ��GPIO
    RCC_APB2PeriphClockCmd(KEY_CLK[key_num], ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = KEY_PIN[key_num];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                                                	
    GPIO_Init(KEY_PORT[key_num], &GPIO_InitStruct);
	
	//��ʼ��EXIT
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(KEY_PortSource[key_num], KEY_PinSource[key_num]);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
}

#include "bsp_key.h"
#include "includes.h"

static uint32_t KEY_CLK[KEYn] = {WKUP_GPIO_CLK, KEY1_GPIO_CLK, KEY2_GPIO_CLK};
static GPIO_TypeDef * KEY_PORT[KEYn] = {WKUP_GPIO_PORT, KEY1_GPIO_PORT, KEY2_GPIO_PORT};
static uint16_t KEY_PIN[KEYn] = {WKUP_GPIO_PIN, KEY1_GPIO_PIN, KEY2_GPIO_PIN};

/*!
    \brief      按键初始化
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
    \brief      按键检测函数
    \Note	    none			
    \param[in]  KEY1, KEY2
    \retval     none
*/
uint8_t Key_Scan_HIGH(key_typedef_enum key_num)
{
	if(KEY_HIGH == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]))
	{
		//松手检测
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
		//松手检测
		while(KEY_HIGH == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]));
		
		return KEY_LOW;
	}
	else
	{
		return KEY_HIGH;
	}
	
}

////使用该函数时,建议使用switch语句
//uint8_t Key_Scan(key_typedef_enum key_num, key_state_enum key_state)
//{
//	if(KEY_HIGH == key_state)
//		return Key_Scan_HIGH(key_num);
//	else if(KEY_LOW == key_state)
//		return Key_Scan_LOW(key_num);
//	else 
//		return KEY_ERROR;
//}
//按键检测
uint8_t WKUP_Scan(key_typedef_enum key_num){
	OS_ERR err;
	
	if(KEY_HIGH == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]))
	{
		//松手检测
		//while(KEY_LOW == GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]));
		OSTimeDlyHMSM(0, 0, 0, 15, OS_OPT_TIME_DLY, &err);
		
		if(KEY_HIGH==GPIO_ReadInputDataBit(KEY_PORT[key_num], KEY_PIN[key_num]))
			return KEY_HIGH;
	}
	return KEY_LOW;
}

static uint8_t KEY_PortSource[KEYn] = {WKUP_EXTI_GPIO_PORT, KEY1_EXTI_GPIO_PORT, KEY2_EXTI_GPIO_PORT};
static uint8_t KEY_PinSource[KEYn] = {WKUP_EXTI_GPIO_PIN, KEY1_EXTI_GPIO_PIN, KEY2_EXTI_GPIO_PIN};
static uint8_t KEY_NVIC_IRQChannel[KEYn] = {EXTI0_IRQn,EXTI2_IRQn, EXTI3_IRQn};

/* KEY */

//中断优先级配置函数
static void EXTI_NVIC_Config(key_typedef_enum key_num, u8 pre, u8 sub)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	//配置中断源
	NVIC_InitStruct.NVIC_IRQChannel = KEY_NVIC_IRQChannel[key_num];
	
	//配置抢占优先级 配置子优先级
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = pre;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = sub;
	
	//使能中断
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*!
    \brief      GPIO中断配置函数
    \Note	    KEY1, KEY2
    \param[in]  none
    \retval     none
*/
void EXTI_Key_Config(key_typedef_enum key_num, u8 pre, u8 sub)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	
	//配置中断优先级
	EXTI_NVIC_Config(key_num, pre, sub);
	
	//初始化GPIO
    RCC_APB2PeriphClockCmd(KEY_CLK[key_num], ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = KEY_PIN[key_num];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                                                	
    GPIO_Init(KEY_PORT[key_num], &GPIO_InitStruct);
	
	//初始化EXIT
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(KEY_PortSource[key_num], KEY_PinSource[key_num]);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
}

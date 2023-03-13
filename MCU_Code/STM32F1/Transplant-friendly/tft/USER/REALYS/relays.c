#include "relays.h"

static uint32_t RELAYS_CLK[RELAYSn] = {RELAYS1_GPIO_CLK, RELAYS2_GPIO_CLK};
static GPIO_TypeDef* RELAYS_PORT[RELAYSn] = {RELAYS1_GPIO_PORT, RELAYS2_GPIO_PORT};
static uint16_t RELAYS_PIN[RELAYSn] = {RELAYS1_GPIO_PIN, RELAYS2_GPIO_PIN};

/*!
    \brief      继电器初始化函数
    \Note	    			
	\param[in]  RELAYS1,RELAYS2
    \retval     none
*/
void Relays_GPIO_Config(relays_typedef_enum relays_num)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(RELAYS_CLK[relays_num], ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = RELAYS_PIN[relays_num];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
                                                	
    GPIO_Init(RELAYS_PORT[relays_num], &GPIO_InitStruct); 
	GPIO_ResetBits(RELAYS_PORT[relays_num], RELAYS_PIN[relays_num]);
}

/*!
    \brief      Relays状态控制函数
    \Note	    假设高电平导通
    \param[in]  RELAYS1, RELAYS2
				OFF ON TOGGLE
    \retval     none
*/
void Relays_State_Config(relays_typedef_enum relays_num, relays_state_enum relays_state)
{
	if(RELAYS_ON == relays_state)
	{
		GPIO_SetBits(RELAYS_PORT[relays_num], RELAYS_PIN[relays_num]);
	}
	else if(RELAYS_OFF == relays_state)
	{
		GPIO_ResetBits(RELAYS_PORT[relays_num], RELAYS_PIN[relays_num]);
	}
	else if(RELAYS_TOGGLE == relays_state)
	{
		((GPIO_TypeDef *) RELAYS_PORT[relays_num])->ODR ^= RELAYS_PIN[relays_num];
	}
	else
	{}
}
uint8_t Relays_Scan(relays_typedef_enum relays_num)
{
	if(RELAYS_ON == GPIO_ReadInputDataBit(RELAYS_PORT[relays_num], RELAYS_PIN[relays_num]))
	{	
		return RELAYS_ON;
	}
	else
	{
		return RELAYS_OFF;
	}
	
}



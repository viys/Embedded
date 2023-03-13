#include "led.h"

static uint32_t LED_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK};
static GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};
static uint16_t LED_PIN[LEDn] = {LED1_GPIO_PIN, LED2_GPIO_PIN, LED3_GPIO_PIN};

/*!
    \brief      LED初始化函数
    \Note	    			
	\param[in]  LED1, LED2
    \retval     none
*/
void LED_GPIO_Config(led_typedef_enum led_num)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(LED_CLK[led_num], ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = LED_PIN[led_num];
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
                                                	
    GPIO_Init(LED_PORT[led_num], &GPIO_InitStruct);          
}

/*!
    \brief      LED状态控制函数
    \Note	    假设高电平导通
    \param[in]  LED1, LED2
				OFF ON TOGGLE
    \retval     none
*/
void LED_State_Config(led_typedef_enum led_num, led_state_enum led_state)
{
	if(ON == led_state)
	{
		GPIO_SetBits(LED_PORT[led_num], LED_PIN[led_num]);
	}
	else if(OFF == led_state)
	{
		GPIO_ResetBits(LED_PORT[led_num], LED_PIN[led_num]);
	}
	else if(TOGGLE == led_state)
	{
//		GPIO_WriteBit(LED_PORT[led_num], LED_PIN[led_num], \
//					  (BitAction)(1-GPIO_ReadOutputDataBit(LED_PORT[led_num], LED_PIN[led_num])));
		((GPIO_TypeDef *) LED_PORT[led_num])->ODR ^= LED_PIN[led_num];
	}
	else
	{}
}
uint8_t LED_Scan(led_typedef_enum led_num)
{
	if(LED_ON == GPIO_ReadInputDataBit(LED_PORT[led_num], LED_PIN[led_num]))
	{	
		return LED_ON;
	}
	else
	{
		return LED_OFF;
	}
	
}

#ifndef _KEY_H
#define _KEY_H

#include "stm32f10x.h"

typedef enum 
{
	WKUP = 0,
    KEY1 = 1,
    KEY2 = 2
} key_typedef_enum;


typedef enum 
{
	KEY_LOW = 0,
    KEY_HIGH = 1,
	KEY_ERROR = 2
} key_state_enum;

#define KEYn	3U

#define WKUP_GPIO_PORT		GPIOA
#define WKUP_GPIO_PIN		GPIO_Pin_0
#define WKUP_GPIO_CLK		RCC_APB2Periph_GPIOA
#define WKUP_GPIO_Mode		GPIO_Mode_IN_FLOATING
#define WKUP_EXTI_GPIO_PORT	GPIO_PortSourceGPIOA
#define WKUP_EXTI_GPIO_PIN	GPIO_PinSource0

#define KEY1_GPIO_PORT		GPIOA
#define KEY1_GPIO_PIN		GPIO_Pin_2
#define KEY1_GPIO_CLK		RCC_APB2Periph_GPIOA
#define KEY1_GPIO_Mode		GPIO_Mode_IN_FLOATING
#define KEY1_EXTI_GPIO_PORT	GPIO_PortSourceGPIOA
#define KEY1_EXTI_GPIO_PIN	GPIO_PinSource2

#define KEY2_GPIO_PORT		GPIOA
#define KEY2_GPIO_PIN		GPIO_Pin_3
#define KEY2_GPIO_CLK		RCC_APB2Periph_GPIOA
#define KEY2_GPIO_Mode		GPIO_Mode_IN_FLOATING
#define KEY2_EXTI_GPIO_PORT	GPIO_PortSourceGPIOA
#define KEY2_EXTI_GPIO_PIN	GPIO_PinSource3

void KEY_GPIO_Cofig(key_typedef_enum key_num);
uint8_t Key_Scan_HIGH(key_typedef_enum key_num);
uint8_t Key_Scan_LOW(key_typedef_enum key_num);
uint8_t Key_Scan(key_typedef_enum key_num, key_state_enum key_state);
void EXTI_Key_Config(key_typedef_enum key_num, u8 pre, u8 sub);

#endif

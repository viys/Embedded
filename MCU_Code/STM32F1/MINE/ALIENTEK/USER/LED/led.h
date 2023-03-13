#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_rcc.h"

typedef enum 
{
    LED1 = 0,
    LED2 = 1,
	LED3 = 2
} led_typedef_enum;

typedef enum
{
	OFF = 0,
	ON  = 1,
	TOGGLE =2
} led_state_enum;

#define LED_ON 1
#define LED_OFF 0

#define LEDn	3U

#define LED1_GPIO_PORT		GPIOD
#define LED1_GPIO_PIN		GPIO_Pin_2
#define LED1_GPIO_CLK		RCC_APB2Periph_GPIOD
#define LED1_GPIO_Mode		GPIO_Mode_Out_PP
#define LED1_GPIO_Speed 	GPIO_Speed_50MHz

#define LED2_GPIO_PORT		GPIOA
#define LED2_GPIO_PIN		GPIO_Pin_5
#define LED2_GPIO_CLK		RCC_APB2Periph_GPIOA
#define LED2_GPIO_Mode		GPIO_Mode_Out_PP
#define LED2_GPIO_Speed		GPIO_Speed_50MHz

#define LED3_GPIO_PORT		GPIOB
#define LED3_GPIO_PIN		GPIO_Pin_0
#define LED3_GPIO_CLK		RCC_APB2Periph_GPIOB
#define LED3_GPIO_Mode		GPIO_Mode_Out_PP
#define LED3_GPIO_Speed		GPIO_Speed_50MHz

void LED_GPIO_Config(led_typedef_enum led_num);
void LED_State_Config(led_typedef_enum led_num, led_state_enum led_state);
uint8_t LED_Scan(led_typedef_enum led_num);

#endif

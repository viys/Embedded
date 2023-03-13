#ifndef __RELAYS_H
#define __RELAYS_H

#include "stm32f10x.h"

typedef enum 
{
    RELAYS1 = 0,
    RELAYS2 = 1
} relays_typedef_enum;

typedef enum
{
	RELAYS_OFF = 0,
	RELAYS_ON  = 1,
	RELAYS_TOGGLE =2
} relays_state_enum;

#define RELAYSn	2U

#define RELAYS1_GPIO_PORT		GPIOC
#define RELAYS1_GPIO_PIN		GPIO_Pin_3
#define RELAYS1_GPIO_CLK		RCC_APB2Periph_GPIOC
#define RELAYS1_GPIO_Mode		GPIO_Mode_Out_PP
#define RELAYS1_GPIO_Speed	 	GPIO_Speed_50MHz

#define RELAYS2_GPIO_PORT		GPIOC
#define RELAYS2_GPIO_PIN		GPIO_Pin_13
#define RELAYS2_GPIO_CLK		RCC_APB2Periph_GPIOC
#define RELAYS2_GPIO_Mode		GPIO_Mode_Out_PP
#define RELAYS2_GPIO_Speed		GPIO_Speed_50MHz

void Relays_Config(relays_typedef_enum relays_num);
uint8_t Relays_Scan(relays_typedef_enum relays_num);
void Relays_State(relays_typedef_enum relays_num, relays_state_enum relays_state);

#endif

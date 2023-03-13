#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f10x.h"

#define OFF 0
#define ON 1
#define TOGGLE 2

#define GPIO_OFF 			GPIO_State (1,OFF);
#define GPIO_ON 			GPIO_State (1,ON);
#define GPIO_TOGGLE 	GPIO_State (1,TOGGLE);
#define GPIOB1_TOGGLE   GPIOB->ODR ^= (1 << 1);
#define GPIOB0_TOGGLE   GPIOB->ODR ^= (1 << 0);

void GPIOBx_Init(u8 pin);

#endif

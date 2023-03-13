#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

#define LED_ALL 5

#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4


#define LED_OFF 0
#define LED_ON 1
#define LED_TOGGLE 2
#define LED_ALL_OFF 3
#define LED_ALL_ON	4
#define LED_ALL_TOGGLE 5

#define LED1_OFF LED_State (1,LED_OFF);
#define LED2_OFF LED_State (2,LED_OFF);
#define LED3_OFF LED_State (3,LED_OFF);
#define LED4_OFF LED_State (4,LED_OFF);
#define LED1_ON LED_State (1,LED_ON);
#define LED2_ON LED_State (2,LED_ON);
#define LED3_ON LED_State (3,LED_ON);
#define LED4_ON LED_State (4,LED_ON);
#define LED1_TOGGLE LED_State (1,LED_TOGGLE);
#define LED2_TOGGLE LED_State (2,LED_TOGGLE);
#define LED3_TOGGLE LED_State (3,LED_TOGGLE);
#define LED4_TOGGLE LED_State (4,LED_TOGGLE);

void LED_Init(u8 led);
void LED_State(u8 led, u8 state);
void LED_Running(u8 keystate,u8 times);

#endif

#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"

#define KEY_ALL 5

#define KEY1 (((GPIOA->IDR & (1 << 0))))
#define KEY2 (!(GPIOE->IDR & (1 << 2)))
#define KEY3 (!(GPIOE->IDR & (1 << 3)))
#define KEY4 (!(GPIOE->IDR & (1 << 4)))

void KEY_Init(u8);
void delay(u32 t);
u8 key_Scan(void);


#endif


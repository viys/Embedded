#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

#define SYSCLK (168 * 1000000 / 8)

void SysTick_us(u32 us);
void SysTick_ms(u32 ms);
void SysTick_inter_ms(u32 ms);

#endif 

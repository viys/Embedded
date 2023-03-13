#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

#define SYSCLK (72 * 1000000 / 8)

void SysTick_ms(u32 ms);
void SysTick_inter_ms(u32 ms);

#endif

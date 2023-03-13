#ifndef _RCCCLK_H
#define _RCCCLK_H

#include "stm32f10x.h"

void HSE_SetSysCLK(uint32_t RCC_PLLMul_x);
void HSI_SetSysClock(uint32_t RCC_PLLMul_x);
void MCO_GPIO_Config(void);

#endif


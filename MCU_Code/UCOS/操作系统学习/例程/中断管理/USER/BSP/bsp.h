#ifndef __BSP_H
#define __BSP_H

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart1.h"

typedef	uint32_t	CPU_INT32U;

CPU_INT32U BSP_CPU_ClkFreq(void);
void BSP_Init(void);

#endif

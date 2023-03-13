#ifndef __BSP_H
#define __BSP_H

#include "cpu.h"
#include "stm32f10x.h"
#include "bsp_relays.h"
#include "bsp_usart.h"

void BSP_Init(void);
CPU_INT32U BSP_CPU_ClkFreq(void);

#endif

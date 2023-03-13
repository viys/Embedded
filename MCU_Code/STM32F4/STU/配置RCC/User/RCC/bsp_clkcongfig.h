#ifndef __BSP_CLKCONFIG_H

#include "stm32f4xx.h"

#define __BSP_CLKCONFIG_H
#define PLL_M      25
#define PLL_N      336
#define PLL_P      2
#define PLL_Q      7


void User_HSE_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ);
void User_HEI_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ);


#endif /* __BSP_CLKCONFIG_H */


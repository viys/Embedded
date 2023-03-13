#ifndef _IWDG_H
#define _IWDG_H

#include "stm32f10x.h"

extern uint8_t WWDG_CNT;

void IWDG_Init(uint8_t prer, uint16_t rlr);

void WWDG_NVIC_Init(void);
void WWDG_Set_Counter(uint8_t cnt);
void WWDG_Init(uint8_t tr, uint8_t wr, uint32_t fprer);

#endif

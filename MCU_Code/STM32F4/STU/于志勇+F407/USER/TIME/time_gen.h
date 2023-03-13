#ifndef __TIME_GEN_H
#define __TIME_GEN_H

#include "stm32f4xx.h"

void Time5_CH1_Init(u16 psc, u16 arr, u16 ccr);
void Time9_CH1_Init(u16 psc, u16 arr);
void TIM10_CH1_Init(u16 psc, u16 arr, u16 ccr);
void TIM14_CH1_Init(u16 psc, u16 arr);

#endif

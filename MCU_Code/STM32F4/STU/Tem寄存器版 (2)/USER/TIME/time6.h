#ifndef __TIME6_H
#define __TIME6_H

#include "stm32f4xx.h"

void Time6_Init(void);
void Time6_delay(u16 psc, u16 arr);
void Time6_inter_Init(u16 psc, u16 arr);

#endif

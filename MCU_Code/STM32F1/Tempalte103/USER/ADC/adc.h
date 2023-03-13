#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

void ADC1_Init(void );
void ADC2_Init(void );
u16 ADC_GETD(void );
void ADC1_CH5_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);

#endif

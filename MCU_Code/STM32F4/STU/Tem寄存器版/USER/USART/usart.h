#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include "stdio.h"

void USART1_Init(u32 fclk, u32 bood, u8 over8);
void USART1_Send_str(char *str);


#endif

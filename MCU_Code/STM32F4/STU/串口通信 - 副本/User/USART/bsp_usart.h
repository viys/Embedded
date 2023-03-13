#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f4xx.h"

void USART1_Init(u32 pck,u32 fck,u8 over8);
void USER_USART_Config(void);
void NVIC_Configuration(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);

#endif /* __BSP_USART_H */

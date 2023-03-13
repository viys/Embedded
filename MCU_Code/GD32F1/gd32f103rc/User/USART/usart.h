#ifndef _USART_H
#define _USART_H

#include "gd32f10x.h"

#define COMn                             2U

#define COM0                        USART0
#define COM0_CLK                    RCU_USART0
#define COM0_TX_PIN                 GPIO_PIN_6
#define COM0_RX_PIN                 GPIO_PIN_7
#define COM0_GPIO_PORT              GPIOB
#define COM0_GPIO_CLK               RCU_GPIOB


#define COM1                        USART1
#define COM1_CLK                    RCU_USART1
#define COM1_TX_PIN                 GPIO_PIN_2
#define COM1_RX_PIN                 GPIO_PIN_3
#define COM1_GPIO_PORT              GPIOA
#define COM1_GPIO_CLK               RCU_GPIOA

void usart_init(uint32_t com);
uint16_t usart0_get(uint32_t usart_periph);
void usart_interrupt(uint32_t com);

#endif

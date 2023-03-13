#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include <stdio.h>

typedef enum 
{
    USART_1 = 0,
    USART_2 = 1,
	USART_3 = 2
} usart_typedef_enum;

typedef enum 
{
    USART_MODE_COMMON = 0,
    USART_MODE_EXTI = 1
} usart_mode_typedef_enum;

#define	USART_OFF		0
#define	USART_ON		1

#define USARTn	3U

#define USART1_BAUDRATE			115200


#define USART1_GPIO_CLK			RCC_APB2Periph_GPIOA
#define USART1_TX_GPIO_PORT		GPIOA
#define USART1_TX_GPIO_PIN		GPIO_Pin_9
#define USART1_RX_GPIO_PORT		GPIOA
#define USART1_RX_GPIO_PIN		GPIO_Pin_10
#define USART1_CLK				RCC_APB2Periph_USART1 	
#define USART1_Which			USART1
#define USART1_IRQ				USART1_IRQn

#define USART2_GPIO_CLK			RCC_APB2Periph_GPIOA
#define USART2_TX_GPIO_PORT		GPIOA
#define USART2_TX_GPIO_PIN		GPIO_Pin_2
#define USART2_RX_GPIO_PORT		GPIOA
#define USART2_RX_GPIO_PIN		GPIO_Pin_3
#define USART2_CLK				RCC_APB1Periph_USART2
#define USART2_Which			USART2
#define USART2_IRQ				USART2_IRQn

#define USART3_GPIO_CLK			RCC_APB2Periph_GPIOB
#define USART3_TX_GPIO_PORT		GPIOB
#define USART3_TX_GPIO_PIN		GPIO_Pin_10
#define USART3_RX_GPIO_PORT		GPIOB
#define USART3_RX_GPIO_PIN		GPIO_Pin_11
#define USART3_CLK				RCC_APB1Periph_USART3
#define USART3_Which			USART3
#define USART3_IRQ				USART3_IRQn

void USART_Config(usart_typedef_enum usart_num, usart_mode_typedef_enum usart_mode, u32 baudrate);

#endif

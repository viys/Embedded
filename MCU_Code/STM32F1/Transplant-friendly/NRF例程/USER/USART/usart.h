#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern char Usart3_RxCompleted ;               //外部声明，其他文件可以调用该变量
extern unsigned int Usart3_RxCounter;          //外部声明，其他文件可以调用该变量
extern char Usart3_RxBuff[1024]; //外部声明，其他文件可以调用该变量

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

#define USART1_TXBUFF_SIZE   256   		//定义串口1发送缓冲区大小为256字节
#define USART3_TXBUFF_SIZE   1024   		   //定义串口2 发送缓冲区大小 1024字节
#define USART3_RXBUFF_SIZE   1024              //定义串口2 接收缓冲区大小 1024字节

void USART_Config(usart_typedef_enum usart_num, usart_mode_typedef_enum usart_mode, u32 baudrate);
void u3_TxData(unsigned char *data);
void u3_printf(char* fmt, ...) ;

#endif

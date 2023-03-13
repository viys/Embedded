#ifndef		__MY_UART_H__
#define		__MY_UART_H__

//--------------------- include files ---------------------
#include "gd32f1x0.h"
#include "stdio.h"
//--------------------- macro define ----------------------
#define		IO_MY_UART1_TX			PB6
#define		IO_MY_UART1_RX			PB7

#define		MY_UART1_TX_RCU			RCU_GPIOB
#define		MY_UART1_TX_PORT		GPIOB
#define		MY_UART1_TX_PIN			GPIO_PIN_6

#define		MY_UART1_RX_RCU			RCU_GPIOB
#define		MY_UART1_RX_PORT		GPIOB
#define		MY_UART1_RX_PIN			GPIO_PIN_7

#define		MY_UART1_RCU			RCU_USART1

#define		MY_UART1				USART1
//--------------------- function declare ----------------------
void my_uart_init(void);

#endif	//__MY_UART_H__


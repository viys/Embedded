/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "gd32f1x0.h"
#include <stdio.h>
#include "myuart.h"

/****************************************************************************************
Function:
Description: 
    \brief      configure uart
    \param[in]  none
    \param[out] none
    \retval     none
****************************************************************************************/
void my_uart_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable( MY_UART1_TX_RCU );						// clock already seted
    rcu_periph_clock_enable( MY_UART1_RX_RCU );

    /* enable USART clock */
    rcu_periph_clock_enable(MY_UART1_RCU);

    /* connect port to USARTx_Tx */
    gpio_af_set(MY_UART1_TX_PORT, GPIO_AF_4, MY_UART1_TX_PIN);					// TX = PA8

    /* connect port to USARTx_Rx */
    gpio_af_set(MY_UART1_RX_PORT, GPIO_AF_4, MY_UART1_RX_PIN);					// RX = PB0		

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(MY_UART1_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,MY_UART1_TX_PIN);
    gpio_output_options_set(MY_UART1_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ,MY_UART1_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(MY_UART1_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,MY_UART1_RX_PIN);
    gpio_output_options_set(MY_UART1_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ,MY_UART1_RX_PIN);

    /* USART configure */
    usart_deinit(MY_UART1);
    usart_baudrate_set(MY_UART1,115200U);
    usart_transmit_config(MY_UART1, USART_TRANSMIT_ENABLE);
    usart_receive_config(MY_UART1, USART_RECEIVE_ENABLE);
    usart_enable(MY_UART1);
}


/****************************************************************************************
Function:
Description: 
				retarget the C library printf function to the USART
****************************************************************************************/
int fputc(int ch, FILE *f)
{
    usart_data_transmit(MY_UART1, (uint8_t)ch);
    while(RESET == usart_flag_get(MY_UART1, USART_FLAG_TBE));
    return ch;
}





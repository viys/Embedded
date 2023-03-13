#include "usart.h"
#include <stdio.h>

static rcu_periph_enum COM_CLK[COMn] = {COM0_CLK, COM1_CLK};
static uint32_t COM_TX_PIN[COMn] = {COM0_TX_PIN, COM1_TX_PIN};
static uint32_t COM_RX_PIN[COMn] = {COM0_RX_PIN, COM1_RX_PIN};
static uint32_t COM_GPIO_PORT[COMn] = {COM0_GPIO_PORT, COM1_GPIO_PORT};
static rcu_periph_enum COM_GPIO_CLK[COMn] = {COM0_GPIO_CLK, COM1_GPIO_CLK};
static uint32_t USART_EXTI_IRQn[COMn] = {USART0_IRQn, USART1_IRQn};

/*!
    \brief      configure COM port
    \param[in]  com: COM on the board
      \arg        EVAL_COM0: COM0 on the board
      \arg        EVAL_COM1: COM1 on the board
    \param[out] none
    \retval     none
*/
void usart_init(uint32_t com)
{
    uint32_t com_id = 0U;
    if(COM0 == com){
        com_id = 0U;
    }else if(COM1 == com){
        com_id = 1U;
    }
    
    /* enable GPIO clock */
    rcu_periph_clock_enable(COM_GPIO_CLK[com_id]);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[com_id]);

		if(COM0 == com)
		{
			rcu_periph_clock_enable(RCU_AF); 
			gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);//PB6,PB7需要重映射
    }
    /* connect port to USARTx_Tx */
    gpio_init(COM_GPIO_PORT[com_id], GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, COM_TX_PIN[com_id]);

    /* connect port to USARTx_Rx */
    gpio_init(COM_GPIO_PORT[com_id], GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, COM_RX_PIN[com_id]);

    /* USART configure */
    usart_deinit(com);//复位外设USART
    usart_baudrate_set(com, 115200U);//配置波特率
    usart_word_length_set(com, USART_WL_8BIT);//配置USART字长
    usart_stop_bit_set(com, USART_STB_1BIT);//配置停止位
    usart_parity_config(com, USART_PM_NONE);//配置奇偶校验
    //usart_hardware_flow_rts_config(com, USART_RTS_DISABLE);
    //usart_hardware_flow_cts_config(com, USART_CTS_DISABLE);//配置硬件控制流
    usart_receive_config(com, USART_RECEIVE_ENABLE);//接收器使能
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);//发送器使能
    usart_enable(com);//串口使能
}

//#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
//#define TRANSMIT_SIZE            (ARRAYNUM(txbuffer) - 1)

//uint8_t txbuffer[] = "\n\rUSART interrupt test\n\r";
//uint8_t rxbuffer[32];
//uint8_t tx_size = TRANSMIT_SIZE;
//uint8_t rx_size = 32;
//__IO uint8_t txcount = 0; 
//__IO uint16_t rxcount = 0; 

//void usart_interrupt(uint32_t com)
//{
//	uint32_t com_id = 0U;
//  if(COM0 == com){
//      com_id = 0U;
//  }else if(COM1 == com){
//      com_id = 1U;
//  }
//  
//	nvic_irq_enable(USART_EXTI_IRQn[com_id], 0U, 0U);
//	usart_interrupt_enable(com, USART_INT_TBE);
//    
//  /* wait until USART send the transmitter_buffer */
//  while(txcount < tx_size);
//  
//  while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
//  
//  usart_interrupt_enable(com, USART_INT_RBNE);
//  
//  /* wait until USART receive the receiver_buffer */
//  while(rxcount < rx_size);
//  if(rxcount == rx_size)
//	{
//		printf("\n\rUSART receive successfully!\n\r");
//	}
//}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
uint16_t usart0_get(uint32_t usart_periph)
{
	return (uint16_t)(usart_data_receive(usart_periph));
}


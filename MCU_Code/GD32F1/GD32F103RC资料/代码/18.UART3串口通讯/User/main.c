/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
uint8_t transmitter_buffer[] = "HELLOWORLD";
uint8_t receiver_buffer[10];//接收数组，在UART3_IRQHandler函数中会存储
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 10;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 

//usart3 pc10,pc11作为串口，波特率9600,8位数据，1位停止位，无校验 
void gd_eval_com_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART3);

    /* configure USART Tx as alternate function push-pull */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    /* configure USART Rx as alternate function push-pull */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_11);


    /* USART configure */
    usart_deinit(UART3);
    usart_baudrate_set(UART3,9600);
		usart_word_length_set(UART3, USART_WL_8BIT);
    usart_stop_bit_set(UART3, USART_STB_1BIT);
    usart_parity_config(UART3, USART_PM_NONE);
	  usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);
    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);
    usart_enable(UART3);
		
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
	delay_1ms(1000);//等待1秒
	
	//USART相关配置
	gd_eval_com_init();
	nvic_irq_enable(UART3_IRQn, 0, 0);//中断使能
	usart_interrupt_enable(UART3, USART_INT_RBNE);//接收中断打开
	
    while(1)
    {

      if(rxcount >= receivesize)
		{
			rxcount=0;
			txcount=0;
			usart_interrupt_enable(UART3, USART_INT_TBE);//发送中断打开
			while(txcount < transfersize);
			while (RESET == usart_flag_get(UART3, USART_FLAG_TC));//发送完成判断
			 usart_interrupt_enable(UART3, USART_INT_RBNE);//接收中断打开
		}
    }

}

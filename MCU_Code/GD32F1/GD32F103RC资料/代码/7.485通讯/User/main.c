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
uint8_t transmitter_buffer[] = "HELLOWORLD";//定义发送数组
uint8_t receiver_buffer[10];//定义接收数组
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)//计算大小
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 10;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 

//USART0初始化，使用PB6(TX),PB7(RX)脚，9600波特率，无校验，8位数据，1位停止
void gd_eval_com_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);//使能GPIOB时钟

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);//使能USART0时钟
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);//PB6,PB7需要重映射
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);//PB6输出

    /* configure USART Rx as alternate function push-pull */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_7);//PB7输入
    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0,9600);
		usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
	  usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_enable(USART0);//使能USART0
		
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF); //管脚复用时钟alternate function clock使能
	delay_1ms(1000);
	//USART相关配置
	gd_eval_com_init();
	nvic_irq_enable(USART0_IRQn, 0, 0);//使能USART0中断
	usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开
	
    while(1)
    {
			
			if(rxcount >= receivesize)//接收满10个字节，在USART0_IRQHandler函数中计数
			{
				rxcount=0;
				txcount=0;
				usart_interrupt_enable(USART0, USART_INT_TBE);//发送中断打开
				while(txcount < transfersize);//等待发送完成，在USART0_IRQHandler函数中计数
				while (RESET == usart_flag_get(USART0, USART_FLAG_TC));//发送完成判断
				 usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开
			}
    }

}

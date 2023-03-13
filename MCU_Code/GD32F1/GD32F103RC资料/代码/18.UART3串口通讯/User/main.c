/*****************************************************************************
  �ļ�: main.c
  ����: Zhengyu https://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20210401
	ƽ̨:MINI-GD32F103RCT6������
	΢�ź�:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
uint8_t transmitter_buffer[] = "HELLOWORLD";
uint8_t receiver_buffer[10];//�������飬��UART3_IRQHandler�����л�洢
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 10;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 

//usart3 pc10,pc11��Ϊ���ڣ�������9600,8λ���ݣ�1λֹͣλ����У�� 
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
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);//�ȴ�1��
	
	//USART�������
	gd_eval_com_init();
	nvic_irq_enable(UART3_IRQn, 0, 0);//�ж�ʹ��
	usart_interrupt_enable(UART3, USART_INT_RBNE);//�����жϴ�
	
    while(1)
    {

      if(rxcount >= receivesize)
		{
			rxcount=0;
			txcount=0;
			usart_interrupt_enable(UART3, USART_INT_TBE);//�����жϴ�
			while(txcount < transfersize);
			while (RESET == usart_flag_get(UART3, USART_FLAG_TC));//��������ж�
			 usart_interrupt_enable(UART3, USART_INT_RBNE);//�����жϴ�
		}
    }

}

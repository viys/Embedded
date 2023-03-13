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
uint8_t transmitter_buffer[] = "HELLOWORLD";//���巢������
uint8_t receiver_buffer[10];//�����������
#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)//�����С
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 10;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 

//USART0��ʼ����ʹ��PB6(TX),PB7(RX)�ţ�9600�����ʣ���У�飬8λ���ݣ�1λֹͣ
void gd_eval_com_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);//ʹ��GPIOBʱ��

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);//ʹ��USART0ʱ��
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);//PB6,PB7��Ҫ��ӳ��
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);//PB6���

    /* configure USART Rx as alternate function push-pull */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_7);//PB7����
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
    usart_enable(USART0);//ʹ��USART0
		
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF); //�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);
	//USART�������
	gd_eval_com_init();
	nvic_irq_enable(USART0_IRQn, 0, 0);//ʹ��USART0�ж�
	usart_interrupt_enable(USART0, USART_INT_RBNE);//�����жϴ�
	
    while(1)
    {
			
			if(rxcount >= receivesize)//������10���ֽڣ���USART0_IRQHandler�����м���
			{
				rxcount=0;
				txcount=0;
				usart_interrupt_enable(USART0, USART_INT_TBE);//�����жϴ�
				while(txcount < transfersize);//�ȴ�������ɣ���USART0_IRQHandler�����м���
				while (RESET == usart_flag_get(USART0, USART_FLAG_TC));//��������ж�
				 usart_interrupt_enable(USART0, USART_INT_RBNE);//�����жϴ�
			}
    }

}

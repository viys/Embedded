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
#include "string.h"

FlagStatus receive_flag;//判断数据是否有接收到
uint8_t transmit_number = 0x0;
can_receive_message_struct receive_message;//接收数据结构体
can_trasnmit_message_struct transmit_message;//发送数据结构体
#define CAN0_USED
#define CANX CAN0//CAN0

//CAN设置，250kbps
void can_networking_init(void)
{
    can_parameter_struct            can_parameter;
    can_filter_parameter_struct     can_filter;
    
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
    /* initialize CAN register */
    can_deinit(CANX);
    /* initialize CAN */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;
    /* baudrate 250kbps */
    can_parameter.prescaler = 24;
    can_init(CANX, &can_parameter);

    /* initialize filter */
#ifdef  CAN0_USED
    /* CAN0 filter number */
    can_filter.filter_number = 0;
#else
    /* CAN1 filter number */
    can_filter.filter_number = 15;
#endif
    /* initialize filter */    
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;  
    can_filter.filter_fifo_number = CAN_FIFO1;
    can_filter.filter_enable = ENABLE;
    can_filter_init(&can_filter);
}
//CAN接收中断使能
void nvic_config(void)
{
    nvic_irq_enable(CAN0_RX1_IRQn,0,0);

}
//PA11,PA12配置成CAN口
void gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);  

}
int main(void)
{
		rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
		systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
		rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
		gpio_config();//CAN GPIO配置
		nvic_config();//中断配置
		can_networking_init();//配置250kbps
		/* enable CAN receive FIFO1 not empty interrupt */
		can_interrupt_enable(CANX, CAN_INT_RFNE1);
		/* initialize transmit message */
		can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);
		transmit_message.tx_sfid = 0x321;
		transmit_message.tx_efid = 0x01;
		transmit_message.tx_ft = CAN_FT_DATA;
		transmit_message.tx_ff = CAN_FF_STANDARD;//标准帧
		transmit_message.tx_dlen = 8;
		/* initialize receive message */
		can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);

	
	
    while(1)
    {
			if(receive_flag==SET)//判断是否有数据收到，CAN0_RX1_IRQHandler函数中置位
			{
				receive_flag=RESET;
				transmit_message.tx_sfid=receive_message.rx_sfid;
				memcpy(transmit_message.tx_data,receive_message.rx_data,8);//把接收数据拷贝到发送结构体
				can_message_transmit(CANX, &transmit_message);//发送数据
				delay_1ms(1000); 
			}				

    }

}

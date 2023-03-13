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


unsigned char EEDATA;//定义存放EE数据的变量
extern void EE_IIC_Init(void);
extern uint8_t EE_EE_IIC_SendByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t data);
extern uint8_t EE_IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf);
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF); //管脚复用时钟alternate function clock使能
	delay_1ms(1000);//等待1秒
	rcu_periph_clock_enable(RCU_GPIOB);//使能GPIOB时钟
	EE_IIC_Init();//EE初始化，采用模拟IIC引脚驱动EEPROM
	EE_EE_IIC_SendByteToSlave(0xA0,0x00,0x33);//EEPROM的0地址写入0x33
	delay_1ms(10);//等待10ms
	EE_IIC_ReadByteFromSlave(0xA0,0x00,&EEDATA);//从EEPROM的0地址读出数据
    while(1)
    {

    }
}

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


unsigned char EEDATA;//������EE���ݵı���
extern void EE_IIC_Init(void);
extern uint8_t EE_EE_IIC_SendByteToSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t data);
extern uint8_t EE_IIC_ReadByteFromSlave(uint8_t I2C_Addr,uint8_t reg,uint8_t *buf);
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF); //�ܽŸ���ʱ��alternate function clockʹ��
	delay_1ms(1000);//�ȴ�1��
	rcu_periph_clock_enable(RCU_GPIOB);//ʹ��GPIOBʱ��
	EE_IIC_Init();//EE��ʼ��������ģ��IIC��������EEPROM
	EE_EE_IIC_SendByteToSlave(0xA0,0x00,0x33);//EEPROM��0��ַд��0x33
	delay_1ms(10);//�ȴ�10ms
	EE_IIC_ReadByteFromSlave(0xA0,0x00,&EEDATA);//��EEPROM��0��ַ��������
    while(1)
    {

    }
}

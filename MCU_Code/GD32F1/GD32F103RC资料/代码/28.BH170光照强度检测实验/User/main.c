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
#include "oled.h"
#define I2C1_SLAVE_ADDRESS7    0x46
void i2c_config(void)
{
    /* configure I2C1 clock */
    i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
    /* configure I2C1 address */
    i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C1_SLAVE_ADDRESS7);
    /* enable I2C1 */
    i2c_enable(I2C1);
    /* enable acknowledge */
    i2c_ack_config(I2C1, I2C_ACK_ENABLE);
}
//д��1�ֽ����ݵ�BH170,�豸��ַ0x46
void BH170_WriteReg(uint8_t reg_add)
{

	  /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus*/
    i2c_master_addressing(I2C1, I2C1_SLAVE_ADDRESS7, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set*/
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
 

		/* send a data byte */
		i2c_data_transmit(I2C1,reg_add);
		/* wait until the transmission data register is empty*/
		while(!i2c_flag_get(I2C1, I2C_FLAG_TBE));
		 i2c_stop_on_bus(I2C1);
  
}

//��ȡBH170����,�豸��ַ0x46
void BH170_ReadData(unsigned char*Read)
{

	
	 /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C1);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C1, I2C1_SLAVE_ADDRESS7, I2C_RECEIVER);
    /* disable ACK before clearing ADDSEND bit */
    i2c_ack_config(I2C1, I2C_ACK_DISABLE);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
    /* Wait until the last data byte is received into the shift register */
    /* wait until the RBNE bit is set */
    while(!i2c_flag_get(I2C1, I2C_FLAG_RBNE));

    /* read a data from I2C_DATA */
    Read[0] = i2c_data_receive(I2C1);

    /* read a data from I2C_DATA */
    Read[1] = i2c_data_receive(I2C1);
    /* send a stop condition */
    i2c_stop_on_bus(I2C1);

	
}

uint8_t DataBuff[2];//�洢����
float LightData;//����ǿ�ȣ���С��λ����λlx��
uint32_t LightData_Hex;//����ǿ�ȣ���������λlx��
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ�� 
	delay_1ms(1000);//�ȴ�1��
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOBʱ��ʹ��
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);//PB10Ҫ���ó�I2C1 SCL,PB11Ҫ���ó�I2C1 SDA
  rcu_periph_clock_enable(RCU_I2C1);
	i2c_config();
	
	OLED_Init();//OLED��ʼ��  
	OLED_Clear();//OLED���� 

	
	BH170_WriteReg(0x01);// power on
	BH170_WriteReg(0x10);//H- resolution mode
	HAL_Delay(180);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		BH170_WriteReg(0x01);// power on
		BH170_WriteReg(0x10);//H- resolution mode
		HAL_Delay(180);//�ȴ�180ms
		BH170_ReadData(DataBuff);//��ȡ����
		LightData=((DataBuff[0]<<8)+DataBuff[1])/1.2f;//����ת���ɹ�ǿ�ȣ���λlx
    LightData_Hex=LightData;//floatת��������
		OLED_ShowNum(6,3,LightData_Hex,5,16);//��ʾ��ǿ��
		OLED_ShowString(72,3,"lx");//��ʾ��ǿ�ȵ�λlx
		OLED_ShowCHinese(18,0,0);//��
		OLED_ShowCHinese(36,0,1);//��
		OLED_ShowCHinese(54,0,2);//��
		OLED_ShowCHinese(72,0,3);//��
		OLED_ShowCHinese(90,0,4);//��
		
	}	  
 
}

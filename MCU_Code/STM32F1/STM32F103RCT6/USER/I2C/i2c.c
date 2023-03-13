#include "i2c.h"

void I2C_EE_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef  I2C_InitStruct;

	//��GPIO��ʱ��
	EEPROM_I2C1_GPIO_APBxClkCmd(EEPROM_I2C1_SCL_GPIO_CLK | EEPROM_I2C1_SDA_GPIO_CLK, ENABLE);
	
	//��I2C1��ʱ��
	EEPROM_I2C1_APBxClkCmd(EEPROM_I2C1_CLK, ENABLE);
	
	//����SCL GPIO����ģʽ
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C1_SCL_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C1_SCL_GPIO_PORT, &GPIO_InitStruct);
	//����SDA GPIO����ģʽ
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C1_SDA_GPIO_PIN;
	GPIO_Init(EEPROM_I2C1_SDA_GPIO_PORT, &GPIO_InitStruct);
	
	//����i2c����ģʽ
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = EEPROM_I2C1_BAUDRATE;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = STM32_I2C_OWN_ADDR;
	I2C_Init(EEPROM_I2C1, &I2C_InitStruct);
	
	//ʹ��i2c
	I2C_Cmd(EEPROM_I2C1, ENABLE);
}

//��eepromд������
void EEPROM_Byte_Write(uint8_t addr, uint8_t data)
{
	//������ʼ�ź�
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//�����豸��ַ
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//����Ҫ�����Ĵ��浥Ԫ��ַ
	I2C_SendData(EEPROM_I2C1, addr);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	//����Ҫ�洢������
	I2C_SendData(EEPROM_I2C1, data);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//���ݴ������
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
}

//��eeprom��ȡ����
void EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead)
{
	//������ʼ�ź�
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//�����豸��ַ
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//����Ҫ�����Ĵ��浥Ԫ��ַ
	I2C_SendData(EEPROM_I2C1, addr);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	//�ڶ�����ʼ�ź�
	//������ʼ�ź�
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//�����豸��ַ
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	while(numByteToRead)
	{
		
		if(1 == numByteToRead)
		{
			I2C_AcknowledgeConfig(EEPROM_I2C1, DISABLE);
			while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
			*data = I2C_ReceiveData(EEPROM_I2C1);
		}
		while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		//����Ҫ��ȡ������
		*data = I2C_ReceiveData(EEPROM_I2C1);
		numByteToRead--;
	}
	//���ݴ������
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
	//��������ACKʹ��,�Ա��´�ͨѶ
	I2C_AcknowledgeConfig(EEPROM_I2C1, DISABLE);
}
	
//�ȴ��ڲ�ʱ�����
void EEPROM_WaitForWritedEnd(void)
{
	do
	{
	//������ʼ�ź�
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(RESET == I2C_GetFlagStatus(EEPROM_I2C1, I2C_FLAG_SB));
	//�����豸��ַ
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	}
	while(RESET == I2C_GetFlagStatus(EEPROM_I2C1, I2C_FLAG_ADDR));
	//�ڲ�ʱ�����
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
}

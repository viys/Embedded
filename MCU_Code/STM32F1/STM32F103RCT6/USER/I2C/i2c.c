#include "i2c.h"

void I2C_EE_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef  I2C_InitStruct;

	//打开GPIO的时钟
	EEPROM_I2C1_GPIO_APBxClkCmd(EEPROM_I2C1_SCL_GPIO_CLK | EEPROM_I2C1_SDA_GPIO_CLK, ENABLE);
	
	//打开I2C1的时钟
	EEPROM_I2C1_APBxClkCmd(EEPROM_I2C1_CLK, ENABLE);
	
	//配置SCL GPIO复用模式
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C1_SCL_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_I2C1_SCL_GPIO_PORT, &GPIO_InitStruct);
	//配置SDA GPIO复用模式
	GPIO_InitStruct.GPIO_Pin = EEPROM_I2C1_SDA_GPIO_PIN;
	GPIO_Init(EEPROM_I2C1_SDA_GPIO_PORT, &GPIO_InitStruct);
	
	//配置i2c工作模式
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = EEPROM_I2C1_BAUDRATE;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = STM32_I2C_OWN_ADDR;
	I2C_Init(EEPROM_I2C1, &I2C_InitStruct);
	
	//使能i2c
	I2C_Cmd(EEPROM_I2C1, ENABLE);
}

//向eeprom写入数据
void EEPROM_Byte_Write(uint8_t addr, uint8_t data)
{
	//产生起始信号
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//发送设备地址
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//发送要操作的储存单元地址
	I2C_SendData(EEPROM_I2C1, addr);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	//发送要存储的数据
	I2C_SendData(EEPROM_I2C1, data);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//数据传输完成
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
}

//从eeprom读取数据
void EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead)
{
	//产生起始信号
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//发送设备地址
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//发送要操作的储存单元地址
	I2C_SendData(EEPROM_I2C1, addr);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	//第二次起始信号
	//产生起始信号
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(ERROR == I2C_CheckEvent(EEPROM_I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//发送设备地址
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
		//接收要读取的数据
		*data = I2C_ReceiveData(EEPROM_I2C1);
		numByteToRead--;
	}
	//数据传输完成
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
	//重新配置ACK使能,以便下次通讯
	I2C_AcknowledgeConfig(EEPROM_I2C1, DISABLE);
}
	
//等待内部时序完成
void EEPROM_WaitForWritedEnd(void)
{
	do
	{
	//产生起始信号
	I2C_GenerateSTART(EEPROM_I2C1, ENABLE);
	while(RESET == I2C_GetFlagStatus(EEPROM_I2C1, I2C_FLAG_SB));
	//发送设备地址
	I2C_Send7bitAddress(EEPROM_I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	}
	while(RESET == I2C_GetFlagStatus(EEPROM_I2C1, I2C_FLAG_ADDR));
	//内部时序完成
	I2C_GenerateSTOP(EEPROM_I2C1, ENABLE);
}

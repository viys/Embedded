#include "i2c.h"
#include "systick.h"
#include "usart.h"

uint16_t timeout = 1000;

static void I2C1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitSturcture;
	
	I2C1_GPIO_CLKCMD(I2C1_SCL_GPIO_CLK,ENABLE);
	I2C1_GPIO_CLKCMD(I2C1_SDA_GPIO_CLK,ENABLE);
	
	
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitSturcture.GPIO_Pin = I2C1_SCL_GPIO_PIN;
	GPIO_Init(I2C1_SCL_GPIO_PORT,&GPIO_InitSturcture);
	GPIO_InitSturcture.GPIO_Pin = I2C1_SDA_GPIO_PIN;
	GPIO_Init(I2C1_SDA_GPIO_PORT,&GPIO_InitSturcture);
}

static void I2C1_Mode_Config(void)
{
	I2C_InitTypeDef I2C_InitSturcture;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	I2C1_CLKCMD(I2C1_CLK,ENABLE);
	
	I2C_InitSturcture.I2C_Ack = I2C_Ack_Enable;
	I2C_InitSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitSturcture.I2C_ClockSpeed = 100000;
	I2C_InitSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitSturcture.I2C_Mode = I2C_Mode_I2C;
	I2C_InitSturcture.I2C_OwnAddress1 = I2C_MASTER_ADDR;
	I2C_Init(I2C1,&I2C_InitSturcture);
	
	I2C_Cmd(I2C1,ENABLE);

}

void I2C1_Init(void)
{
	I2C1_Mode_Config();
	
	I2C1_GPIO_Config();
}

void BH1750FVI_Send(uint8_t opcode)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{	
		if((timeout--)==0)
			printf("EV5 Fail");
	}timeout=1000;//ev5事件被检测到
	I2C_Send7bitAddress(I2C1,I2C_SLAVE_ADDR,I2C_Direction_Transmitter);
	//I2C_SendData(I2C1,0x46);
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((timeout--)==0)
			printf("EV6 Fail");
	}timeout=1000;//ev6事件被检测到
	I2C_SendData(I2C1,opcode);
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{	
		if((timeout--)==0)
			printf("EV8 Fail");
	}timeout=1000;//ev8事件被检测到
	I2C_GenerateSTOP(I2C1,ENABLE);
}

void BH1740FVI_Mread(uint8_t *data1, uint8_t *data2)
{
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{	
		if((timeout--)==0)
			printf("EV5 Fail");
	}timeout=1000;//ev5事件被检测到
	I2C_Send7bitAddress(I2C1,I2C_SLAVE_ADDR,I2C_Direction_Receiver);
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{	
		if((timeout--)==0)
			printf("EV6 Fail");
	}timeout=1000;//ev6事件被检测到
	
	while(ERROR == I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))
	{	
		if((timeout--)==0)
			printf("EV7 Fail");
	}timeout=1000;//ev7事件被检测到
	*data1 = I2C_ReceiveData(I2C1);
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	*data2 = I2C_ReceiveData(I2C1);
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	I2C_AcknowledgeConfig(I2C1,ENABLE);
}


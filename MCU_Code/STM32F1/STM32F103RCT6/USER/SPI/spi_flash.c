#include "spi_flash.h"

//SPI FLASH初始化
void SPI_FLASH_Init(void)
{
	SPI_GPIO_Config();
	SPI_Mode_Config();
}

//SPI GPIO初始化
void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//使能SPI有关的时钟
	FLASH_SPI_APBxClockCmd(FLASH_SPI_CLK, ENABLE);
	FLASH_SPI_CS_APBxClockCmd(FLASH_SPI_CS_CLK, ENABLE);
	FLASH_SPI_SCK_APBxClockCmd(FLASH_SPI_SCK_CLK, ENABLE);
	FLASH_SPI_MISO_APBxClockCmd(FLASH_SPI_MISO_CLK, ENABLE);
	FLASH_SPI_MOSI_APBxClockCmd(FLASH_SPI_MOSI_CLK, ENABLE);
	//SCK MOSI MISO
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);
	//软件初始化CS
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStruct);
	
	FLASH_SPI_CS_HIGH;
}	

//初始化SPI工作模式
void SPI_Mode_Config(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	//SPI使用模式3
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CRCPolynomial = 0;//不使用CRC功能
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(FLASH_SPIx, &SPI_InitStruct);
	
	SPI_Cmd(FLASH_SPIx, ENABLE);
}

uint8_t SPI_FLASH_Send_Byte(uint8_t data)
{
	//检查并等待至TX缓冲区为空
	while(RESET == SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE));
	//程序执行到此处,TX缓冲区已空
	SPI_I2S_SendData(FLASH_SPIx, data);
	while(RESET == SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE));
	//程序执行到此处说明数据发送完毕,并接收到一个字节
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

uint8_t SPI_FLASH_Read_Byte(void)
{
	return SPI_FLASH_Send_Byte(DUMMY);
}

uint32_t SPI_Read_ID(void)
{
	uint32_t flash_id;
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(READ_GEDEC_ID);
	flash_id = SPI_FLASH_Read_Byte();
	flash_id <<= 8;
	flash_id |= SPI_FLASH_Read_Byte();
	flash_id <<= 8;
	flash_id |= SPI_FLASH_Read_Byte();
	FLASH_SPI_CS_HIGH;
	return flash_id;
}

//写入使能
void SPI_Write_Enable(void)
{
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(WRITE_ENABLE);
	FLASH_SPI_CS_HIGH;
}

//擦除flash指定扇区
void SPI_Erase_Sector(uint32_t addr)
{
	SPI_Write_Enable();
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(ERASE_SECTOR);
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	SPI_FLASH_Send_Byte(addr&0xff);
	FLASH_SPI_CS_HIGH;
	SPI_WaitForWriteEnd();
}

//等待flash内部时序操作完成
void SPI_WaitForWriteEnd(void)
{
	uint8_t status_reg = 0;
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(READ_STATUS);
	do
	{
		status_reg = SPI_FLASH_Send_Byte(DUMMY);
	}
	while(1 == (status_reg & 0x01));//忙碌
	FLASH_SPI_CS_HIGH;
}

//读取flash的内容
void SPI_Read_Data(uint32_t addr, uint8_t *reafbuff, uint32_t numByteToRead)
{
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(READ_DATA);
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	SPI_FLASH_Send_Byte(addr&0xff);
	while(numByteToRead--)
	{
		*reafbuff = SPI_FLASH_Send_Byte(DUMMY);
		reafbuff++;
	}
	FLASH_SPI_CS_HIGH;
}

//写入flash
void SPI_Write_Data(uint32_t addr, uint8_t *writebuff, uint32_t numByteToRead)
{
	SPI_Write_Enable();
	FLASH_SPI_CS_LOW;
	SPI_FLASH_Send_Byte(WRITE_DATA);
	SPI_FLASH_Send_Byte((addr>>16)&0xff);
	SPI_FLASH_Send_Byte((addr>>8)&0xff);
	SPI_FLASH_Send_Byte(addr&0xff);
	while(numByteToRead--)
	{
		SPI_FLASH_Send_Byte(*writebuff);
		writebuff++;
	}
	FLASH_SPI_CS_HIGH;
	SPI_WaitForWriteEnd();
}

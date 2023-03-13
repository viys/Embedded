#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include "stm32f10x.h"

/*SPI接口定义-开头****************************/
#define	FLASH_SPIx                      SPI1
#define	FLASH_SPI_APBxClockCmd          RCC_APB2PeriphClockCmd
#define	FLASH_SPI_CLK                   RCC_APB2Periph_SPI1

//CS(NSS)引脚 片选选普通GPIO即可
#define	FLASH_SPI_CS_APBxClockCmd       RCC_APB2PeriphClockCmd
#define	FLASH_SPI_CS_CLK                RCC_APB2Periph_GPIOA   
#define	FLASH_SPI_CS_PORT               GPIOA
#define	FLASH_SPI_CS_PIN                GPIO_Pin_4

//SCK引脚
#define	FLASH_SPI_SCK_APBxClockCmd      RCC_APB2PeriphClockCmd
#define	FLASH_SPI_SCK_CLK               RCC_APB2Periph_GPIOA   
#define	FLASH_SPI_SCK_PORT              GPIOA   
#define	FLASH_SPI_SCK_PIN               GPIO_Pin_5
//MISO引脚
#define	FLASH_SPI_MISO_APBxClockCmd     RCC_APB2PeriphClockCmd
#define	FLASH_SPI_MISO_CLK              RCC_APB2Periph_GPIOA    
#define	FLASH_SPI_MISO_PORT             GPIOA 
#define	FLASH_SPI_MISO_PIN              GPIO_Pin_6
//MOSI引脚
#define	FLASH_SPI_MOSI_APBxClockCmd     RCC_APB2PeriphClockCmd
#define	FLASH_SPI_MOSI_CLK              RCC_APB2Periph_GPIOA    
#define	FLASH_SPI_MOSI_PORT             GPIOA 
#define	FLASH_SPI_MOSI_PIN              GPIO_Pin_7

#define FLASH_SPI_CS_HIGH 				GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define FLASH_SPI_CS_LOW 				GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
//命令
#define DUMMY 			0x00
#define READ_GEDEC_ID	0x9f
#define ERASE_SECTOR	0x20
#define READ_STATUS		0x05
#define READ_DATA		0x03
#define WRITE_ENABLE	0x06
#define WRITE_DATA		0x02

void SPI_GPIO_Config(void);
void SPI_Mode_Config(void);
void SPI_FLASH_Init(void);
uint8_t SPI_FLASH_Send_Byte(uint8_t data);
uint8_t SPI_FLASH_Read_Byte(void);
uint32_t SPI_Read_ID(void);
void SPI_WaitForWriteEnd(void);
void SPI_Erase_Sector(uint32_t addr);
void SPI_WaitForWriteEnd(void);
void SPI_Read_Data(uint32_t addr, uint8_t *reafbuff, uint32_t numByteToRead);
void SPI_Write_Enable(void);
void SPI_Write_Data(uint32_t addr, uint8_t *writebuff, uint32_t numByteToRead);

#endif

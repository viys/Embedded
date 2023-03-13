#ifndef _I2C_H
#define _I2C_H

#include "stm32f10x.h"

#define STM32_I2C_OWN_ADDR			0x5f
#define EEPROM_ADDR					0xa0

#define EEPROM_I2C1					I2C1
#define EEPROM_I2C1_CLK				RCC_APB1Periph_I2C1
#define EEPROM_I2C1_APBxClkCmd		RCC_APB1PeriphClockCmd
#define EEPROM_I2C1_BAUDRATE 		400000
#define EEPROM_I2C1_SCL_GPIO_CLK	RCC_APB2Periph_GPIOB
#define EEPROM_I2C1_SDA_GPIO_CLK	RCC_APB2Periph_GPIOB
#define EEPROM_I2C1_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
#define EEPROM_I2C1_SCL_GPIO_PORT	GPIOB
#define EEPROM_I2C1_SCL_GPIO_PIN	GPIO_Pin_6
#define EEPROM_I2C1_SDA_GPIO_PORT	GPIOB
#define EEPROM_I2C1_SDA_GPIO_PIN	GPIO_Pin_7

void I2C_EE_Config(void);
void EEPROM_Byte_Write(uint8_t addr, uint8_t data);
void EEPROM_Read(uint8_t addr, uint8_t *data, uint8_t numByteToRead);

#endif

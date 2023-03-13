#ifndef __I2C_H
#define	__I2C_H

#include "stm32f10x.h"

#define I2Cx_OWN_ADDRESS7	0x30
#define EEPROM_ADDRESS		0x52
#define I2C_SPEED			400000

#define EEPROM_I2Cx				I2C1
#define EEPROM_I2C_CLK			RCC_APB1Periph_I2C1
#define EEPROM_I2C_GPIO_CLK		RCC_APB2Periph_GPIOB
#define EEPROM_SCL_PROT			GPIOB
#define EEPROM_SCL_PIN			GPIO_Pin_6
#define EEPROM_SDA_PORT			GPIOB
#define EEPROM_SDA_PIN			GPIO_Pin_7
#define EEPROM_I2C_GPIO_MODE	GPIO_Mode_AF_OD

void I2C_EE_Init(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);

#endif

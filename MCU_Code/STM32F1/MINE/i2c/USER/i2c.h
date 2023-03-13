#ifndef _I2C_H
#define _I2C_H

#include "stm32f10x.h"

#define I2C_MASTER_ADDR		0x5f
#define I2C_SLAVE_ADDR		0x46//1011100
#define BH1750_POWERON		0x01
#define BH1750_MEASUERMENT	0X10


#define I2C_1			I2C1
#define I2C1_CLK		RCC_APB1Periph_I2C1
#define I2C1_CLKCMD		RCC_APB1PeriphClockCmd
#define I2C1_BAUDRADE	400000

#define I2C1_GPIO_CLKCMD		RCC_APB2PeriphClockCmd

#define I2C1_SCL_GPIO_CLK		RCC_APB2Periph_GPIOB
#define I2C1_SCL_GPIO_PORT		GPIOB
#define I2C1_SCL_GPIO_PIN		GPIO_Pin_6

#define I2C1_SDA_GPIO_CLK		RCC_APB2Periph_GPIOB
#define I2C1_SDA_GPIO_PORT		GPIOB
#define I2C1_SDA_GPIO_PIN		GPIO_Pin_7

void I2C1_Init(void);
void BH1750FVI_Send(uint8_t opcode);
void BH1740FVI_Mread(uint8_t *data1, uint8_t *data2);
void I2Cx_Init(void);

#endif

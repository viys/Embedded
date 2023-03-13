#ifndef		__IIC_SIM_H__
#define		__IIC_SIM_H__

//--------------------- include files ---------------------
#include "gd32f1x0.h"
//--------------------- macro define ----------------------
#define		IO_SIM_SDA			PA1
#define		IO_SIM_SCL			PA0

#define		SIM_SDA_RCU			RCU_GPIOA
#define		SIM_SDA_PORT		GPIOA
#define		SIM_SDA_PIN			GPIO_PIN_1

#define		SIM_SCL_RCU			RCU_GPIOA
#define		SIM_SCL_PORT		GPIOA
#define		SIM_SCL_PIN			GPIO_PIN_0


#define	SIM_SDA_INIT() 	do{	\
							rcu_periph_clock_enable(SIM_SDA_RCU);			\
							gpio_mode_set(SIM_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SIM_SDA_PIN);	\
							gpio_output_options_set(SIM_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, SIM_SDA_PIN);	\
							gpio_bit_set(SIM_SDA_PORT,SIM_SDA_PIN);	\
						}while(0)

#define	SIM_SCL_INIT() 	do{	\
							rcu_periph_clock_enable(SIM_SCL_RCU);			\
							gpio_mode_set(SIM_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SIM_SCL_PIN);	\
							gpio_output_options_set(SIM_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, SIM_SCL_PIN);	\
							gpio_bit_set(SIM_SCL_PORT,SIM_SCL_PIN);		\
						}while(0)

#define	SIM_SDA_OUT() 	do{	\
							gpio_mode_set(SIM_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SIM_SDA_PIN);	\
							gpio_output_options_set(SIM_SDA_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SIM_SDA_PIN);	\
						}while(0)

#define	SIM_SCL_OUT() 	do{	\
							gpio_mode_set(SIM_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SIM_SCL_PIN);	\
							gpio_output_options_set(SIM_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SIM_SCL_PIN);	\
						}while(0)

#define	SIM_SDA_IN() 	do{	\
							gpio_mode_set(SIM_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SIM_SDA_PIN);	\
						}while(0)

#define	SIM_SCL_IN() 	do{	\
							gpio_mode_set(SIM_SCL_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, SIM_SCL_PIN);	\
						}while(0)

#define	SIM_READ_SDA()	gpio_input_bit_get(SIM_SDA_PORT,SIM_SDA_PIN)
						
#define	SIM_SDA_HIGH()	gpio_bit_set(SIM_SDA_PORT,SIM_SDA_PIN)
#define	SIM_SDA_LOW()	gpio_bit_reset(SIM_SDA_PORT,SIM_SDA_PIN)
#define	SIM_SCL_HIGH()	gpio_bit_set(SIM_SCL_PORT,SIM_SCL_PIN)
#define	SIM_SCL_LOW()	gpio_bit_reset(SIM_SCL_PORT,SIM_SCL_PIN)
//--------------------- macro define ----------------------
#define	I2C_F_RD				0x01
#define	I2C_F_WR				(~(I2C_F_RD))
//--------------------- variable declare ----------------------
//--------------------- function declare ----------------------
void SIM_I2C_Init(void);
int SIM_ii2_master_write(unsigned char slave_addr, unsigned char *buff, int length);
int SIM_ii2_master_read(unsigned char slave_addr, unsigned char *buff, int length);


#endif	//__IIC_SIM_H__


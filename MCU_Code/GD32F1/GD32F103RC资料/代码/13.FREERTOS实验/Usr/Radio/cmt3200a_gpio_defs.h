#ifndef __CMT3200A_GPIO_DEFS_H__
#define __CMT3200A_GPIO_DEFS_H__

//-------------------------------- include files ---------------------------------
#include "gd32e10x.h"
//-------------------------------- macro define ----------------------------------
#define	IO_CMT_SPI_CSB			PB13
#define	IO_CMT_SPI_SCLK			PA8
#define	IO_CMT_SPI_SDIO			PB14

#define	IO_CMT_SPI_FSB			PB12

#define	IO_CMT_GPIO_PIN1		PB15
#define	IO_CMT_GPIO_PIN2		PB15
#define	IO_CMT_GPIO_PIN3		PB15

// CSB
#define	CMT_SPI_CSB_RCU			RCU_GPIOB
#define	CMT_SPI_CSB_PORT		GPIOB
#define	CMT_SPI_CSB_PIN			GPIO_PIN_13

// SCLK
#define	CMT_SPI_SCLK_RCU		RCU_GPIOA
#define	CMT_SPI_SCLK_PORT		GPIOA
#define	CMT_SPI_SCLK_PIN		GPIO_PIN_8

// SDIO
#define	CMT_SPI_SDIO_RCU		RCU_GPIOB
#define	CMT_SPI_SDIO_PORT		GPIOB
#define	CMT_SPI_SDIO_PIN		GPIO_PIN_14

// FSB
#define	CMT_SPI_FSB_RCU			RCU_GPIOB
#define	CMT_SPI_FSB_PORT		GPIOB
#define	CMT_SPI_FSB_PIN			GPIO_PIN_12

// GPIO1
#define	CMT_SPI_GPIO1_RCU			RCU_GPIOB
#define	CMT_SPI_GPIO1_PORT			GPIOB
#define	CMT_SPI_GPIO1_PIN			GPIO_PIN_15

// GPIO2
#define	CMT_SPI_GPIO2_RCU			RCU_GPIOB
#define	CMT_SPI_GPIO2_PORT			GPIOB
#define	CMT_SPI_GPIO2_PIN			GPIO_PIN_15

// GPIO3
#define	CMT_SPI_GPIO3_RCU			RCU_GPIOB
#define	CMT_SPI_GPIO3_PORT			GPIOB
#define	CMT_SPI_GPIO3_PIN			GPIO_PIN_15

// CSB
#define	CMT_SPI_CSB_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_CSB_RCU);			\
							gpio_init(CMT_SPI_CSB_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_CSB_PIN);	\
							gpio_bit_set(CMT_SPI_CSB_PORT,IO_LED1_PIN);		\
						}while(0)

#define	CMT_SPI_CSB_OUT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_CSB_RCU);			\
							gpio_init(CMT_SPI_CSB_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_CSB_PIN);	\
						}while(0)

#define	CMT_SPI_CSB_HIGH()			gpio_bit_set(CMT_SPI_CSB_PORT,CMT_SPI_CSB_PIN)
#define	CMT_SPI_CSB_LOW()			gpio_bit_reset(CMT_SPI_CSB_PORT,CMT_SPI_CSB_PIN)
						
// SCLK
#define	CMT_SPI_SCLK_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_SCLK_RCU);			\
							gpio_init(CMT_SPI_SCLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_SCLK_PIN);	\
							gpio_bit_reset(CMT_SPI_SCLK_PORT,CMT_SPI_SCLK_PIN);		\
						}while(0)

#define	CMT_SPI_SCLK_OUT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_SCLK_RCU);			\
							gpio_init(CMT_SPI_SCLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_SCLK_PIN);	\
						}while(0)

#define	CMT_SPI_SCLK_HIGH()			gpio_bit_set(CMT_SPI_SCLK_PORT,CMT_SPI_SCLK_PIN)
#define	CMT_SPI_SCLK_LOW()			gpio_bit_reset(CMT_SPI_SCLK_PORT,CMT_SPI_SCLK_PIN)

// SDIO
#define	CMT_SPI_SDIO_INIT() 	do{	\
									rcu_periph_clock_enable(CMT_SPI_SDIO_RCU);			\
									gpio_init(CMT_SPI_SDIO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_SDIO_PIN);	\
								}while(0)

#define	CMT_SPI_SDIO_IN() 	do{	\
								rcu_periph_clock_enable(CMT_SPI_SDIO_RCU);			\
								gpio_init(CMT_SPI_SDIO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_SDIO_PIN);	\
							}while(0)
							
							
#define	CMT_SPI_SDIO_OUT() 	do{	\
								rcu_periph_clock_enable(CMT_SPI_SDIO_RCU);			\
								gpio_init(CMT_SPI_SDIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_SDIO_PIN);	\
								gpio_bit_reset(CMT_SPI_SDIO_PORT,CMT_SPI_SDIO_PIN);		\
							}while(0)

#define	CMT_SPI_SDIO_HIGH()			gpio_bit_set(CMT_SPI_SDIO_PORT,CMT_SPI_SDIO_PIN)
#define	CMT_SPI_SDIO_LOW()			gpio_bit_reset(CMT_SPI_SDIO_PORT,CMT_SPI_SDIO_PIN)
#define	CMT_SPI_SDIO_READ()			gpio_input_bit_get(CMT_SPI_SDIO_PORT,CMT_SPI_SDIO_PIN)
						

// FSB
#define	CMT_SPI_FSB_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_FSB_RCU);			\
							gpio_init(CMT_SPI_FSB_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_FSB_PIN);	\
							gpio_bit_set(CMT_SPI_FSB_PORT,CMT_SPI_FSB_PIN);		\
						}while(0)

#define	CMT_SPI_FSB_OUT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_FSB_RCU);			\
							gpio_init(CMT_SPI_FSB_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, CMT_SPI_FSB_PIN);	\
						}while(0)
						
#define	CMT_SPI_FSB_HIGH()			gpio_bit_set(CMT_SPI_FSB_PORT,CMT_SPI_FSB_PIN)
#define	CMT_SPI_FSB_LOW()			gpio_bit_reset(CMT_SPI_FSB_PORT,CMT_SPI_FSB_PIN)
	
						
// GPIO1
#define	CMT_SPI_GPIO1_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO1_RCU);			\
							gpio_init(CMT_SPI_GPIO1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO1_PIN);	\
							gpio_bit_reset(CMT_SPI_GPIO1_PORT,CMT_SPI_GPIO1_PIN);		\
						}while(0)

#define	CMT_SPI_GPIO1_IN() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO1_RCU);			\
							gpio_init(CMT_SPI_GPIO1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO1_PIN);	\
						}while(0)

#define	CMT_SPI_GPIO1_HIGH()		gpio_bit_set(CMT_SPI_GPIO1_PORT,CMT_SPI_GPIO1_PIN)
#define	CMT_SPI_GPIO1_LOW()			gpio_bit_reset(CMT_SPI_GPIO1_PORT,CMT_SPI_GPIO1_PIN)
#define	CMT_SPI_GPIO1_READ()		gpio_input_bit_get(CMT_SPI_GPIO1_PORT,CMT_SPI_GPIO1_PIN)

						
// GPIO2
#define	CMT_SPI_GPIO2_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO2_RCU);			\
							gpio_init(CMT_SPI_GPIO2_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO2_PIN);	\
							gpio_bit_reset(CMT_SPI_GPIO2_PORT,CMT_SPI_GPIO2_PIN);		\
						}while(0)

#define	CMT_SPI_GPIO2_IN() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO2_RCU);			\
							gpio_init(CMT_SPI_GPIO2_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO2_PIN);	\
						}while(0)

#define	CMT_SPI_GPIO2_HIGH()		gpio_bit_set(CMT_SPI_GPIO2_PORT,CMT_SPI_GPIO2_PIN)
#define	CMT_SPI_GPIO2_LOW()			gpio_bit_reset(CMT_SPI_GPIO2_PORT,CMT_SPI_GPIO2_PIN)
#define	CMT_SPI_GPIO2_READ()		gpio_input_bit_get(CMT_SPI_GPIO2_PORT,CMT_SPI_GPIO2_PIN)
						
						
// GPIO3
#define	CMT_SPI_GPIO3_INIT() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO3_RCU);			\
							gpio_init(CMT_SPI_GPIO3_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO3_PIN);	\
							gpio_bit_reset(CMT_SPI_GPIO3_PORT,CMT_SPI_GPIO3_PIN);		\
						}while(0)

#define	CMT_SPI_GPIO3_IN() 	do{	\
							rcu_periph_clock_enable(CMT_SPI_GPIO3_RCU);			\
							gpio_init(CMT_SPI_GPIO3_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, CMT_SPI_GPIO3_PIN);	\
						}while(0)

#define	CMT_SPI_GPIO3_HIGH()		gpio_bit_set(CMT_SPI_GPIO3_PORT,CMT_SPI_GPIO3_PIN)
#define	CMT_SPI_GPIO3_LOW()			gpio_bit_reset(CMT_SPI_GPIO3_PORT,CMT_SPI_GPIO3_PIN)
#define	CMT_SPI_GPIO3_READ()		gpio_input_bit_get(CMT_SPI_GPIO3_PORT,CMT_SPI_GPIO3_PIN)
//-------------------------------- macro define ----------------------------------						
#define	CMT_CSB_GPIO_OUT()					CMT_SPI_CSB_OUT()				
#define	CMT_FCSB_GPIO_OUT()					CMT_SPI_FSB_OUT()
#define	CMT_SCLK_GPIO_OUT()					CMT_SPI_SCLK_OUT()
#define	CMT_SDIO_GPIO_OUT()					CMT_SPI_SDIO_OUT()
#define	CMT_SDIO_GPIO_IN()					CMT_SPI_SDIO_IN()
#define	CMT_GPIO1_GPIO_IN()					CMT_SPI_GPIO1_IN()
#define	CMT_GPIO2_GPIO_IN()					CMT_SPI_GPIO2_IN()
#define	CMT_GPIO3_GPIO_IN()					CMT_SPI_GPIO3_IN()
						
						
						
						
#define	CMT_CSB_GPIO_HIGH()					CMT_SPI_CSB_HIGH()
#define	CMT_CSB_GPIO_LOW()					CMT_SPI_CSB_LOW()
#define	CMT_FCSB_GPIO_HIGH()				CMT_SPI_FSB_HIGH()
#define	CMT_FCSB_GPIO_LOW()					CMT_SPI_FSB_LOW()
#define	CMT_SCLK_GPIO_HIGH()				CMT_SPI_SCLK_HIGH()
#define	CMT_SCLK_GPIO_LOW()					CMT_SPI_SCLK_LOW()
#define	CMT_SDIO_GPIO_HIGH()				CMT_SPI_SDIO_HIGH()
#define	CMT_SDIO_GPIO_LOW()					CMT_SPI_SDIO_LOW()

#define	CMT_SDIO_GPIO_READ()				CMT_SPI_SDIO_READ()
#define	CMT_GPIO1_GPIO_READ()				CMT_SPI_GPIO1_READ()
#define	CMT_GPIO2_GPIO_READ()				CMT_SPI_GPIO2_READ()
#define	CMT_GPIO3_GPIO_READ()				CMT_SPI_GPIO3_READ()
//-------------------------------- macro define ----------------------------------
						
						
						
						

#endif	// __CMT3200A_GPIO_DEFS_H__


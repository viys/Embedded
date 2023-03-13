#ifndef		__LCD_PORT_H__
#define		__LCD_PORT_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_OLED_SDA		PB8					// 数据口
#define		IO_OLED_SCL		PB5					// 时钟口
#define		IO_OLED_RST		PB4					// 复位口

#define		IO_OLED_CS		PA15				// 片选
#define		IO_OLED_DC		PB3					// 命令/数据
//---------------------------------------------------------------------------------------------------			
#define		OLED_SDA_RCU			RCU_GPIOB
#define		OLED_SDA_PORT			GPIOB
#define		OLED_SDA_PIN			GPIO_PIN_8

#define		OLED_SCL_RCU			RCU_GPIOB
#define		OLED_SCL_PORT			GPIOB
#define		OLED_SCL_PIN			GPIO_PIN_9

#define		OLED_RST_RCU			RCU_GPIOB
#define		OLED_RST_PORT			GPIOB
#define		OLED_RST_PIN			GPIO_PIN_4

#define		OLED_CS_RCU				RCU_GPIOA
#define		OLED_CS_PORT			GPIOA
#define		OLED_CS_PIN				GPIO_PIN_15

#define		OLED_DC_RCU				RCU_GPIOB
#define		OLED_DC_PORT			GPIOB
#define		OLED_DC_PIN				GPIO_PIN_3
//---------------------------------------------------------------------------------------------------			
#define		IO_OLED_SDA_INIT()	\
			do{	\
				rcu_periph_clock_enable(OLED_SDA_RCU);	\
				gpio_init(OLED_SDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_SDA_PIN);	\
				gpio_bit_set(OLED_SDA_PORT,OLED_SDA_PIN);		\
			}while(0)

#define		IO_OLED_SCL_INIT()	\
			do{	\
				rcu_periph_clock_enable(OLED_SCL_RCU);	\
				gpio_init(OLED_SCL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_SCL_PIN);	\
				gpio_bit_set(OLED_SCL_PORT,OLED_SCL_PIN);	\
			}while(0)

#define		IO_OLED_RST_INIT()	\
			do{	\
				rcu_periph_clock_enable(OLED_RST_RCU);	\
				gpio_init(OLED_RST_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_RST_PIN);	\
				gpio_bit_reset(OLED_RST_PORT,OLED_RST_PIN);		\
			}while(0)
			
#define		IO_OLED_CS_INIT()	\
			do{ \
				rcu_periph_clock_enable(OLED_CS_RCU);	\
				gpio_init(OLED_CS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_CS_PIN);	\
				gpio_bit_set(OLED_CS_PORT,OLED_CS_PIN);		\
			}while(0)

#define		IO_OLED_DC_INIT()	\
			do{ \
				rcu_periph_clock_enable(OLED_DC_RCU);	\
				gpio_init(OLED_DC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_DC_PIN);	\
				gpio_bit_set(OLED_DC_PORT,OLED_DC_PIN); 	\
			}while(0)
//---------------------------------------------------------------------------------------------------			
#define		IO_OLED_SDA_OUT()	\
			do{	\
				rcu_periph_clock_enable(OLED_SDA_RCU);	\
				gpio_init(OLED_SDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_SDA_PIN);	\
			}while(0)

#define		IO_OLED_SCL_OUT()	\
			do{	\
				rcu_periph_clock_enable(OLED_SCL_RCU);	\
				gpio_init(OLED_SCL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_SCL_PIN);	\
			}while(0)

#define		IO_OLED_RST_OUT()	\
			do{	\
				rcu_periph_clock_enable(OLED_RST_RCU);	\
				gpio_init(OLED_RST_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_RST_PIN);	\
			}while(0)
			
#define		IO_OLED_SDA_IN()	\
			do{	\
				rcu_periph_clock_enable(OLED_SDA_RCU);	\
				gpio_init(OLED_SDA_PORT, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, OLED_SDA_PIN);	\
			}while(0)

#define		IO_OLED_CS_OUT()	\
			do{ \
				rcu_periph_clock_enable(OLED_CS_RCU);	\
				gpio_init(OLED_CS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_CS_PIN);	\
			}while(0)
			
#define		IO_OLED_DC_OUT()	\
			do{ \
				rcu_periph_clock_enable(OLED_DC_RCU);	\
				gpio_init(OLED_DC_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, OLED_DC_PIN);	\
			}while(0)
//---------------------------------------------------------------------------------------------------			
#define		OLED_SDA_LOW()		gpio_bit_reset(OLED_SDA_PORT,OLED_SDA_PIN)
#define		OLED_SDA_HIGH()		gpio_bit_set(OLED_SDA_PORT,OLED_SDA_PIN)
			
#define		OLED_SCL_LOW()		gpio_bit_reset(OLED_SCL_PORT,OLED_SCL_PIN)
#define		OLED_SCL_HIGH()		gpio_bit_set(OLED_SCL_PORT,OLED_SCL_PIN)
			
#define		OLED_RST_LOW()		gpio_bit_reset(OLED_RST_PORT,OLED_RST_PIN)
#define		OLED_RST_HIGH()		gpio_bit_set(OLED_RST_PORT,OLED_RST_PIN)

#define		OLED_CS_LOW()		gpio_bit_reset(OLED_CS_PORT,OLED_CS_PIN)
#define		OLED_CS_HIGH()		gpio_bit_set(OLED_CS_PORT,OLED_CS_PIN)

#define		OLED_DC_LOW()		gpio_bit_reset(OLED_DC_PORT,OLED_DC_PIN)
#define		OLED_DC_HIGH()		gpio_bit_set(OLED_DC_PORT,OLED_DC_PIN)
//---------------------------------------------------------------		
#define		OLED_RES_Clr()		OLED_RST_LOW()
#define		OLED_RES_Set()		OLED_RST_HIGH()
			
#define		OLED_SDA_Clr()		OLED_SDA_LOW()
#define		OLED_SDA_Set()		OLED_SDA_HIGH()
			
#define		OLED_SCL_Clr()		OLED_SCL_LOW()
#define		OLED_SCL_Set()		OLED_SCL_HIGH()

#define		OLED_DC_Set()		OLED_DC_HIGH()
#define		OLED_DC_Clr()		OLED_DC_LOW()

#define		OLED_CS_Set()		OLED_CS_HIGH()
#define		OLED_CS_Clr()		OLED_CS_LOW()
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void oled_port_init(void);
void oled_ch1115_spi_init(void);





#endif	//__LCD_PORT_H__


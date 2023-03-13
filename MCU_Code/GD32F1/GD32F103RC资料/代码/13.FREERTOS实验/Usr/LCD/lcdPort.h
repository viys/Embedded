#ifndef		__LCD_PORT_H__
#define		__LCD_PORT_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_LCD_SPI_NSS		PB12
#define		IO_LCD_SPI_SCK		PB13
#define		IO_LCD_SPI_MISO		PB14
#define		IO_LCD_SPI_MOSI		PB15

#define		IO_LCD_RESET			PA8
//---------------------------------------------------------------------------------------------------			
#define		LCD_PORT_SPI			SPI1
#define		LCD_PORT_SPI_RCU		RCU_SPI1
//---------------------------------------------------------------------------------------------------			
#define		LCD_SPI_NSS_RCU			RCU_GPIOB
#define		LCD_SPI_NSS_PORT		GPIOB
#define		LCD_SPI_NSS_PIN			GPIO_PIN_12

#define		LCD_SPI_SCK_RCU			RCU_GPIOB
#define		LCD_SPI_SCK_PORT		GPIOB
#define		LCD_SPI_SCK_PIN			GPIO_PIN_13

#define		LCD_SPI_MISO_RCU		RCU_GPIOB
#define		LCD_SPI_MISO_PORT		GPIOB
#define		LCD_SPI_MISO_PIN		GPIO_PIN_14

#define		LCD_SPI_MOSI_RCU		RCU_GPIOB
#define		LCD_SPI_MOSI_PORT		GPIOB
#define		LCD_SPI_MOSI_PIN		GPIO_PIN_15

#define		LCD_RESET_RCU			RCU_GPIOA
#define		LCD_RESET_PORT			GPIOA
#define		LCD_RESET_PIN			GPIO_PIN_8

#define		LCD_BACKLIGHT_RCU			RCU_GPIOA
#define		LCD_BACKLIGHT_PORT			GPIOA
#define		LCD_BACKLIGHT_PIN			GPIO_PIN_2
//---------------------------------------------------------------------------------------------------			
#define		HD_LCD_SPI_NSS_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_NSS_RCU);	\
				gpio_init(LCD_SPI_NSS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_NSS_PIN);	\
				gpio_bit_set(LCD_SPI_NSS_PORT,LCD_SPI_NSS_PIN);		\
			}while(0)

#define		HD_LCD_SPI_SCK_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_SCK_RCU);	\
				gpio_init(LCD_SPI_SCK_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SPI_SCK_PIN);	\
				gpio_bit_reset(LCD_SPI_SCK_PORT,LCD_SPI_SCK_PIN);	\
			}while(0)

#define		HD_LCD_SPI_SDA_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_MOSI_RCU);	\
				gpio_init(LCD_SPI_MOSI_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MOSI_PIN);	\
				gpio_bit_set(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN);	\
			}while(0)

#define		HD_LCD_SPI_DCS_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_MISO_RCU);	\
				gpio_init(LCD_SPI_MISO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MISO_PIN);	\
				gpio_bit_reset(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN);		\
			}while(0)
//---------------------------------------------------------------------------------------------------			
#define		IO_LCD_SPI_NSS_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_NSS_RCU);	\
				gpio_init(LCD_SPI_NSS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_NSS_PIN);	\
				gpio_bit_set(LCD_SPI_NSS_PORT,LCD_SPI_NSS_PIN);		\
			}while(0)

#define		IO_LCD_SPI_SCK_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_SCK_RCU);	\
				gpio_init(LCD_SPI_SCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_SCK_PIN);	\
				gpio_bit_set(LCD_SPI_SCK_PORT,LCD_SPI_SCK_PIN);	\
			}while(0)

#define		IO_LCD_SPI_MISO_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_MISO_RCU);	\
				gpio_init(LCD_SPI_MISO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MISO_PIN);	\
				gpio_bit_set(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN);		\
			}while(0)

#define		IO_LCD_SPI_MOSI_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_SPI_MOSI_RCU);	\
				gpio_init(LCD_SPI_MOSI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MOSI_PIN);	\
				gpio_bit_set(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN);	\
			}while(0)

#define		IO_LCD_RESET_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_RESET_RCU);	\
				gpio_init(LCD_RESET_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_RESET_PIN);	\
				gpio_bit_set(LCD_RESET_PORT,LCD_RESET_PIN);	\
			}while(0)
			
#define		IO_LCD_BACKLIGHT_INIT()	\
			do{	\
				rcu_periph_clock_enable(LCD_BACKLIGHT_RCU);	\
				gpio_init(LCD_BACKLIGHT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_BACKLIGHT_PIN);	\
				gpio_bit_reset(LCD_BACKLIGHT_PORT,LCD_BACKLIGHT_PIN);	\
			}while(0)
//---------------------------------------------------------------------------------------------------			
#define		LCD_SPI_NSS_LOW()		gpio_bit_reset(LCD_SPI_NSS_PORT,LCD_SPI_NSS_PIN)
#define		LCD_SPI_NSS_HIGH()		gpio_bit_set(LCD_SPI_NSS_PORT,LCD_SPI_NSS_PIN)
			
#define		LCD_SPI_SCK_LOW()		gpio_bit_reset(LCD_SPI_SCK_PORT,LCD_SPI_SCK_PIN)
#define		LCD_SPI_SCK_HIGH()		gpio_bit_set(LCD_SPI_SCK_PORT,LCD_SPI_SCK_PIN)
			
#define		LCD_SPI_MISO_LOW()		gpio_bit_reset(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN)
#define		LCD_SPI_MISO_HIGH()		gpio_bit_set(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN)

#define		LCD_SPI_DCS_LOW()		gpio_bit_reset(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN)
#define		LCD_SPI_DCS_HIGH()		gpio_bit_set(LCD_SPI_MISO_PORT,LCD_SPI_MISO_PIN)
			
#define		LCD_SPI_MOSI_LOW()		gpio_bit_reset(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN)
#define		LCD_SPI_MOSI_HIGH()		gpio_bit_set(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN)

#define		LCD_SPI_SDA_LOW()		gpio_bit_reset(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN)
#define		LCD_SPI_SDA_HIGH()		gpio_bit_set(LCD_SPI_MOSI_PORT,LCD_SPI_MOSI_PIN)

#define		LCD_RESET_LOW()			gpio_bit_reset(LCD_RESET_PORT,LCD_RESET_PIN)
#define		LCD_RESET_HIGH()		gpio_bit_set(LCD_RESET_PORT,LCD_RESET_PIN)

#define		LCD_BACKLIGHT_LOW()			gpio_bit_reset(LCD_BACKLIGHT_PORT,LCD_BACKLIGHT_PIN)
#define		LCD_BACKLIGHT_HIGH()		gpio_bit_set(LCD_BACKLIGHT_PORT,LCD_BACKLIGHT_PIN)
//---------------------------------------------------------------
#define		LCD_SDA_SET		LCD_SPI_MOSI_HIGH()
#define		LCD_SDA_CLR		LCD_SPI_MOSI_LOW()

#define		LCD_SCL_CLR		LCD_SPI_SCK_LOW()
#define		LCD_SCL_SET		LCD_SPI_SCK_HIGH()

#define		LCD_CS_CLR		LCD_SPI_NSS_LOW()
#define		LCD_CS_SET		LCD_SPI_NSS_HIGH()

#define		LCD_RS_CLR		LCD_SPI_MISO_LOW()
#define		LCD_RS_SET		LCD_SPI_MISO_HIGH()

#define		LCD_RST_CLR		LCD_RESET_LOW()
#define		LCD_RST_SET		LCD_RESET_HIGH()

//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void lcd_spi_init(void);
void spi_hd_init(void);

uint8_t spi_lcd_send_byte(uint8_t byte);
uint8_t spi_flash_read_byte(void);
void lcd_spi_enb(unsigned char Enb);

void lcd_reset_pin_init(void);
void lcd_reset_enb(unsigned char Enb);

void lcd_backlight_init(void);
void lcd_backlight_control(unsigned char Enb);



#endif	//__LCD_PORT_H__


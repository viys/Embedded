/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "lcdPort.h"
//--------------------- variable declare ----------------------
#define WRITE            0x02     /* write to memory instruction */
#define WRSR             0x01     /* write status register instruction */
#define WREN             0x06     /* write enable instruction */

#define READ             0x03     /* read from memory instruction */
#define RDSR             0x05     /* read status register instruction  */
#define RDID             0x9F     /* read identification */
#define SE               0x20     /* sector erase instruction */
#define BE               0xC7     /* bulk erase instruction */

#define WIP_FLAG         0x01     /* write in progress(wip)flag */
#define DUMMY_BYTE       0xA5

/****************************************************************************************
Function:
Description:
				LCD SPI = SPI1
****************************************************************************************/
void spi_hd_init(void)
{
    spi_parameter_struct spi_init_struct;
	
	HD_LCD_SPI_NSS_INIT();														// SPI port initialize
	HD_LCD_SPI_SCK_INIT();
	HD_LCD_SPI_SDA_INIT();
	HD_LCD_SPI_DCS_INIT();

	rcu_periph_clock_enable(LCD_PORT_SPI_RCU);									// SPI clock enable

    /* chip select invalid*/
    LCD_SPI_NSS_HIGH();

    /* LCD SPI parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;			// bidirectional transmit data,do not use MISO
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_HARD;
    spi_init_struct.prescale             = SPI_PSC_8 ;							// SPI clock set
    spi_init_struct.endian               = SPI_ENDIAN_MSB;						// most significant bit send first
    spi_init(LCD_PORT_SPI, &spi_init_struct);

	
	spi_bidirectional_transfer_config(LCD_PORT_SPI,SPI_BIDIRECTIONAL_TRANSMIT);	// transmit-only mode 
	
	spi_nss_output_enable(LCD_PORT_SPI);										// NSS mode as hardware output mode
	
    /* set crc polynomial */
    //spi_crc_polynomial_set(LCD_PORT_SPI,7);
	
    /* enable LCD SPI */
    spi_enable(LCD_PORT_SPI);
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_reset_pin_init(void)
{
	IO_LCD_RESET_INIT();
}

/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_reset_enb(unsigned char Enb)
{
	if(Enb)		{ LCD_RESET_LOW(); }
	else		{ LCD_RESET_HIGH(); }
}

/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_backlight_init(void)
{
	IO_LCD_BACKLIGHT_INIT();
}

/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_backlight_control(unsigned char Enb)
{
	if(Enb)		{ LCD_BACKLIGHT_HIGH(); }
	else		{ LCD_BACKLIGHT_LOW(); }
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_spi_init(void)
{
	IO_LCD_SPI_NSS_INIT();
	IO_LCD_SPI_SCK_INIT();
	IO_LCD_SPI_MISO_INIT();
	IO_LCD_SPI_MOSI_INIT();
}

/*************************************************************
Function: 
Description: 
*************************************************************/
uint8_t spi_lcd_send_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(LCD_PORT_SPI,SPI_FLAG_TBE));

    /* send byte through the SPI0 peripheral */
    spi_i2s_data_transmit(LCD_PORT_SPI,byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(LCD_PORT_SPI,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(LCD_PORT_SPI));
}

/*************************************************************
Function: 
Description: 
*************************************************************/
uint8_t spi_flash_read_byte(void)
{
    return(spi_lcd_send_byte(DUMMY_BYTE));
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void lcd_spi_enb(unsigned char Enb)
{
	if(Enb)		{ LCD_SPI_NSS_LOW(); }
	else		{ LCD_SPI_NSS_HIGH(); }
}


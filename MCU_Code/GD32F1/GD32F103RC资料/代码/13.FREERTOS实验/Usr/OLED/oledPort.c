/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "oledPort.h"
//--------------------- variable declare ----------------------


/****************************************************************************************
Function:
Description:
	OLED通信口初始化
****************************************************************************************/
void oled_port_init(void)
{
	//IO_OLED_SDA_IN();
	IO_OLED_SDA_INIT();
	IO_OLED_SCL_INIT();
	IO_OLED_RST_INIT();
	
	
	OLED_SCL_HIGH();
	OLED_SDA_HIGH();
}

/****************************************************************************************
Function:
Description:
	OLED通信口初始化
****************************************************************************************/
void oled_ch1115_spi_init(void)
{
	IO_OLED_CS_INIT();
	IO_OLED_DC_INIT();
	IO_OLED_SDA_INIT();
	IO_OLED_SCL_INIT();
	IO_OLED_RST_INIT();

	OLED_RST_LOW();
	OLED_CS_HIGH();
	OLED_DC_HIGH();
	OLED_SCL_HIGH();
	OLED_SDA_HIGH();
}




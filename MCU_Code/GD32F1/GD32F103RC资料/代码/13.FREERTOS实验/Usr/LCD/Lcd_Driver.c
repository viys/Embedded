/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "lcdPort.h"
#include "FreeRTOS.h"
#include "task.h"

#include "LCD_Config.h"
//--------------------- variable declare ----------------------
#ifndef u8
typedef	unsigned char u8;
typedef	unsigned short int u16;
typedef	unsigned long int u32;
#endif 
//--------------------- variable declare ----------------------
#ifndef		delay_1ms
#define		delay_1ms(x)		vTaskDelay(x / portTICK_PERIOD_MS);
#endif


//液晶IO初始化配置
void LCD_GPIO_Init(void)
{
	lcd_reset_pin_init();
	lcd_reset_enb(0);
	
	lcd_backlight_init();
	lcd_backlight_control(0);
	
	//lcd_spi_init();						// simulate SPI
	spi_hd_init();							// hardware SPI
}

/****************************************************************************************
Function:
Description:
//向SPI总线传输一个8位数据
****************************************************************************************/
void  SPI_SIM_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
void  SPI_HD_WriteData(u8 Data)
{
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(LCD_PORT_SPI,SPI_FLAG_TBE));

    /* send byte through the LCD_PORT_SPI peripheral */
    spi_i2s_data_transmit(LCD_PORT_SPI,Data);
	
    /* loop while data register in not emplty */		
    while (SET == spi_i2s_flag_get(LCD_PORT_SPI,SPI_FLAG_TRANS));		// sending or receiving
}


/****************************************************************************************
Function:
Description:
****************************************************************************************/
//向液晶屏写一个8位指令
void Lcd_WriteIndex(u8 Index)
{
   //SPI 写命令时序开始
	LCD_CS_CLR;
	LCD_RS_CLR;

	//SPI_SIM_WriteData(Index);
	SPI_HD_WriteData(Index);

	LCD_CS_SET;
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
//向液晶屏写一个8位数据
void Lcd_WriteData(u8 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;

	//SPI_SIM_WriteData(Data);
	SPI_HD_WriteData(Data);

	LCD_CS_SET; 
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(u16 Data)
{
	LCD_CS_CLR;
	LCD_RS_SET;

	//SPI_SIM_WriteData(Data>>8); 		//写入高8位数据
	SPI_HD_WriteData(Data>>8);

	//SPI_SIM_WriteData(Data); 			//写入低8位数据
	SPI_HD_WriteData(Data);

	LCD_CS_SET; 
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
void Lcd_WriteReg(u8 Index,u8 Data)
{
	Lcd_WriteIndex(Index);
	Lcd_WriteData(Data);
}




/****************************************************************************************
Function:
Description:
****************************************************************************************/
void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_1ms(100);
	LCD_RST_SET;
	delay_1ms(50);
}

//LCD Init For 1.44Inch LCD Panel with ST7735R.
void Lcd_Init(void)
{	
	LCD_GPIO_Init();
	
	Lcd_Reset(); //Reset before LCD Init.

	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	Lcd_WriteIndex(0x11);//Sleep exit 
	delay_1ms(120);
		
	//ST7735R Frame Rate
	Lcd_WriteIndex(0xB1); 					// frame rate control 1,set the frame frequency of the normal mode
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB2); 					// frame rate control 2,set the frame frequency of the idle mode 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB3); 					// frame rate control 3,set the frame frequency of partial mode
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	
	Lcd_WriteIndex(0xB4); //Column inversion set(default:0x07)
	Lcd_WriteData(0x07); 
	
	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0); 					// PWCTR1,power control setting
	Lcd_WriteData(0xA2); 					// AVDD=5,GVDD=4.6,MODE=AUTO,GVCL=-4.6
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x84); 
	Lcd_WriteIndex(0xC1); 
	Lcd_WriteData(0xC5); 

	Lcd_WriteIndex(0xC2); 					// PWCTR3,current set
	Lcd_WriteData(0x0A); 					// 
	Lcd_WriteData(0x00); 					// 

	Lcd_WriteIndex(0xC3); 					// PWCTR4,current set
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0x2A); 
	
	Lcd_WriteIndex(0xC4);  					// PWCTR5,current set
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0xEE); 
	
	Lcd_WriteIndex(0xC5); //VMCRT1,VCOM voltage set
	Lcd_WriteData(0x0E); 
	
	Lcd_WriteIndex(0x36); //MX, MY, RGB mode (MADCTL,memory data address control)
	Lcd_WriteData(0xC8);	// lcd fresh direction set,RGB data sequence=RGB.
	
	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0); 	// gamma adjustment 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1a); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x18); 
	Lcd_WriteData(0x2f); 
	Lcd_WriteData(0x28); 
	Lcd_WriteData(0x20); 
	Lcd_WriteData(0x22); 
	Lcd_WriteData(0x1f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x23); 
	Lcd_WriteData(0x37); 
	Lcd_WriteData(0x00); 	
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x10); 

	Lcd_WriteIndex(0xe1);  	// gamma adjustment 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x17); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x2c); 
	Lcd_WriteData(0x29); 
	Lcd_WriteData(0x2e); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x39); 
	Lcd_WriteData(0x3f); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x03); 
	Lcd_WriteData(0x10);  
	
	Lcd_WriteIndex(0x2a);	//column address set
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x7f);

	Lcd_WriteIndex(0x2b);	//row address set
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x9f);
	
	Lcd_WriteIndex(0xF0); //Enable test command  
	Lcd_WriteData(0x01); 
	Lcd_WriteIndex(0xF6); //Disable ram power save mode 
	Lcd_WriteData(0x00); 
	
	Lcd_WriteIndex(0x3A); //65k mode 
	Lcd_WriteData(0x05); 	// set the interface pixel format which is to be transferred via the MCU interface
	
	
	Lcd_WriteIndex(0x29);//Display on	 
		
}


/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{		
	Lcd_WriteIndex(0x2a);				// command:column address set
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);				// command row address set
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+3);
	
	Lcd_WriteIndex(0x2c);				// command memory write

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);			// set column address and row address
	LCD_WriteData_16Bit(Data);

}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}

/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void LCD_DrawRegion(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 color) //刷单色区域
{
  	int i,j;
	
	if( (0>(yEnd-yStart)) && (0>(xEnd-xStart)) )		// check if beyond range
	{
		return;
	}
	
	if(xStart>xEnd)	return;
	if(yStart>yEnd)	return;

	Lcd_SetRegion(xStart,yStart,xEnd,yEnd);				// 
	
	Lcd_WriteIndex(0x2C);
	for( i=yStart;i<(yEnd+1);i++)						// write color data
	{
		for(j=xStart;j<(xEnd+1);j++)
		{
			LCD_WriteData_16Bit(color);  						
		}
	}
}


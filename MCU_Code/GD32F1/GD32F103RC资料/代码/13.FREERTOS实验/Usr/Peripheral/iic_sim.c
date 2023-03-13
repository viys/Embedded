/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "iic_sim.h"
//---------------------- variable define -----------------------

/*************************************************************
Function:
Description: 
*************************************************************/
static void delay_us(int time)
{
	int i;
	
	for(i=0;i<time;i++)
	{
		__nop();
		__nop();
	}
}

/*************************************************************
Function:
Description: 
*************************************************************/
void SIM_I2C_Init(void)
{
	SIM_SDA_INIT();
	SIM_SCL_INIT();
}

/*
*********************************************************************************************************
*	函 数 名: Key_Key_IIC_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SIM_IIC_Start(void)                            
{                                          
	SIM_SDA_OUT();     //sda线输出
	SIM_SDA_HIGH();	  	  
	SIM_SCL_OUT();
	SIM_SCL_HIGH();
	delay_us(4);
	SIM_SDA_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SIM_SCL_LOW();//钳住I2C总线，准备发送或接收数据 
}  


/*
*********************************************************************************************************
*	函 数 名: Key_I2C_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SIM_IIC_Stop(void)
{
	SIM_SDA_OUT();//sda线输出
	SIM_SCL_LOW();
	SIM_SDA_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SIM_SCL_HIGH(); 
	delay_us(4);
	SIM_SDA_HIGH();//发送I2C总线结束信号
	delay_us(4);
}                

/*
*********************************************************************************************************
*	函 数 名: Key_Key_IIC_Wait_Ack
*	功能说明: 等待应答信号到来
*	形    参:  无
*	返 回 值: 1，接收应答失败
            0，接收应答成功
*********************************************************************************************************
*/
unsigned char SIM_IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	
	SIM_SDA_IN();      //SDA设置为输入  
	SIM_SDA_HIGH();delay_us(1);	   
	SIM_SCL_HIGH();delay_us(1);	 
	while(SIM_READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			SIM_IIC_Stop();
			return 1;
		}
	}
	SIM_SCL_LOW();//时钟输出0 	   
	return 0;  
}
/*
*********************************************************************************************************
*	函 数 名: Key_Key_IIC_Ack
*	功能说明: 产生ACK应答
*	形    参:  无
*	返 回 值:  无
*********************************************************************************************************
*/
void SIM_IIC_Ack(void)
{
	SIM_SCL_LOW();
	SIM_SDA_OUT();
	SIM_SDA_LOW();
	delay_us(2);
	SIM_SCL_HIGH();
	delay_us(2);
	SIM_SCL_LOW();
}
/*
*********************************************************************************************************
*	函 数 名: SIM_IIC_NAck
*	功能说明: 不产生ACK应答
*	形    参:  无
*	返 回 值:  无
*********************************************************************************************************
*/		    
void SIM_IIC_NAck(void)
{
	SIM_SCL_LOW();
	SIM_SDA_OUT();
	SIM_SDA_HIGH();
	delay_us(2);
	SIM_SCL_HIGH();
	delay_us(2);
	SIM_SCL_LOW();
}

/*
*********************************************************************************************************
*	函 数 名: Key_Key_IIC_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void SIM_IIC_SendByte(unsigned char Byte)
{
	unsigned char i;
	
	SIM_SDA_OUT(); 	    
	SIM_SCL_LOW();//拉低时钟开始数据传输
	for (i = 0; i < 8; i++)
	{
		if((Byte & 0x80)>>7)	{SIM_SDA_HIGH();}
		else					{SIM_SDA_LOW();}	
		Byte <<= 1;	     /* 左移一个bit */
		delay_us(2);
		SIM_SCL_HIGH();
		delay_us(2);
		SIM_SCL_LOW();
		delay_us(2);
	}
}

/*
*********************************************************************************************************
*	函 数 名: Key_Key_IIC_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参: 读1个字节，ack=1时，发送ACK，ack=0，发送nACK 
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
unsigned char SIM_IIC_ReadByte(unsigned char ack)
{
	unsigned char i;
	unsigned int value=0;
	
	SIM_SDA_IN();                  //SDA设置为输入
	for (i = 0; i < 8; i++)
	{
		SIM_SCL_LOW();
		delay_us(2);
		SIM_SCL_HIGH();
    
		if (SIM_READ_SDA())
		{ 
			value<<=1;
			value++;
		}	 
		else
		{
		   value<<=1;
		}
		delay_us(1); 
	}
	if (!ack)
        SIM_IIC_NAck();//发送nACK
    else
        SIM_IIC_Ack(); //发送ACK
	return value;
}



/*************************************************************
Function:
Description: 
*************************************************************/
int SIM_ii2_master_write(unsigned char slave_addr, unsigned char *buff, int length)
{
	int i;
	
	SIM_IIC_Start();
	SIM_IIC_SendByte( (slave_addr<<1)&I2C_F_WR );
	if(SIM_IIC_Wait_Ack())
	{
		return -1;
	}
	for(i=0;i<length;i++)
	{
		SIM_IIC_SendByte( buff[i] );
		if(SIM_IIC_Wait_Ack())
		{
			return -1;
		}
	}
	SIM_IIC_Stop();
	return 0;
}

/*************************************************************
Function:
Description: 
*************************************************************/
int SIM_ii2_master_read(unsigned char slave_addr, unsigned char *buff, int length)
{
	int i;
	
	SIM_IIC_Start();
	SIM_IIC_SendByte((slave_addr<<1)|I2C_F_RD);						// read command
	if(SIM_IIC_Wait_Ack())
	{
		return -1;
	}
	for(i=0;i<length;i++)
	{
		if( i==(length-1) )											// when read the last byte do not send ACK
		{
			buff[i] = SIM_IIC_ReadByte(0);
		}
		else
		{
			buff[i] = SIM_IIC_ReadByte(1);
		}
	}
	SIM_IIC_Stop();
	return 0;
}






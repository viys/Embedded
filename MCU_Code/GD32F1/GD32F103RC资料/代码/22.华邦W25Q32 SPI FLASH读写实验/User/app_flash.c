


#ifndef ___W25Q_APP_FLASH__
#define ___W25Q_APP_FLASH__

#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#define  FLASH_WRITE_ENABLE_CMD 		0x06
#define  FLASH_WRITE_DISABLE_CMD		0x04
#define  FLASH_READ_SR_CMD				   0x05
#define  FLASH_WRITE_SR_CMD				   0x01
#define  FLASH_READ_DATA				     0x03
#define  FLASH_FASTREAD_DATA			   0x0b
#define  FLASH_WRITE_PAGE			        0x02
#define  FLASH_ERASE_PAGE      			  0x81
#define  FLASH_ERASE_SECTOR       		0x20
#define	 FLASH_ERASE_BLOCK				    0xd8
#define	 FLASH_ERASE_CHIP				      0xc7
#define  FLASH_POWER_DOWN				      0xb9
#define  FLASH_RELEASE_POWER_DOWN       0xab
#define  FLASH_READ_DEVICE_ID      		0x90
#define  FLASH_READ_JEDEC_ID      		0x9f

#define 	W25Q_FLASH_SIZE	 (1*1024*1024)	// 1M字节
#define		PAGE_SIZE			8192	// 256 bytes
#define 	SECTOR_SIZE		512	 // 4-Kbyte
#define		BLOCK_SIZE		32	// 64-Kbyte	

#define PAGE_LEN		255	 //一页256字节

__align(4) uint8_t g_DataTmpBuffer[1000] = {0};
#define SectorBuf  g_DataTmpBuffer
//定义FLASH相关属性定义
struct Flash_Attr  {
	uint16_t flash_id;
	uint16_t page_size;
	uint16_t sector_size;
	uint8_t block_size;
};	

#define FALSH_TEST_ADDR		(0x100)


#define FLASH_CS_0()			{gpio_bit_write(GPIOA, GPIO_PIN_4,RESET);delay_1ms(10);}
#define FLASH_CS_1() 			{gpio_bit_write(GPIOA, GPIO_PIN_4,SET);delay_1ms(10);}

__align(4) uint16_t g_WriteReadcnt = 0;
__align(4) uint8_t g_WriteData[0x100] = {0};
__align(4) uint16_t g_Count = 0;
__align(4) uint8_t g_ucPos = 0;


void Flash_TestDataInit(void)
{
	uint8_t usMinData = 0;
	uint8_t usMaxData = 0xFF;
	uint16_t usCnt = 0;

    for(usCnt = 0; usCnt < 0x100 ; usCnt++)
	{
		if(g_WriteReadcnt % 2)
		{
			g_WriteData[usCnt] = usMinData;
			usMinData++;		
		}
		else
		{
			g_WriteData[usCnt] = usMaxData;
			usMaxData--;
		}	
		
	}
}
uint8_t spi_master_send_recv_byte(uint8_t spi_byte)
{		
	uint8_t ByteSend,ByteRecv;
	ByteSend=spi_byte;

 while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	spi_i2s_data_transmit(SPI0,ByteSend);
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
  ByteRecv=spi_i2s_data_receive(SPI0);
	return ByteRecv;

}

void spi_master_recv_some_bytes( uint8_t *pbdata, uint16_t recv_length)
{
	uint8_t *temp_data = pbdata;

	while (recv_length--)
	{
		*temp_data++ = spi_master_send_recv_byte(0xFF);	//发送 0xff 为从设备提供时钟
	}
	
}


uint32_t spi_flash_read_id(void)
{
	uint32_t ulJedId = 0;
	uint8_t recv_buff[5] = {0};
	
	 FLASH_CS_0();

	spi_master_send_recv_byte(FLASH_READ_JEDEC_ID);	//9fh
	 
	spi_master_recv_some_bytes(recv_buff, sizeof(recv_buff));
	
	ulJedId = (recv_buff[0] <<16) | (recv_buff[1] <<8) | (recv_buff[2]);
//	spi_i2s_data_transmit(SPI0,FLASH_READ_JEDEC_ID);
//	for(i=0;i<5;i++)
//	{
//	recv_buff[i]=spi_i2s_data_receive(SPI0);
//	}
//	
//ulJedId = (recv_buff[0] <<16) | (recv_buff[1] <<8) | (recv_buff[2]);
	 FLASH_CS_1();
	
	return ulJedId;
}

uint16_t SFLASH_ReadID(void)
{
  uint16_t ID = 0;
   FLASH_CS_0();                              //使能器件

	spi_master_send_recv_byte(0x90);//发送读取ID命令	    
	spi_master_send_recv_byte(0x00); 	    
	spi_master_send_recv_byte(0x00); 	    
	spi_master_send_recv_byte(0x00); 

	ID |= spi_master_send_recv_byte(0xFF)<<8;              //读取ID
	ID |= spi_master_send_recv_byte(0xFF);
  
   FLASH_CS_1();                             //失能器件
	
  return ID;
}

void Flash_WriteDisable(void)
{
	uint8_t command = FLASH_WRITE_DISABLE_CMD;
	FLASH_CS_0();  
	spi_master_send_recv_byte(command);
	FLASH_CS_1(); 
}


void Flash_WriteEnable(void)
{
	uint8_t command = FLASH_WRITE_ENABLE_CMD;

	FLASH_CS_0();	
	spi_master_send_recv_byte(command);//开启写使能
	FLASH_CS_1();
}


uint8_t Flash_ReadSR(void)
{
	uint8_t ucTmpVal = 0;
	uint8_t command = FLASH_READ_SR_CMD;

	FLASH_CS_0();	
	
	spi_master_send_recv_byte(command);	//05h
	spi_master_recv_some_bytes(&ucTmpVal,1);
	
	FLASH_CS_1();
	
	return ucTmpVal;
}
void Flash_WaitNobusy(void)
{
	
	while(((Flash_ReadSR()) & 0x01)==0x01);	//等待BUSY位清空
}
void Flash_WriteSR(uint8_t _ucByte)
{
	uint8_t command = FLASH_WRITE_SR_CMD;

	Flash_WriteEnable();	
	Flash_WaitNobusy();

	FLASH_CS_0();	
	spi_master_send_recv_byte(command);	//01h
	spi_master_send_recv_byte(_ucByte);	//写入一个字节
  FLASH_CS_1();
}

void Flash_ErasePage(uint32_t _ulPageAddr)
{
	_ulPageAddr *= 256;
	
	Flash_WriteEnable();
	Flash_WaitNobusy();
	
	FLASH_CS_0();
	spi_master_send_recv_byte(FLASH_ERASE_PAGE);//页擦除指令
	spi_master_send_recv_byte((uint8_t)(_ulPageAddr>>16));	//写入24位地址
	spi_master_send_recv_byte((uint8_t)(_ulPageAddr>>8));
	spi_master_send_recv_byte((uint8_t)(_ulPageAddr>>0));
	FLASH_CS_1();
	Flash_WaitNobusy();	//等待写入结束
}


void Flash_EraseSector(uint32_t _ulSectorAddr)
{
	uint8_t command = FLASH_ERASE_SECTOR;
	uint8_t temp_buff[3] = {0};
	
	temp_buff[0] = (uint8_t)(_ulSectorAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulSectorAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulSectorAddr >> 0);
	
	_ulSectorAddr *= 4096;	//1个扇区 4 KBytes
	
	Flash_WriteEnable();
	Flash_WaitNobusy();
	
	FLASH_CS_0();
	spi_master_send_recv_byte(command);
	spi_master_send_recv_byte(temp_buff[0]);
	spi_master_send_recv_byte(temp_buff[1]);
	spi_master_send_recv_byte(temp_buff[2]);
	FLASH_CS_1();
	
	Flash_WaitNobusy();	//等待写入结束
}


void Flash_EraseBlock(uint32_t _ulBlockAddr)
{
	uint8_t command = FLASH_ERASE_BLOCK;
	_ulBlockAddr *= 65536;	//块地址,一块64K
	
	Flash_WriteEnable();
	Flash_WaitNobusy();

	FLASH_CS_0();
	spi_master_send_recv_byte(command);
	spi_master_send_recv_byte(_ulBlockAddr>>16);
	spi_master_send_recv_byte(_ulBlockAddr>>8);
	spi_master_send_recv_byte(_ulBlockAddr>>0);
	FLASH_CS_1();

	Flash_WaitNobusy();	//等待写入结束
}


void Flash_EraseChip(void)
{
	uint8_t command = FLASH_ERASE_CHIP;

	Flash_WriteEnable();	//flash芯片写使能
	Flash_WaitNobusy();	//等待写操作完成
	
	FLASH_CS_0();
	spi_master_send_recv_byte(command);
	FLASH_CS_1();
	Flash_WaitNobusy();	//等待写入结束
}
void Flash_ReadSomeBytes(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte)
{
	uint8_t command = FLASH_READ_DATA;
	uint8_t temp_buff[3] = {0};

	temp_buff[0] = (uint8_t)(_ulReadAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulReadAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulReadAddr >> 0);

	FLASH_CS_0();
	
	spi_master_send_recv_byte(command);
	spi_master_send_recv_byte(temp_buff[0]);
	spi_master_send_recv_byte(temp_buff[1]);
	spi_master_send_recv_byte(temp_buff[2]);

	spi_master_recv_some_bytes(ucpBuffer, _usNByte);
	
		FLASH_CS_1();
}
void Flash_WritePage(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte)
{
	

	Flash_WriteEnable();	//写使能
	Flash_WaitNobusy();	//等待写入结束
	

	
  FLASH_CS_0();
	
	spi_master_send_recv_byte(FLASH_WRITE_PAGE);	//02h
	spi_master_send_recv_byte(_ulWriteAddr>>16);	//写入24位地址
	spi_master_send_recv_byte(_ulWriteAddr>>8);
	spi_master_send_recv_byte(_ulWriteAddr>>0);
	while(_usNByte--)
	{
		spi_master_send_recv_byte(*ucpBuffer);	//SPI 写入单个字节
		ucpBuffer++;
	}
	FLASH_CS_1();
	Flash_WaitNobusy();	//等待写入结束
}

//不带校验写入，得保证FLASH数据已经被擦除成0xFF
void Flash_WriteNoCheck(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte)
{
	uint16_t PageByte = 256 - _ulWriteAddr % 256;//单页剩余可写字节数

	if(_usNByte <= PageByte)	//不大于256字节
	{
		PageByte = _usNByte;
	}
	
	while(1)
	{
		Flash_WritePage(ucpBuffer, _ulWriteAddr, PageByte);
		if(_usNByte == PageByte)	//写入结束
			break;
		else
		{
			ucpBuffer += PageByte;	//下一页写入的数据
			_ulWriteAddr += PageByte;	//下一页写入的地址
			_usNByte -= PageByte;	//待写入的字节数递减
			if(_usNByte > 256)
			{
				PageByte = 256;
			}
			else
			{
				PageByte = _usNByte;
			}
		}
	}
}

void Flash_WriteSomeBytes(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte)
{
	uint32_t ulSecPos = 0;				//得到扇区位置
	uint16_t usSecOff = 0;				//扇区偏移
	uint16_t usSecRemain = 0;		//剩余扇区
	uint32_t i = 0;

	ulSecPos = _ulWriteAddr / 4096;//地址所在扇区(0--511)
	usSecOff = _ulWriteAddr % 4096;//扇区内地址偏移
	usSecRemain = 4096 - usSecOff;//扇区除去偏移，还剩多少字节

	if(_usNByte <= usSecRemain)	//写入数据大小 < 剩余扇区空间大小
	{
		usSecRemain = _usNByte;
	}

	while(1)
	{
		Flash_ReadSomeBytes(SectorBuf, ulSecPos*4096, 4096);//读出整个扇区的内容
		for (i = 0; i < usSecRemain; i++)	//校验数据
		{
			if (SectorBuf[usSecOff + i] != 0xFF)//储存数据不为0xFF，需要擦除
				break;
		}
		
		if(i < usSecRemain)	//需要擦除
		{
			Flash_EraseSector(ulSecPos);	//擦除这个扇区
			for(i = 0; i < usSecRemain; i++)	//保存写入的数据
			{
				SectorBuf[usSecOff + i] = ucpBuffer[i];
			}
			Flash_WriteNoCheck(SectorBuf, ulSecPos*4096, 4096);	//写入整个扇区(扇区=老数据+新写入数据)
		}
		else
		{
			Flash_WriteNoCheck(ucpBuffer, _ulWriteAddr, usSecRemain);//不需要擦除,直接写入扇区
		}
		if(_usNByte == usSecRemain)	//写入结束
		{
			Flash_WriteDisable();
			break;
		}
		else
		{
			ulSecPos++;		//扇区地址增加1
			usSecOff = 0;		//扇区偏移归零
			ucpBuffer += usSecRemain;	//指针偏移
			_ulWriteAddr += usSecRemain;	//写地址偏移
			_usNByte -= usSecRemain;	//待写入的字节递减

			if(_usNByte > 4096)
			{
				usSecRemain = 4096;	//待写入一扇区(4096字节大小)
			}
			else
			{
				usSecRemain = _usNByte;		//待写入少于一扇区的数据
			}
		}
		
	}
	
}

#endif	//__PRJ_STM32F10X_APP_FLASH_C__

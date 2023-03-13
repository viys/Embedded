/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "flash_spi.h"

/* Definitions of physical drive number for each drive */

#define SD_CARD 0
#define FLASH_SPI 1

#define START_ADDRESS 0  	//扇区起始地址
#define SECTORS_NUM   1024	//扇区个数,一个扇区4kb

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	//int result;

	switch (pdrv) {
	case SD_CARD :
		//result = SD_CARD_disk_status();

		// translate the reslut code here

		return stat;

	case FLASH_SPI :
		if(sFLASH_ID == SPI_FLASH_ReadID())
		{
			//状态正常
			stat = 0;
		}
		else
		{
			//状态不正常
			stat = STA_NOINIT;
		}

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	//int result;

	switch (pdrv) {
	case SD_CARD :
		//result = SD_CARD_disk_initialize();

		// translate the reslut code here

		return stat;

	case FLASH_SPI :
		
		SPI_FLASH_Init();
		SPI_Flash_WAKEUP();
		return disk_status(FLASH_SPI);

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* DSD_CARD buffer to store read dSD_CARD */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	//int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here

		//	result = SD_CARD_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case FLASH_SPI :
		sector += START_ADDRESS;
		SPI_FLASH_BufferRead(buff, sector*4096, count*4096);
		res = RES_OK;
		
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* DSD_CARD to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	//int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here

		//result = SD_CARD_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case FLASH_SPI :
		//一定要先擦除再写入
		sector += START_ADDRESS;
		SPI_FLASH_SectorErase(sector*4096);
		SPI_FLASH_BufferWrite((uint8_t *)buff, sector*4096, count*4096);
		res = RES_OK;
	
		return res;
	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control dSD_CARD */
)
{
	DRESULT res;
	//int result;

	switch (pdrv) {
	case SD_CARD :

		// Process of the command for the SD_CARD drive

		return res;

	case FLASH_SPI :
		switch(cmd)
		{
			//返回扇区个数
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SECTORS_NUM;
			break;
			//返回扇区大小
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 4096;
			break;
			//返回每次擦除扇区的最小个数(单位为扇区)
			case GET_BLOCK_SIZE:
				*(WORD*)buff = 1;
			break;
		}
		// Process of the command for the FLASH_SPI/SD card
		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}
#endif

DWORD get_fattime(void)
{
	
	return 0;
}

/****************************************************************************************************
file name:
Description:
	note: the last page of the aprom has been used as EEPROM.the valid address is 0x4700 to 0x47ff.
	so when you access the aprom,please cares the address.
****************************************************************************************************/
//---------------------- include files -----------------------
#include "all.h"
#include "oscillators.h"
//--------------------- variable declare ------------------
code unsigned char eeprom[128] _at_ 0x4700;					// occupy 128 bytes for eeprom access
volatile unsigned char xdata page_buffer[128];


/*********************************************************************************************
Function:
Description: 
*********************************************************************************************/
UINT8 Read_APROM_BYTE(UINT16 code *u16_addr)
{
	UINT8 rdata;
	rdata = *u16_addr>>8;
	return rdata;
}


/*********************************************************************************************
Function:
Description: 
	note: the address should not exceed the page range,one page is 128 bytes.
*********************************************************************************************/
void read_from_APROM(unsigned int u16AddrAprom, unsigned char *buff, unsigned int length)
{
	int i;
	
	for(i=0;i<length;i++)
	{
		buff[i] = Read_APROM_BYTE( (unsigned int code *)(u16AddrAprom+i) );
	}
}


/*********************************************************************************************
Function:
Description: 
	note: the address should not exceed the page range,one page is 128 bytes.
*********************************************************************************************/
void write_to_APROM(unsigned int u16AddrAprom, unsigned char *buff, unsigned int length)
{
	unsigned char looptmp;
	unsigned int u16_addrl_r;
	unsigned int validLn;
	
	//Check page start address
	u16_addrl_r=(u16AddrAprom/128)*128;
	validLn = u16AddrAprom%128;
	if(0==validLn)	validLn = 128;
	if(length>validLn)	length = validLn;
	//Save APROM data to XRAM0
	for(looptmp=0;looptmp<0x80;looptmp++)
	{
		page_buffer[looptmp] = Read_APROM_BYTE((unsigned int code *)(u16_addrl_r+looptmp));
	}
	// Modify customer data in XRAM
	for(looptmp=0;looptmp<length;looptmp++)
	{
		page_buffer[looptmp] = buff[looptmp];
	}
	//Erase APROM DATAFLASH page
	IAPAL = u16_addrl_r&0xff;
	IAPAH = (u16_addrl_r>>8)&0xff;
	IAPFD = 0xFF;
	set_IAPEN; 
	set_APUEN;	
	IAPCN = 0x22; 		
	set_IAPGO; 
		
	//Save changed RAM data to APROM DATAFLASH
	set_IAPEN; 
	set_APUEN;
	IAPCN = 0x21;
	for(looptmp=0;looptmp<0x80;looptmp++)
	{
		IAPAL = (u16_addrl_r&0xff)+looptmp;
		IAPAH = (u16_addrl_r>>8)&0xff;
		IAPFD = page_buffer[looptmp];
		set_IAPGO;			
	}
	clr_APUEN;
	clr_IAPEN;
}

/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
//------------------------------- include files ----------------------------------------
#include "gd32e10x.h" 
#include "flash_zone.h"
//-------------------------------- macro define ----------------------------------------


/*******************************************************************************
Function:
Descripton:
			erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
*******************************************************************************/
void fmc_erase_pages(unsigned int page_start_addr,unsigned int pages_cnt)
{
    uint32_t EraseCounter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR));
    
    /* erase the flash pages */
    for(EraseCounter = 0; EraseCounter < pages_cnt; EraseCounter++)
	{
        fmc_page_erase(page_start_addr + (FLASH_PAGE_SIZE * EraseCounter));
        fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR));
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*******************************************************************************
Function:
Descripton:
			program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
*******************************************************************************/
void fmc_program(unsigned int start_addr,unsigned int end_addr,unsigned int *buff,unsigned int buf_ln)
{
	unsigned int data_addr = start_addr;
	
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* program flash */
    while( (data_addr<end_addr) && ((data_addr-start_addr)/BYTES_OF_WORD<=buf_ln) )
	{
        fmc_word_program(data_addr, *buff);
        data_addr += 4;
		buff++;
        fmc_flag_clear( (fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR) );
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*******************************************************************************
Function:
Descripton:
			check fmc erase result
*******************************************************************************/
void fmc_erase_pages_check(unsigned int page_start_addr,unsigned int pages_cnt)
{
	uint32_t *ptrd;
	uint32_t i;

    ptrd = (uint32_t *)page_start_addr;

    /* check flash whether has been erased */
    for(i = 0; i < pages_cnt*FLASH_PAGE_SIZE; i++)
	{
        if(0xFFFFFFFF != (*ptrd))
		{
            break;
        }
		else
		{
            ptrd++;
        }
    }
}


/*******************************************************************************
Function:
Descripton:
			check fmc program result
*******************************************************************************/
void fmc_program_check(unsigned int start_addr,unsigned int end_addr,unsigned int *buff,unsigned int buf_ln)
{
	uint32_t *ptrd;
    uint32_t i;

    ptrd = (uint32_t *)start_addr;

    /* check flash whether has been programmed */
    for(i = 0; i < buf_ln; i++)
	{
        if((*ptrd) != buff[i])
		{
            break;
        }
		else
		{
            ptrd++;
        }
    }
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
int fmc_erase_test(void)
{
	//unsigned int fmc_test_data[] = {0x01020304,0x05060708};
	
    /* step1: erase pages and check if it is successful. If not, light the LED2. */
    //fmc_erase_pages(BK_DATA_START_ADDRESS+FLASH_PAGE_SIZE*3,1);
//    fmc_erase_pages_check();

//    /* step2: program and check if it is successful. If not, light the LED3. */
//    fmc_program();
//    fmc_program_check();

	
	// 向备份空间的最后8个地址写8字节数据。
	// 按字写入，共写2个字
	//fmc_program((BK_DATA_START_ADDRESS+BK_DATA_LENGTH)-8,(BK_DATA_START_ADDRESS+BK_DATA_LENGTH),fmc_test_data,2);
	
	return(0);
}



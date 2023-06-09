/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
//------------------------------- include files ----------------------------------------
#include "gd32e10x.h" 
#include <stdio.h>
//-------------------------------- macro define ----------------------------------------
#define FLASH_PAGE_PROGRAM
//#define WRITE_PROTECTION_ENABLE 
#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;
#define FLASH_PAGE_SIZE ((uint16_t)0x400)
#define FMC_PAGES_PROTECTED (OB_WP_6 | OB_WP_7)

#define BANK0_WRITE_START_ADDR  ((uint32_t)0x08001400)
#define BANK0_WRITE_END_ADDR    ((uint32_t)0x08008000)
//-------------------------------- variable define -------------------------------------
uint32_t erase_counter = 0x0, Address = 0x0;
uint16_t data = 0x1753;
uint32_t wp_value = 0xFFFFFFFF, protected_pages = 0x0;
uint32_t NbrOfPage;
__IO fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;



/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
int fmc_write_test(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();

    fmc_flag_clear( (fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR) );
    
    /* Get pages write protection status */
    wp_value = ob_write_protection_get();

#ifdef WRITE_PROTECTION_DISABLE
    /* Get pages already write protected */
    protected_pages = ~(wp_value | FMC_PAGES_PROTECTED);
  
    /* Check if desired pages are already write protected */
    if((wp_value | (~FMC_PAGES_PROTECTED)) != 0xFFFFFFFF )
	{
        /* Erase all the option Bytes */
        fmc_state = ob_erase();
    
        /* Check if there is write protected pages */
        if(protected_pages != 0x0)
		{
            /* Restore write protected pages */
            fmc_state = ob_write_protection_enable(protected_pages);
        }
        /* Generate System Reset to load the new option byte values */
        NVIC_SystemReset();
    }

#elif defined WRITE_PROTECTION_ENABLE
    /* Get current write protected pages and the new pages to be protected */
    protected_pages =  (~wp_value) | FMC_PAGES_PROTECTED; 
  
    /* Check if desired pages are not yet write protected */
    if(((~wp_value) & FMC_PAGES_PROTECTED )!= FMC_PAGES_PROTECTED)
	{
  
        /* Erase all the option Bytes because if a program operation is 
        performed on a protected page, the Flash memory returns a 
        protection error */
        fmc_state = ob_erase();

        /* Enable the pages write protection */
        fmc_state = ob_write_protection_enable(protected_pages);
  
        /* Generate System Reset to load the new option byte values */
        NVIC_SystemReset();
    }
#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM  
    /* Get the number of pages to be erased */
    NbrOfPage = (BANK0_WRITE_END_ADDR - BANK0_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    /* The selected pages are not write protected */
    if ( (wp_value & FMC_PAGES_PROTECTED) != 0x00)
	{
        /* Clear All pending flags */
        fmc_flag_clear( (fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR) );

        /* erase the FLASH pages */
        for(erase_counter = 0; (erase_counter < NbrOfPage) && (fmc_state == FMC_READY); erase_counter++)
		{
            fmc_state = fmc_page_erase(BANK0_WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter));
        }
  
        /* FLASH Half Word program of data 0x1753 at addresses defined by  BANK1_WRITE_START_ADDR and BANK1_WRITE_END_ADDR */
        Address = BANK0_WRITE_START_ADDR;

        while((Address < BANK0_WRITE_END_ADDR) && (fmc_state == FMC_READY))
		{
            fmc_state = fmc_halfword_program(Address, data);
            Address = Address + 2;
        }

        /* Check the correctness of written data */
        Address = BANK0_WRITE_START_ADDR;

        while((Address < BANK0_WRITE_END_ADDR) && (program_state != FAILED))
		{
            if((*(__IO uint16_t*) Address) != data)
			{
                program_state = FAILED;
            }
            Address += 2;
        }
    }
    else
	{ 
        /* Error to program the flash : The desired pages are write protected */ 
        program_state = FAILED;
    }
#endif /* FLASH_PAGE_PROGRAM */
    while(1){
    }
}

/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
#ifndef		__FMC_ERASE_H__
#define		__FMC_ERASE_H__
//------------------------------- include files ----------------------------------------
#include "gd32e10x.h" 
//-------------------------------- macro define ----------------------------------------
//------------------------------ function declare --------------------------------------
void fmc_erase_pages(unsigned int page_start_addr,unsigned int pages_cnt);
void fmc_program(unsigned int start_addr,unsigned int end_addr,unsigned int *buff,unsigned int buf_ln);
void fmc_erase_pages_check(unsigned int page_start_addr,unsigned int pages_cnt);
void fmc_program_check(unsigned int start_addr,unsigned int end_addr,unsigned int *buff,unsigned int buf_ln);
int fmc_erase_test(void);




#endif	// __FMC_ERASE_H__



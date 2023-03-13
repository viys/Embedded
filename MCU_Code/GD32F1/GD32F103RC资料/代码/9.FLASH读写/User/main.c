/*****************************************************************************
  �ļ�: main.c
  ����: Zhengyu https://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20210401
	ƽ̨:MINI-GD32F103RCT6������
	΢�ź�:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"

#define FMC_PAGE_SIZE           ((uint16_t)0x400U)//FLASHҳ��С��1024���ֽ�
#define FMC_WRITE_START_ADDR    ((uint32_t)0x08004000U)//д������ʼ��ַ
#define FMC_WRITE_END_ADDR      ((uint32_t)0x08004800U)//д���ݽ�����ַ

uint32_t *ptrd;
uint32_t address = 0x00;
uint32_t data0   = 0x01234567U;//д�������


/* calculate the num of page to be programmed/erased */
uint32_t page_num = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;//����ҳ��
/* calculate the num of page to be programmed/erased */
uint32_t word_num = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);//����������

//����FLASHҳ
void fmc_erase_pages(void)
{
    uint32_t erase_counter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
    
    /* erase the flash pages */
    for(erase_counter = 0; erase_counter < page_num; erase_counter++){
        fmc_page_erase(FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * erase_counter));
        fmc_flag_clear(FMC_FLAG_BANK0_END);
        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

//��FLASH��ַд������
void fmc_program(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_word_program(address, data0);
        address += 4;
        fmc_flag_clear(FMC_FLAG_BANK0_END);
        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR); 
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

//��ѯFLASH�Ƿ�����ɾ�
void fmc_erase_pages_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < word_num; i++){
        if(0xFFFFFFFF != (*ptrd)){
    
     
            break;
        }else{
            ptrd++;
        }
    }
}

//��ѯFLASHд���Ƿ���ȷ
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0; i < word_num; i++){
        if((*ptrd) != data0){
       
      
            break;
        }else{
            ptrd++;
        }
    }
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	fmc_erase_pages();//����FLASH
	fmc_erase_pages_check();//�������
	fmc_program();//FLASHд��0x01234567
	fmc_program_check();//д�����ݼ��
	while(1)
	{

	}
}

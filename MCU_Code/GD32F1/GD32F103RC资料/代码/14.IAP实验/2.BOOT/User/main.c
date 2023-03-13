/***************************************************************************//**
  �ļ�: main.c
  ����: Zhengyu https://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20210401
	ƽ̨:MINI-GD32F103RCT6

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "ymodem.h"
void delay(int time)
{
    while(time--);
    
    return;
}
#define FMC_PAGE_SIZE           ((uint16_t)0x400U)//FLASH ҳ��С1024�ֽ�
#define FMC_WRITE_START_ADDR    ((uint32_t)0x08004000U)//��ʼ��ַ
#define FMC_WRITE_END_ADDR      ((uint32_t)0x08004800U)//������ַ

uint32_t *ptrd;
uint32_t address = 0x00;
uint32_t data0   = 0x01234567U;//д������

typedef  void (*pFunction)(void);
 pFunction Jump_To_Application;
 uint32_t JumpAddress;
/* calculate the num of page to be programmed/erased */
uint32_t page_num = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the num of page to be programmed/erased */
uint32_t word_num = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);

//����1ҳ
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

//FLASH����д��
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
/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
}
//FLASH����
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t flashaddress;
  
  flashaddress = StartSector;
  
  while (flashaddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
    if (fmc_page_erase(flashaddress) == FMC_READY)
    {
      flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      return (1);
    }
  }
  return (0);
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;

  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* the operation will be done by word */ 
    if (fmc_word_program(*FlashAddress, *(uint32_t*)(Data+i)) == FMC_READY)
    {
     /* Check the written value */
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }

  return (0);
}
/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval If the sector is write-protected, the corresponding bit in returned
  *         value is set.
  *         If the sector isn't write-protected, the corresponding bit in returned
  *         value is reset.
  */


uint32_t FLASH_If_GetWriteProtectionStatus(void)
{ 
  return(~ob_write_protection_get() & FLASH_PROTECTED_PAGES);
}

//�������
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

//д����
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;
    for(i = 0; i < word_num; i++){
        if((*ptrd) != data0){
       
      
            break;
        }else{
            ptrd++;
        }
    }
}
//��������USART0 ,PB6,PB7�ţ�������115200����У�飬8λ���ݣ�1λֹͣλ
void gd_eval_com_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);//PB6,PB7��Ҫ��ӳ��
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_7);
    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0,115200);
		usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
	  usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_enable(USART0);
}
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF); //�ܽŸ���ʱ��alternate function clockʹ��
	gd_eval_com_init();//���ڳ�ʼ��
	FLASH_If_Init();//FLASH����
	while(1)
	{
	   Main_Menu();//ʵ�ֳ�����������ת��YmodemЭ��
	}
    return 0;
}

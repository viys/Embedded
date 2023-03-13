/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"

uint32_t FlashJedecid,FlashDeviceid;//FLASH ID变量
extern void Flash_TestDataInit(void);
extern uint32_t spi_flash_read_id(void);
extern uint16_t SFLASH_ReadID(void);
uint8_t ReadBuff[10],WriteBuff[10]={0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};//读写存储
extern void Flash_ReadSomeBytes(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte);
extern void Flash_WriteSomeBytes(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte);
extern void Flash_WriteSR(uint8_t _ucByte);
extern uint8_t Flash_ReadSR(void);

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_128;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    spi_enable(SPI0);	
   // spi_nss_internal_high(SPI0);

}
int main(void)
{
	rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
	rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);//设置主频108M(#define __SYSTEM_CLOCK_108M_PLL_HXTAL         (uint32_t)(108000000)),8M外部晶振  (#define HXTAL_VALUE    ((uint32_t)8000000))
  systick_config();//配置1ms SysTick
	rcu_periph_clock_enable(RCU_GPIOA);//使能GPIOA时钟   
	rcu_periph_clock_enable(RCU_AF);//AF时钟使能 
	rcu_periph_clock_enable(RCU_SPI0);//使能SPI0时钟   

	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7); /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//CS脚配成输出
	spi_config();//SPI初始化
	
	FlashJedecid = spi_flash_read_id();//读取Jedecid
	FlashDeviceid=SFLASH_ReadID();//读取Device ID
  /* USER CODE END 2 */
	Flash_ReadSomeBytes(ReadBuff,0,8);//从FLASH 0地址读取8字节内容放入ReadBuff数组
	Flash_WriteSR(0x42);//解除保护
	delay_1ms(100);
	Flash_ReadSR();//读状态寄存器
	Flash_WriteSomeBytes(WriteBuff,0,8);//把WriteBuff数组中的内容写入FLASH 0地址
	delay_1ms(100);
	Flash_ReadSomeBytes(ReadBuff,0,8);//从FLASH 0地址读取8字节内容放入ReadBuff数组
	while(1)
	{
			
	}
 
}

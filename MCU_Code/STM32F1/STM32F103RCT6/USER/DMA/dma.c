#include "dma.h"

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
 * const关键字将aSRC_Const_Buffer数组变量定义为常量类型
 * 表示数据存储在内部的FLASH中
 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

									/* 定义DMA传输目标存储器
 * 存储在内部的SRAM中																		
 */
uint32_t aDST_Buffer[BUFFER_SIZE];

uint8_t SendBuff[SENDBUFF_SIZE];

									
/* M to M */
					
//储存器到储存器									
void MtM_DMA_Config(const uint32_t* src_const_buffer, uint32_t* dst_buffer, uint16_t buffer_size)
{
	DMA_InitTypeDef DMA_InitStruct;
	//打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//源数据地址
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)src_const_buffer;
	//目标地址
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dst_buffer;
	//方向：外设到存储器（这里的外设是内部的FLASH）
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	//配置外设
	//传输大小
	DMA_InitStruct.DMA_BufferSize = buffer_size;
	//外设（内部的FLASH）地址递增
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	// 外设数据单位	
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	//内存地址递增
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//内存数据单位
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	//MDA模式
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	//配置优先级
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
	
	DMA_Init(DMA1_Channel6, &DMA_InitStruct);
	DMA_ClearFlag(DMA1_FLAG_TC6);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	while(RESET == DMA_GetFlagStatus(DMA1_FLAG_TC6));
}

/* M to P */

//该函数还需配合USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE)使用
void MtUSART_DMA_Config(uint8_t* dst_buffer, uint16_t buffer_size)
{
	DMA_InitTypeDef DMA_InitStruct;
	//打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//源数据地址
	DMA_InitStruct.DMA_PeripheralBaseAddr = (USART1_BASE+0x04);
	//M地址
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dst_buffer;
	//方向：存储器到串口1
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	//配置外设
	//传输大小
	DMA_InitStruct.DMA_BufferSize = buffer_size;
	//外设地址递增
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 外设数据单位	
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//内存地址递增
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//内存数据单位
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//MDA模式
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	//配置优先级
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel4, &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/**
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t DMABuffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* 数据长度递减 */
  while(BufferLength--)
  {
    /* 判断两个数据源是否对应相等 */
    if(*pBuffer != *pBuffer1)
    {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1;  
}

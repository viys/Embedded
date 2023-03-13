#include "dma.h"

/* ����aSRC_Const_Buffer������ΪDMA��������Դ
 * const�ؼ��ֽ�aSRC_Const_Buffer�����������Ϊ��������
 * ��ʾ���ݴ洢���ڲ���FLASH��
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

									/* ����DMA����Ŀ��洢��
 * �洢���ڲ���SRAM��																		
 */
uint32_t aDST_Buffer[BUFFER_SIZE];

uint8_t SendBuff[SENDBUFF_SIZE];

									
/* M to M */
					
//��������������									
void MtM_DMA_Config(const uint32_t* src_const_buffer, uint32_t* dst_buffer, uint16_t buffer_size)
{
	DMA_InitTypeDef DMA_InitStruct;
	//��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//Դ���ݵ�ַ
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)src_const_buffer;
	//Ŀ���ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dst_buffer;
	//�������赽�洢����������������ڲ���FLASH��
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	//��������
	//�����С
	DMA_InitStruct.DMA_BufferSize = buffer_size;
	//���裨�ڲ���FLASH����ַ����
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	// �������ݵ�λ	
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	//�ڴ��ַ����
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//�ڴ����ݵ�λ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	//MDAģʽ
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	//�������ȼ�
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
	
	DMA_Init(DMA1_Channel6, &DMA_InitStruct);
	DMA_ClearFlag(DMA1_FLAG_TC6);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	while(RESET == DMA_GetFlagStatus(DMA1_FLAG_TC6));
}

/* M to P */

//�ú����������USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE)ʹ��
void MtUSART_DMA_Config(uint8_t* dst_buffer, uint16_t buffer_size)
{
	DMA_InitTypeDef DMA_InitStruct;
	//��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//Դ���ݵ�ַ
	DMA_InitStruct.DMA_PeripheralBaseAddr = (USART1_BASE+0x04);
	//M��ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)dst_buffer;
	//���򣺴洢��������1
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	//��������
	//�����С
	DMA_InitStruct.DMA_BufferSize = buffer_size;
	//�����ַ����
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// �������ݵ�λ	
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	//�ڴ��ַ����
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//�ڴ����ݵ�λ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	//MDAģʽ
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	//�������ȼ�
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel4, &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/**
  * �ж�ָ�����ȵ���������Դ�Ƿ���ȫ��ȣ�
  * �����ȫ��ȷ���1��ֻҪ����һ�����ݲ���ȷ���0
  */
uint8_t DMABuffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* ���ݳ��ȵݼ� */
  while(BufferLength--)
  {
    /* �ж���������Դ�Ƿ��Ӧ��� */
    if(*pBuffer != *pBuffer1)
    {
      /* ��Ӧ����Դ����������˳�������������0 */
      return 0;
    }
    /* ������������Դ�ĵ�ַָ�� */
    pBuffer++;
    pBuffer1++;
  }
  /* ����жϲ��Ҷ�Ӧ������� */
  return 1;  
}

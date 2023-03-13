#ifndef _DMA_H
#define _DMA_H

#include "stm32f10x.h"

#define BUFFER_SIZE 32

#define SENDBUFF_SIZE 5000

void MtM_DMA_Config(const uint32_t* src_const_buffer, uint32_t* dst_buffer, uint16_t buffer_size);
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
void MtUSART_DMA_Config(uint8_t* dst_buffer, uint16_t buffer_size);

#endif

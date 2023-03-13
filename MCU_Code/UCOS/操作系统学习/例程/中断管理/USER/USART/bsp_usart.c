#include "bsp_usart.h"

static uint32_t USART_GPIO_CLK[USARTn] = {USART1_GPIO_CLK, USART2_GPIO_CLK, USART3_GPIO_CLK};
static uint32_t USART_CLK[USARTn] = {USART1_CLK, USART2_CLK, USART3_CLK};
static GPIO_TypeDef * USART_TX_PORT[USARTn] = {USART1_TX_GPIO_PORT, USART2_TX_GPIO_PORT, USART3_TX_GPIO_PORT};
static uint16_t USART_TX_PIN[USARTn] = {USART1_TX_GPIO_PIN, USART2_TX_GPIO_PIN, USART3_TX_GPIO_PIN};
static GPIO_TypeDef * USART_RX_PORT[USARTn] = {USART1_RX_GPIO_PORT, USART2_RX_GPIO_PORT, USART3_RX_GPIO_PORT};
static uint16_t USART_RX_PIN[USARTn] = {USART1_RX_GPIO_PIN, USART2_RX_GPIO_PIN, USART3_RX_GPIO_PIN};
static USART_TypeDef* USART_Which[USARTn] = {USART1_Which, USART2_Which, USART3_Which};
static uint8_t USART_NVIC_IRQChannel[USARTn] = {USART1_IRQ, USART2_IRQ, USART3_IRQ};

//串口中断优先级配置函数
static void USART_NVIC_Config(usart_typedef_enum usart_num)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART_NVIC_IRQChannel[usart_num];
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
	USART_ITConfig(USART_Which[usart_num], USART_IT_RXNE, ENABLE);
}

char Usart_Rx_Buf[USART_RBUFF_SIZE];

void USART1_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 设置DMA源地址：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1->DR;
	// 内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Usart_Rx_Buf;
	// 方向：从内存到外设	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 传输大小	
	DMA_InitStructure.DMA_BufferSize = USART_RBUFF_SIZE;
	// 外设地址不增	    
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// 内存数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
	// DMA模式，一次或者循环模式
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// 优先级：中	
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 配置DMA通道		   
	DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitStructure);		
	// 清除DMA所有标志
	DMA_ClearFlag(DMA1_FLAG_GL5);
	DMA_ITConfig(USART_RX_DMA_CHANNEL, DMA_IT_TE, ENABLE);
	// 使能DMA
	DMA_Cmd(USART_RX_DMA_CHANNEL,ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

//串口配置函数
void USART_Config(usart_typedef_enum usart_num, usart_mode_typedef_enum usart_mode, u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	
	//打开GPIO时钟
	RCC_APB2PeriphClockCmd(USART_GPIO_CLK[usart_num], ENABLE);
	
	//打开串口时钟
	if(USART_1 == usart_num)
	{
		RCC_APB2PeriphClockCmd(USART_CLK[usart_num], ENABLE);
	}
	else if(USART_2 == usart_num | USART_3 == usart_num)
	{
		RCC_APB1PeriphClockCmd(USART_CLK[usart_num], ENABLE);
	}
	else
	{
		
	}
	
	//将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStruct.GPIO_Pin = USART_TX_PIN[usart_num];
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TX_PORT[usart_num], &GPIO_InitStruct);
	
	//将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStruct.GPIO_Pin = USART_RX_PIN[usart_num];
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX_PORT[usart_num], &GPIO_InitStruct);
	
	//配置串口的参数
	//配置波特率
	USART_Initstruct.USART_BaudRate = baudrate;
	//配置帧数据字长
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;
	//配置停止位
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	//配置校验位
	USART_Initstruct.USART_Parity = USART_Parity_No;
	//配置硬件流控制
	USART_Initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//配置工作模式 收发一起
	USART_Initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_Which[usart_num],&USART_Initstruct);
	
	//串口接收中断
	if (usart_mode == USART_MODE_EXTI) {
        USART_NVIC_Config(usart_num);
    }
	
	//使能串口
	USART_Cmd(USART_Which[usart_num], ENABLE);
}
                                                                                          
///重定向c库函数printf到串口，重定向后可使用printf函数                                                
int fputc(int ch, FILE *f)                                                                         
{                                                                                                  
		/* 发送一个字节数据到串口 */                                                                 
		USART_SendData(USART1, (uint8_t) ch);                                                      
		                                                                                           
		/* 等待发送完毕 */                                                                          
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}


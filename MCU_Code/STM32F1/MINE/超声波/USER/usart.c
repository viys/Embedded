#include "usart.h"

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	
	//打开GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//打开串口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	
	//将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//配置串口的参数
	//配置波特率
	USART_Initstruct.USART_BaudRate = 115200;
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
	USART_Init(USART1,&USART_Initstruct);
	
	//使能串口
	USART_Cmd(USART1, ENABLE);
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



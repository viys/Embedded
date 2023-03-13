#include "usart.h"

/**
@brief	usart初始化
@Note		PA9 -- TX   PA10 -- RX
@param	flck , bood, over8
@retval	None
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	u32 Priority;
	float USARTDIV;    
	u32  DIV_Mantissa;   //整数
	u32  DIV_Fraction;   //小数

	//AFIO ->MAPR &= ~AFIO_MAPR_USART1_REMAP;//没有重映射
	
	RCC->APB2ENR |= 1 << 2;    //GPIOA时钟使能
	RCC->APB2ENR |= 1 << 14;   //串口时钟的使能
	
	/* RX为1000，TX为1011 */
	GPIOA->CRH&=0XFFFFF00F;    //IO状态设置
	GPIOA->CRH|=0X000008B0;    //IO状态设置 
	
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位
	

	/* 配置波特率 */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16倍过采样
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8倍过采样
	}
	
	USARTDIV =(fclk*1000000)/(bood*16);
  DIV_Mantissa = USARTDIV;
	DIV_Fraction = (USARTDIV - DIV_Mantissa) * 16;
	DIV_Mantissa<<=4;
	DIV_Mantissa+=DIV_Fraction; 
	USART1->BRR=DIV_Mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
	//接受中断使能
	USART1 ->CR1 |= 1 << 5;
	
	//配置NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (USART1_IRQn ,Priority );
	NVIC_EnableIRQ (USART1_IRQn );//使能中断源
	
	/* 使能USART
	当该位被清零，USART的分频器和输出在当前字节传输完成后停止工作，以减少功耗。该
	位的置起和清零，是由软件操作的。
	*/
	//USART1->CR1 |= 1 << 13;  
}

/**
@brief	串口发送函数
@Note		
@param	*str
@retval	None
*/
void USART1_Send_str(char  *str)
{
	while(*str != '\0')
	{
		USART1->DR = *str;
		while(!(USART1 ->SR & (1 << 7)))
		{
			
		}
		str++;
	}
}

/***************重定向fputc函数****************/
int fputc( int ch ,FILE *stream)
{
	USART1 ->DR = ch;
	while(!(USART1 ->SR & (1 << 7)))
	{
		//等待发送完成
	}
	return ch;
}


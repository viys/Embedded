#include "usart.h"

/**
@brief	usart初始化
@Note		PA9 -- RX   PA10 -- TX
@param	flck , bood, over8
@retval	None
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	u32 Priority;
	float USARTDIV;    
	u32  DIV_Mantissa;   //整数
	u32  DIV_Fraction;   //小数
	  /* GPIO时钟使能 */
	RCC->AHB1ENR |= 1 << 0;    //GPIOA时钟使能
	
	 /* PA9 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 9));
	GPIOA->MODER |= 2 << (2 * 9);  //复用模式
	
	GPIOA->AFR[1] &= ~(15 << (4 * (9 - 8)));
	GPIOA->AFR[1] |= 7 << (4 * (9-8));  //复用为usart1
	
	/* PA10 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 10));
	GPIOA->MODER |= 2 << (2 * 10);  //复用模式
	
	GPIOA->AFR[1] &= ~(15 << (4 * (10-8)));
	GPIOA->AFR[1] |= 7 << (4 * (10-8));  //复用为usart1
	
	/* 使能串口时钟 */
	RCC->APB2ENR |= 1 << 4;   
	
	/* 设置串口工作方式 */
	USART1->CR1 &= ~(1 << 12);   //数据位8位
	USART1->CR2 &= ~(3 << 12);   //1位停止位
	USART1->CR1 |= 1 << 3;       //发送使能
	USART1->CR1 |= 1 << 2;       //接收使能
	
	/* 配置波特率 */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16倍过采样
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8倍过采样
	}
	
	USARTDIV = fclk * 1000000 / (8 * (2 - over8) * bood);
	DIV_Mantissa = USARTDIV;   //整数
	DIV_Fraction = (USARTDIV - DIV_Mantissa) * 16; 
	 
	USART1->BRR = DIV_Mantissa << 4 | DIV_Fraction;
  
	//接受中断使能
	USART1 ->CR1 |= 1 << 5;
	
	//配置NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (USART1_IRQn ,Priority );
	NVIC_EnableIRQ (USART1_IRQn );//使能中断源
	
	/* 使能USART*/
	USART1->CR1 |= 1 << 13;  
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

/***************配置printf函数****************/
int fputc( int ch ,FILE *stream)
{
	USART1 ->DR = ch;
	while(!(USART1 ->SR & (1 << 7)))
	{
		//等待发送完成
	}
	return ch;
}

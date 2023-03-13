#include "usart.h"

/**
@brief	usart��ʼ��
@Note		PA9 -- RX   PA10 -- TX
@param	flck , bood, over8
@retval	None
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	u32 Priority;
	float USARTDIV;    
	u32  DIV_Mantissa;   //����
	u32  DIV_Fraction;   //С��
	  /* GPIOʱ��ʹ�� */
	RCC->AHB1ENR |= 1 << 0;    //GPIOAʱ��ʹ��
	
	 /* PA9 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 9));
	GPIOA->MODER |= 2 << (2 * 9);  //����ģʽ
	
	GPIOA->AFR[1] &= ~(15 << (4 * (9 - 8)));
	GPIOA->AFR[1] |= 7 << (4 * (9-8));  //����Ϊusart1
	
	/* PA10 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 10));
	GPIOA->MODER |= 2 << (2 * 10);  //����ģʽ
	
	GPIOA->AFR[1] &= ~(15 << (4 * (10-8)));
	GPIOA->AFR[1] |= 7 << (4 * (10-8));  //����Ϊusart1
	
	/* ʹ�ܴ���ʱ�� */
	RCC->APB2ENR |= 1 << 4;   
	
	/* ���ô��ڹ�����ʽ */
	USART1->CR1 &= ~(1 << 12);   //����λ8λ
	USART1->CR2 &= ~(3 << 12);   //1λֹͣλ
	USART1->CR1 |= 1 << 3;       //����ʹ��
	USART1->CR1 |= 1 << 2;       //����ʹ��
	
	/* ���ò����� */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16��������
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8��������
	}
	
	USARTDIV = fclk * 1000000 / (8 * (2 - over8) * bood);
	DIV_Mantissa = USARTDIV;   //����
	DIV_Fraction = (USARTDIV - DIV_Mantissa) * 16; 
	 
	USART1->BRR = DIV_Mantissa << 4 | DIV_Fraction;
  
	//�����ж�ʹ��
	USART1 ->CR1 |= 1 << 5;
	
	//����NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (USART1_IRQn ,Priority );
	NVIC_EnableIRQ (USART1_IRQn );//ʹ���ж�Դ
	
	/* ʹ��USART*/
	USART1->CR1 |= 1 << 13;  
}

/**
@brief	���ڷ��ͺ���
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

/***************����printf����****************/
int fputc( int ch ,FILE *stream)
{
	USART1 ->DR = ch;
	while(!(USART1 ->SR & (1 << 7)))
	{
		//�ȴ��������
	}
	return ch;
}

#include "usart.h"

/**
@brief	usart��ʼ��
@Note		PA9 -- TX   PA10 -- RX
@param	flck , bood, over8
@retval	None
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	u32 Priority;
	float USARTDIV;    
	u32  DIV_Mantissa;   //����
	u32  DIV_Fraction;   //С��

	//AFIO ->MAPR &= ~AFIO_MAPR_USART1_REMAP;//û����ӳ��
	
	RCC->APB2ENR |= 1 << 2;    //GPIOAʱ��ʹ��
	RCC->APB2ENR |= 1 << 14;   //����ʱ�ӵ�ʹ��
	
	/* RXΪ1000��TXΪ1011 */
	GPIOA->CRH&=0XFFFFF00F;    //IO״̬����
	GPIOA->CRH|=0X000008B0;    //IO״̬���� 
	
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ
	

	/* ���ò����� */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16��������
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8��������
	}
	
	USARTDIV =(fclk*1000000)/(bood*16);
  DIV_Mantissa = USARTDIV;
	DIV_Fraction = (USARTDIV - DIV_Mantissa) * 16;
	DIV_Mantissa<<=4;
	DIV_Mantissa+=DIV_Fraction; 
	USART1->BRR=DIV_Mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//�����ж�ʹ��
	USART1 ->CR1 |= 1 << 5;
	
	//����NVIC
	NVIC_SetPriorityGrouping (7 - 2);
	Priority = NVIC_EncodePriority (7 - 2, 1, 2);
	NVIC_SetPriority (USART1_IRQn ,Priority );
	NVIC_EnableIRQ (USART1_IRQn );//ʹ���ж�Դ
	
	/* ʹ��USART
	����λ�����㣬USART�ķ�Ƶ��������ڵ�ǰ�ֽڴ�����ɺ�ֹͣ�������Լ��ٹ��ġ���
	λ����������㣬������������ġ�
	*/
	//USART1->CR1 |= 1 << 13;  
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

/***************�ض���fputc����****************/
int fputc( int ch ,FILE *stream)
{
	USART1 ->DR = ch;
	while(!(USART1 ->SR & (1 << 7)))
	{
		//�ȴ��������
	}
	return ch;
}


#include "bsp_usart.h"


/*
�������ܣ�usart��ʼ��
����ֵ����
�βΣ�flck , bood, over8
��ע��PA9 -- RX   PA10 -- TX
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	float USARTDIV;    
	u32  DIV_Mantissa;   //����
  u32  DIV_Fraction;   //С��
	  /* GPIOʱ��ʹ�� */
	 RCC->AHB1ENR |= 1 << 0;    //GPIOAsʱ��ʹ��
	
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
	
	 /* ʹ��USART*/
	USART1->CR1 |= 1 << 13;  
}





//USART1��ʼ��
//pck��������	fck��ʱ������Ƶ��	over8��������
//void USART1_Init(u32 pck,u32 fck,u8 over8)
//{
//	float USARTDIV;
//	u32 DIV_Mantissa;
//	u32 DIV_Fraction;
//	
//	RCC->AHB1ENR |= 1 << 0;	//ʹ��GPIOAʱ��
//	
//	//PA9 -- TX
//	GPIOA->MODER &= ~( 3 << ( 2  * 9 ) );
//	GPIOA->MODER |= 2 << ( 2  * 9 );//����ģʽ
//	GPIOA->AFR[1] &= ~( 15 << (4 * ( 9 - 8 ) ) );
//	GPIOA->AFR[1]  |= 7 << (4 * ( 9 - 8 ) );//����Ϊ����1
//	
//	//PA10 -- TX
//	GPIOA->MODER &= ~( 3 << ( 2 * 10 ) );
//	GPIOA->MODER |= 2 << ( 2 * 10 );//����ģʽ
//	GPIOA->AFR[1] &= ~( 15 << ( 4 * ( 10 - 8 ) ) );
//	GPIOA->AFR[1]  |= 7 << (4 * ( 10 - 8 ) );//����Ϊ����1
//	
//	//���ô���1
//	RCC->AHB2ENR |= 1 << 4;//ʹ�ܴ���ʱ��
//	USART1->CR1 &= ~( 1 << 12 );//8λ����λ
//	USART1->CR2 &= ~( 3 << 12);//1λֹͣλ
//	USART1->CR1 |= 1 << 3;//����ʹ��
//	USART1->CR1 |= 1 << 2;//����ʹ��
//	
//	//���ò�����
//	if(over8 ==0)
//		USART1->CR1 &= ~( 1 << 15 );
//	else
//		USART1->CR1 |=  1 << 15;//8��������
//	
//	USARTDIV = fck * 1000000 / ( pck * 8 * ( 2 - over8 ));
//	
//	DIV_Mantissa = (u32)USARTDIV ;
//	DIV_Fraction = (USARTDIV - DIV_Mantissa ) * 16;
//	USART1 ->BRR = DIV_Mantissa << 4 | DIV_Fraction ;
//	
//	//ʹ�ܴ���
//	USART1 ->CR1 |= 1 << 13;
//	
//}

//static void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /* ����NVICΪ���ȼ���1 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//  /* �����ж�Դ*/
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//  /* ������ռ���ȼ���1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* ���������ȼ���1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* ʹ���ж�ͨ�� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//}

//void USER_USART_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	//��һ�� ��ʼ��GPIO
//	
//	GPIO_InitStructure .GPIO_OType = GPIO_OType_PP ;
//	GPIO_InitStructure .GPIO_PuPd = GPIO_PuPd_UP ;
//	GPIO_InitStructure .GPIO_Speed = GPIO_Speed_50MHz ;
//	
//	//����TX����Ϊ���ù���
//	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
//	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_9 ;
//	GPIO_Init(GPIOA,&GPIO_InitStructure );
//	
//	//����RX����Ϊ���ù���
//	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
//	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_10 ;
//	GPIO_Init(GPIOA,&GPIO_InitStructure );
//	
//	//GPIO���帴��ʲô�ڶ�����
//	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource9 ,GPIO_AF_USART1);
//	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource10 ,GPIO_AF_USART1);
//	
//	//�ڶ��� ���ô��ڳ�ʼ���ṹ��
//	USART_InitTypeDef UASRT_InitStructure;
//	//ʹ��USARTʱ��
//	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 ,ENABLE );
//	//���ô�DEBUG_USARTģʽ
//	//����������
//	UASRT_InitStructure.USART_BaudRate = 115200;
//	//�ֳ�
//	UASRT_InitStructure.USART_WordLength = USART_WordLength_8b ;
//	//ֹͣλ
//	UASRT_InitStructure.USART_StopBits = USART_StopBits_1 ;
//	//У��λѡ��
//	UASRT_InitStructure.USART_Parity = USART_Parity_No ;
//	//Ӳ��������
//	UASRT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
//	//USARTģʽ����
//	UASRT_InitStructure.USART_Mode = USART_Mode_Tx  | USART_Mode_Rx;
//	//���USART��ʼ������
//	USART_Init (USART1 ,&UASRT_InitStructure);
//	
//	//������ ���ڵĽ����ж�
//	//���� NVIC
//  NVIC_Configuration();
//	
//	//ʹ�ܴ��ڽ����ж�
//	USART_ITConfig(USART1 ,USART_IT_RXNE,ENABLE );
//	
//	//���Ĳ� ʹ�ܴ���
//	USART_Cmd(USART1 ,ENABLE );
//}

///*****************  ����һ���ַ� **********************/
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* ����һ���ֽ����ݵ�USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* �ȴ��������ݼĴ���Ϊ�� */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}


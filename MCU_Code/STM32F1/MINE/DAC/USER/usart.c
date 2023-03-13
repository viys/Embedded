#include "usart.h"

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	
	//��GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//�򿪴���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	
	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//���ô��ڵĲ���
	//���ò�����
	USART_Initstruct.USART_BaudRate = 115200;
	//����֡�����ֳ�
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;
	//����ֹͣλ
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	//����У��λ
	USART_Initstruct.USART_Parity = USART_Parity_No;
	//����Ӳ��������
	USART_Initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//���ù���ģʽ �շ�һ��
	USART_Initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_Initstruct);
	
	//ʹ�ܴ���
	USART_Cmd(USART1, ENABLE);
}
                                                                                          
///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����                                                
int fputc(int ch, FILE *f)                                                                         
{                                                                                                  
		/* ����һ���ֽ����ݵ����� */                                                                 
		USART_SendData(USART1, (uint8_t) ch);                                                      
		                                                                                           
		/* �ȴ�������� */                                                                          
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}



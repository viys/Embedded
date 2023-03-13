#include "usart.h"

static uint32_t USART_GPIO_CLK[USARTn] = {USART1_GPIO_CLK, USART2_GPIO_CLK, USART3_GPIO_CLK};
static uint32_t USART_CLK[USARTn] = {USART1_CLK, USART2_CLK, USART3_CLK};
static GPIO_TypeDef * USART_TX_PORT[USARTn] = {USART1_TX_GPIO_PORT, USART2_TX_GPIO_PORT, USART3_TX_GPIO_PORT};
static uint16_t USART_TX_PIN[USARTn] = {USART1_TX_GPIO_PIN, USART2_TX_GPIO_PIN, USART3_TX_GPIO_PIN};
static GPIO_TypeDef * USART_RX_PORT[USARTn] = {USART1_RX_GPIO_PORT, USART2_RX_GPIO_PORT, USART3_RX_GPIO_PORT};
static uint16_t USART_RX_PIN[USARTn] = {USART1_RX_GPIO_PIN, USART2_RX_GPIO_PIN, USART3_RX_GPIO_PIN};
static USART_TypeDef* USART_Which[USARTn] = {USART1_Which, USART2_Which, USART3_Which};
static uint8_t USART_NVIC_IRQChannel[USARTn] = {USART1_IRQ, USART2_IRQ, USART3_IRQ};

//�����ж����ȼ����ú���
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

//�������ú���
void USART_Config(usart_typedef_enum usart_num, usart_mode_typedef_enum usart_mode, u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	
	//��GPIOʱ��
	RCC_APB2PeriphClockCmd(USART_GPIO_CLK[usart_num], ENABLE);
	
	//�򿪴���ʱ��
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
	
	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStruct.GPIO_Pin = USART_TX_PIN[usart_num];
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TX_PORT[usart_num], &GPIO_InitStruct);
	
	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStruct.GPIO_Pin = USART_RX_PIN[usart_num];
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX_PORT[usart_num], &GPIO_InitStruct);
	
	//���ô��ڵĲ���
	//���ò�����
	USART_Initstruct.USART_BaudRate = baudrate;
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
	USART_Init(USART_Which[usart_num],&USART_Initstruct);
	
	//���ڽ����ж�
	if (usart_mode == USART_MODE_EXTI) {
        USART_NVIC_Config(usart_num);
    }
	
	//ʹ�ܴ���
	USART_Cmd(USART_Which[usart_num], ENABLE);
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


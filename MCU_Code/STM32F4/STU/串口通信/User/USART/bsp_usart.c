#include "bsp_usart.h"

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����NVICΪ���ȼ���1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* �����ж�Դ*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* ������ռ���ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void USER_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//��һ�� ��ʼ��GPIO
	
	GPIO_InitStructure .GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure .GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_InitStructure .GPIO_Speed = GPIO_Speed_50MHz ;
	
	//����TX����Ϊ���ù���
	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_9 ;
	GPIO_Init(GPIOA,&GPIO_InitStructure );
	
	//����RX����Ϊ���ù���
	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_10 ;
	GPIO_Init(GPIOA,&GPIO_InitStructure );
	
	//GPIO���帴��ʲô�ڶ�����
	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource9 ,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource10 ,GPIO_AF_USART1);
	
	//�ڶ��� ���ô��ڳ�ʼ���ṹ��
	USART_InitTypeDef UASRT_InitStructure;

	//���ô�DEBUG_USARTģʽ
	//����������
	UASRT_InitStructure.USART_BaudRate = 115200;
	//�ֳ�
	UASRT_InitStructure.USART_WordLength = USART_WordLength_8b ;
	//ֹͣλ
	UASRT_InitStructure.USART_StopBits = USART_StopBits_1 ;
	//У��λѡ��
	UASRT_InitStructure.USART_Parity = USART_Parity_No ;
	//Ӳ��������
	UASRT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
	//USARTģʽ����
	UASRT_InitStructure.USART_Mode = USART_Mode_Tx  | USART_Mode_Rx;
	//���USART��ʼ������
	USART_Init (USART1 ,&UASRT_InitStructure);
	//ʹ��USARTʱ��
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 ,ENABLE );
	
	//������ ���ڵĽ����ж�
	//���� NVIC
  NVIC_Configuration();
	
	//ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART1 ,USART_IT_RXNE,ENABLE );
	
	//���Ĳ� ʹ�ܴ���
	USART_Cmd(USART1 ,ENABLE );
}

/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


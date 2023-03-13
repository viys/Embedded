#include "bsp_usart.h"

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置中断源*/
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* 配置抢占优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void USER_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//第一步 初始化GPIO
	
	GPIO_InitStructure .GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure .GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_InitStructure .GPIO_Speed = GPIO_Speed_50MHz ;
	
	//配置TX引脚为复用功能
	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_9 ;
	GPIO_Init(GPIOA,&GPIO_InitStructure );
	
	//配置RX引脚为复用功能
	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_10 ;
	GPIO_Init(GPIOA,&GPIO_InitStructure );
	
	//GPIO具体复用什么第二功能
	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource9 ,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource10 ,GPIO_AF_USART1);
	
	//第二步 配置串口初始化结构体
	USART_InitTypeDef UASRT_InitStructure;

	//配置串DEBUG_USART模式
	//波特率设置
	UASRT_InitStructure.USART_BaudRate = 115200;
	//字长
	UASRT_InitStructure.USART_WordLength = USART_WordLength_8b ;
	//停止位
	UASRT_InitStructure.USART_StopBits = USART_StopBits_1 ;
	//校验位选择
	UASRT_InitStructure.USART_Parity = USART_Parity_No ;
	//硬件流控制
	UASRT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
	//USART模式控制
	UASRT_InitStructure.USART_Mode = USART_Mode_Tx  | USART_Mode_Rx;
	//完成USART初始化配置
	USART_Init (USART1 ,&UASRT_InitStructure);
	//使能USART时钟
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 ,ENABLE );
	
	//第三步 串口的接收中断
	//配置 NVIC
  NVIC_Configuration();
	
	//使能串口接收中断
	USART_ITConfig(USART1 ,USART_IT_RXNE,ENABLE );
	
	//第四步 使能串口
	USART_Cmd(USART1 ,ENABLE );
}

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


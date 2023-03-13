#include "bsp_usart.h"


/*
函数功能：usart初始化
返回值：无
形参：flck , bood, over8
备注：PA9 -- RX   PA10 -- TX
*/
void USART1_Init(u32 fclk, u32 bood, u8 over8)
{
	float USARTDIV;    
	u32  DIV_Mantissa;   //整数
  u32  DIV_Fraction;   //小数
	  /* GPIO时钟使能 */
	 RCC->AHB1ENR |= 1 << 0;    //GPIOAs时钟使能
	
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
	
	 /* 使能USART*/
	USART1->CR1 |= 1 << 13;  
}





//USART1初始化
//pck：波特率	fck：时钟总线频率	over8：过采样
//void USART1_Init(u32 pck,u32 fck,u8 over8)
//{
//	float USARTDIV;
//	u32 DIV_Mantissa;
//	u32 DIV_Fraction;
//	
//	RCC->AHB1ENR |= 1 << 0;	//使能GPIOA时钟
//	
//	//PA9 -- TX
//	GPIOA->MODER &= ~( 3 << ( 2  * 9 ) );
//	GPIOA->MODER |= 2 << ( 2  * 9 );//复用模式
//	GPIOA->AFR[1] &= ~( 15 << (4 * ( 9 - 8 ) ) );
//	GPIOA->AFR[1]  |= 7 << (4 * ( 9 - 8 ) );//复用为串口1
//	
//	//PA10 -- TX
//	GPIOA->MODER &= ~( 3 << ( 2 * 10 ) );
//	GPIOA->MODER |= 2 << ( 2 * 10 );//复用模式
//	GPIOA->AFR[1] &= ~( 15 << ( 4 * ( 10 - 8 ) ) );
//	GPIOA->AFR[1]  |= 7 << (4 * ( 10 - 8 ) );//复用为串口1
//	
//	//配置串口1
//	RCC->AHB2ENR |= 1 << 4;//使能串口时钟
//	USART1->CR1 &= ~( 1 << 12 );//8位数据位
//	USART1->CR2 &= ~( 3 << 12);//1位停止位
//	USART1->CR1 |= 1 << 3;//发送使能
//	USART1->CR1 |= 1 << 2;//接受使能
//	
//	//配置波特率
//	if(over8 ==0)
//		USART1->CR1 &= ~( 1 << 15 );
//	else
//		USART1->CR1 |=  1 << 15;//8倍过采样
//	
//	USARTDIV = fck * 1000000 / ( pck * 8 * ( 2 - over8 ));
//	
//	DIV_Mantissa = (u32)USARTDIV ;
//	DIV_Fraction = (USARTDIV - DIV_Mantissa ) * 16;
//	USART1 ->BRR = DIV_Mantissa << 4 | DIV_Fraction ;
//	
//	//使能串口
//	USART1 ->CR1 |= 1 << 13;
//	
//}

//static void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /* 配置NVIC为优先级组1 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//  /* 配置中断源*/
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//  /* 配置抢占优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* 配置子优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* 使能中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  
//}

//void USER_USART_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	//第一步 初始化GPIO
//	
//	GPIO_InitStructure .GPIO_OType = GPIO_OType_PP ;
//	GPIO_InitStructure .GPIO_PuPd = GPIO_PuPd_UP ;
//	GPIO_InitStructure .GPIO_Speed = GPIO_Speed_50MHz ;
//	
//	//配置TX引脚为复用功能
//	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
//	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_9 ;
//	GPIO_Init(GPIOA,&GPIO_InitStructure );
//	
//	//配置RX引脚为复用功能
//	GPIO_InitStructure .GPIO_Mode = GPIO_Mode_AF ;
//	GPIO_InitStructure .GPIO_Pin = GPIO_Pin_10 ;
//	GPIO_Init(GPIOA,&GPIO_InitStructure );
//	
//	//GPIO具体复用什么第二功能
//	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource9 ,GPIO_AF_USART1);
//	GPIO_PinAFConfig(GPIOA ,GPIO_PinSource10 ,GPIO_AF_USART1);
//	
//	//第二步 配置串口初始化结构体
//	USART_InitTypeDef UASRT_InitStructure;
//	//使能USART时钟
//	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 ,ENABLE );
//	//配置串DEBUG_USART模式
//	//波特率设置
//	UASRT_InitStructure.USART_BaudRate = 115200;
//	//字长
//	UASRT_InitStructure.USART_WordLength = USART_WordLength_8b ;
//	//停止位
//	UASRT_InitStructure.USART_StopBits = USART_StopBits_1 ;
//	//校验位选择
//	UASRT_InitStructure.USART_Parity = USART_Parity_No ;
//	//硬件流控制
//	UASRT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
//	//USART模式控制
//	UASRT_InitStructure.USART_Mode = USART_Mode_Tx  | USART_Mode_Rx;
//	//完成USART初始化配置
//	USART_Init (USART1 ,&UASRT_InitStructure);
//	
//	//第三步 串口的接收中断
//	//配置 NVIC
//  NVIC_Configuration();
//	
//	//使能串口接收中断
//	USART_ITConfig(USART1 ,USART_IT_RXNE,ENABLE );
//	
//	//第四步 使能串口
//	USART_Cmd(USART1 ,ENABLE );
//}

///*****************  发送一个字符 **********************/
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* 发送一个字节数据到USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* 等待发送数据寄存器为空 */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}


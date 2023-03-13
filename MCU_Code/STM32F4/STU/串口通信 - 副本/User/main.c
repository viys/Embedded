#include "stm32f4XX.h"

#include "bsp_led.h"
#include "bsp_usart.h"

int main(void)
{
	/* 此处写入程序 */
	u8 rec;
	
//	USER_USART_Config();
//	Usart_SendByte( USART1, 'a');
	USART1_Init(84,115200,0);
	while(1)
	{
		while(!(USART1 ->SR & ( 1 << 5)));//等待接收完成
		rec = USART1 ->DR ;
		
		USART1 ->DR = rec;
		while(!(USART1 ->SR & ( 1 << 7)));//等待发送完成
	}
}

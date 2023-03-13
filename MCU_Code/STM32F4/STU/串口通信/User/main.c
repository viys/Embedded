#include "stm32f4XX.h"

#include "bsp_led.h"
#include "bsp_usart.h"

int main(void)
{
	/* 此处写入程序 */
	
	USER_USART_Config();
	Usart_SendByte( USART1, 'a');
	while(1)	
	{
		LED_TOGGLE;
	}
}

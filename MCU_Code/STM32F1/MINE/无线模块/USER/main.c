#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"

int main()
{
	USART_Config(USART_1,USART_MODE_COMMON,115200);
	while(1)
	{
		SysTick_Delay(100);
		printf("hello");
	}
}

#include "stm32f10x.h"
#include "systick.h"
#include "tim.h"
#include "usart.h"
#include <stdio.h>

int main()
{


	// TIM 计数器的驱动时钟
	
	USART_Config();
	GENERAL_TIM_Init();

	Tim4_Init();
	printf("HELLO\r\n");
	SysTick_Delay(1000);
	while(1)
	{
		
	}
}

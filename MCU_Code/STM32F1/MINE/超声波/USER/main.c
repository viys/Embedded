#include "stm32f10x.h"
#include "systick.h"
#include "tim.h"
#include "usart.h"
#include <stdio.h>

int main()
{


	// TIM ������������ʱ��
	
	USART_Config();
	GENERAL_TIM_Init();

	Tim4_Init();
	printf("HELLO\r\n");
	SysTick_Delay(1000);
	while(1)
	{
		
	}
}

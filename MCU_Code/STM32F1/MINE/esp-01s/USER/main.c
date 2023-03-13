#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"

int main()
{
	USART_Config(USART_3,USART_MODE_EXTI,115200);
	USART_Config(USART_1,USART_MODE_EXTI,115200);
	SysTick_Delay(4000);
	u3_printf("AT+CWMODE=3\r\n");
	SysTick_Delay(1000);
	u3_printf("AT+CWJAP_CUR=""CMCC1","Wxyz100%1014.""\r\n");
	
	
	while(1)
	{
//		u3_printf("HELLO\r\n");
//		SysTick_Delay(100);
	}
}


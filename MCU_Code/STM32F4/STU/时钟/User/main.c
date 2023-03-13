#include "stm32f4XX.h"
#include "bsp_led.h"
#include "bsp_systick.h"

int main(void)
{
	/* 此处写入程序 */
	LED_GPIO_Config ();
	
	//SysTick_Init();
	
	while(1)
	{
		LED_TOGGLE;
		Delay_Ms(500);
		LED_TOGGLE;
		Delay_Ms(500);
	}
}


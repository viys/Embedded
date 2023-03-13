#include "stm32f10x.h"
#include "led.h"

int main()
{
	LED_GPIO_Config(LED1);
	LED_State_Config(LED1,OFF);
	while(1)
	{
		
	}
}

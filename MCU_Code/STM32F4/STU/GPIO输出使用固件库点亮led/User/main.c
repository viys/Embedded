#include "stm32f4xx.h"
#include "bsp_led.h"
void Delay(uint32_t count)
{
	for(;count!=0;count--);
}
int main(void)
{
	/* 此处写入程序 */
	
	
	LED_GPIO_Config1();
	LED_GPIO_Config2();
	LED_GPIO_Config3();
	LED_GPIO_Config4();
	PIN_GPIO_Config1();
	PIN_GPIO_Config2();
	PIN_GPIO_Config3();
	
	
	
	while(1)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		Delay(0xffffff);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		//Delay(0xffffff);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		Delay(0xffffff);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		//Delay(0xffffff);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		Delay(0xffffff);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		//Delay(0xffffff);
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		Delay(0xffffff);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
		//Delay(0xffffff);
	}

}


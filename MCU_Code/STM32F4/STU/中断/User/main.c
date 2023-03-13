#include "stm32f4xx.h"
#include "bsp_led.h"
#include "bsp_exti.h"

void Delay(uint32_t count)
{
	for(;count!=0;count--);
}
int main(void)
{
	/* 此处写入程序 */
	
	LED_GPIO_Config1();
	
	EXTI_Key_Config();

	while(1)
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
//		Delay(0xffffff);
//		GPIO_SetBits(GPIOC, GPIO_Pin_4);
//		Delay(0xffffff);
//		EXTI0_IQRHandler();
	}

}


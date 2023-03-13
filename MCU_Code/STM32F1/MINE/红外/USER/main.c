#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructrue;
	
	GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructrue.GPIO_Pin = GPIO_Pin_1;
	
	GPIO_Init(GPIOA,&GPIO_InitStructrue);
}

int main()
{
	GPIO_Config();
	USART_Config();
	printf("!!!!!!!!!!!!!!!!!!!!!!!!1\r\n");
	while(1)
	{
		if(Bit_RESET == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
			
		}
	}
}

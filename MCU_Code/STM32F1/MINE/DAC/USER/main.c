#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "adc.h"

extern uint16_t ADC_ConvertedValue;
float ADC_ConvertedValue_Local;

int main()
{
	float vol_1;
	USART_Config();
	ADCx_Init();//pc1
	DAC1_Init();//pa4
	DAC1_Set_Vol(1000);
	printf("hello\r\n");
	
	while(1)
	{
		ADC_ConvertedValue_Local = (float)ADC_ConvertedValue;
		vol_1 = ADC_ConvertedValue_Local/4095*3.3;
		printf("µÁ—π÷µ :%f\r\n",vol_1);
		SysTick_Delay(1000);
	}
}

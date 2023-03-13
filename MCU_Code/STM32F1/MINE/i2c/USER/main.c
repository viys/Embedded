#include "stm32f10x.h"
#include "usart.h"
#include "systick.h"
#include "i2c.h"

uint8_t *data1;
uint8_t *data2;

int main()
{

	
	USART_Config(USART_1,USART_MODE_COMMON,115200);
	printf("hello\r\n");
	I2C1_Init();
	
	while(1)
	{
		BH1750FVI_Send(BH1750_POWERON);
		BH1750FVI_Send(BH1750_MEASUERMENT);
		SysTick_Delay(200);
		BH1740FVI_Mread(data1,data2);
		printf("1:%d 2:%d\r\n",*data1,*data2);
		SysTick_Delay(500);
	}
	
}

#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"
#include "gpio.h"

void Fivedianji(uint8_t *table);
void MotorCW(void);

uint8_t table1[4] = {0x03,0x06,0x0c,0x09};
uint8_t table2[4] = {0x03,0x09,0x0c,0x06};

uint16_t phasecw[4] = {GPIO_Pin_4,GPIO_Pin_3,GPIO_Pin_2,GPIO_Pin_1};
uint16_t phaseccw[4] = {GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4};

int main()
{
	USART_Config(USART_1,USART_MODE_COMMON,115200);
	GPIO_Config();

	while(1)
	{
		printf("HELLO\r\n");
		//SysTick_Delay(50);//等待50ms			
		for(uint16_t i=0;i<500;i++)
		{
		MotorCW();   //正转
		} 
//		Fivedianji(table1);
	}
}

void Fivedianji(uint8_t *table)
{
	for(int i=0; i<4; i++)
	{
		GPIO_Write(GPIOA,*table);
		SysTick_Delay(4);
		GPIO_Write(GPIOA,0x0000);
		table += i;
		
	}
}

void MotorCW(void)
{
 for(uint8_t i=0;i<4;i++)
  {
		GPIO_WriteBit(GPIOA,phasecw[i],Bit_SET);//输出高
		SysTick_Delay(10);//等待5ms
		GPIO_WriteBit(GPIOA,phasecw[i],Bit_RESET);//输出低
  }
}

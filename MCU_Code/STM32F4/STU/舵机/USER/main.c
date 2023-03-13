#include <string.h>
#include <stdio.h>
#include "led.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "nvic.h"
#include "adc.h"
#include "systick.h"
#include "time6.h"
#include "time_gen.h"
#include "time.h"
#include "adc.h"
#include "method.h"

u8 keystate;
u8 KEYSTATE = 0;
u8 rcc = 0;
int main(void )
{
	LED_Init (LED_ALL);
	USART1_Init(84,115200,0);
	KEY_Init (KEY_ALL);
	TIM14_CH1_Init(84,2000);
	ADC_Init();
	SysTick_inter_ms(50);
	TIM10_CH1_Init(168,2,1);
	while(1)
	{		
//		rcc = 185;
//		SysTick_ms(300);
//		rcc = 175;
//		SysTick_ms(300);
	}
}

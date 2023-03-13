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

u8 keystate;
u8 KEYSTATE = 0;
int main(void )
{
	LED_Init (LED_ALL);
	USART1_Init(84,115200,0);
	KEY_Init (KEY_ALL);
	TIM10_CH1_Init(168,10000,5000);//10ms
	TIM14_CH1_Init(86,2000);//20ms
	
//	Time6_inter_Init(500);
//	Time6_Init();
//	SysTick_inter_ms(500);
//	while(0 == (keystate = key_Scan ()));
//	LED_Running(keystate, 2);
//	printf ("KEY%d\r\n",keystate );
	while(1)
	{
			printf ("high = %dus\r\n",high);
			printf ("low = %dus\r\n",low);
//		if(width)
//		{
//			printf ("width =%d \r\n",width  );
//			SysTick_ms(10);
//		}
		//SysTick_ms(500);
		//LED1_TOGGLE;
//			if(0 != (keystate = key_Scan ()))
//			{
//				printf ("LED%d",keystate );
//				LED_Running (keystate,2);
//			}
//		for(int i=0;i<1000;i++)
//		{
//			TIM3_CH1_Init(86,1000,i);
//			SysTick_ms (1);
//		}
//		for(int i=1000;i>0;i--)
//		{
//			TIM3_CH1_Init(86,1000,i);
//			SysTick_ms (1);
//		}
	}
}

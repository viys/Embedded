#include "nvic.h"
#include "systick.h"
#include "led.h"
#include "usart.h"
#include "key.h"
#include "adc.h"
#include "time_gen.h"

extern u8 keystate;
char rec[20] = {0};
u8 i =0;
u8 flag = 0;//表示接受状态  1：接受完成
//中断服务函数
/*************************内部中断*************************/
void SysTick_Handler(void)
{
	float Vx = 0;
	u16 temp_light = 0;
	if(SysTick ->CTRL & (1 << 16))
	{	
		temp_light = ADC_GETD();
		Vx = (3.3 * temp_light) / 4096;
		printf ("光强=%.1f\r\n",3.3-Vx);
		printf ("low = %d\r\n",low);
		TIM10_CH1_Init(168,330,((3.3-Vx)*100));
	}
}

/*************************外部中断*************************/

void USART1_IRQHandler(void)	 //串口1中断服务函数
{
	if(USART1 ->SR & (1 << 5))
	{
		USART1 ->SR &= ~(1 << 5);//接收中断标志位清零
		rec[i] = USART1 ->DR ;
		
		if(rec[i] == '#' && flag == 0)
		{
			i = 0;
			flag = 1;
		}
		else
		{
			i++;
		}
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if(TIM6 ->SR & (1 << 0))
	{
		TIM6 ->SR &= ~(1 << 0);//清除标志位
		LED4_TOGGLE;
	}
}

//u8 width;//脉冲宽度

//void TIM8_TRG_COM_TIM14_IRQHandler(void)
//{
//	static u16 CNT1;
//	u16 CNT2;
//	
//	if(TIM14 ->SR & (1 << 1))	//上升沿捕获
//	{
//		TIM14 ->SR &= ~(1 << 1); //清理标志位
//		CNT2 = TIM14 ->CCR1;
//		if(CNT2 < CNT1 )
//		{
//			width = 10000 - CNT1 + CNT2 ;
//		}
//		else
//		{
//			width = CNT2 - CNT1 ;
//		}
//		CNT1 = CNT2 ;
//	}
//	if(TIM14 ->SR & (1 << 0))	//更新中断
//	{
//		TIM14 ->SR &= ~(1 << 0);
//	}
//}
u16 high =0;
u16 low = 0;
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	static u8 tim14_flag = 1;
	static u8 update_flag = 0;
	if(TIM14 ->SR & (1 << 1))	
	{
		TIM14 ->CNT = 0;//保证从0开始计数
		TIM14 ->SR &= ~(1 << 1);//标志位tim		if(TIM14_flag)//产生上升沿
		if(tim14_flag )	
		{
			tim14_flag = 0;
			low = update_flag * 2000 + TIM14 ->CCR1;//捕获低电平的时间
		}
		else
		{
			tim14_flag = 1;
			high = update_flag * 2000 + TIM14 ->CCR1;
		}
			update_flag = 0;
			TIM14 ->CCER ^= 1 << 1;//边沿切换
		}
		if(TIM14 ->SR & (1 << 0))
		{
			update_flag ++;
			TIM14 ->SR &= ~(1 << 0);
		}
}
/************中断函数中最好不要加延迟和循环语句*************/


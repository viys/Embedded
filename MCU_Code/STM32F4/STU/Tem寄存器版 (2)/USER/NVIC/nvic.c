#include "nvic.h"
#include "systick.h"

char rec[20] = {0};
u8 i =0;
u8 flag = 0;//表示接受状态  1：接受完成
//中断服务函数
/*************************内部中断*************************/
void SysTick_Handler(void )
{
	if(SysTick ->CTRL & (1 << 16))
	{
		GPIOC->ODR ^= 1 << 4;
	}
}

/*************************外部中断*************************/

void USART1_IRQHandler(void)//串口1中断服务函数
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
		GPIOC ->ODR ^= 1 << 4;
	}
}

u8 width;//脉冲宽度

void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 CNT1;
	u16 CNT2;
	if(TIM9 ->SR & (1 << 1))//上升沿捕获
	{
		TIM9 ->SR &= ~(1 << 1);//清理标志位
		CNT2 = TIM9 ->CCR1 ;
		if(CNT2 < CNT1 )
		{
			width = 0xffff - CNT1 + CNT2 ;
		}
		else
		{
			width = CNT2 - CNT1 ;
		}
		CNT1 = CNT2 ;
	}
	if(TIM9 ->SR & (1 << 0))//更新中断
	{
		TIM9 ->SR &= ~(1 << 0);
	}
}

//中断函数中最好不要加延迟和循环语句

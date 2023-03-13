#include "nvic.h"
#include "systick.h"


/**
@brief	滴答定时器中断服务函数
@Note		根据ADC捕获参数更新占空比
@param	None
@retval	None
*/
void SysTick_Handler(void)
{
	if(SysTick ->CTRL & (1 << 16))
	{	
		/* code */
	}
}

char rec[20] = {0};
u8 i =0;
u8 flag = 0;//表示接受状态  1：接受完成

/**
@brief	串口1中断服务函数
@Note		
@param	None
@retval	None
*/
void USART1_IRQHandler(void)
{
	if(USART1 ->SR & (1 << 5))
	{
		USART1 ->SR &= ~(1 << 5);//接收中断标志位清零
		rec[i] = USART1 ->DR ;
		
		if(rec[i] == '#' && flag == 0)
		{
			i = 0;
			flag = 1;
			/* code */
		}
		else
		{
			i++;
		}
	}
}

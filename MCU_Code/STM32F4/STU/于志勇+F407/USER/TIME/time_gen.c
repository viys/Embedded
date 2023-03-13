#include "time_gen.h"

/**
@brief	Time10初始化
@Note	  PB8复用为TIM10	
@param	（预分频 重装载值 比较值）
@retval	None
*/
void TIM10_CH1_Init(u16 psc, u16 arr, u16 ccr)
{
	/*配置定时器对应的GPIO*/
	RCC ->AHB1ENR |= 1 << 1;						//使能GPIOB的时钟
	
	GPIOB ->MODER &= ~(3 << (2 * 8));
	GPIOB ->MODER |= (2 << (2 * 8));		//复用模式
	    
	GPIOB ->AFR [1] &= ~(15 << (4 * (8 - 8)));
	GPIOB ->AFR [1] |= 3 << (4 * (8 - 8));		//复用为Tim10
	/*配置计数器*/
	RCC ->APB2ENR |= 1 << 17;           //使能Tim10时钟
	TIM10 ->CR1 |= 1 << 7;	            //使用影子寄存器
	TIM10 ->CR1 &= ~(1 << 3);           //循环计数
	TIM10 ->CR1 &= ~(1 << 2);           //更新请求源
	TIM10 ->CR1 &= ~(1 << 1);           //更新使能
	/*设置预分频值*/
	TIM10 ->PSC = psc - 1; 				   		//设定的预分频值
	TIM10 ->ARR = arr - 1;		 				  //设定的计数次数
	TIM10 ->CCR1 = ccr;	   				   		//设置占空比
	TIM10 ->EGR |= 1 << 0; 				   		//把设定的值更新到影子寄存器
	TIM10->SR  &=~(1<<0);  //清除更新标志位 
	/*配置PWM的工作方式*/
	TIM10 ->CCMR1 &= ~(1 << 7);         //不受外部时钟的影响
	TIM10 ->CCMR1 |= 7 << 4;	          //PWM模式2
	TIM10 ->CCMR1 &= ~(3 << 0);         //通道1配置为输出模式
	TIM10 ->CCER &= ~(1 << 1);          //高电平有效
	TIM10 ->CCER |= 1 << 0;             //使能输出通道								          
	/*使能计数器*/
	TIM10 ->CR1 |= 1 << 0;
	
}

/**
@brief	TIM14中断初始化
@Note		PA7复用为TIM14
@param	预分频 重装载值
@retval	None
*/
void TIM14_CH1_Init(u16 psc, u16 arr)
{
	/*配置定时器对应的GPIO*/
	RCC ->AHB1ENR |= 1 << 0;									//使能GPIOA时钟
	
	GPIOA ->MODER &= ~(3 << (2 * 7));
	GPIOA ->MODER |= (2 << (2 * 7));					//复用模式
	GPIOA ->AFR [0] &= ~(15 << (4 * 7));
	GPIOA ->AFR [0] |= 9 << (4 * 7);					//复用为Time14
	
	/*配置计数器*/
	RCC ->APB1ENR |= 1 << 8;//使能Time14时钟
	TIM14 ->CR1 |= 1 << 7;	 //使用影子寄存器
	TIM14 ->CR1 &= ~(1 << 3);//循环计数
	TIM14 ->CR1 &= ~(1 << 2);//更新请求源
	TIM14 ->CR1 &= ~(1 << 1);//更新使能
	
	/*设置预分频值*/
	TIM14 ->PSC = psc - 1;//设定的预分频值
	TIM14 ->ARR = arr;		//设定的计数次数
	TIM14 ->EGR |= 1 << 0;//把设定的值更新到影子寄存器
	TIM14 ->CNT =0;				//计数器清零
	
	/*配置输入捕获工作方式*/
	TIM14 ->CCMR1 &= ~(15 << 4);//无滤波
	TIM14 ->CCMR1 &= ~(3 << 2);	//无分频
	TIM14 ->CCMR1 |= 1 << 0;    //配置为输入模式
	TIM14 ->CCER &= ~(1 << 3);	
	TIM14 ->CCER &= ~(1 << 1);	//上升沿捕获
	
	/*配置捕获中断*/
	TIM14 ->DIER |= 1 << 1;	                  //通道1捕获中断使能
	TIM14 ->DIER |= 1 << 0;			        	  	//更新中断使能
	NVIC_EnableIRQ (TIM8_TRG_COM_TIM14_IRQn); //中断使能
	
	TIM14 ->CCER |= 1 << 0;                   //通道1使能
	
	/*计数器使能*/
	TIM14 ->CR1 |= 1 << 0;
}

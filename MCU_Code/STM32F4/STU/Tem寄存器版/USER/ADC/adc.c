#include "adc.h"

#if 0
//PA4 -- ADC1_IN4 
void ADC_Init(void )
{
	RCC ->AHB1ENR |= 1 << 0;//使能GPIOA时钟
	
	GPIOA ->MODER &= ~(3 << (2 * 4));
	GPIOA ->MODER |= 3 << (2 * 4);//模拟输入模式
	
	//ADC设置
	RCC ->APB2ENR |= 1 << 8;//使能ADC1时钟
	
	ADC1 ->CR1 &= ~(3 << 24);//12位分辨率
	
	ADC1 ->CR1 &= ~(1 << 8);//禁止扫描
	
	ADC1 ->CR2 &= ~(1 << 11);//右对齐
	
	ADC1 ->CR2 |= 1 << 10;//使能EOC标志位
	
	ADC1 ->CR2 &= ~(1 << 1);//单次转换
	
	ADC1 ->SMPR2 &= ~(7 << 12);
	ADC1 ->SMPR2 |= 7 << 12;//480个周期
	
	//设置转换序列和哪个转换序列
	ADC1 ->SQR1 &= ~(15 << 20);//一个序列
	ADC1 ->SQR3 |= 4 << 0;//通道4
	
	ADC1 ->CR2 |= 1 << 0;//使能ADC1
}

void ADC1_CH5_Init(void)
{
	/* GPIO初始化 */
	RCC->AHB1ENR |= 1 << 0;   //GPIA时钟使能
	GPIOA->MODER &= ~(3 << (2 * 5));
	GPIOA->MODER |= 3 << (2 * 5);  //模拟输入
	
	/* ADC工作方式 */
	RCC->APB2ENR |= 1 << 8;    //使能ADC1时钟
	ADC1->CR1 &= ~(3 << 24);   //12位分辨率
	ADC1->CR2 &= ~(3 << 28);   //使用软件触发
	ADC1->CR2 &= ~(1 << 11);   //右对齐
	ADC1->CR2 |= 1 << 10;      //使能规则EOC标志位
	ADC1->CR2 &= ~(1 << 1);    //单次转换
	
	/* 设置预分频和时间周期 */
	ADC->CCR &= ~(3 << 16);
	ADC->CCR |= 1 << 16;       //4分频
	ADC1->SMPR2 &= ~(7 << 15);
	ADC1->SMPR2 |= 7 << 15;    //480个周期采样时间
	
	/* 设置转换次数 和 转换通道 */
	ADC1->SQR1 &= ~(15 << 20);    //1次转换
	ADC1->SQR3 &= ~(0x1f << 0);
	ADC1->SQR3 |= 5 << 0;          //通道5
	
	ADC1->CR2 |= 1 << 0;     //使能ADC1
}

//ADC转换
u16 ADC_GETD(void )
{
	u16 data;
	ADC1 ->CR2 |= 1 << 30;//规则组开关使能
	
	while(!(ADC1 ->SR & (1 << 1)))
	{
		//等待转换完成
	}
	ADC1 ->SR &= ~(1 << 11);//标志位清零
	data = ADC1 ->DR ;
	
	return data;
}
#endif

#if 1

/*
函数的功能：ADC初始化
形参：void
返回值：void
备注：PA4-----ADC1_IN4
*/

void ADC_Init(void)
{
	/* GPIO口的配置 */
	RCC->AHB1ENR |= 1<<0;//使能GPIOA时钟
	
	GPIOA->MODER &= ~(3<<(2*4));//模式清零
	
	GPIOA->MODER |= 3<<(2*4);//模拟输入模式
	
	/*ADC的配置*/
	RCC ->APB2ENR |= 1<<8;//使能ADC时钟
	
	ADC1 ->CR1 &= ~(3 <<24); //12位分辨率
	ADC1 ->CR1 &= ~(1<<8);   //禁止扫描
	
	ADC1 ->CR2 &= ~(1<<11);  //右对齐
	ADC1 ->CR2 |= 1<<10;     //使能EOC标志位
	ADC1 ->CR2 &= ~(1<<1);   //单次转换
	

	ADC1 ->SMPR2 &= ~(7<<12);//清零速度设置位	
	ADC1 ->SMPR2 |= 7<<12 ;  //设置速度位480
	/*设置转换序列个数和哪个序列*/
	ADC1->SQR1 &= ~(15<<20);
	ADC1->SQR3 |= 4 << 0;
	ADC1 ->CR2 |= 1 << 0;


}


/*
函数的功能：ADC转换
形参：void
返回值：12位数字量
备注：PA4-----ADC1_IN4
*/
u16 ADC_GETD(void)
{
	u16 date;
	ADC1->CR2 |= 1<<30;//规则组使能
	
	while(!(ADC1->SR &(1 << 1)))
	{
	//等待转换完成
	}
	ADC1->SR &=~(1<<11);//清零标志位
	date =  ADC1->DR ;
	return date;                                        
}


#endif

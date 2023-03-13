#include "adc.h"
/*
函数的功能：ADC初始化
形参：void
返回值：void
备注：PC4-----ADC12_IN14
*/

void ADC1_Init(void)
{
	/* GPIO口的配置 */
	RCC->APB2ENR |= 1<<4;//使能GPIOC时钟
	GPIOC->CRL &= ~(15 << (4*4));//模式清零模拟输入模式
	RCC ->APB2ENR |= 1 << 9;//使能ADC1时钟
	RCC->APB2RSTR |= 1 << 9;//ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束
	
	RCC ->CFGR &= ~(3 << 14);//更改adc时钟频率为六分频(12MHZ)
	RCC ->CFGR |= (2 << 14);
	
	ADC1->CR1 &= 0XF0FFFF; //工作模式清零
	ADC1->CR1 |= 0 << 16; //独立工作模式
	ADC1->CR1 &= ~(1 << 8);   //禁止扫描
	ADC1->CR1 &= ~(31 << 0);//选择通道14
	ADC1->CR1 |= (14 << 0);
	
	ADC1->CR2 &= ~(1 << 1); //单次转换模式
	ADC1->CR2 &= ~(7 << 17);//软件控制转换 
  ADC1->CR2 |= 7 << 17; 
	ADC1->CR2 |= 1 << 20; //使用用外部触发(SWSTART)!!! 必须使用一个事件来触发
	ADC1->CR2 &= ~(1 << 11); //右对齐

	
	ADC1 ->CR2 |= 1<<5;     //使能EOC标志位
	ADC1 ->CR2 &= ~(1<<1);   //单次转换 ***  连续转换模式
	
	ADC1->SMPR1 &= ~(7 << 12);
	ADC1->SMPR1 |= (7 << 12);
	
	ADC1 ->SQR1 &= ~(15<<20);
	
	ADC1 ->CR2 |= 1 << 0;//ADC的使能
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1|=0<<20; //1 个转换在规则序列中 也就是只转换规则序列 1
	//设置通道 1 的采样时间
	ADC1->SMPR2&=~(3*1); //通道 1 采样时间清空 
	ADC1->SMPR2|=7<<(3*1); //通道 1 239.5 周期,提高采样时间可以提高精确度
	ADC1->CR2|=1<<0; //开启 AD 转换器
	ADC1->CR2|=1<<3; //使能复位校准 
	while(ADC1->CR2&1<<3); //等待校准结束
	ADC1->CR2|=1<<2; //开启 AD 校准 
	while(ADC1->CR2&1<<2); //等待校准结束

}

/*
函数的功能：ADC初始化
形参：void
返回值：void
备注：PC4-----ADC12_IN14
*/

void ADC2_Init(void)
{
	/* GPIO口的配置 */
	RCC->APB2ENR |= 1<<4;//使能GPIOC时钟
	GPIOC->CRL &= ~(15 << (4*4));//模式清零模拟输入模式
	RCC ->APB2ENR |= 1 << 9;//使能ADC1时钟
	RCC->APB2RSTR |= 1 << 9;//ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束
	
	RCC ->CFGR &= ~(3 << 14);//更改adc时钟频率为六分频(12MHZ)
	RCC ->CFGR |= (2 << 14);
	
	ADC1->CR1 &= 0XF0FFFF; //工作模式清零
	ADC1->CR1 |= 0 << 16; //独立工作模式
	ADC1->CR1 &= ~(1 << 8);   //禁止扫描
	ADC1->CR1 &= ~(31 << 0);//选择通道14
	ADC1->CR1 |= (14 << 0);
	
	ADC1->CR2 &= ~(1 << 1); //单次转换模式
	ADC1->CR2 &= ~(7 << 17);//软件控制转换 
  ADC1->CR2 |= 7 << 17; 
	ADC1->CR2 |= 1 << 20; //使用用外部触发(SWSTART)!!! 必须使用一个事件来触发
	ADC1->CR2 &= ~(1 << 11); //右对齐

	
	ADC1 ->CR2 |= 1<<5;     //使能EOC标志位
	ADC1 ->CR2 &= ~(1<<1);   //单次转换 ***  连续转换模式
	
	ADC1->SMPR1 &= ~(7 << 12);
	ADC1->SMPR1 |= (7 << 12);
	
	ADC1 ->SQR1 &= ~(15<<20);
	
	ADC1 ->CR2 |= 1 << 0;//ADC的使能
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1|=0<<20; //1 个转换在规则序列中 也就是只转换规则序列 1
	//设置通道 1 的采样时间
	ADC1->SMPR2&=~(3*1); //通道 1 采样时间清空 
	ADC1->SMPR2|=7<<(3*1); //通道 1 239.5 周期,提高采样时间可以提高精确度
	ADC1->CR2|=1<<0; //开启 AD 转换器
	ADC1->CR2|=1<<3; //使能复位校准 
	while(ADC1->CR2&1<<3); //等待校准结束
	ADC1->CR2|=1<<2; //开启 AD 校准 
	while(ADC1->CR2&1<<2); //等待校准结束

}


u16 Get_Adc(u8 ch) 
{
//设置转换序列 
ADC1->SQR3&=0XFFFFFFE0;//规则序列 1 通道 ch
ADC1->SQR3|=ch; 
ADC1->CR2|=1<<22; //启动规则转换通道
while(!(ADC1->SR&1<<1)); //等待转换结束 
return ADC1->DR; //返回 adc 值 
//获取
}
/*
函数的功能：ADC转换
形参：void
返回值：12位数字量
备注：PA4-----ADC1_IN4
*/
u16 Get_Adc_Average(u8 ch,u8 times)
{
u32 temp_val=0;
u8 t;
for(t=0;t<times;t++)
{
temp_val+=Get_Adc(ch);
}
return temp_val/times;
}

u16 ADC_GETD(void)
{
	u16 date;
	ADC1->CR2 |= 1<<22;//规则组使能
	
	while(!(ADC1->SR &(1 << 1)))
	{
	//等待转换完成
	}
	ADC1->SR &=~(1<<11);//清零标志位
	date =  ADC1->DR ;
	return date;                                        
}



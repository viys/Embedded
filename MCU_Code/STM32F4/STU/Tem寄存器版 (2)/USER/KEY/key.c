#include "key.h"
#include "systick.h"

/*
函数功能：key初始化
返回值：无
形参：无
备注：key1 -- PA0  没有按下是低电平  按下为高电平
*/
void KEY_Init(void)
{
	/* 时钟使能 */
	RCC->AHB1ENR |= 1 << 0;  //使能GPIOA时钟
	RCC->AHB1ENR |= 1 << 4;  //使能GPIOE时钟
	
	/* 设置GPIO模式 */
	GPIOA->MODER &= ~(3 << (2 * 0)); //输入模式
	/* 设置上下拉 */
	GPIOA->PUPDR &= ~(3 << (2 * 0));  //无上下拉
	
		/* 设置GPIOE.2模式 */
	GPIOE->MODER &= ~(3 << (2 * 2)); //输入模式
	/* 设置上下拉 */
	GPIOE->PUPDR &= ~(3 << (2 * 2));
	
	
		/* 设置GPIOE.3模式 */
	GPIOE->MODER &= ~(3 << (2 * 3)); //输入模式
	/* 设置上下拉 */
	GPIOE->PUPDR &= ~(3 << (2 * 3));
	
	
		/* 设置GPIOE.4模式 */
	GPIOE->MODER &= ~(3 << (2 * 4)); //输入模式
	/* 设置上下拉 */
	GPIOE->PUPDR &= ~(3 << (2 * 4));
	
}


/*
函数功能：key扫描
返回值：1,2,3,4
形参：无
备注：key1 -- PA0   没有按下是低电平  按下为高电平
      key2 -- PE2   没有按下是高电平  按下为低电平
      key3 -- PE3   没有按下是高电平  按下为低电平
			key4 -- PE4   没有按下是高电平  按下为低电平
*/
u8 key_Scan(void)
{
	  static u8 flag = 1;
	   u8 key = 0;
	
	  if((KEY1 || KEY2 || KEY3 || KEY4) && (flag == 1))
		{ 
			  SysTick_ms(10); //10ms-20ms
			  if(KEY1)
				{
					  flag = 0;  //标志位清零
					  key = 1;
				}
				else if(KEY2)
				{
					 flag = 0;
					 key = 2;
				}
				else if(KEY3)
				{
					 flag = 0;
					 key = 3;
				}
				else if(KEY4)
				{
					 flag = 0;
					 key = 4;
				}
		}
		
		if(!KEY1 && !KEY2 && !KEY3 && !KEY4 && (flag == 0)) //没有按键按下
		{
			 flag = 1;
		}
		return key;
}





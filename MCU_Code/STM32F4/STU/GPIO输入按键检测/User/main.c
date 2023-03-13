#include "stm32f4XX.h"
#include "bsp_led.h"
#include "bsp_key.h"
//#define LEDC4(a) if (a) \
//	GPIO_SetBits(GPIOC, GPIO_Pin_4);\
//else\
//	GPIO_ResetBits(GPIOC, GPIO_Pin_4);

void Delay(uint32_t count)
{
	for(;count!=0;count--);
}

int main(void)
{
	/* 此处写入程序 */
	/* LED端口初始化 */
	LED_GPIO_Config();
	
	/* KEY端口初始化 */
	KEY_GPIOA_Config();
	KEY_GPIOE_Config();
	
	/* 按键扫描 */
	//GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	
	while(1)
	{
//		if(KEY_Scan (GPIOA,GPIO_Pin_0)  == KEY_ON )
//		{
//			Delay(0x00ffff);
			if(KEY_Scan (GPIOA,GPIO_Pin_0)  == KEY_ON)
			{
				LED_TOGGLE;
				while(KEY_Scan (GPIOA,GPIO_Pin_0) == KEY_ON ){}
				
			}
//		}

//				LED_TOGGLE;
//		Delay(0xffffff);
//		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)  // 如果按键被按下
//{
//	HAL_Delay(10);  // 消抖处理
//	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)  // 如果此时依然为低电平
//	{
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);  // 翻转此引脚电平
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);  // 翻转此引脚电平
//		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET);  // 松开按键后再执行下一轮循环
//	}
//}

//		if()
//	}

	}
}

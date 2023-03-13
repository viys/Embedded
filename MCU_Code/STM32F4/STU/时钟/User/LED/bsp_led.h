#ifndef _BSP_LED_H
#define _BSP_LED_H

#define LED_GPIO_PORT 	GPIOC
#define LED_PIN 				GPIO_Pin_4      
//#define LED_GPIO_CLK    RCC_AHB1Periph_GPIOC

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//设置为高电平
#define digitalLo(p,i)			 {p->BSRRH=i;}		//输出低电平
#define digitalToggle(p,i)	 {p->ODR ^=i;}		//输出反转状态

/* 定义控制IO的宏 */
#define LED_TOGGLE		digitalToggle(LED_GPIO_PORT,LED_PIN)
#define LED_OFF				digitalHi(LED_GPIO_PORT,LED_PIN)
#define LED_ON				digitalLo(LED_GPIO_PORT,LED_PIN)


#include "stm32f4xx.h"
void LED_GPIO_Config(void);

#endif /* _BSP_LED_H */

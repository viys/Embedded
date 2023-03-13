#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f10x.h"

#define IN1(x)	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)x)
#define IN2(x)	GPIO_WriteBit(GPIOA,GPIO_Pin_2,(BitAction)x)
#define IN3(x)	GPIO_WriteBit(GPIOA,GPIO_Pin_3,(BitAction)x)
#define IN4(x)	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)x)

void GPIO_Config(void);

#endif

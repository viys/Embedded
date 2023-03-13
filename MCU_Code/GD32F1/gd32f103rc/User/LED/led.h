#ifndef _LED_H
#define _LED_H

#include "gd32f10x.h"

#define LED_GPIO_PORT 	GPIOB
#define LED_PIN 				GPIO_PIN_4   

void led_init(void);

#endif

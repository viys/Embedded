#ifndef		__MY_GPIO_H__
#define		__MY_GPIO_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
//--------------------- function declare ----------------------
void gpio_config(void);
void my_gpio_toggle(uint32_t gpio_periph,uint32_t pin);

#endif	//__MY_GPIO_H__


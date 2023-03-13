#ifndef		__MYPWM_H__
#define		__MYPWM_H__

//---------------------- include files -----------------------
#include "gd32f1x0.h"
//--------------------- macro define ----------------------
#define		PORT_PWM_LOW()	
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void pwm_timer0_ch2_init(unsigned int Freq);
void pwm_timer0_ch2_control(unsigned char flag);



#endif	//__MYPWM_H__


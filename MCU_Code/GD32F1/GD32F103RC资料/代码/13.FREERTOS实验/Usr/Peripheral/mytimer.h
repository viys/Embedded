#ifndef		__MY_TIMER_H__
#define		__MY_TIMER_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
//--------------------- variable declare ----------------------
extern char timer_1ms;
//--------------------- function declare ----------------------
void timer0_init(void);
void timer13_init(unsigned int Freq);
void timer13_capture_configuration(void);
void timer13_control(unsigned char Flag);
void timer1_capture_configuration(void);
void timer1_timer_configuration(unsigned int Freq);
void timer1_control(unsigned char Flag);
void timer2_configuration(void);
void timer2_control(unsigned char Flag);
void timer11_capture_configuration(void);
void timer11_control(unsigned char Flag);


#endif	//__MY_TIMER_H__


#ifndef		__MOTOR_H__
#define		__MOTOR_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_MOTOR			PA9

#define		IO_MOTOR_RCU		RCU_GPIOA
#define		IO_MOTOR_PORT		GPIOA
#define		IO_MOTOR_PIN		GPIO_PIN_9

#define		IO_MOTOR_INIT()	\
			do{	\
				rcu_periph_clock_enable(IO_MOTOR_RCU);	\
				gpio_init(IO_MOTOR_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_MOTOR_PIN);	\
				gpio_bit_reset(IO_MOTOR_PORT,IO_MOTOR_PIN);		\
			}while(0)

#define		MOTOR_RUN()			gpio_bit_set(IO_MOTOR_PORT,IO_MOTOR_PIN)
#define		MOTOR_STOP()		gpio_bit_reset(IO_MOTOR_PORT,IO_MOTOR_PIN)

#define		VIBRATE_TIME_UNIT		10u										// unit of beep time = 5ms
			
#define		TIME_VIBRATE_POINT1		100u									// run
#define		TIME_VIBRATE_POINT2		(TIME_VIBRATE_POINT1+100U)				// stop
#define		TIME_VIBRATE_POINT3		(TIME_VIBRATE_POINT2+100U)				// run
#define		TIME_VIBRATE_CIRCLE		(TIME_VIBRATE_POINT3+500U)
//----------------------------------------------------------
typedef	struct _vibrate_type_
{
	unsigned char flag;
	unsigned int setTime;
	unsigned int RunTime;
}vibrate_type_t;
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void motor_vibration_set(unsigned int time, unsigned char flag);
void motor_init(void);
void mot_vibrate(void);



#endif	//__MOTOR_H__


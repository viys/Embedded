#ifndef		__BUZZER_H__
#define		__BUZZER_H__

//--------------------- include files ---------------------
#include "mytimer.h"
#include "mygpio.h"
//--------------------- macro define ----------------------
#define		IO_BUZZER				PA15

#define		IO_BUZZER_AF_RCU		RCU_AF
#define		IO_BUZZER_RCU			RCU_GPIOA
#define		IO_BUZZER_PORT			GPIOA
#define		IO_BUZZER_PIN			GPIO_PIN_15

#define		BUZZER_FRQ			2700u

#define		IO_BUZZER_MOD_PWM()	do{								\
									rcu_periph_clock_enable(IO_BUZZER_RCU);		\
									gpio_init(IO_BUZZER_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_2MHZ, IO_BUZZER_PIN);	\
									gpio_bit_reset(IO_BUZZER_PORT,IO_BUZZER_PIN);		\
								}while(0)

#define		IO_BUZZER_MOD_GPIO()	do{								\
									rcu_periph_clock_enable(IO_BUZZER_AF_RCU);	\
									gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);	\
									rcu_periph_clock_enable(IO_BUZZER_RCU);		\
									gpio_init(IO_BUZZER_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_BUZZER_PIN);	\
									gpio_bit_reset(IO_BUZZER_PORT,IO_BUZZER_PIN);		\
								}while(0)

#define		BUZZER_OUT(FLAG)	do{								\
									(FLAG)?gpio_bit_set(IO_BUZZER_PORT,IO_BUZZER_PIN):gpio_bit_reset(IO_BUZZER_PORT,IO_BUZZER_PIN);	\
								}while(0)

#define		BUZZER_GPIO_TOGGLE()	do{								\
										my_gpio_toggle(IO_BUZZER_PORT,IO_BUZZER_PIN);	\
								}while(0)

#define		BEEP_TYPE_3BP		0										// beep type
#define		BEEP_TYPE_1BP		1

#define		BP_TIMES_3BP		3										// play time of 3BP
#define		BP_TIMES_1BP		1										// play time of 1BP

#define		BP_TIME_UNIT		10u										// unit of beep time = 10ms
								
#define		TYPE_3BP_DUR		100u									// duration of 3BP(5u = 5ms)
#define		TYPE_3BP_ITV		40u										// interval of 3BP
#define		TIME_3BP_TONE		(3*(TYPE_3BP_DUR+TYPE_3BP_ITV))			// the play time of 3BP
#define		HALT_3BP_TONE		300u

#define		TYPE_1BP_DUR		100u									// duration of 3BP
#define		TYPE_1BP_ITV		0u										// interval of 3BP
#define		TIME_1BP_TONE		(TYPE_1BP_DUR+TYPE_1BP_ITV)				// the play time of 1BP
#define		HALT_1BP_TONE		0u

#define		TIME_BUZZER_100MS	100U
//--------------------- macro define ----------------------
#define		BUZZER_PWM_INIT(FRQ)	timer1_timer_configuration(FRQ)
#define		BUZZER_PWM_CTL(FLAG)	timer1_control(FLAG)
//--------------------- macro define ----------------------
typedef	struct _tone_type_
{
	unsigned int beepDuration;				// the count of beep time
	unsigned int interval;					// interval between two beep
	unsigned int beepTimes;					// the times of beep
}tone_type_t;

typedef struct _buz_music_type_
{
	unsigned char stage;					// stage
	tone_type_t *toneType;
	unsigned int tonePlayTimes;				// the times of beep
	unsigned int halt;						// interval between two beep
}buz_music_type_t;
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void buzzer_init(void);
void buzzer_beep(void);
void buzzer_set(unsigned char typeInx, unsigned int playTimes, unsigned int halt);
void buzzer_beep_stop(void);



#endif	//__BUZZER_H__


#ifndef		__LED_H__
#define		__LED_H__

//--------------------- include files ---------------------
#include "mygpio.h"
//--------------------- macro define ----------------------
#define	IO_LED_RED				PA11
#define	IO_LED_GREEN			PA12

#define	IO_RLED_RCU			RCU_GPIOA
#define	IO_RLED_PORT		GPIOA
#define	IO_RLED_PIN			GPIO_PIN_11

#define	IO_GLED_RCU			RCU_GPIOA
#define	IO_GLED_PORT		GPIOA
#define	IO_GLED_PIN			GPIO_PIN_12

#define	IO_RLED_INIT() 	do{	\
							rcu_periph_clock_enable(IO_RLED_RCU);		\
							gpio_init(IO_RLED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_RLED_PIN);	\
							gpio_bit_reset(IO_RLED_PORT,IO_RLED_PIN);		\
						}while(0)

#define	IO_RLED_OUT() 	do{	\
							rcu_periph_clock_enable(IO_RLED_RCU);		\
							gpio_init(IO_RLED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_RLED_PIN);	\
							gpio_bit_reset(IO_RLED_PORT,IO_RLED_PIN);		\
						}while(0)

#define	IO_GLED_INIT() 	do{	\
							rcu_periph_clock_enable(IO_GLED_RCU);		\
							gpio_init(IO_GLED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_GLED_PIN);	\
							gpio_bit_reset(IO_GLED_PORT,IO_GLED_PIN);		\
						}while(0)

#define	IO_GLED_OUT() 	do{	\
							rcu_periph_clock_enable(IO_GLED_RCU);		\
							gpio_init(IO_GLED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_GLED_PIN);	\
							gpio_bit_reset(IO_GLED_PORT,IO_GLED_PIN);		\
						}while(0)

#define	IO_RLED_HIGH()			gpio_bit_set(IO_GLED_PORT,IO_RLED_PIN)
#define	IO_RLED_LOW()			gpio_bit_reset(IO_RLED_PORT,IO_RLED_PIN)
#define	IO_RLED_TOGGLE()		my_gpio_toggle(IO_RLED_PORT,IO_RLED_PIN)

#define	IO_RLED_ON()			IO_RLED_HIGH()
#define	IO_RLED_OFF()			IO_RLED_LOW()

#define	IO_GLED_HIGH()			gpio_bit_set(IO_GLED_PORT,IO_GLED_PIN)
#define	IO_GLED_LOW()			gpio_bit_reset(IO_GLED_PORT,IO_GLED_PIN)
#define	IO_GLED_TOGGLE()		my_gpio_toggle(IO_GLED_PORT,IO_GLED_PIN)

#define	IO_GLED_ON()			IO_GLED_HIGH()
#define	IO_GLED_OFF()			IO_GLED_LOW()
//---------------------------------------------------------------
#define	TIME_ALERT_LED_POINT1		400u
#define	TIME_ALERT_LED_CIRCLE		(TIME_ALERT_LED_POINT1+400U)
//---------------------------------------------------------------
enum
{
	LED_TYPE_OFF,
	LED_TYPE_ALERT,
	LED_TYPE_LOW_POW,
	LED_TYPE_CHARGING,
	LED_TYPE_FULL_CHARGED,
	LED_TYPE_RUNNING,
};
typedef	struct _led_type_
{
	unsigned char flag;
	unsigned char type;
	unsigned int disTime;
}led_type_t;
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void led_init(void);
void led_flash(void);
void led_volt_dis_set(unsigned char type, unsigned int time, unsigned char flag);



#endif	//__LED_H__


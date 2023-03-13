#ifndef		__BS814A_H__
#define		__BS814A_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_TKEY_SET					PB4
#define		IO_TKEY_UP					PB5
#define		IO_TKEY_DOWN				PB6
#define		IO_TKEY_RETURN				PB7

#define		TKEY_SET_RCU		RCU_GPIOB
#define		TKEY_SET_PORT		GPIOB
#define		TKEY_SET_PIN		GPIO_PIN_4

#define		TKEY_UP_RCU			RCU_GPIOB
#define		TKEY_UP_PORT		GPIOB
#define		TKEY_UP_PIN			GPIO_PIN_5

#define		TKEY_DOWN_RCU		RCU_GPIOB
#define		TKEY_DOWN_PORT		GPIOB
#define		TKEY_DOWN_PIN		GPIO_PIN_6

#define		TKEY_RETURN_RCU		RCU_GPIOB
#define		TKEY_RETURN_PORT	GPIOB
#define		TKEY_RETURN_PIN		GPIO_PIN_7


#define		KEY_SET_ININT() do{	\
									rcu_periph_clock_enable(TKEY_SET_RCU);	\
									gpio_init(TKEY_SET_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, TKEY_SET_PIN);	\
								}while(0)

#define		KEY_UP_ININT() do{	\
									rcu_periph_clock_enable(TKEY_UP_RCU);	\
									gpio_init(TKEY_UP_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, TKEY_UP_PIN);	\
								}while(0)

#define		KEY_DOWN_ININT() do{	\
									rcu_periph_clock_enable(TKEY_DOWN_RCU);	\
									gpio_init(TKEY_DOWN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, TKEY_DOWN_PIN);	\
								}while(0)

#define		KEY_RETURN_ININT() do{	\
									rcu_periph_clock_enable(TKEY_RETURN_RCU);	\
									gpio_init(TKEY_RETURN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, TKEY_RETURN_PIN);	\
								}while(0)
								
#define		READ_KEY_SET()			gpio_input_bit_get(TKEY_SET_PORT,TKEY_SET_PIN)
#define		READ_KEY_UP()			gpio_input_bit_get(TKEY_UP_PORT,TKEY_UP_PIN)
#define		READ_KEY_DOWN()			gpio_input_bit_get(TKEY_DOWN_PORT,TKEY_DOWN_PIN)
#define		READ_KEY_RETURN()		gpio_input_bit_get(TKEY_RETURN_PORT,TKEY_RETURN_PIN)
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void bs814a_port_init(void);
unsigned int get_key_bs814a(void);



#endif	//__BS814A_H__


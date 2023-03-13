#ifndef		__MECHANICAL_KEY_H__
#define		__MECHANICAL_KEY_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_TKEY_ENTER			PA1
#define		IO_TKEY_FUNCTION		PA0

#define		TKEY_ENTER_RCU			RCU_GPIOA
#define		TKEY_ENTER_PORT			GPIOA
#define		TKEY_ENTER_PIN			GPIO_PIN_1

#define		TKEY_FUN_RCU			RCU_GPIOA
#define		TKEY_FUN_PORT			GPIOA
#define		TKEY_FUN_PIN			GPIO_PIN_0

#define		KEY_ENTER_ININT() do{	\
									rcu_periph_clock_enable(TKEY_ENTER_RCU);	\
									gpio_init(TKEY_ENTER_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, TKEY_ENTER_PIN);	\
								}while(0)

#define		KEY_FUN_ININT() do{	\
									rcu_periph_clock_enable(TKEY_FUN_RCU);	\
									gpio_init(TKEY_FUN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, TKEY_FUN_PIN);	\
								}while(0)
								
#define		READ_KEY_ENTER()		gpio_input_bit_get(TKEY_ENTER_PORT,TKEY_ENTER_PIN)
#define		READ_KEY_FUNCTION()		gpio_input_bit_get(TKEY_FUN_PORT,TKEY_FUN_PIN)
//--------------------- variable declare ----------------------
//--------------------- function declare ----------------------
void MechanicalKey_init(void);
unsigned int get_key_Mechanical(void);
int is_ExitKey_Waupup_sys(void);
void cleanExitKey_Waupup_Flag(void);



#endif	//__POEWR_KEY_H__


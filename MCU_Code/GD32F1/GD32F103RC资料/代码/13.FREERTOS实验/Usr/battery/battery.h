#ifndef		__BATTERY_H__
#define		__BATTERY_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_POWER_DET			PB0
#define		IO_POWER_CTL			PB11
#define		IO_CHG_DET				PB1

#define		IO_POWER_DET_RCU		RCU_GPIOB
#define		IO_POWER_DET_PORT		GPIOB
#define		IO_POWER_DET_PIN		GPIO_PIN_0

#define		IO_POWER_CTL_RCU		RCU_GPIOB
#define		IO_POWER_CTL_PORT		GPIOB
#define		IO_POWER_CTL_PIN		GPIO_PIN_11

#define		IO_CHG_DET_RCU			RCU_GPIOB
#define		IO_CHG_DET_PORT			GPIOB
#define		IO_CHG_DET_PIN			GPIO_PIN_1


#define		IO_POWER_DET_INIT() do{	\
									rcu_periph_clock_enable(IO_POWER_DET_RCU);			\
									gpio_init(IO_POWER_DET_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, IO_POWER_DET_PIN); \
								}while(0)
		
#define		IO_POWER_CTL_INIT()	do{	\
									rcu_periph_clock_enable(IO_POWER_CTL_RCU); 	\
									gpio_init(IO_POWER_CTL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, IO_POWER_CTL_PIN);	\
									gpio_bit_set(IO_POWER_CTL_PORT,IO_POWER_CTL_PIN);		\
								}while(0)

#define		IO_CHG_DET_INIT()	do{	\
									rcu_periph_clock_enable(IO_CHG_DET_RCU);			\
									gpio_init(IO_CHG_DET_PORT, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, IO_CHG_DET_PIN);	\
								}while(0)

#define		IO_POWER_CTL_LOW()		gpio_bit_reset(IO_POWER_CTL_PORT,IO_POWER_CTL_PIN)
#define		IO_POWER_CTL_HIGH()		gpio_bit_set(IO_POWER_CTL_PORT,IO_POWER_CTL_PIN)

#define		READ_POWER_DET()		gpio_input_bit_get(IO_POWER_DET_PORT,IO_POWER_DET_PIN)
#define		READ_CHG_DET()			gpio_input_bit_get(IO_CHG_DET_PORT,IO_CHG_DET_PIN)
//--------------------- macro define ----------------------
#define		ADC_FULL_VOT			2574u		//4.15v (2.075V/3.3V * 4095)
#define		ADC_LOW_POWER			2295u		//3.7v (1.85V/3.3V * 4095)
#define		ADC_LOW_POWER1			2358u		//3.8v (1.9V/3.3V * 4095)
#define		ADC_NORMAL_VOT			2388u		//3.85v (1.925V/3.3V * 4095)
#define		ADC_NORMAL_VOT1			2450u		//3.95v (1.75V/3.3V * 4095)
#define		ADC_POWER_OFF			2110u		//3.4v (1.9V/3.3V * 4095)

#define		CONST_TIME_3S			3000

#define		VALID_PLUGIN			0X0
#define		nVALID_PLUGIN			0Xff

#define		VALID_FULL_CHG			0X0
#define		nVALID_FULL_CHG			0XFF

#define		VALID_LOW_POWER			0X0
#define		nVALID_LOW_POWER		0XFF

#define		VALID_CHARGING			0X0
#define		nVALID_CHARGING			0XFF

#define		VALID_AUTO_OFF			0X0
#define		nVALID_AUTO_OFF			0XFF
//--------------------- function declare ----------------------
extern unsigned int adc_bat;
//--------------------- function declare ----------------------
void get_bat_adc_value(void);
void battery_volt_det(void);
void charge_det(void);
void power_switch(unsigned char flag);
void plugin_and_charge_det(void);
void volt_det_init(void);
void charge_det_init(void);
unsigned char IsCharger_PlugIn(void);
void AutoPowerOff_Set(unsigned char Flag);
void Auto_PowerOff(void);


#endif	//__BATTERY_H__


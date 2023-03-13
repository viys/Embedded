#ifndef		__MYADC_H__
#define		__MYADC_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		ADC_TIMES		32
//--------------------- macro define ----------------------
#define		IO_DET_BAT			PA2

#define		RCU_AD_PORT			RCU_GPIOA
#define		PORT_ADC_BAT		GPIOA
#define		PIN_ADC_BAT			GPIO_PIN_2

#define		ADC_SET_BAT			ADC1

#define		RCU_AD_BAT			RCU_ADC1
#define		PORT_AD				GPIOA
#define		INPUT_PIN_AD		GPIO_PIN_2
#define		BOARD_ADC_CHANNEL	ADC_CHANNEL_2
#define		ADC_CHN_NUM			2
#define		ADC_RANK_NUM		0
//--------------------- macro define ----------------------
#define		ADC_DO_START() do{	\
								adc_flag_clear(ADC_SET_BAT,ADC_FLAG_EOC);	\
								adc_software_trigger_enable(ADC_SET_BAT,ADC_REGULAR_CHANNEL);	\
							}while(0)

#define		IS_ADC_OK() 		(SET == adc_flag_get(ADC_SET_BAT,ADC_FLAG_EOC))
#define		GET_ADC_VAL()		adc_regular_data_read(ADC_SET_BAT)
#define		RESET_ADC_STAT()	adc_flag_clear(ADC_SET_BAT,ADC_FLAG_EOC|ADC_FLAG_EOIC|ADC_FLAG_STIC|ADC_FLAG_STRC)
//--------------------- variable declare ----------------------
//--------------------- function declare ----------------------
void ad_convert(unsigned int *adc_val);
void adc_init(void);



#endif	//__MYADC_H__


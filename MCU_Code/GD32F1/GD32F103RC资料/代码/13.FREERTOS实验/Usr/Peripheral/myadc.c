/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "myadc.h"
//--------------------- variable declare ------------------


/*************************************************************
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*************************************************************/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_AD_PORT);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_AD_BAT);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV16);
}

/*************************************************************
    \brief      configure the gpio peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*************************************************************/
void adc_in_channel_config(void)
{
    /* config the GPIO as analog mode */
	rcu_periph_clock_enable(RCU_AD_PORT);
	gpio_init(PORT_ADC_BAT, GPIO_MODE_AIN, GPIO_OSPEED_2MHZ, PIN_ADC_BAT);
}

/*************************************************************
Function:
Description: 
	configure the adc peripheral
*************************************************************/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_SET_BAT,ADC_REGULAR_CHANNEL,ADC_CHN_NUM);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC_SET_BAT,ADC_RANK_NUM,BOARD_ADC_CHANNEL,ADC_SAMPLETIME_239POINT5);
    //adc_regular_channel_config(ADC_RANK_NUM,BOARD_ADC_CHANNEL,ADC_SAMPLETIME_71POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_SET_BAT,ADC_REGULAR_CHANNEL,ENABLE);

	/* ADC external trigger source config */
	adc_external_trigger_source_config(ADC_SET_BAT,ADC_REGULAR_CHANNEL,ADC0_1_EXTTRIG_REGULAR_NONE);
	
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_SET_BAT,ADC_DATAALIGN_RIGHT);

	/* enable ADC interface */
    adc_enable(ADC_SET_BAT);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC_SET_BAT);
}

/*************************************************************
Function:
Description: 
*************************************************************/
void adc_init(void)
{
	rcu_config();
	adc_in_channel_config();
	adc_config();
}


/*************************************************************
Function:
Description: 
*************************************************************/
void ad_convert(unsigned int *adc_val)
{
	static unsigned long adc_sum = 0;
	static unsigned char convert_cnt = 0;
	static unsigned int ad_val_max = 0;
	static unsigned int ad_val_min = 0xffff;
	static char AdcStage = 0;
	unsigned int val;
	
	switch(AdcStage)
	{
		case 0:													// satrt
		{
			ADC_DO_START();										// ADC start
			AdcStage = 1;
			break;
		}
		case 1:													// satrt
		{
			if(IS_ADC_OK())
			{
				val = GET_ADC_VAL();
				RESET_ADC_STAT();
				if(ad_val_max<val)	ad_val_max = val;
				if(ad_val_min>val)	ad_val_min = val;
				
				adc_sum += val;
				if(++convert_cnt>=(ADC_TIMES+2))
				{
					convert_cnt = 0;
					adc_sum -= ad_val_max;
					adc_sum -= ad_val_min;
					*adc_val = adc_sum/ADC_TIMES;
					ad_val_max = 0;
					ad_val_min = 0xffff;
					adc_sum = 0;
				}
				AdcStage = 0;
			}
			break;
		}
		default:
			break;
	}
}





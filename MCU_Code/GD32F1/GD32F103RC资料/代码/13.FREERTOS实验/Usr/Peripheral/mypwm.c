/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "mypwm.h"



/*********************************************************************************************
Function:
Description: 
	SystemCoreClock = 72MHz
	PWM frequency = Fpwm/((PWMPH,PWMPL) + 1) <Fpwm = Fsys/PWM_CLOCK_DIV> 
								= (16MHz/8)/(0x7CF + 1)
								= 1KHz (1ms)
	=============================================================================
    TIMER0 configuration: generate 1 PWM signals:
    TIMER0CLK = SystemCoreClock / 72 = 1MHz
    TIMER0 channel 2 duty cycle = (8000/ 16000)* 100  = 50%
*********************************************************************************************/
void pwm_timer0_ch2_init(unsigned int Freq)
{
	unsigned long pwm_period;
    timer_oc_parameter_struct	timer_ocintpara;
    timer_parameter_struct		timer_initpara;
	unsigned int pwm_prescaler = 0;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

	pwm_period = SystemCoreClock/Freq;
	while(pwm_period>65535)
	{
		pwm_prescaler++;
		pwm_period /= (pwm_prescaler+1);
	}
	
    /* TIMER0 configuration */
    timer_initpara.prescaler         = pwm_prescaler;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = pwm_period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

    /* CH2 configuration in PWM0 mode */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;

    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocintpara);

    /* CH2 configuration in PWM mode1,duty cycle 50% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, pwm_period/2);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* auto-reload preload enable */
}

/*********************************************************************************************
Function:
Description: 
*********************************************************************************************/
void pwm_timer0_ch2_control(unsigned char flag)
{
	if(flag)
	{
		timer_primary_output_config(TIMER0, ENABLE);
		timer_enable(TIMER0);
	}
	else
	{
		timer_primary_output_config(TIMER0, DISABLE);
		timer_disable(TIMER0);
	}
}



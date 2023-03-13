/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "gd32e10x.h"
#include "mytimer.h"
//--------------------- macro define ----------------------
//---------------------- variable define -----------------------
char timer_1ms = 0;


/****************************************************************************************
Function:
Description: 
    TIMER0 configuration to:
    generate 3 complementary PWM signals with 3 different duty cycles:
    TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 3600 so the 
    TIMER0 counter clock used is 20KHz.
    the three duty cycles are computed as the following description: 
    the channel 0 duty cycle is set to 25% so channel 1N is set to 75%.		PA8
    the channel 1 duty cycle is set to 50% so channel 2N is set to 50%.		PA9
    the channel 2 duty cycle is set to 75% so channel 3N is set to 25%.		PA10
****************************************************************************************/
void timer0_init(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 3599;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 15999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

     /*  */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0,3999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 7999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 11999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);
}

/*************************************************************
Function:
Description:
	APB1 = APB = 36MHz
	set the timer's period as 1 ms.
*************************************************************/
void timer13_init(unsigned int Freq)
{
    timer_parameter_struct timer_initpara;

	Freq = Freq;
    rcu_periph_clock_enable(RCU_TIMER13);							// enable the clock of timer13
    timer_deinit(TIMER13);

    /* TIMER configuration */
    timer_initpara.prescaler         = (36-1);
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (1000-1);
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER13, &timer_initpara);

    timer_primary_output_config(TIMER13, DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER13);
	
	/*
	timer_counter_value_config();
	timer_autoreload_value_config();
	timer_repetition_value_config();
	timer_counter_up_direction();
	*/

	//timer_update_event_enable(TIMER13);						// if uncomment interrupt will not occur

	/* interrupt_enable */
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_UP);
	timer_interrupt_enable(TIMER13,TIMER_INT_UP);
	
	// run
    timer_enable(TIMER13);
	
}

/****************************************************************************************
Function:
Description:
    \brief      configure the TIMER peripheral,system clock is 36MHz
    \param[in]  none
    \param[out] none
    \retval     none
	定时器2挂在APB1总线上。目前系统时钟为120M，APB1时钟为60M，timer2时钟为120M
	定时器2通道2(PA7)作为RF输出脚.
***************************************************************************************/
void timer13_capture_configuration(void)
{
    /* TIMER13 configuration: input capture mode -------------------
    the external signal is connected to TIMER2 CH0 pin(PA6)
    the rising edge is used as active edge
    the TIMER2 CH0CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER13);

    timer_deinit(TIMER13);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 0xfffe;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER13, &timer_initpara);

    /* TIMER2  configuration */
    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_BOTH_EDGE;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER13,TIMER_CH_0,&timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER13);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER13,TIMER_INT_FLAG_CH0);
    /* channel 0 interrupt enable */
    //timer_interrupt_enable(TIMER13,TIMER_INT_CH0);

    /* TIMER counter enable */
    //timer_enable(TIMER13);
}

/****************************************************************************************
Function:
Description:
***************************************************************************************/
void timer13_control(unsigned char Flag)
{
	if(Flag)
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_CH0);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER13, TIMER_INT_CH0);
		
		// run
		timer_enable(TIMER13);
	}
	else
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_CH0);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER13, TIMER_INT_CH0);
		
		// run
		timer_disable(TIMER13);
	}
}

/****************************************************************************************
Function:
Description:
    \brief      configure the TIMER peripheral,
    \param[in]  none
    \param[out] none
    \retval     none
***************************************************************************************/
void timer1_timer_configuration(unsigned int Freq)
{
	#define		FCLK_TIMER1		(SystemCoreClock/2)*2						// AHB1 = SYSCLK/2,TIMER_CLK = AHB1*2
	unsigned int period_temp =0;
	unsigned int timer_prescaler = 0;
	unsigned int timer_period = 0;
	
    timer_parameter_struct timer_initpara;

	period_temp = FCLK_TIMER1/Freq;
	if(period_temp>0xffff)
	{
		timer_prescaler = period_temp/0xffff;
	}
	
	timer_period = FCLK_TIMER1/(timer_prescaler+1)/Freq;
	
    rcu_periph_clock_enable(RCU_TIMER1);								// enable the timer clock before setting register
    timer_deinit(TIMER1);												// set the register as default value

    /* TIMER configuration */
    timer_initpara.prescaler         = timer_prescaler;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = timer_period/2-1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    timer_primary_output_config(TIMER1, DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);

	//timer_update_event_enable(TIMER1);						// if uncomment interrupt will not occur

}

/****************************************************************************************
Function:
Description:
    \brief      configure the TIMER peripheral,
    \param[in]  none
    \param[out] none
    \retval     none
***************************************************************************************/
void timer1_capture_configuration(void)
{
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);								// enable the timer clock before setting register
    timer_deinit(TIMER1);												// set the register as default value

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 36-1;							// divided the clock to 1MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 0xffffff00;						// about 4000 seconds
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* TIMER1  configuration */
    /* TIMER1 CH0 input capture configuration */
    //timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_BOTH_EDGE;			// detect rising or falling edge
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER1, TIMER_CH_0, &timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH0);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(TIMER1, TIMER_INT_CH0);

    /* TIMER1 counter enable */
    timer_enable(TIMER1);
}

/****************************************************************************************
Function:
Description:
***************************************************************************************/
void timer1_control(unsigned char Flag)
{
	if(Flag)
	{
		/* interrupt_enable */
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH1);
		timer_interrupt_enable(TIMER1,TIMER_INT_UP);
		
		// run
		timer_enable(TIMER1);
	}
	else
	{
		/* interrupt_enable */
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH1);
		timer_interrupt_disable(TIMER1,TIMER_INT_UP);
		
		// run
		timer_disable(TIMER1);
	}
}


/****************************************************************************************
Function:
Description:
    \brief      configure the TIMER peripheral,system clock is 36MHz
    \param[in]  none
    \param[out] none
    \retval     none
	定时器2挂在APB1总线上。目前系统时钟为120M，APB1时钟为60M，timer2时钟为120M
	定时器2通道2(PA7)作为RF输出脚.
***************************************************************************************/
void timer2_configuration(void)
{
    /* TIMER2 configuration: input capture mode -------------------
    the external signal is connected to TIMER2 CH0 pin(PA6)
    the rising edge is used as active edge
    the TIMER2 CH0CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* TIMER2  configuration */
    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_BOTH_EDGE;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER2,TIMER_CH_1,&timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH1);
    /* channel 0 interrupt enable */
    //timer_interrupt_enable(TIMER2,TIMER_INT_CH1);

    /* TIMER2 counter enable */
    //timer_enable(TIMER2);
}

/****************************************************************************************
Function:
Description:
***************************************************************************************/
void timer2_control(unsigned char Flag)
{
	if(Flag)
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH1);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER2, TIMER_INT_CH1);
		
		// run
		timer_enable(TIMER2);
	}
	else
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH1);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER2, TIMER_INT_CH1);
		
		// run
		timer_disable(TIMER2);
	}
}

/****************************************************************************************
Function:
Description:
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
	定时器11挂在APB1总线上。目前系统时钟为72M，APB1时钟为36M，timer11时钟为72M
	定时器11通道1(Pc13)作为RF输出脚.
***************************************************************************************/
void timer11_capture_configuration(void)
{
    /* TIMER11 configuration: input capture mode -------------------
    the external signal is connected to TIMER2 CH0 pin(PA6)
    the rising edge is used as active edge
    the TIMER2 CH0CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER11);

    timer_deinit(TIMER11);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 72-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 0xfffe;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER11, &timer_initpara);

    /* TIMER11  configuration */
    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_BOTH_EDGE;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER11,TIMER_CH_1,&timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER11);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER11,TIMER_INT_FLAG_CH1);
    /* channel 0 interrupt enable */
    //timer_interrupt_enable(TIMER11,TIMER_INT_CH1);

    /* TIMER counter enable */
    //timer_enable(TIMER11);
}

/****************************************************************************************
Function:
Description:
***************************************************************************************/
void timer11_control(unsigned char Flag)
{
	if(Flag)
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER11, TIMER_INT_FLAG_CH1);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER11, TIMER_INT_CH1);
		
		// run
		timer_enable(TIMER11);
	}
	else
	{
		/* clear channel 2 interrupt bit */
		timer_interrupt_flag_clear(TIMER11, TIMER_INT_FLAG_CH1);
		/* channel 2 interrupt enable */
		timer_interrupt_enable(TIMER11, TIMER_INT_CH1);
		
		// run
		timer_disable(TIMER11);
	}
}

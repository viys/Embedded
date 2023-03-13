/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "myadc.h"
#include "battery.h"
//--------------------- variable declare ----------------------
unsigned int adc_bat=0;
volatile unsigned char fg_charging = nVALID_CHARGING;
volatile unsigned char fg_low_power = nVALID_LOW_POWER;
volatile unsigned char fg_plugin = nVALID_PLUGIN;
volatile unsigned char fg_full_charged = nVALID_FULL_CHG;
static unsigned int volt_det_cnt = 0;
static unsigned int LP_AutoOff_det_cnt = 0;
static unsigned char fg_Auto_PowerOff = nVALID_AUTO_OFF;


/*************************************************************
Function:
Description:
	this routine will be invoked per 10ms
*************************************************************/
void get_bat_adc_value(void)
{
	ad_convert(&adc_bat);
}

/*************************************************************
Function:
Description:
	this routine will be invoked per 10ms
*************************************************************/
void full_charged_det(unsigned int adv)
{
	static unsigned int full_chg_cnt0 = 0;
	static unsigned char full_chg_cnt1 = 0;
	static unsigned int full_chg_cnt2 = 0;
	
	if(VALID_FULL_CHG!=fg_full_charged)					// full charged detect
	{
		full_chg_cnt2 = 0;
		
		if(ADC_FULL_VOT<adv)							// full charge filter
		{
			full_chg_cnt0++;
			if(full_chg_cnt0>=(60*10))					// 60*10*100ms = 1 minute
			{
				full_chg_cnt0 = 0;
				full_chg_cnt1++;
				if(full_chg_cnt1>=50)					// 50 minutes
				{
					full_chg_cnt1 = 0;
					fg_full_charged = VALID_FULL_CHG;
				}
			}
		}
		else											// charging filter
		{
			full_chg_cnt0 = 0;
			full_chg_cnt1 = 0;
		}
	}
	else												// normal voltage detect
	{
		full_chg_cnt0 = 0;
		full_chg_cnt1 = 0;
		
		if(ADC_FULL_VOT<adv)							// full charge filter
		{
			full_chg_cnt2 = 0;
		}
		else											// charging filter
		{
			full_chg_cnt2++;
			if(full_chg_cnt2>=(60*10))					// 60*10*100ms = 1 minute
			{
				full_chg_cnt2 = 0;
				fg_full_charged = nVALID_FULL_CHG;
			}
		}
	}
}

/*************************************************************
Function:
Description:
	this routine will be invoked per 100ms
*************************************************************/
void battery_low_pow_det(unsigned int adc_val)
{
	if(VALID_LOW_POWER!=fg_low_power)							// low power detect
	{
		if(ADC_LOW_POWER>adc_val)
		{
			volt_det_cnt++;
			if(volt_det_cnt>=(30*10))							// 30*10*100ms = 30S
			{
				volt_det_cnt = 0;
				fg_low_power = VALID_LOW_POWER;					// low
			}
		}
		else
		{
			volt_det_cnt = 0;
		}
	}
	else														// normal detect
	{
		if(ADC_NORMAL_VOT<adc_val)
		{
			volt_det_cnt++;
			if(volt_det_cnt>=(30*10))							// 30*10*100ms = 30S
			{
				volt_det_cnt = 0;
				fg_low_power = nVALID_LOW_POWER;				// normal
			}
		}
		else
		{
			volt_det_cnt = 0;
		}
	}
}

/*************************************************************
Function:
Description:
	when the battery voltage lower than 3.4V turn off the slave
	this routine will be invoked per 100ms
*************************************************************/
void battery_Low_AutoOff(unsigned int adc_val)
{	
	if(ADC_POWER_OFF>adc_val)
	{
		if(++LP_AutoOff_det_cnt>=(30*10))						// 30*10*100ms = 30S
		{
			LP_AutoOff_det_cnt = 0;
			power_switch(0);
		}
	}
	else
	{
		LP_AutoOff_det_cnt = 0;
	}
}

/*************************************************************
Function:
Description:
	this routine will be invoked per 100ms
*************************************************************/
void battery_volt_det(void)
{
	
	if(VALID_PLUGIN==fg_plugin)	
	{
		fg_low_power = nVALID_LOW_POWER;
		volt_det_cnt = 0;
		full_charged_det(adc_bat);								// check if full charged
		return;													// do not detect when charging
	}
	
	battery_low_pow_det(adc_bat);								// low power detect

	if(VALID_LOW_POWER==fg_low_power)
	{
		battery_Low_AutoOff(adc_bat);							// check if turn off or not
	}
}


/*************************************************************
Function:
Description:
	this routine will be invoked per 21ms
*************************************************************/
void plugin_and_charge_det(void)
{
	static unsigned char chg_det_cnt = 0;
	static unsigned char plug_det_cnt = 0;
	static unsigned int settingEnb_cnt = 0;
	
	if(VALID_PLUGIN==fg_plugin)											// plug out detect
	{
		if(READ_POWER_DET()==0)											// plug out
		{
			plug_det_cnt++;
			if(plug_det_cnt>=10)										// 10*21ms = 220ms
			{
				fg_plugin = nVALID_PLUGIN;
				fg_charging = nVALID_CHARGING;
				fg_full_charged = nVALID_FULL_CHG;
			}
		}
		else
		{
			plug_det_cnt = 0;
		}
		
		if(++settingEnb_cnt>=CONST_TIME_3S)								// setting disabled 3 seconds after charger plug in
		{
			settingEnb_cnt = 0;
		}
	}
	else																// plug in detect
	{
		if(READ_POWER_DET())											// plug in
		{
			plug_det_cnt++;
			if(plug_det_cnt>=10)										// 10*21ms = 220ms
			{
				fg_plugin = VALID_PLUGIN;
				fg_low_power = nVALID_LOW_POWER;
				fg_charging = VALID_CHARGING;							// assume that slave in charging
				settingEnb_cnt = 0;
			}
		}
		else
		{
			plug_det_cnt = 0;
		}
	}
	
	if(VALID_PLUGIN==fg_plugin)											// confirm charger plug in first
	{
		if(VALID_CHARGING==fg_charging)									// full charged detect
		{
			if(READ_CHG_DET()==1)										// full charged
			{
				chg_det_cnt++;
				if(chg_det_cnt>=10)										// 21*10 = 210ms
				{
					chg_det_cnt = 0;
					fg_charging = nVALID_CHARGING;
					fg_full_charged = VALID_FULL_CHG;
				}
			}
			else
			{
				chg_det_cnt = 0;
			}
		}
		else															// charging detect
		{
			if(READ_CHG_DET()==0)										// charging
			{
				chg_det_cnt++;
				if(chg_det_cnt>=10)										// 21*10 = 210ms
				{
					chg_det_cnt = 0;
					fg_charging = VALID_CHARGING;
					fg_full_charged = nVALID_FULL_CHG;
				}
			}
			else
			{
				chg_det_cnt = 0;
			}
		}
	}
	else
	{
		fg_charging = nVALID_CHARGING;
		chg_det_cnt = 0;
	}
}


/*************************************************************
Function:
Description:
*************************************************************/
void charge_det_init(void)
{
	IO_CHG_DET_INIT();
	IO_POWER_CTL_INIT();
	IO_POWER_DET_INIT();
}

/*************************************************************
Function:
Description:
*************************************************************/
void power_switch(unsigned char flag)
{
	if(flag)	
	{
		IO_POWER_CTL_INIT();
		IO_POWER_CTL_HIGH();
	}
	else		
	{
		IO_POWER_CTL_INIT();
		IO_POWER_CTL_LOW();
	}
}

/*************************************************************
Function:
Description:
*************************************************************/
void volt_det_init(void)
{
	adc_init();
}

/*************************************************************
Function:
Description:
*************************************************************/
unsigned char IsCharger_PlugIn(void)
{
	return(VALID_PLUGIN==fg_plugin);
}

/*************************************************************
Function:
Description:
	this rountine wil invoked per 100ms
*************************************************************/
void AutoPowerOff_Set(unsigned char Flag)
{
	if(Flag)
	{
		fg_Auto_PowerOff = VALID_AUTO_OFF;
	}
	else
	{
		fg_Auto_PowerOff = nVALID_AUTO_OFF;
	}
}
	
	
/*************************************************************
Function:
Description:
	this rountine wil invoked per 100ms.
	delay 1 second before real turn off to allow buzzer beep.
*************************************************************/
void Auto_PowerOff(void)
{
	static unsigned char PowerOff_Delay = 0;
	
	if(VALID_AUTO_OFF==fg_Auto_PowerOff)
	{
		if(++PowerOff_Delay>=5)								// 10*100 = 1S
		{
			fg_Auto_PowerOff = nVALID_AUTO_OFF;
			PowerOff_Delay = 0;
			power_switch(0);								// do power off 
		}
	}
	else
	{
		PowerOff_Delay = 0;
	}
}


/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "led.h"
//--------------------- variable declare ----------------------
led_type_t	led_volt = {0};								// low power and charge indication


/*************************************************************
Function: 
Description: 
			this routine will be invoked per 10ms
*************************************************************/
void led_flash(void)
{
	#define	TBASE_LED	10u
	static unsigned int volt_flash_cnt = 0;

	if(led_volt.flag)
	{
		switch(led_volt.type)
		{
			case LED_TYPE_LOW_POW:
			{
				if( ++volt_flash_cnt>=(1500/TBASE_LED) )	volt_flash_cnt = 0;

				if( volt_flash_cnt<(750/TBASE_LED) )
				{
					IO_RLED_ON();
				}
				else
				{
					IO_RLED_OFF();
				}
				break;
			}
			case LED_TYPE_CHARGING:
			{
				//IO_GLED_OFF();
				IO_RLED_ON();
				break;
			}
			case LED_TYPE_FULL_CHARGED:
			{
				//IO_GLED_ON();
				IO_RLED_OFF();
				break;
			}
			case LED_TYPE_RUNNING:
			{
				//IO_GLED_OFF();
				IO_RLED_OFF();
				break;
			}
			default:
			{
				//IO_GLED_OFF();
				IO_RLED_OFF();
				break;
			}
		}
	}
	else
	{
		volt_flash_cnt = 0;
		//IO_GLED_OFF();
		IO_RLED_OFF();
	}
}


/*************************************************************
Function: 
Description: 
				set low voltage indication
				type - indication type(LED_TYPE_LOW_POW,LED_TYPE_CHARGE)
				time - flash loops,maximum is 0xffff
				flag - determine indicate or not
*************************************************************/
void led_volt_dis_set(unsigned char type, unsigned int time, unsigned char flag)
{
	led_volt.flag = flag;
	led_volt.type = type;
	led_volt.disTime = time;
}


/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
*************************************************************/
void led_init(void)
{
	IO_RLED_INIT();
	IO_GLED_INIT();
	
	IO_RLED_OFF();	
	IO_GLED_OFF();	
}



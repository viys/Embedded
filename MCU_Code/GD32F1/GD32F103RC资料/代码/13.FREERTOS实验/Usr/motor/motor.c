/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "motor.h"
//--------------------- variable declare ----------------------
vibrate_type_t	mot_vibration = {0};					// vibration data


/*************************************************************
Function: 
Description: 
				motor vibrate when alert 
	this routine will be invoked per 10ms
*************************************************************/
void mot_vibrate(void)
{
	static unsigned int vibrateCnt_mot = 0;
	
	if(mot_vibration.flag)								// if run motor or not 
	{
		if(0==mot_vibration.setTime)
		{
			MOTOR_STOP();								// stop motor when time out
			mot_vibration.flag = 0;
			mot_vibration.RunTime = 0;
		}
		else
		{
			if(++vibrateCnt_mot>=TIME_VIBRATE_CIRCLE/VIBRATE_TIME_UNIT)	// one circle
			{
				vibrateCnt_mot = 0;
				if(mot_vibration.RunTime<0xffff)	mot_vibration.RunTime++;				
				if(mot_vibration.RunTime>=mot_vibration.setTime)
				{
					mot_vibration.setTime = 0;
					mot_vibration.RunTime = 0;
					mot_vibration.flag = 0;
					vibrateCnt_mot = 0xffff;
					MOTOR_STOP();
				}
			}
			
			if(vibrateCnt_mot<TIME_VIBRATE_POINT1/VIBRATE_TIME_UNIT)		// RUN
			{
				MOTOR_RUN();
			}
			else if(vibrateCnt_mot<TIME_VIBRATE_POINT2/VIBRATE_TIME_UNIT)	// stop
			{
				MOTOR_STOP();
			}
			else if(vibrateCnt_mot<TIME_VIBRATE_POINT3/VIBRATE_TIME_UNIT)	// run
			{
				MOTOR_RUN();
			}
			else										// stop
			{
				MOTOR_STOP();
			}
		}
		
	}
	else
	{
		MOTOR_STOP();
		mot_vibration.setTime = 0;
		mot_vibration.RunTime = 0;
		vibrateCnt_mot = 0;
	}
}

/*************************************************************
Function: 
Description: 
				set the alet time of vibration
				time - mot vibration loops,maximum is 0xffff
				flag - determine vibrate or not
*************************************************************/
void motor_vibration_set(unsigned int time, unsigned char flag)
{
	mot_vibration.flag = flag;
	mot_vibration.setTime = time;
	mot_vibration.RunTime = 0;
}


/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
*************************************************************/
void motor_init(void)
{
	IO_MOTOR_INIT();
	MOTOR_STOP();								// stop when power up
}



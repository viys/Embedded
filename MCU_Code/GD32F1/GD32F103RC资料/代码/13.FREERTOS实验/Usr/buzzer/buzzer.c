/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "buzzer.h"
//--------------------- variable declare ----------------------
int routineclock_cnt = 0;
buz_music_type_t bpMusic;
const tone_type_t tonetypes[] = 
{//	beepDuration	interval	beepTimes
	{	TYPE_3BP_DUR/BP_TIME_UNIT,	TYPE_3BP_ITV/BP_TIME_UNIT,	BP_TIMES_3BP},				// 连续鸣叫3声
	{	TYPE_1BP_DUR/BP_TIME_UNIT,	TYPE_1BP_ITV/BP_TIME_UNIT,	BP_TIMES_1BP},				// 鸣叫1声
};


/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
*************************************************************/
void isr_buz_routine(void)
{
	BUZZER_GPIO_TOGGLE();
}

/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
*************************************************************/
void buzzer_init(void)
{
	IO_BUZZER_MOD_GPIO();
	BUZZER_PWM_INIT(BUZZER_FRQ);
	bpMusic.stage = 0xff;									// all over
    nvic_irq_enable(TIMER1_IRQn, 7, 0);						// main routine clock
}

/*************************************************************
Function: 
Description: 
			typeInx		- beep tone type index
			playTimes	- the beep tone play times
			halt		- the halt time between two beep tone
*************************************************************/
void buzzer_set(unsigned char typeInx, unsigned int playTimes, unsigned int halt)
{	
	bpMusic.toneType = (tone_type_t*)&tonetypes[typeInx];
	bpMusic.halt = halt;
	bpMusic.tonePlayTimes = playTimes;
	bpMusic.stage = 0;
}

/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
				this routine will be invoked per 10ms
*************************************************************/
void buzzer_beep(void)
{
	static tone_type_t buzzer;
	static unsigned int halt;
	static unsigned int tonePlayTimes;
	
	switch(bpMusic.stage)
	{
		case 0:
		{
			buzzer.beepDuration = bpMusic.toneType->beepDuration;
			buzzer.interval = bpMusic.toneType->interval;
			buzzer.beepTimes = bpMusic.toneType->beepTimes;
			halt = bpMusic.halt;
			tonePlayTimes = bpMusic.tonePlayTimes;
			IO_BUZZER_MOD_GPIO();
			BUZZER_PWM_INIT(BUZZER_FRQ);
			BUZZER_PWM_CTL(1);
			bpMusic.stage = 1;
			break;
		}
		case 1:															// reduce beep duration time
		{
			if(buzzer.beepDuration)	buzzer.beepDuration--;

			if(0==buzzer.beepDuration)
			{
				BUZZER_PWM_CTL(0);
				IO_BUZZER_MOD_GPIO();
				BUZZER_OUT(0);
				bpMusic.stage = 2;
			}
			break;
		}
		case 2:															// reduce beep interval time
		{
			if(buzzer.interval)	buzzer.interval--;

			if(0==buzzer.interval)
			{
				BUZZER_PWM_CTL(0);
				IO_BUZZER_MOD_GPIO();
				BUZZER_OUT(0);
				bpMusic.stage = 3;
			}
			break;
		}
		case 3:															// check beep loop times
		{
			if(buzzer.beepTimes)	buzzer.beepTimes--;

			if(buzzer.beepTimes)
			{
				buzzer.beepDuration = bpMusic.toneType->beepDuration;
				buzzer.interval = bpMusic.toneType->interval;
				IO_BUZZER_MOD_GPIO();
				BUZZER_PWM_INIT(BUZZER_FRQ);
				BUZZER_PWM_CTL(1);
				bpMusic.stage = 1;										// back to stage 1
			}
			else
			{
				bpMusic.stage = 4;										// go to stage halt
			}
			break;
		}
		case 4:															// reduce music halt
		{
			if(halt)	halt--;

			if(0==halt)
			{
				bpMusic.stage = 5;										// go to stage tonePlayTimes check
			}
			break;
		}
		case 5:
		{
			if(tonePlayTimes)	tonePlayTimes--;
			
			if(tonePlayTimes)
			{
				buzzer.beepDuration = bpMusic.toneType->beepDuration;
				buzzer.interval = bpMusic.toneType->interval;
				buzzer.beepTimes = bpMusic.toneType->beepTimes;
				halt = bpMusic.halt;
				IO_BUZZER_MOD_GPIO();
				BUZZER_PWM_INIT(BUZZER_FRQ);
				BUZZER_PWM_CTL(1);
				bpMusic.stage = 1;
			}
			else
			{
				BUZZER_PWM_CTL(0);
				IO_BUZZER_MOD_GPIO();
				BUZZER_OUT(0);
				bpMusic.stage = 0xff;									// all over
			}
			break;
		}
		default:
			break;
	}
}

/*************************************************************
Function: 
Description: 
				routineclock_cnt - time of beep
*************************************************************/
void buzzer_beep_stop(void)
{
	BUZZER_PWM_CTL(0);
	IO_BUZZER_MOD_GPIO();
	BUZZER_OUT(0);
	bpMusic.stage = 0xff;									// all over
}





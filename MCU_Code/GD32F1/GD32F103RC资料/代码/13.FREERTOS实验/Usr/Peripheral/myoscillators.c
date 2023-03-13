/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "all.h"
//---------------------- variable define -----------------------
unsigned long SysClk = 0;

/*************************************************************
Function:
Description:
*************************************************************/
void Oscillator_select(char Osc_Mod)
{
	switch(Osc_Mod)
	{
		case OSC_HIR:
				TA = 0xAA;
				TA = 0x55;
				CKEN |= (1<<5);								// enable HIR
				while(!(CKSWT&(1<<5)));				// wait for oscillator stable
				TA = 0xAA;
				TA = 0x55;
				CKSWT = 0x00;
				break;
		case OSC_LIR:
				break;
		case OSC_ECLK:
				break;
		default:
				break;
	}
}


/*************************************************************
Function:
Description:
*************************************************************/
void sys_clock_set(unsigned long Freq)
{
	SysClk = Freq;
	//Oscillator_select(OSC_HIR);		// use HIR,HIR=16M
	CKDIV = 16000000/SysClk/2;			// div
}
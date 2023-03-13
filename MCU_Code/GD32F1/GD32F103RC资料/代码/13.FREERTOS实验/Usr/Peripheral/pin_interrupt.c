/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "all.h"


/*************************************************************
Function:
Description:
*************************************************************/
void PinInterrupt_ISR (void) interrupt 7
{
	//=================================================
	P14 = !P14;
	//=================================================
	rf_decode_PinInt_isr();
	PIF = 0x00;                             	//clear interrupt flag
}

/*************************************************************
Function:
Description:
*************************************************************/
void rf_pin_int_init(void)
{
	P30_Input_Mode;								// P30 as input mode 
	set_P3S_0;									// set p30 as schmitt triggered input
	Enable_INT_Port3;							// select P30 as a interrupt pin
	Enable_BIT0_BothEdge_Trig;					// set the 0 channel of pin-interrupt as both edge trig mode
    set_EPI;									// Enable pin interrupt
}



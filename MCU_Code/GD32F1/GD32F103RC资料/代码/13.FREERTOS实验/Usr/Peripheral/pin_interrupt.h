#ifndef		__PIN_INTERRUPT_H__
#define		__PIN_INTERRUPT_H__

//--------------------- include files ---------------------
#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "oscillators.h"
//--------------------- macro define ----------------------
//--------------------- function declare ----------------------
void rf_pin_int_init(void);
extern void rf_decode_PinInt_isr(void);


#endif	//__PIN_INTERRUPT_H__


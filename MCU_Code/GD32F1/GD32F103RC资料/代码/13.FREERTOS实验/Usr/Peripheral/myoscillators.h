#ifndef		__OSCILLATORS_H__
#define		__OSCILLATORS_H__

//--------------------- include files ---------------------
#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
//--------------------- macro define ----------------------
#define	OSC_HIR		0
#define	OSC_LIR		1
#define	OSC_ECLK	2


//--------------------- function declare ------------------
extern unsigned long SysClk;
//--------------------- function declare ------------------
void sys_clock_set(unsigned long Freq);

#endif	//__OSCILLATORS_H__


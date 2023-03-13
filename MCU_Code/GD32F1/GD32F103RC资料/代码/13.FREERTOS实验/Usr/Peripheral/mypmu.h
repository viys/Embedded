#ifndef		__MY_PMU_H__
#define		__MY_PMU_H__

//---------------------- include files -----------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void PMU_init(void);
void mcu_goto_sleep(void);
void mcu_goto_deepsleep(void);
void recover_sys_clock(void);




#endif	//__MY_PMU_H__


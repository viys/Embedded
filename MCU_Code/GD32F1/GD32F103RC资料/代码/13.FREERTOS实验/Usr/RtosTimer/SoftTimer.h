/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
#ifndef		__SOFT_TIMER_H__
#define		__SOFT_TIMER_H__
//------------------------------- include files ----------------------------------------
#include "gd32e10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
//-------------------------------- macro define ----------------------------------------
//#define TIME_CLOCK_COLON_FLASH		( pdMS_TO_TICKS( 500UL ) )
#define TIME_CLOCK_COLON_FLASH		( 500 / portTICK_PERIOD_MS )
//------------------------------ varialbe declare --------------------------------------
//------------------------------ function declare --------------------------------------
void ClockColonFlash_timer_init(void);
void ClockColonFlash_timer_stop(void);
void ClockColonFlash_timer_reset(void);





#endif	// __SOFT_TIMER_H__



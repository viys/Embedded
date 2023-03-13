#ifndef		__CMT2300A_DELAY_DEFS_H__
#define		__CMT2300A_DELAY_DEFS_H__

//--------------------------------- include files ---------------------
#include "gd32e10x.h"
#include "FreeRTOS.h"
#include "task.h"
//--------------------------------- macro define ----------------------
#ifndef		delay_1ms
#define		delay_1ms(x)		vTaskDelay(x / portTICK_PERIOD_MS);
#endif
#define		CMT2300A_DelayMs(x)		delay_1ms(x)
#define		CMT2300A_DelayUs(us)    do{	\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
										__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();			\
									}while(0)


//-------------------------------- variable declare -------------------
//-------------------------------- function declare -------------------



#endif	//__CMT2300A_DELAY_DEFS_H__


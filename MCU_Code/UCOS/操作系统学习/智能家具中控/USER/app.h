#ifndef __APP_H
#define __APP_H

#include "includes.h"

extern OS_TCB AppTaskNRFTCB;
extern OS_TCB AppTaskLCDTCB;
extern OS_TCB AppTaskWKUPTCB;

extern CPU_STK AppTaskNRFStk[APP_TASK_NRF_STK_SIZE];
extern CPU_STK AppTaskLCDStk[APP_TASK_LCD_STK_SIZE];
extern CPU_STK AppTaskWKUPStk[APP_TASK_WKUP_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskLCD(void *p_arg);
void AppTaskNRF(void *p_arg);
void AppTaskWKUP(void * p_arg);

#endif

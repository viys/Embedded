#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_TCB AppTaskStartTCB; //启动任务控制块
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_TCB AppTaskTmrTCB;

static CPU_STK AppTaskTmrStk[APP_TASK_TMR_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskTmr(void *p_arg);
void TmrCallback(OS_TMR *p_tmr,void *p_arg);

#endif

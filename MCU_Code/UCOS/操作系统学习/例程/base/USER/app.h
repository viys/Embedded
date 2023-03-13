#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_TCB 	AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_TCB 	AppTaskRelays1TCB;
static CPU_STK AppTaskRelays1Stk[APP_TASK_RELAYS1_STK_SIZE];

static OS_TCB 	AppTaskRelays2TCB;
static CPU_STK AppTaskRelays2Stk[APP_TASK_RELAYS2_STK_SIZE];

void AppTaskStart(void *p_arg);
void Relays1_Task(void *p_arg);
void Relays2_Task(void *p_arg);
#endif

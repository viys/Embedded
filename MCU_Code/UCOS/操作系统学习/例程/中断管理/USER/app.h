#ifndef __APP_H
#define __APP_H

#include "includes.h"

extern OS_TCB AppTaskWKUPTCB;
extern OS_TCB AppTaskUsartTCB;

static CPU_STK AppTaskWKUPStk[APP_TASK_WKUP_STK_SIZE];
static CPU_STK AppTaskUsartStk[APP_TASK_USART_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskWKUP(void *p_arg);
void AppTaskUsart(void *p_arg);

#endif

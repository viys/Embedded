#ifndef __APP_H
#define __APP_H

#include "includes.h"


extern OS_TCB AppTaskLED1TCB;
extern OS_TCB AppTaskLED2TCB;
extern OS_TCB AppTaskUSART1TCB;
extern OS_TCB AppTaskStatusTCB;

static CPU_STK AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskUsartStk[APP_TASK_USART1_STK_SIZE];
static CPU_STK AppTaskStatusStk[APP_TASK_STATUS_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskLed1(void *p_arg);
void AppTaskLed2(void *p_arg);
void AppTaskUsart1(void * p_arg);
void AppTaskStatus(void *p_arg);

#endif

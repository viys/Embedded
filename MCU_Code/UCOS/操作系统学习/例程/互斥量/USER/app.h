#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_TCB AppTaskStartTCB; //����������ƿ�
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_MUTEX TestMutex; //������

static OS_TCB AppTaskLED1TCB;
static OS_TCB AppTaskLED2TCB;
static OS_TCB AppTaskLED3TCB;

static CPU_STK AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskLed1(void *p_arg);
void AppTaskLed2(void *p_arg);
void AppTaskLed3(void *p_arg);

#endif

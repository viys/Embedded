#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_TCB AppTaskStartTCB; //启动任务控制块
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_SEM TestSem; //信号量

static OS_TCB AppTaskUSART1TCB;
static OS_TCB AppTaskWKUPTCB;
static OS_TCB AppTaskLED1TCB;
static OS_TCB AppTaskLED2TCB;
static OS_TCB AppTaskLED3TCB;

static CPU_STK AppTaskUsartStk[APP_TASK_USART1_STK_SIZE];
static CPU_STK AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];
static	CPU_STK AppTaskWKUPStk[APP_TASK_WKUP_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskLed1(void *p_arg);
void AppTaskLed2(void *p_arg);
void AppTaskLed3(void *p_arg);
void AppTaskWKUP(void *p_arg);
void AppTaskUsart1(void * p_arg);

#endif

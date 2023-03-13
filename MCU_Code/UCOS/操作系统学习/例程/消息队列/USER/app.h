#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_Q queue; //声明消息队列

static OS_TCB AppTaskStartTCB;    //任务控制块
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_TCB AppTaskUSART1TCB;
static OS_TCB AppTaskPostTCB;
static OS_TCB AppTaskPendTCB;
static OS_TCB AppTaskLED1TCB;
static OS_TCB AppTaskLED2TCB;

static CPU_STK AppTaskUsartStk [ APP_TASK_USART1_STK_SIZE ];
static CPU_STK AppTaskPostStk [ APP_TASK_POST_STK_SIZE ];
static CPU_STK AppTaskPendStk [ APP_TASK_PEND_STK_SIZE ];
static CPU_STK AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];
static CPU_STK AppTaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];


void AppTaskStart(void *p_arg);
void AppTaskUsart1(void * p_arg);
void AppTaskPost(void * p_arg);
void AppTaskPend(void * p_arg);
void AppTaskLed1(void *p_arg);
void AppTaskLed2(void *p_arg);

#endif

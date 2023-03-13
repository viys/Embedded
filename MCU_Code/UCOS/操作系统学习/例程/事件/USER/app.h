#ifndef __APP_H
#define __APP_H

#include "includes.h"

static OS_FLAG_GRP flag_grp; //�����¼���־��

#define EVENT1 (0x01<<0) //�����¼������λ0
#define EVENT2 (0x01<<1) //�����¼������λ1

static OS_TCB AppTaskStartTCB; //����������ƿ�
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

static OS_TCB AppTaskPostTCB;
static OS_TCB AppTaskPendTCB;

static CPU_STK AppTaskPostStk[APP_TASK_POST_STK_SIZE];
static CPU_STK AppTaskPendStk[APP_TASK_PEND_STK_SIZE];

void AppTaskStart(void *p_arg);
void AppTaskPost(void *p_arg);
void AppTaskPend(void *p_arg);

#endif

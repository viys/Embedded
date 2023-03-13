#include "app.h"
#include <string.h>
int main(){
	OS_ERR err;
	
	CPU_Init();
	
	BSP_Init();
	
	OSInit(&err);
	
	OSTaskCreate(&AppTaskStartTCB,
				"APPTASKStart",
				AppTaskStart,
				0,//参数
				APP_TASK_START_PRIO,//优先级
				AppTaskStartStk,//任务栈起始地址
				APP_TASK_START_STK_SIZE/10,//剩余栈
				APP_TASK_START_STK_SIZE,//任务栈大小
				5,//消息队列大小
				0,
				0,
				OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				&err);

	printf("HELLO_UCOSII\r\n");
	
	OSStart(&err);
}


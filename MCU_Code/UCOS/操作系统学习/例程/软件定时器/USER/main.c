#include "app.h"

int main(){
	OS_ERR err;	
	
	BSP_Init();
	OSInit(&err);

	OSTaskCreate(&AppTaskStartTCB,"Task_Start",AppTaskStart,(void *)0,1,\
					&AppTaskStartStk[0], APP_TASK_START_STK_SIZE/10,APP_TASK_START_STK_SIZE,\
						5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	printf("任务初始化成功\r\n");
	
	OSStart(&err);
}



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
				0,//����
				APP_TASK_START_PRIO,//���ȼ�
				AppTaskStartStk,//����ջ��ʼ��ַ
				APP_TASK_START_STK_SIZE/10,//ʣ��ջ
				APP_TASK_START_STK_SIZE,//����ջ��С
				5,//��Ϣ���д�С
				0,
				0,
				OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				&err);

	printf("HELLO_UCOSII\r\n");
	
	OSStart(&err);
}


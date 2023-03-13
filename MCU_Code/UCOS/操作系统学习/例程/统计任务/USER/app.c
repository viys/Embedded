#include "app.h"

OS_TCB AppTaskLED1TCB;
OS_TCB AppTaskLED2TCB;
OS_TCB AppTaskUSART1TCB;
OS_TCB AppTaskStatusTCB;

void AppTaskLed1(void *p_arg){
	OS_ERR      err;
	
	(void)p_arg;
	
	while(DEF_TRUE){
		LED_State_Config(LED1,TOGGLE);
		printf("AppTaskLed1 Running\n");
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
	}
}

void AppTaskLed2(void *p_arg){
	OS_ERR err;
	
	(void)p_arg;
	
	while(DEF_TRUE){
		LED_State_Config(LED2,TOGGLE);
		printf("AppTaskLed2 Running\n");
		OSTimeDlyHMSM(0,0,0,1000,OS_OPT_TIME_PERIODIC,&err);
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;

	(void)p_arg;

	while(DEF_TRUE) {                                 //�����壬ͨ��д��һ����ѭ��                            
		printf("AppTaskUsart Running\n");
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
		}
}

void AppTaskStatus(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OS_CRITICAL_ENTER();
		
		printf("CPU������:%d.%d%\r\n",OSStatTaskCPUUsage/100,OSStatTaskCPUUsage%100);
		printf("CPU��������ʣ�%d.%d%%\r\n",OSStatTaskCPUUsageMax/100,OSStatTaskCPUUsageMax%100);

//		printf("LED1�����CPU�����ʣ�%d.%d%%\r\n",AppTaskLED1TCB.CPUUsageMax/100,AppTaskLED1TCB.CPUUsageMax%100); 
//		printf("LED2�����CPU�����ʣ�%d.%d%%\r\n",AppTaskLED2TCB.CPUUsageMax/100,AppTaskLED2TCB.CPUUsageMax%100); 
//		printf("USART1�����CPU�����ʣ�%d.%d%%\r\n",AppTaskUSART1TCB.CPUUsageMax/100,AppTaskUSART1TCB.CPUUsageMax%100); 
//		printf("ͳ�������CPU�����ʣ�%d.%d%%\r\n",AppTaskStatusTCB.CPUUsageMax/100,AppTaskStatusTCB.CPUUsageMax%100); 

//		printf("LED1��������úͿ���ջ��С�ֱ�Ϊ��%d,%d\r\n",AppTaskLED1TCB.StkUsed,AppTaskLED1TCB.StkFree); 
//		printf("LED2��������úͿ���ջ��С�ֱ�Ϊ��%d,%d\r\n",AppTaskLED2TCB.StkUsed,AppTaskLED2TCB.StkFree); 
//		printf("USART1���������ջ�Ϳ���ջ��С�ֱ�Ϊ��%d,%d\r\n",AppTaskUSART1TCB.StkUsed,AppTaskUSART1TCB.StkFree);     
//		printf("ͳ�����������ջ�Ϳ���ջ��С�ֱ�Ϊ��%d,%d\r\n",AppTaskStatusTCB.StkUsed,AppTaskStatusTCB.StkFree);
		
		OS_CRITICAL_EXIT();
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
	}
	
}




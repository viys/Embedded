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

	while(DEF_TRUE) {                                 //任务体，通常写成一个死循环                            
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
		
		printf("CPU利用率:%d.%d%\r\n",OSStatTaskCPUUsage/100,OSStatTaskCPUUsage%100);
		printf("CPU最大利用率：%d.%d%%\r\n",OSStatTaskCPUUsageMax/100,OSStatTaskCPUUsageMax%100);

//		printf("LED1任务的CPU利用率：%d.%d%%\r\n",AppTaskLED1TCB.CPUUsageMax/100,AppTaskLED1TCB.CPUUsageMax%100); 
//		printf("LED2任务的CPU利用率：%d.%d%%\r\n",AppTaskLED2TCB.CPUUsageMax/100,AppTaskLED2TCB.CPUUsageMax%100); 
//		printf("USART1任务的CPU利用率：%d.%d%%\r\n",AppTaskUSART1TCB.CPUUsageMax/100,AppTaskUSART1TCB.CPUUsageMax%100); 
//		printf("统计任务的CPU利用率：%d.%d%%\r\n",AppTaskStatusTCB.CPUUsageMax/100,AppTaskStatusTCB.CPUUsageMax%100); 

//		printf("LED1任务的已用和空闲栈大小分别为：%d,%d\r\n",AppTaskLED1TCB.StkUsed,AppTaskLED1TCB.StkFree); 
//		printf("LED2任务的已用和空闲栈大小分别为：%d,%d\r\n",AppTaskLED2TCB.StkUsed,AppTaskLED2TCB.StkFree); 
//		printf("USART1任务的已用栈和空闲栈大小分别为：%d,%d\r\n",AppTaskUSART1TCB.StkUsed,AppTaskUSART1TCB.StkFree);     
//		printf("统计任务的已用栈和空闲栈大小分别为：%d,%d\r\n",AppTaskStatusTCB.StkUsed,AppTaskStatusTCB.StkFree);
		
		OS_CRITICAL_EXIT();
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
	}
	
}




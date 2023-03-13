#include "app.h"
#include "cstring"

OS_TCB AppTaskWKUPTCB;
OS_TCB AppTaskUsartTCB;

extern char Usart_Rx_Buf[USART_RBUFF_SIZE];

void AppTaskWKUP(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OSTaskSemPend((OS_TICK)0,
					  (OS_OPT)OS_OPT_PEND_BLOCKING,
					  (CPU_TS *)0,
					  (OS_ERR *)&err);
		
		OS_CRITICAL_ENTER();	
		printf("触发按键中断\r\n");
		OS_CRITICAL_EXIT();
	}
}

void AppTaskUsart(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OSTaskSemPend((OS_TICK )0, //无期限等待
					  (OS_OPT )OS_OPT_PEND_BLOCKING, //如果信号量不可用就等待
					  (CPU_TS *)0,
					  (OS_ERR *)&err);
		
		OS_CRITICAL_ENTER();
		printf("收到数据:%s\r\n",Usart_Rx_Buf);
		memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);
		OS_CRITICAL_EXIT();
	}
}






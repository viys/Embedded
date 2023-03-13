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
		printf("���������ж�\r\n");
		OS_CRITICAL_EXIT();
	}
}

void AppTaskUsart(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OSTaskSemPend((OS_TICK )0, //�����޵ȴ�
					  (OS_OPT )OS_OPT_PEND_BLOCKING, //����ź��������þ͵ȴ�
					  (CPU_TS *)0,
					  (OS_ERR *)&err);
		
		OS_CRITICAL_ENTER();
		printf("�յ�����:%s\r\n",Usart_Rx_Buf);
		memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);
		OS_CRITICAL_EXIT();
	}
}






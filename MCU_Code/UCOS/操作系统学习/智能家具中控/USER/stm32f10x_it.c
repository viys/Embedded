#include "stm32f10x_it.h"
#include "includes.h"

extern OS_TCB AppTaskWKUPTCB;
extern OS_TCB AppTaskUsartTCB;

void EXTI0_IRQHandler(void){
	OS_ERR err;
	
	OSIntEnter(); //进入中断
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		OSTaskSemPost((OS_TCB *)&AppTaskWKUPTCB, //目标任务
					  (OS_OPT  )OS_OPT_POST_NONE, //没选项要求
					  (OS_ERR *)&err);
		EXTI_ClearITPendingBit(EXTI_Line0); //清楚中断标志位
	}
	
	OSIntExit(); //退出中断
}

//void USART1_IRQHandler(void){
//	OS_ERR err;
//	
//	OSIntEnter();
//	
//	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET){
//		DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);

//		USART_ReceiveData(USART1); /* 清除标志位 */

//		//清DMA标志位
//		DMA_ClearFlag(DMA1_FLAG_TC5);
//		//重新赋值计数值，必须大于等于最大可能接收到的数据帧数目/ 
//		USART_RX_DMA_CHANNEL->CNDTR = USART_RBUFF_SIZE;
//		DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);

//		//给出信号量，发送接收到新数据标志，供前台程序查询

//		/* 发送任务信号量到任务 AppTaskKey */
//		OSTaskSemPost((OS_TCB *)&AppTaskUsartTCB, //目标任务
//					  (OS_OPT )OS_OPT_POST_NONE, //没选项要求
//					  (OS_ERR *)&err);
//	
//	}
//	OSIntExit();
//}





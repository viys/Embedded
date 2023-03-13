#include "stm32f10x_it.h"
#include "includes.h"

extern OS_TCB AppTaskWKUPTCB;
extern OS_TCB AppTaskUsartTCB;

void EXTI0_IRQHandler(void){
	OS_ERR err;
	
	OSIntEnter(); //�����ж�
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		OSTaskSemPost((OS_TCB *)&AppTaskWKUPTCB, //Ŀ������
					  (OS_OPT  )OS_OPT_POST_NONE, //ûѡ��Ҫ��
					  (OS_ERR *)&err);
		EXTI_ClearITPendingBit(EXTI_Line0); //����жϱ�־λ
	}
	
	OSIntExit(); //�˳��ж�
}

//void USART1_IRQHandler(void){
//	OS_ERR err;
//	
//	OSIntEnter();
//	
//	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET){
//		DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);

//		USART_ReceiveData(USART1); /* �����־λ */

//		//��DMA��־λ
//		DMA_ClearFlag(DMA1_FLAG_TC5);
//		//���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ/ 
//		USART_RX_DMA_CHANNEL->CNDTR = USART_RBUFF_SIZE;
//		DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);

//		//�����ź��������ͽ��յ������ݱ�־����ǰ̨�����ѯ

//		/* ���������ź��������� AppTaskKey */
//		OSTaskSemPost((OS_TCB *)&AppTaskUsartTCB, //Ŀ������
//					  (OS_OPT )OS_OPT_POST_NONE, //ûѡ��Ҫ��
//					  (OS_ERR *)&err);
//	
//	}
//	OSIntExit();
//}





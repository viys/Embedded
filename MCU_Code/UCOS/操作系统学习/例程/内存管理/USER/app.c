#include "app.h"

void AppTaskStart(void *p_arg){
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

    (void)p_arg;

    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    /* ����ʱ��Ƭ��ת���� */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //ʹ��ʱ��Ƭ��ת����
		                 (OS_TICK       )0,                     //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
					     (OS_ERR       *)&err);
	//�����ڴ�������
	OSMemCreate((OS_MEM    *)&mem,
				(CPU_CHAR  *)"Mem For Test",
				(void     *)ucArray,
				(OS_MEM_QTY )3,
				(OS_MEM_SIZE)20,
				(OS_ERR    *)&err);
	//����Post����		 
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Post",                             //��������
                 (OS_TASK_PTR ) AppTaskPost,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskPostStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
	//����Pend����		 
	OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Pend",                             //��������
                 (OS_TASK_PTR ) AppTaskPend,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_PEND_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskPendStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
				 
	OSTaskDel (0, &err);                     //ɾ����ʼ������������������
}

extern size_t strlen(const char *);

void AppTaskPost(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	char *p_mem_blk;
	uint32_t ulCount=0;
	
	while(DEF_TRUE){
		//��mem��ȡ�ڴ�
		p_mem_blk = OSMemGet((OS_MEM *)&mem,
							 (OS_ERR *)&err);
		
		sprintf(p_mem_blk,"%d",ulCount++); //���ڴ���ȡ����ֵ
		
		OSTaskQPost((OS_TCB    *)&AppTaskPendTCB,
					 (void     *)p_mem_blk, //��Ϣ���ݵ��׵�ַ
					 (OS_MSG_SIZE)strlen(p_mem_blk),
					 (OS_OPT     )OS_OPT_POST_FIFO,
					 (OS_ERR    *)&err);
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}

void AppTaskPend(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	char *pMsg;
	OS_MSG_SIZE msg_size;
	
	while(DEF_TRUE){
		//��������,�ȴ���Ϣ
		pMsg = OSTaskQPend((OS_TICK      )0, //�����ڵȴ�
						   (OS_OPT    	 )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
						   (OS_MSG_SIZE *)&msg_size, //������Ϣ����
						   (CPU_TS 		*)0,
						   (OS_ERR 		*)&err);
		
		printf("\r\n���յ�����Ϣ������:%s,������:%d�ֽ�.\r\n",pMsg,msg_size);
		
		OSMemPut((OS_MEM *)&mem,
				 (void *)pMsg,
				 (OS_ERR *)&err);
	}
}






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

	//�����¼���־��
	OSFlagCreate((OS_FLAG_GRP *)&flag_grp,
				 (CPU_CHAR *)"FLAG For Test",
				 (OS_FLAGS)0,
				 (OS_ERR *)&err);
	//����AppTaskPost����
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                            	//������ƿ��ַ
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
	//����AppTaskPend����
    OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                            	//������ƿ��ַ
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
    
	OSTaskDel (0, &err); //ɾ����ʼ������������������
}

void AppTaskPost(void *p_arg){
	OS_ERR err;
	(void)p_arg;

	while(DEF_TRUE){
		switch(KEY_Scan(WKUP)){
			case KEY_HIGH:
				printf("WKUP ������\r\n");
				OSFlagPost(&flag_grp,EVENT1,OS_OPT_POST_FLAG_SET,&err); //����־λ��BIT0��1
				OSFlagPost(&flag_grp,EVENT2,OS_OPT_POST_FLAG_SET,&err); //����־λ��BIT0��1
				break;
			case KEY_LOW:
				//printf("WKUP δ������\r\n");
				break;
			default:
				break;
		}
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_DLY,&err);
	}
}

void AppTaskPend(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	OS_FLAGS flags_rdy;
	
	while(DEF_TRUE){
		//�ȴ���־���BIT0��BIT1������1
		flags_rdy = OSFlagPend(&flag_grp,
							   (EVENT1|EVENT2),
							   0,
							   (OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_BLOCKING|OS_OPT_PEND_FLAG_CONSUME),
							   0,
							   &err);
		if((flags_rdy&(EVENT1|EVENT2)) == (EVENT1|EVENT2)){
			printf ("ͬ���ɹ�\r\n");
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
	}
}





















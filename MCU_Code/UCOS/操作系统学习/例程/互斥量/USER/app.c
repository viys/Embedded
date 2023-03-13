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

	/* ����������TestMutex */
	OSMutexCreate((OS_MUTEX *)&TestMutex, //ָ�򻥳���������ָ��
				  (CPU_CHAR *)"Mutex For Test", //������������
				  (OS_ERR   *)&err);
	
	//����LED1����
    OSTaskCreate((OS_TCB     *)&AppTaskLED1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led1",                             //��������
                 (OS_TASK_PTR ) AppTaskLed1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
	//����LED2����
    OSTaskCreate((OS_TCB     *)&AppTaskLED2TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led2",                             //��������
                 (OS_TASK_PTR ) AppTaskLed2,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed2Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
	//����LED3����
    OSTaskCreate((OS_TCB     *)&AppTaskLED3TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led3",                             //��������
                 (OS_TASK_PTR ) AppTaskLed3,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED3_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed3Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED3_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);

    
	OSTaskDel (0, &err); //ɾ����ʼ������������������
}

void AppTaskLed1(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	uint32_t i;
	while(DEF_TRUE){
		OSMutexPend((OS_MUTEX *)&TestMutex, //���뻥����
				    (OS_TICK   )0, //�����޵ȴ�
					(OS_OPT    )OS_OPT_PEND_BLOCKING, //���û���ź������þ͵ȴ�
					(CPU_TS   *)0, //����ȡʱ���
					(OS_ERR   *)&err);
		if(OS_ERR_NONE==err)
			printf("AppTaskLed1 ��ȡ������\r\n");
		
		for(i=0;i<60;i++)//ģ������ȼ�����ռ�û�����
		{
			OSSched();//�����������
		}

		OSMutexPost((OS_MUTEX *)&TestMutex, //�ͷŻ�����
					(OS_OPT    )OS_OPT_POST_NONE, //�����������
					(OS_ERR   *)&err);
		if(OS_ERR_NONE==err)
			printf("AppTaskLed1 ��ȡ������\r\n");
		
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
	}
}

void AppTaskLed2(void *p_arg){
	OS_ERR err;
	
	(void)p_arg;
	
	while(DEF_TRUE){
		
		printf("AppTaskLed2 Running\r\n");

		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);
		
	}
}

void AppTaskLed3(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	uint32_t i;
	while(DEF_TRUE){
		OSMutexPend((OS_MUTEX *)&TestMutex, //���뻥����
					(OS_TICK   )0, //�����޵ȴ�
					(OS_OPT	   )OS_OPT_PEND_BLOCKING, //����������뵽�ź�������������
					(CPU_TS   *)0, //����ȡʱ���
					(OS_ERR   *)&err);
		if(OS_ERR_NONE==err)
			printf("AppTaskLed3 ��ȡ������\r\n");
		
		OSMutexPost((OS_MUTEX *)&TestMutex, //�ͷŻ�����
					(OS_OPT	   )OS_OPT_POST_NONE, //�����������
					(OS_ERR   *)&err);
		if(OS_ERR_NONE==err)
			printf("AppTaskLed3 �ͷŻ�����\r\n");
		
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);
	}
}





















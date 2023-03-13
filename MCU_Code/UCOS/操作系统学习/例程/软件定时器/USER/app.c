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


	//����AppTaskPost����
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                            	//������ƿ��ַ
                 (CPU_CHAR   *)"App Task Tmr",                             //��������
                 (OS_TASK_PTR ) AppTaskTmr,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
    
	OSTaskDel (0, &err); //ɾ����ʼ������������������
}

void AppTaskTmr(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	OS_TMR my_tmr;
	
	//���������ʱ��
	OSTmrCreate((OS_TMR *)&my_tmr, //�����ʱ��
				(CPU_CHAR *)"MySoftTimer",
				(OS_TICK)10, //��ʱ����ʼֵ
				(OS_TICK)10, //��ʱ����������ֵ
				(OS_OPT)OS_OPT_TMR_PERIODIC, //�����Զ�ʱ
				(OS_TMR_CALLBACK_PTR)TmrCallback, //�ص�����
				(void *)"Timer Over!", //����ʵ�θ��ص�����
				(OS_ERR *)&err);
	
	//���������ʱ��
	OSTmrStart((OS_TMR *)&my_tmr, //�����ʱ������
			   (OS_ERR *)&err);
				
	while(DEF_TRUE){
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
	
}

void TmrCallback(OS_TMR *p_tmr,void *p_arg){
	printf("�����ʱ��������\r\n");
}


















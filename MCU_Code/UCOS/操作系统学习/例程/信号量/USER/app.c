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
					     (OS_ERR       *)&err);                //���ش�������

	/* �����ź��� SemOfKey */
	OSSemCreate((OS_SEM *)&SemOfKey, //ָ���ź���������ָ��
	(CPU_CHAR *)"SemOfKey", //�ź���������
	(OS_SEM_CTR )0,//�ź���������ָʾ�¼�����,���Ը�ֵΪ0,��ʾ�¼���û�з���
	(OS_ERR *)&err);
	
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

	//����USART����
    OSTaskCreate((OS_TCB     *)&AppTaskUSART1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task USART1",                             //��������
                 (OS_TASK_PTR ) AppTaskUsart1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
				 
	//����WKUP����
    OSTaskCreate((OS_TCB     *)&AppTaskWKUPTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task WKUP",                             //��������
                 (OS_TASK_PTR ) AppTaskWKUP,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_WKUP_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskWKUPStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
    
	OSTaskDel (0, &err); //ɾ����ʼ������������������
}

void AppTaskWKUP(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	while(DEF_TRUE){
		switch(WKUP_Scan(WKUP)){
			case KEY_HIGH:
				printf("WKUP is High\r\n");
				OSSemPost(&SemOfKey,OS_OPT_POST_ALL,&err);
				break;
			case KEY_LOW:
				printf("WKUP is Low\r\n");
				break;
			default:
				printf("WKUP is err\r\n");
				break;
		}
		OSTimeDlyHMSM(0, 0, 0, 20, OS_OPT_TIME_DLY, &err);
	}
	
}

void AppTaskLed1(void *p_arg){
	OS_ERR err;
	(void)p_arg;

	while(DEF_TRUE){
		OSSemPend(&SemOfKey,
				  0, //�����޵ȴ�
				  OS_OPT_PEND_BLOCKING, //���û���ź������þ͵ȴ�
				  (CPU_TS *)0, //����ȡʱ���
				  &err);
		LED_State_Config(LED1,TOGGLE);

		OSTimeDly(100,	OS_OPT_TIME_DLY, &err);
	}
}

void AppTaskLed2(void *p_arg){
	OS_ERR err;
	
	(void)p_arg;
	
	while(1){
		LED_State_Config(LED2,TOGGLE);
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (1) {                                 //�����壬ͨ��д��һ����ѭ��                            
		printf("AppTaskUsart Running\n");
		value = OSTaskRegGet ( 0, 0, & err );            //��ȡ��������Ĵ���ֵ
		
		if ( value < 5 )                                 //�������Ĵ���ֵ<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //�����ۼ�����Ĵ���ֵ
		}
		else                                             //����ۼӵ�5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //������Ĵ���ֵ����
			printf("����Usart����������\n");
			OSTaskSuspend ( 0, & err );                    //��������
			
		}

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		}
}
		




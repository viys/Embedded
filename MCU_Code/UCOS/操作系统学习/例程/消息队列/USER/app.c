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
	
	//������Ϣ����
    OSQCreate ((OS_Q         *)&queue,            //ָ����Ϣ���е�ָ��
               (CPU_CHAR     *)"Queue For Test",  //���е�����
               (OS_MSG_QTY    )20,                //���ɴ����Ϣ����Ŀ
               (OS_ERR       *)&err);
	//����USART1����
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

void AppTaskPost(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	while(DEF_TRUE){
		OSQPost((OS_Q 	   *)&queue, //��Ϣ���� ָ��
				(void	   *)"UCOSIII", //Ҫ���͵����ݵ�ָ��
				(OS_MSG_SIZE)sizeof("UCOSIII"),//�����ֽڴ�С
				(OS_OPT		)OS_OPT_POST_FIFO|OS_OPT_POST_ALL, //�Ƚ��ȳ��ͷ�����ȫ���������ʽ
				(OS_ERR    *)&err);
		//OSTimeDly(500, OS_OPT_TIME_DLY, &err);
				OSTimeDlyHMSM ( 0, 0, 0, 500, OS_OPT_TIME_DLY, & err );
	}
}


void AppTaskPend(void * p_arg){
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	CPU_SR_ALLOC(); //ʹ�õ��ٽ��(�ڹ�/���ж�ʱ)ʱ�����õ��ú�,�ú�������
					//����һ���ֲ�����,���ڱ�����ж�ǰ��CPU״̬�Ĵ���
					//SR(�ٽ�ι��ж�ֻ�豣��SR),���ж�ʱ����ֵ��ԭ
	char * pMsg;
	(void)p_arg;
	
	while(DEF_TRUE){
		pMsg = OSQPend((OS_Q 		*)&queue, //��Ϣ����ָ��
					   (OS_TICK		 )0, //�ȴ�ʱ��Ϊ����
					   (OS_OPT		 )OS_OPT_PEND_BLOCKING, //���û�л�ȡ���ź����͵ȴ�
					   (OS_MSG_SIZE *)&msg_size, //��ȡ��Ϣ���ֽڴ�С
					   (CPU_TS 		*)0, //��ȡ�����͵�ʱ���
					   (OS_ERR 		*)&err);
		if(err==OS_ERR_NONE){
			OS_CRITICAL_ENTER();
			printf("\r\n������Ϣ����:%d�ֽ�,����:%s\r\n",msg_size,pMsg);
			OS_CRITICAL_EXIT();
		}
	}
}

void AppTaskLed1(void *p_arg){
	OS_ERR      err;
	OS_REG      value;
	
	(void)p_arg;
	
	while(1){
		LED_State_Config(LED1,TOGGLE);
		value = OSTaskRegGet(0,0,&err);
		
		if(value<10){
			OSTaskRegSet(0, 0, ++ value, &err);
		}
		else{
			OSTaskRegSet(0, 0, 0, &err);
			
			OSTaskResume(&AppTaskLED2TCB, &err);
			printf("�ָ�LED2����\r\n");
			OSTaskResume(&AppTaskUSART1TCB, &err);
			printf("�ָ�USART1����\r\n");
		}
		OSTimeDly(1000,	OS_OPT_TIME_DLY, &err);
	}
}

void AppTaskLed2(void *p_arg){
	OS_ERR err;
	OS_REG value;
	
	(void)p_arg;
	
	while(1){
		LED_State_Config(LED2,TOGGLE);
		
		value = OSTaskRegGet ( 0, 0, & err );            //��ȡ��������Ĵ���ֵ
		
		if ( value < 5 )                                 //�������Ĵ���ֵ<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //�����ۼ�����Ĵ���ֵ
		}
		else                                             //����ۼӵ�5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //������Ĵ���ֵ��0
			printf("����LED2����������\r\n");
			OSTaskSuspend (&AppTaskLED2TCB, & err );       //�������� OSTaskSuspend()��һ��������0ʱ��������
			
		}
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (1) {                                 //�����壬ͨ��д��һ����ѭ��                            
		printf("AppTaskUsart Running\n");
		value = OSTaskRegGet (0, 0, & err);            //��ȡ��������Ĵ���ֵ
		
		if (value < 5)                                 //�������Ĵ���ֵ<5
		{
			OSTaskRegSet (0, 0, ++ value, & err);        //�����ۼ�����Ĵ���ֵ
		}
		else                                             //����ۼӵ�5
		{
			OSTaskRegSet (0, 0, 0, & err);               //������Ĵ���ֵ����
			printf("����Usart����������\n");
			OSTaskSuspend (0, & err);                    //��������
			
		}

		OSTimeDly (1000, OS_OPT_TIME_DLY, & err);      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		}
}
		




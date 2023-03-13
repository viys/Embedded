#include "app.h"

static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

int main(){
	OS_ERR err;	
	
	BSP_Init();
	OSInit(&err);
	
	OSTaskCreate(&AppTaskStartTCB,"Task_Start",AppTaskStart,(void *)0,1,\
					&AppTaskStartStk[0], APP_TASK_START_STK_SIZE/10,APP_TASK_START_STK_SIZE,\
						5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	
	OSStart(&err);
}

void AppTaskStart(void *p_arg){
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

    (void)p_arg;

    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��
	
    /* ����ʱ��Ƭ��ת���� */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //ʹ��ʱ��Ƭ��ת����
		                 (OS_TICK       )0,                     //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
					     (OS_ERR       *)&err);

	//����WKUP����		 
    OSTaskCreate((OS_TCB     *)&AppTaskWKUPTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task WKUP",                             //��������
                 (OS_TASK_PTR ) AppTaskWKUP,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_WKUP_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskWKUPStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵ��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);
	//���� AppTaskUsart ����
    OSTaskCreate((OS_TCB     *)&AppTaskUsartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Usart",                            //��������
                 (OS_TASK_PTR ) AppTaskUsart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);  
	
	OSTaskDel(0,&err);                     //ɾ����ʼ������������������
}





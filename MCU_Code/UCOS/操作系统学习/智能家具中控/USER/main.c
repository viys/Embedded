#include "app.h"

static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

int main(){
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();
	BSP_Init();
	OS_CRITICAL_EXIT();
	
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
	
    //����ʱ��Ƭ��ת����
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //ʹ��ʱ��Ƭ��ת����
		                 (OS_TICK       )0,                     //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
					     (OS_ERR       *)&err);
	//����LCD����
	OSTaskCreate(&AppTaskLCDTCB,"APP Task LCD",AppTaskLCD,(void *)0,1,\
				 &AppTaskLCDStk[0], APP_TASK_LCD_STK_SIZE/10,APP_TASK_LCD_STK_SIZE,\
				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	//����WKUP����
	OSTaskCreate(&AppTaskWKUPTCB,"APP Task WKUP",AppTaskWKUP,(void *)0,1,\
				 &AppTaskWKUPStk[0], APP_TASK_WKUP_STK_SIZE/10,APP_TASK_WKUP_STK_SIZE,\
				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
//	//����NRF����
//	OSTaskCreate(&AppTaskNRFTCB,"APP Task NRF",AppTaskNRF,(void *)0,1,\
//				 &AppTaskNRFStk[0], APP_TASK_NRF_STK_SIZE/10,APP_TASK_NRF_STK_SIZE,\
//				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	
	OSTaskDel(0,&err);
}



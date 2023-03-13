#include "app.h"

void AppTaskStart(void *p_arg){
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

    (void)p_arg;

    BSP_Init();//�弶��ʼ��
    CPU_Init();//��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();//��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;//�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);//���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��
	

	OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //ʹ��ʱ��Ƭ��ת����
		                 (OS_TICK       )0,                     //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
					     (OS_ERR       *)&err);                //���ش�������
	
	OSTaskCreate((OS_TCB     *)&AppTaskRelays1TCB,
				 (CPU_CHAR   *)"Relays1_Task",
                 (OS_TASK_PTR )Relays1_Task,
                 (void    	 *)0,
                 (OS_PRIO     )APP_TASK_RELAYS1_PRIO,
                 (CPU_STK    *)AppTaskRelays1Stk,
                 (CPU_STK_SIZE)APP_TASK_RELAYS1_STK_SIZE/10,
                 (CPU_STK_SIZE)APP_TASK_RELAYS1_STK_SIZE,
                 (OS_MSG_QTY  )5u,
                 (OS_TICK     )0u,
                 (void  	 *)0u,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

	OSTaskCreate((OS_TCB     *)&AppTaskRelays2TCB,
				 (CPU_CHAR   *)"Relays2_Task",
                 (OS_TASK_PTR )Relays2_Task,
                 (void    	 *)0,
                 (OS_PRIO     )APP_TASK_RELAYS2_PRIO,
                 (CPU_STK    *)AppTaskRelays2Stk,
                 (CPU_STK_SIZE)APP_TASK_RELAYS2_STK_SIZE/10,
                 (CPU_STK_SIZE)APP_TASK_RELAYS2_STK_SIZE,
                 (OS_MSG_QTY  )5u,
                 (OS_TICK     )0u,
                 (void  	 *)0u,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	OSTaskDel(0,&err);
}

void Relays1_Task(void *p_arg){
	OS_ERR err;
	while(1){
		Relays_State(RELAYS1,RELAYS_TOGGLE);
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);

	}
}

void Relays2_Task(void *p_arg){
	OS_ERR err;
	while(1){
		Relays_State(RELAYS2,RELAYS_TOGGLE);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);

	}
}


#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)

#define  BSP_REG_DEM_CR			(*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR			(*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT		(*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR		(*(CPU_REG32 *)0xE0042004)

#define  BSP_BIT_DEM_CR_TRCENA		(1<<24)

#define  BSP_BIT_DWT_CR_CYCCNTENA	(1<<0)

void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;

    fclk_freq = BSP_CPU_ClkFreq();

    /* ʹ��DWT���� */
	BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;
	/* DWT CYCCNT�Ĵ���������0 */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
	/* ʹ��Cortex-M3 DWT CYCCNT�Ĵ��� */
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;
	
    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}

CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;
    
    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif

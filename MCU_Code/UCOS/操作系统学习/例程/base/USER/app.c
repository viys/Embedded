#include "app.h"

void AppTaskStart(void *p_arg){
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

    (void)p_arg;

    BSP_Init();//板级初始化
    CPU_Init();//初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();//获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;//根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);//调用 SysTick 初始化函数，设置定时器计数值和启动定时器
	

	OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //使能时间片轮转调度
		                 (OS_TICK       )0,                     //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
					     (OS_ERR       *)&err);                //返回错误类型
	
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

    /* 使能DWT外设 */
	BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;
	/* DWT CYCCNT寄存器计数清0 */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
	/* 使能Cortex-M3 DWT CYCCNT寄存器 */
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

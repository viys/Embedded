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

    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器
	
    //配置时间片轮转调度
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //使能时间片轮转调度
		                 (OS_TICK       )0,                     //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
					     (OS_ERR       *)&err);
	//创建LCD任务
	OSTaskCreate(&AppTaskLCDTCB,"APP Task LCD",AppTaskLCD,(void *)0,1,\
				 &AppTaskLCDStk[0], APP_TASK_LCD_STK_SIZE/10,APP_TASK_LCD_STK_SIZE,\
				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	//创建WKUP任务
	OSTaskCreate(&AppTaskWKUPTCB,"APP Task WKUP",AppTaskWKUP,(void *)0,1,\
				 &AppTaskWKUPStk[0], APP_TASK_WKUP_STK_SIZE/10,APP_TASK_WKUP_STK_SIZE,\
				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
//	//创建NRF任务
//	OSTaskCreate(&AppTaskNRFTCB,"APP Task NRF",AppTaskNRF,(void *)0,1,\
//				 &AppTaskNRFStk[0], APP_TASK_NRF_STK_SIZE/10,APP_TASK_NRF_STK_SIZE,\
//				 5,0,0,OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,&err);
	
	OSTaskDel(0,&err);
}



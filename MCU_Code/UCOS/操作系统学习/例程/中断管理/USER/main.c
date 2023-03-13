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

    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器
	
    /* 配置时间片轮转调度 */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //使能时间片轮转调度
		                 (OS_TICK       )0,                     //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
					     (OS_ERR       *)&err);

	//创建WKUP任务		 
    OSTaskCreate((OS_TCB     *)&AppTaskWKUPTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task WKUP",                             //任务名称
                 (OS_TASK_PTR ) AppTaskWKUP,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_WKUP_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskWKUPStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
	//创建 AppTaskUsart 任务
    OSTaskCreate((OS_TCB     *)&AppTaskUsartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Usart",                            //任务名称
                 (OS_TASK_PTR ) AppTaskUsart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_USART_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskUsartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_USART_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);  
	
	OSTaskDel(0,&err);                     //删除起始任务本身，该任务不再运行
}





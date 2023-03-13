#include "app.h"

void AppTaskStart(void *p_arg){
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

    (void)p_arg;

    BSP_Init();                                                 //板级初始化
    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    /* 配置时间片轮转调度 */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,           //使能时间片轮转调度
		                 (OS_TICK       )0,                     //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
					     (OS_ERR       *)&err);


	//创建AppTaskPost任务
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                            	//任务控制块地址
                 (CPU_CHAR   *)"App Task Tmr",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTmr,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
    
	OSTaskDel (0, &err); //删除起始任务本身，该任务不再运行
}

void AppTaskTmr(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	OS_TMR my_tmr;
	
	//创建软件定时器
	OSTmrCreate((OS_TMR *)&my_tmr, //软件定时器
				(CPU_CHAR *)"MySoftTimer",
				(OS_TICK)10, //定时器初始值
				(OS_TICK)10, //定时器周期重载值
				(OS_OPT)OS_OPT_TMR_PERIODIC, //周期性定时
				(OS_TMR_CALLBACK_PTR)TmrCallback, //回调函数
				(void *)"Timer Over!", //传递实参给回调函数
				(OS_ERR *)&err);
	
	//启动软件定时器
	OSTmrStart((OS_TMR *)&my_tmr, //软件定时器对象
			   (OS_ERR *)&err);
				
	while(DEF_TRUE){
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
	
}

void TmrCallback(OS_TMR *p_tmr,void *p_arg){
	printf("软件定时器运行中\r\n");
}


















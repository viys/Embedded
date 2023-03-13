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

	//创建事件标志组
	OSFlagCreate((OS_FLAG_GRP *)&flag_grp,
				 (CPU_CHAR *)"FLAG For Test",
				 (OS_FLAGS)0,
				 (OS_ERR *)&err);
	//创建AppTaskPost任务
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                            	//任务控制块地址
                 (CPU_CHAR   *)"App Task Post",                             //任务名称
                 (OS_TASK_PTR ) AppTaskPost,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskPostStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
	//创建AppTaskPend任务
    OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                            	//任务控制块地址
                 (CPU_CHAR   *)"App Task Pend",                             //任务名称
                 (OS_TASK_PTR ) AppTaskPend,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_PEND_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskPendStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
    
	OSTaskDel (0, &err); //删除起始任务本身，该任务不再运行
}

void AppTaskPost(void *p_arg){
	OS_ERR err;
	(void)p_arg;

	while(DEF_TRUE){
		switch(KEY_Scan(WKUP)){
			case KEY_HIGH:
				printf("WKUP 被按下\r\n");
				OSFlagPost(&flag_grp,EVENT1,OS_OPT_POST_FLAG_SET,&err); //将标志位的BIT0置1
				OSFlagPost(&flag_grp,EVENT2,OS_OPT_POST_FLAG_SET,&err); //将标志位的BIT0置1
				break;
			case KEY_LOW:
				//printf("WKUP 未被按下\r\n");
				break;
			default:
				break;
		}
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_DLY,&err);
	}
}

void AppTaskPend(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	
	OS_FLAGS flags_rdy;
	
	while(DEF_TRUE){
		//等待标志组的BIT0和BIT1均被置1
		flags_rdy = OSFlagPend(&flag_grp,
							   (EVENT1|EVENT2),
							   0,
							   (OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_BLOCKING|OS_OPT_PEND_FLAG_CONSUME),
							   0,
							   &err);
		if((flags_rdy&(EVENT1|EVENT2)) == (EVENT1|EVENT2)){
			printf ("同步成功\r\n");
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
	}
}





















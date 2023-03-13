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
					     (OS_ERR       *)&err);                //返回错误类型

	/* 创建信号量 SemOfKey */
	OSSemCreate((OS_SEM *)&SemOfKey, //指向信号量变量的指针
	(CPU_CHAR *)"SemOfKey", //信号量的名字
	(OS_SEM_CTR )0,//信号量这里是指示事件发生,所以赋值为0,表示事件还没有发生
	(OS_ERR *)&err);
	
	//创建LED1任务
    OSTaskCreate((OS_TCB     *)&AppTaskLED1TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led1",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
	//创建LED2任务
    OSTaskCreate((OS_TCB     *)&AppTaskLED2TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Led2",                             //任务名称
                 (OS_TASK_PTR ) AppTaskLed2,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_LED2_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskLed2Stk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_LED2_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);

	//创建USART任务
    OSTaskCreate((OS_TCB     *)&AppTaskUSART1TCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task USART1",                             //任务名称
                 (OS_TASK_PTR ) AppTaskUsart1,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_USART1_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskUsartStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_USART1_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_USART1_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
				 
	//创建WKUP任务
    OSTaskCreate((OS_TCB     *)&AppTaskWKUPTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task WKUP",                             //任务名称
                 (OS_TASK_PTR ) AppTaskWKUP,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_WKUP_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskWKUPStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_WKUP_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);
    
	OSTaskDel (0, &err); //删除起始任务本身，该任务不再运行
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
				  0, //无期限等待
				  OS_OPT_PEND_BLOCKING, //如果没有信号量可用就等待
				  (CPU_TS *)0, //不获取时间戳
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
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (1) {                                 //任务体，通常写成一个死循环                            
		printf("AppTaskUsart Running\n");
		value = OSTaskRegGet ( 0, 0, & err );            //获取自身任务寄存器值
		
		if ( value < 5 )                                 //如果任务寄存器值<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //继续累加任务寄存器值
		}
		else                                             //如果累加到5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //将任务寄存器值归零
			printf("挂起Usart任务（自身）！\n");
			OSTaskSuspend ( 0, & err );                    //挂起自身
			
		}

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		}
}
		




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
	
	//创建消息队列
    OSQCreate ((OS_Q         *)&queue,            //指向消息队列的指针
               (CPU_CHAR     *)"Queue For Test",  //队列的名字
               (OS_MSG_QTY    )20,                //最多可存放消息的数目
               (OS_ERR       *)&err);
	//创建USART1任务
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
	//创建Post任务		 
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //任务控制块地址
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
	//创建Pend任务		 
	OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                             //任务控制块地址
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
				 
	OSTaskDel (0, &err);                     //删除起始任务本身，该任务不再运行
}

void AppTaskPost(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	while(DEF_TRUE){
		OSQPost((OS_Q 	   *)&queue, //消息变量 指针
				(void	   *)"UCOSIII", //要发送的数据的指针
				(OS_MSG_SIZE)sizeof("UCOSIII"),//数据字节大小
				(OS_OPT		)OS_OPT_POST_FIFO|OS_OPT_POST_ALL, //先进先出和发布给全部任务的形式
				(OS_ERR    *)&err);
		//OSTimeDly(500, OS_OPT_TIME_DLY, &err);
				OSTimeDlyHMSM ( 0, 0, 0, 500, OS_OPT_TIME_DLY, & err );
	}
}


void AppTaskPend(void * p_arg){
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
					//定义一个局部变量,用于保存关中断前的CPU状态寄存器
					//SR(临界段关中断只需保存SR),开中断时将该值还原
	char * pMsg;
	(void)p_arg;
	
	while(DEF_TRUE){
		pMsg = OSQPend((OS_Q 		*)&queue, //消息变量指针
					   (OS_TICK		 )0, //等待时长为无限
					   (OS_OPT		 )OS_OPT_PEND_BLOCKING, //如果没有获取到信号量就等待
					   (OS_MSG_SIZE *)&msg_size, //获取消息的字节大小
					   (CPU_TS 		*)0, //获取任务发送的时间戳
					   (OS_ERR 		*)&err);
		if(err==OS_ERR_NONE){
			OS_CRITICAL_ENTER();
			printf("\r\n接收消息长度:%d字节,内容:%s\r\n",msg_size,pMsg);
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
			printf("恢复LED2任务\r\n");
			OSTaskResume(&AppTaskUSART1TCB, &err);
			printf("恢复USART1任务\r\n");
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
		
		value = OSTaskRegGet ( 0, 0, & err );            //获取自身任务寄存器值
		
		if ( value < 5 )                                 //如果任务寄存器值<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //继续累加任务寄存器值
		}
		else                                             //如果累加到5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //将任务寄存器值归0
			printf("挂起LED2任务（自身）！\r\n");
			OSTaskSuspend (&AppTaskLED2TCB, & err );       //挂起自身 OSTaskSuspend()第一个参数填0时挂起自身
			
		}
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //相对性延时1000个时钟节拍（1s）
		
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (1) {                                 //任务体，通常写成一个死循环                            
		printf("AppTaskUsart Running\n");
		value = OSTaskRegGet (0, 0, & err);            //获取自身任务寄存器值
		
		if (value < 5)                                 //如果任务寄存器值<5
		{
			OSTaskRegSet (0, 0, ++ value, & err);        //继续累加任务寄存器值
		}
		else                                             //如果累加到5
		{
			OSTaskRegSet (0, 0, 0, & err);               //将任务寄存器值归零
			printf("挂起Usart任务（自身）！\n");
			OSTaskSuspend (0, & err);                    //挂起自身
			
		}

		OSTimeDly (1000, OS_OPT_TIME_DLY, & err);      //相对性延时1000个时钟节拍（1s）
		}
}
		




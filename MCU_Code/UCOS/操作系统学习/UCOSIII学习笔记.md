# μC/OSIII学习笔记

## μC/OSIII的移植

### Cortex-M3

#### 工程的创建

ps:作为一名拥有良好习惯的开发者，我们的工程模板文件不应直接去照搬别人的,或是无脑按照别人的文件结构来创建.我们需要了解自己文件的特点及分类情况,也要清楚μC/OS源码每个文件的作用,这样我们自己创建的工程文件的层次可以更加分明,结构更加合理.

我的文件结构:

```c
Project
    USER
      |--main.c			存放mian函数
      |--app.c			存放任务函数
      |--includes.h		存放各种头文件
      |--app.h			存放任务控制块等重要参数
    BSP
      |--bsp.c			存放BSP_Init和BSP_CPU_ClkFreq等函数
      |--bsp_xxx.c		底层外设代码
      |--...
    CMSIS				STM32CORE代码
    Device				启动文件和固件库代码
    RTOS				μC/OS源码以及xx_cfg.h文件
```

ps:我是用的是keil软件pack包中的环境

1. 文件说明

   - os.h

     包含μC/OS的主要头文件,定义了一些与系统相关的宏定义,常量,全局变量,函数原型等.

   - os_cfg.h

     系统的配置文件,主要是让用户自己配置一些系统默认的功能,用户可以选择某些或者全部的功能,比如消息队列,信号量,互斥量,事件标志位等,系统默认全部使用的,如果用户不需要的话,则可以直接关闭,在对应的宏定义中设置为0即可,这样子就不会占用系统的SRAM,以节省系统资源.

   - cpu_cfg.h

     主要是配置CPU相关的一些宏定义,我们可以选择对不同的CPU进行配置,当然如果我们没有对CPU很熟悉的话,就直接忽略这个文件即可.

   - os_cfg_app.c

     根据os_cfg_app.h中的配置来定义变量和数组.

   - os_core.c

     内核数据结构管理,μC/OS的核心,任务切换等等.

   - os_dbg.c

     μC/OS内核调试相关代码.

   - os_flag.c

     时间块管理,事件标志组管理等相关功能代码.

   - os_int.c

     涵盖内核的初始化相关代码.

   - os_mem.c

     系统内存管理相关代码.

   - os_msg.c

     消息处理相关代码.

   - os_mutex.c

     互斥量相关代码.

   - os_pend_multi.c

     在多个消息队列,信号量等待的相关代码.

   - os_prio.c

     这是一个内部调用的文件,关于系统就绪相关的代码.

   - os_q.c

     消息队列相关代码.

   - os_sem.c

     信号量相关代码.

   - os_stat.c

     任务状态统计相关代码.

   - os_task.c

     任务管理相关代码.

   - os_tick.c

     处理任务相关延时,阻塞状态任务的相关代码.

   - os_time.c

     时间管理相关代码,阻塞延时等.

   - os_tmr.c

     软件定时器相关代码.

   - os_var.c

     μC/OS定义的全局变量.

   - os_type.h

     μC/OS数据类型声明相关代码.

   - app_cfg.h

     存放宏定义,任务栈的宏定义要存放在里面.

   -  os_cpu.h

     定义数据类型,处理器相关代码,声明函数原型.

   - oc_cpu_a.asm

     与处理器相关的汇编代码,主要是与任务切换相关.

   - os_cpu_c.c

     定义用户钩子函数,提供扩充软件功能的的接口.

   - cpu.h存放有CPU_XX数据类型

   - cpu_c.h

     包含了一些数据类型的定义,让μC/OSIII与CPU架构和编译器的字宽无关.同时还指定了CPU使用的是大端模式还是小端模式,还包括一些与 CPU 架构相关的函数的声明.

   - cpu_c.c/cpu_a.asm

     这两个文件主要是 CPU 底层相关的一些 CPU 函数,cpu_c.c 文件中放的是 C 函数,包含了一些

     CPU架构相关的代码,μC/OS为了移植方便而采用C语言编写;而cpu_a.asm存放的是汇编代码,有一些代码只能用汇编实现,包含一些用来开关中断,前导零指令等.

   - cpu_core.c

     包含了适用于所有的CPU架构的C代码(通用代码),是一个很重要的文件.主要包含的函数是CPU名字的命名,时间戳的计算等等,跟CPU底层的移植没有太大的关系,主要保留的是CPU前导零的C语言计算函数以及一些其他的函数,因为前导零指令是依靠硬件实现的,这里采用C语言方式实现,以防止某些CPU不支持前导零指令.

   - cpu_core.h

     主要是对cpu_core.c文件里面一些函数的说明,以及一些时间戳相关等待定义.

   - cpu_def.h

     包含CPU相关的一些宏定义,常量,利用`#defifine`进行定义的相关信息.

   - 

2. 创建完工程后需要对启动文件进行修改

   修改STM32的启动文件`"startup_stm32f10x.s"`,将文件中的`PendSV_Handler`修改为`OS_CPU_PendSVHandler`,

   `SysTick_Handler`修改为`OS_CPU_SysTickHandler`

   还需要注释掉移植过来的`PendSV_Handler`和`SysTick_Handler`函数.

3. 添加文件结构中所需文件即可

## 任务的创建和切换

### 创建任务栈

- 在裸机中由启动文件在RAM中开启一整个连续空间.
- 在操作系统中多任务系统中的任务栈是在统一的一个栈空间里分配一个个独立的房间,每个任务只能使用在各自的房间.
- 在μC/OS中空闲任务的栈最小应该大于等于128.
- μC/OS中栈的数据类型为`CPU_STK`(cpu.h).
- 

#### 定义任务函数

- 任务是一个独立的函数,函数主体无限循环且不能返回.

#### 定义任务控制块TCB

- 系统为了顺利的调度任务,为每个任务都额外定义了一个任务控制块 TCB(Task ControlBlock),这个任务控制块就相当于任务的身份证,里面存有任务的所有信息,比如任务的栈,任务名称,任务的形参等.有了这个任务控制块之后,以后系统对任务的全部操作都可以通过这个 TCB 来实现.
- TCB的数据类型为`OS_TCB`(os.h).

#### 创建任务函数

- 任务创建函数 `OSTaskCreate`将任务的栈,任务的函数实体,任务的 TCB 联系起来,交由系统进行统一调度.

- `OSTaskCreate`(os_task.c).

- ```c
  void  OSTaskCreate (OS_TCB        *p_tcb,
                      CPU_CHAR      *p_name,
                      OS_TASK_PTR    p_task,
                      void          *p_arg,
                      OS_PRIO        prio,
                      CPU_STK       *p_stk_base,
                      CPU_STK_SIZE   stk_limit,
                      CPU_STK_SIZE   stk_size,
                      OS_MSG_QTY     q_size,
                      OS_TICK        time_quanta,
                      void          *p_ext,
                      OS_OPT         opt,
                      OS_ERR        *p_err)
  ```

  1. p_tcb是任务控制块指针.
  2. p_task是任务函数名.
  3. p_arg是任务形参,用于传递任务函数.
  4. p_stk_base用于指向任务栈的起始地址.
  5. stk_size表示任务栈的大小.
  6. p_err用于储存错误码.

- `OSTaskStkInit() `任务栈初始化函数(os_cpu_c.c).

  ```c
  CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                           void          *p_arg,
                           CPU_STK       *p_stk_base,
                           CPU_STK       *p_stk_limit,
                           CPU_STK_SIZE   stk_size,
                           OS_OPT         opt)
  ```

  1. p_task是任务名.
  2. p_arg是任务参数.
  3. p_stk_base表示任务栈的起始地址 .
  4. stk_size表示任务栈的大小.

- 任务创建完成后,需要把任务添加到就序列表,以供系统随时调度.



#### **具体过程**

1. 使用keil自动生成必须文件

   ![环境搭建](.\Image\环境搭建.png)

2. 创建自己的工程文件结构

   ![文件结构](.\Image\文件结构.png)

3. 栈

   在app_cfg.h文件中宏定义自己的任务栈空间大小

   ```c
   #ifndef __APP_CFG_H
   #define __APP_CFG_H
   
   //TASK STACK SIZES
   #define APP_TASK_START_STK_SIZE	128
   
   
   #endif
   
   ```

   在app.h文件中定义任务栈数组和任务控制块

4. 创建任务函数

   在bsp.c文件的`BSP_Init`函数中添加驱动代码

   ```c
   #include "bsp.h"
   
   /*!
       \brief      BSP初始化程序
       \Note	    none			
       \param[in]  none
       \retval     none
   */
   void BSP_Init(void){
   	Relays_Config(RELAYS1);//驱动代码
       
   }
   
   /*!
       \brief      时钟读取函数
       \Note	    none			
       \param[in]  none
       \retval     时钟频率
   */
   CPU_INT32U BSP_CPU_ClkFreq(void){
   	RCC_ClocksTypeDef Clocks_InitStructure;
   	
   	RCC_GetClocksFreq(&Clocks_InitStructure);
   	
   	return ((CPU_INT32U)Clocks_InitStructure.HCLK_Frequency);
   }
   
   ```

   ps:bsp.c文件中需要存放`BSP_CPU_ClkFreq`函数.

5. 定义任务栈数组和任务控制块

   app.h

   ```c
   #ifndef __APP_H
   #define __APP_H
   
   #include "includes.h"
   
   static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
   static OS_TCB 	AppTaskStartTCB;
   
   void AppTaskStart(void *p_arg);
   
   #endif
   
   ```

   

6. 创建任务函数

   app.c

   ```c
   #include "app.h"
   
   void AppTaskStart(void *p_arg){
   	CPU_INT32U  cpu_clk_freq;
       CPU_INT32U  cnts;
       OS_ERR      err;
   	
       (void)p_arg;
   
       BSP_Init();//板级初始化
       CPU_Init();//初始化 CPU 组件(时间戳,关中断时间测量和主机名)
   
       cpu_clk_freq = BSP_CPU_ClkFreq();//获取 CPU 内核时钟频率(SysTick 工作时钟)
       cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;//根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
       OS_CPU_SysTickInit(cnts);//调用 SysTick 初始化函数,设置定时器计数值和启动定时器
   	while(1){
   		Relays_State_Config(RELAYS1,RELAYS_TOGGLE);
   		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
   	}
   }
   
   ```

   ps:任务函数已在第五步中的app.h文件中声明.还有cpu的初始化需要在任务函数中体现.

7. 任务创建

   main.c

   ```c
   #include "app.h"
   
   int main(){
   	OS_ERR err;
   	
   	BSP_Init();
   	
   	OSInit(&err);
   	
   	OSTaskCreate(&AppTaskStartTCB,
   				"Relays_Toggle",
   				AppTaskStart,
   				0,//参数
   				1,//优先级
   				AppTaskStartStk,//任务栈起始地址
   				APP_TASK_START_STK_SIZE/10,//剩余栈
   				APP_TASK_START_STK_SIZE,//任务栈大小
   				5,//消息队列大小
   				0,
   				0,
   				OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
   				&err);
   	OSStart(&err);
   }
   
   ```

8. 编译下载

   初次编译产生报错一般是和头文件有关,根据报错将头文件添加进includes.h文件即可.

至此任务的创建已完成.

推荐任务代码模式:在main()函数中将硬件和RTOS亲统先初始化好,然后创建一个启动任务后就启动调度器,然后在启动任务里面创建各种应用任务,当所有任务都创建成功后,启动任务把自己删除.

### 任务切换

#### 任务的概念

- 从系统角度看,任务是竞争系统资源的最小运行单元.
- μC/OS是个支持多任务的操作系统.
- 任务可以运行和阻塞,可以使用内存空间等系统资源,各任务互相独立,不同任务可以共享一个优先级,处于就态的多个相同优先级任务将会以时间片切换的方式共享处理器.

#### 任务调度

- μC/OS任务调度器是基于优先级的全抢占式调度.
- 理论上可以支持无数个优先级.
- `OSSched()`

#### 任务状态迁移

![任务状态](.\Image\任务状态.png)

- 就绪(OS_TASK_STATE_RDY)
- 延时(OS_TASK_STATE_DLY)
- 等待(OS_TASK_STATE_PEND)
- 运行(OS_TASK_STATE_PEND_TIMEOUT)
- 挂起(OS_TASK_STATE_SUSPENDED)
- 延时+挂起(OS_TASK_STATE_DLY_SUSPENDED)
- 等待+挂起(OS_TASK_STATE_PEND_SUSPENDED)
- 等待超时+挂起(OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED)
- 删除(OS_TASK_STATE_DEL)

#### 任务常用函数

- 任务挂起函数`OS_TaskSuspend()`

  挂起指定任务.

  ps:不可以挂起空闲任务.

  ```c
  void  OS_TaskSuspend (OS_TCB  *p_tcb,
                        OS_ERR  *p_err)
  ```

- 任务恢复函数`OS_TaskResume()`

  让任务重新进入就绪态.

  ```c
  void  OSTaskResume (OS_TCB  *p_tcb,
                       OS_ERR  *p_err)
  ```

  疑问:OSTaskResume和OS_TaskResume有什么区别.

- 任务删除函数`OSTaskDel()`

  删除一个任务,如果是删除自身形参为NULL.

  ```c
  void  OSTaskDel (OS_TCB  *p_tcb,
                   OS_ERR  *p_err)
  ```

- 任务延迟函数

  主动放弃CPU的使用权

  `OSTimeDly()`

  ```c
  void  OSTimeDly (OS_TICK   dly,
                   OS_OPT    opt,
                   OS_ERR   *p_err)
  ```

  `OSTimeDlyHMSM()`

  ```c
  void  OSTimeDlyHMSM (CPU_INT16U   hours,
                       CPU_INT16U   minutes,
                       CPU_INT16U   seconds,
                       CPU_INT32U   milli,
                       OS_OPT       opt,
                       OS_ERR      *p_err)
  ```

- 调度器上锁解锁函数

  可以防止在外设初始化的时候程序发生调度,初始化被打断.

  `OSSchedLock()`

  ```c
  void  OSSchedLock (OS_ERR  *p_err)
  ```

  `OSSchedUnlock()`

  ```c
  void  OSSchedUnlock (OS_ERR  *p_err)
  ```

#### 任务调度点

1. 使用延时函数`OSTimeDly()`或者`OSTimeDlyHMSM()`.
2. 创建任务.
3. 删除任务.
4. 任务通过调用`OSTaskSuspend()`将自身挂起.
5. 任务解挂某个挂起的任务.
6. 用户调用`OSSched()`.
7. 释放信号量或者发送消息,也可通过配置相应的参数不发生任务调度.
8. 任务等待的事情还没发生(等待信号量,消息队列等).
9. 任务取消等待.
10. 删除一个内核对象.
11. 任务改变自身的优先级或者其他任务的优先级.
12. 退出所有的嵌套中断.
13. 通过`OSSchedUnlock()`给调度器解锁.
14. 任务调用`OSSchedRoundRobinYield()`放弃其执行时间片.

### 任务时间片运行

ps:开启时间片轮转之后,相同优先级的任务也可以互相调换.

1. 修改os_cfg.h文件中宏定义的值.

   ```c
   #define OS_CFG_SCHED_ROUND_ROBIN_EN 1u
   ```

2. 在app.c文件中的启动任务中加入`OSSchedRoundRobinCfg()`函数.

   ```c
   #include "app.h"
   
   void AppTaskStart(void *p_arg){
   	CPU_INT32U  cpu_clk_freq;
       CPU_INT32U  cnts;
       OS_ERR      err;
   
       (void)p_arg;
   
       BSP_Init();//板级初始化
       CPU_Init();//初始化 CPU 组件(时间戳,关中断时间测量和主机名)
   
       cpu_clk_freq = BSP_CPU_ClkFreq();//获取 CPU 内核时钟频率(SysTick 工作时钟)
       cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;//根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
       OS_CPU_SysTickInit(cnts);//调用 SysTick 初始化函数,设置定时器计数值和启动定时器
   	
   	//时间片调度函数
   	OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,//使能时间片轮转调度
   		                 (OS_TICK       )0,//把 OSCfg_TickRate_Hz / 10 设为默认时间片值
   					     (OS_ERR       *)&err);
   	
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
   
   ```

## 阻塞延时与空闲任务

- RTOS中的演示叫阻塞延时,即任务需要演示的时候,任务会放弃CPU的使用权,CPU可以执行其他的任务,可以更加充分的利用CPU资源.

- 如果在延时的时候CPU实在没有任务可以运行,RTOS会为CPU创建一个空闲任务去执行.

- 空闲任务是系统在初始化的时候创建的优先级最低的任务.

- 空闲任务是对全局变量 `OSIdleTaskCtr` 进行加操作.
- 在实际应用中,当系统进入空闲任务的时候,可在空闲任务中让单片机进入休眠或者低功耗等.

### 空闲任务

1. 空闲任务栈

   - 空闲任务的栈是已经定义好的,数组的大小由`OS_CFG_IDLE_TASK_STK_SIZE`这个宏控制.
   - 空闲任务的栈起始地址被定义成一个常量.
   - os.h文件中存在具有全局属性的变量`OSCfg_IdleTaskStkBasePtr`和`OSCfg_IdleTaskStkSize`,可以在其他文件里被使用

   os_cfg_app.c中有空闲任务栈的定义

   ```c
   CPU_STK OSCfg_IdleTaskStk[OS_CFG_IDLE_TASK_STK_SIZE];
   /* 空闲任务栈起始地址 */
   CPU_STK * const OSCfg_IdleTaskStkBasePtr = (CPU_STK *)&OSCfg_IdleTaskStk[0];
   
   /* 空闲任务栈大小 */
   CPU_STK_SIZE const OSCfg_IdleTaskStkSize = (CPU_STK_SIZE)OS_CFG_IDLE_TASK_STK_SIZE;
   ```

   

2. 空闲任务TCB

   空闲任务块的TCB在os.h中定义,是一个全局变量.

   ```c
   OS_EXT OS_TCB OSIdleTaskTCB;
   ```

   

3. 空闲任务函数

   空闲任务定义在os_core.c文件中

   ```c
   #if (OS_CFG_TASK_IDLE_EN == DEF_ENABLED)
   void  OS_IdleTask (void  *p_arg)
   {
   #if ((OS_CFG_DBG_EN == DEF_ENABLED) || (OS_CFG_STAT_TASK_EN == DEF_ENABLED))
       CPU_SR_ALLOC();
   #endif
   
   	(void)p_arg;
   /* Prevent compiler warning for not using 'p_arg' */
       while (DEF_ON) {
   #if ((OS_CFG_DBG_EN == DEF_ENABLED) || (OS_CFG_STAT_TASK_EN == DEF_ENABLED))
           CPU_CRITICAL_ENTER();
   #if (OS_CFG_DBG_EN == DEF_ENABLED)
           OSIdleTaskCtr++;
   #endif
   #if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
           OSStatTaskCtr++;
   #endif
           CPU_CRITICAL_EXIT();
   #endif
   
   #if (OS_CFG_APP_HOOKS_EN == DEF_ENABLED)
           OSIdleTaskHook();
   /* Call user definable HOOK */
   #endif
       }
   }
   #endif
   ```

4. 空闲任务初始化

   空闲任务的初始化再`OSInit()`函数中完成

   ```c
   void OSInit(OS_ERR *p_err){
       ...
       OS_IdleTaskInit(p_err);
       ...
   }
   ...
   #if (OS_CFG_TASK_IDLE_EN == DEF_ENABLED)
   void  OS_IdleTaskInit (OS_ERR  *p_err)
   {
   #if (OS_CFG_DBG_EN == DEF_ENABLED)
       OSIdleTaskCtr = 0u;
   #endif
   /* --------------- CREATE THE IDLE TASK --------------- */
       OSTaskCreate(&OSIdleTaskTCB,
                    (CPU_CHAR   *)((void *)"uC/OS-III Idle Task"),
                     OS_IdleTask,
                     DEF_NULL,
                    (OS_CFG_PRIO_MAX - 1u),
                     OSCfg_IdleTaskStkBasePtr,
                     OSCfg_IdleTaskStkLimit,
                     OSCfg_IdleTaskStkSize,
                     0u,
                     0u,
                     DEF_NULL,
                    (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
                     p_err);
   }
   #endif
   ```

### 阻塞延时

- 阻塞延时的阻塞是指任务调用该延时函数后,任务会被剥离CPU的使用权,然后进入阻塞状态,直至延时结束.

- 阻塞延时函数在os_time.c中定义

  ```c
  void OSTimeDly(OS_TICK dly){
      ...
      //设置延时时间
      OSTCBCurPtr->TaskDelayTicks = dly;
      
      OSSched();
  }
  
  struct os_tcb {
  	CPU_STK *StkPtr;
  	CPU_STK_SIZE StkSize;
  	/* 任务延时周期个数 */
  	OS_TICK TaskDelayTicks;
  }OSTCBCurPtr;
  ```

  `TaskDelayTick`是任务控制块的一个成员,用于记录任务需要延时的时间(单位为SysTick的中断周期).

## 时间戳

时间戳使用的是ARM Cortex-M系列内核中的DWT外设.

### 时间戳简介

在μC/OS中,很多地方的代码都加入了时间测量的功能,比如任务关中断的时间,关调度器的时间等.知道了某段代码的运行时间,就明显地知道该代码的执行效率如果时间过长就可以优化或者调整代码策略.

### 时间戳的实现

在ARM Cortex-M系列内核中.有一个 DWT 的外设,该外设有一个32位的寄存器叫CYCCNT,它是一个向上的计数器,记录的是内核时钟HCLK运行的个数,当CYCCNT溢出之后,会清零重新开始向上计数.该计数器在μC/OS中正好被用来实现时间戳的功能.

### 时间戳代码

- `CPU_Init`函数

  ```c
  void CPU_Init(void){
  /* CPU 初始化函数中总共做了三件事
  1,初始化时间戳
  2,初始化中断禁用时间测量
  3,初始化 CPU 名字
  此处为时间戳功能*/
  #if ((CPU_CFG_TS_EN == DEF_ENABLED)||(CPU_CFG_TS_TMR_EN == DEF_ENABLED))
  	CPU_TS_Init();(2)
  #endif
      
  }
  
  ```

  `CPU_CFG_TS_EN`和`CPU_CFG_TS_TMR_EN`这两个宏在cpu_core.h中定义,用于控制时间戳相关功能代码.

  `CPU_CFG_TS_32_EN`和`CPU_CFG_TS_64_EN`这两个宏在cpu_cfg.h中,用于控制时间戳是32还是64为的,默认使用的是32位.

- `CPU_TS_Init()`函数

  ```c
  #if ((CPU_CFG_TS_EN == DEF_ENABLED)||(CPU_CFG_TS_TMR_EN == DEF_ENABLED))
  static void CPU_TS_Init(void){
  #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  	CPU_TS_TmrFreq_Hz = 0u;
  	CPU_TS_TmrInit();
  #endif
  }
  #endif
  
  ```

  `CPU_TS_TmrFreq_Hz`是一个全局变量,表示CPU的具体时钟,具体大小跟硬件相关.

- `CPU_TS_TmrInit() `函数

  ```c
  //时间戳定时器初始化
  #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  void CPU_TS_TmrInit(void){
      CPU_INT32U fclk_freq;
  	fclk_freq = BSP_CPU_ClkFreq();
      /* 启用 DWT 外设 */
  	BSP_REG_DEM_CR |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;
  	/* DWT CYCCNT 寄存器计数清零 */
  	BSP_REG_DWT_CYCCNT = (CPU_INT32U)0u;
  	/* 启用 Cortex-M3 DWT CYCCNT 寄存器 */
  	BSP_REG_DWT_CR |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;
  	CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
  }
  #endif
  
  ```

- `BSP_CPU_ClkFreq()`函数

  个用于获取CPU的HCLK时钟的BSP函数,具体跟硬件相关.

- `CPU_TS_TmrFreqSet()`函数

  把函数`BSP_CPU_ClkFreq()`获取到的CPU的HCLK时钟赋值给全局变量`CPU_TS_TmrFreq_Hz`.

  ```c
  #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  void CPU_TS_TmrFreqSet (CPU_TS_TMR_FREQ freq_hz)
  {
      CPU_TS_TmrFreq_Hz = freq_hz;
  }
  #endif
  
  ```

- `CPU_TS_TmrRd()`函数

  用于获取CYCNNT计数器的值.

  ```c
  #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  CPU_TS_TMR CPU_TS_TmrRd(void){
      CPU_TS_TMR ts_tmr_cnts;
  	ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;
  	return (ts_tmr_cnts);
  }
  #endif
  
  ```

- `OS_TS_GET()`函数

  用于获取CYCNNT计数器的值,实际上是一个宏定义,将CPU底层的函`CPU_TS_TmrRd()`重新取个名字封装,供内核和用户函数使用.

### 时间戳的使用

1. 在os_cfg.h中使能时间戳

   ````c
   #define OS_CFG_TS_EN                    1u
   ````

   在cpu_cfg.h中使能宏定义`CPU_CFG_TS_32_EN`或`CPU_CFG_TS_64_EN`

   ```c
   #define  CPU_CFG_TS_32_EN                       DEF_ENABLED
   #define  CPU_CFG_TS_64_EN                       DEF_DISABLED
   ```

   

2. 定义三个全局变量

   ```c
   uint32_t TimeStart;
   uint32_t TimeEnd;
   uint32_t TimeUse
   ```

3. 在`main()`函数中加入`CPU_Init()`函数

   ```c
   int main(){
   	OS_ERR err;
   	
       CPU_Init();
       
   	BSP_Init();
   	
   	OSInit(&err);
   	...
   
   	OSStart(&err);
   }
   ```

4. 在任务函数中对函数的执行时间进行测量

   ```c
   void Relays1_Task(void *p_arg){
   	OS_ERR err;
   	while(1){
           TimeStart = OS_TS_GET();
           
   		Relays_State(RELAYS1,RELAYS_TOGGLE);
   		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
           
   		TimeEnd = OS_TS_GET;
           TimeUse = TimeEnd - TimeStart;
   	}
   }
   ```

5. 若所用μC/OS代码代码中缺少`CPU_TS_TmrInit`和`CPU_TS_TmrRd`函数,添加即可

   ```c
   #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
   
   #define  BSP_REG_DEM_CR			(*(CPU_REG32 *)0xE000EDFC)
   #define  BSP_REG_DWT_CR			(*(CPU_REG32 *)0xE0001000)
   #define  BSP_REG_DWT_CYCCNT		(*(CPU_REG32 *)0xE0001004)
   #define  BSP_REG_DBGMCU_CR		(*(CPU_REG32 *)0xE0042004)
   
   #define  BSP_BIT_DEM_CR_TRCENA		(1<<24)
   
   #define  BSP_BIT_DWT_CR_CYCCNTENA	(1<<0)
   
   void  CPU_TS_TmrInit(void)
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
   
   CPU_TS_TMR  CPU_TS_TmrRd(void)
   {
       CPU_TS_TMR  ts_tmr_cnts;
       
       ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;
   
       return (ts_tmr_cnts);
   }
   #endif
   
   ```


## 临界段

### 临界段简介

- 临界段用一句话概括就是一段在执行的时候不能被中断的代码段.
- 系统调度和外部中断会打断临界段.

- μC/OS对临界段的保护最终还是对中断的开和关的控制.
- μC/OS中定义了一个进入临界段的宏`OS_CRITICAL_ENTER()`和两个出临界段的宏`OS_CRITICAL_EXIT()`,`OS_CRITICAL_EXIT_NO_SCHED()`.此外还有一个开中断但是锁定调度器的宏定义`OS_CRITICAL_ENTER_CPU_EXIT()`.

### Cortex-M内核快速关中断指令

- 为了快速的开关中断,Cortex-M内核专门设置了一条GPS指令,有四种用法.

  ```c
  CPSID I;PRIMASK=1;//关中断
  CPSIE I;PRIMASK=0;//开中断
  CPSID F;FAULTMASK=1;//关异常
  CPSIE F;FAULTMASK=0;//开异常
  ```

- `PRIMASK`和`FAULTMASK`是Cortex-M内核里面三个中断屏蔽寄存器中的两个,还有一个是`BASEPRI`

  | 名称      | 功能描述                                                     |
  | :-------- | ------------------------------------------------------------ |
  | PRIMASK   | 这是个只有单一比特的寄存器.在它被置1后,就关掉所有可屏蔽的异常,只剩下NMI和硬FAULT可以响应.它的默认值是0,表示没有关中断. |
  | FAULTMASK | 这是个只有1个位的寄存器.当它置1时,只有NMI才能响应,所有其他的异常,甚至是硬FAULT,也通通闭嘴.它的默认值也是0,表示没有关异常. |
  | BASEPRI   | 这个寄存器最多有9位(由表达优先级的位数决定).它定义了被屏蔽优先级的阈值.当它被设成某个值后,所有优先级号大于等于此值的中断都被关(优先级号越大,优先级越低).但若被设成0,则不关闭任何中断,0 也是默认值. |

- 在μC/OS中通常用CPSID I指令来立即关闭中断.

### 关中断

- μC/OS的关中断函数`CPU_SR_Save()`(cpu_a.asm).

  ```assembly
  CPU_SR_Save
  MRSR0, PRIMASK
  CPSID I 
  BX LR
  
  ```

### 开中断

- μC/OS的开中断函数`CPU_SR_Restore()`(cpu_a.asm).

  ```assembly
  CPU_SR_Restore
  MSR PRIMASK, R0
  BX LR
  
  ```

### 临界段代码的应用

- 我们发现开启关闭中断并没有简单的调用CPS指令,而是添加了一些代码,在进入临界段的时候保存PRIMASK的值,防止开关中断嵌套时发生混乱.

- 错误应用

  ```c
  PRIMASK = 0;/* PRIMASK 初始值为 0,表示没有关中断 */
  /* 临界段代码 */
  {
  	/* 临界段 1 开始 */
  	CPU_SR_Save();/* 关中断,PRIMASK = 1 */
  	{
  		/* 临界段 2 */
  		CPU_SR_Save();/* 关中断,PRIMASK = 1 */
  		{
  		}
  		CPU_SR_Restore();/* 开中断,PRIMASK = 0 */(注意)
  	}
  	/* 临界段 1 结束 */
  	CPU_SR_Restore();/* 开中断,PRIMASK = 0 */
  }
  
  ```

  容易看出,临界段未结束时中断已经开启.

- 正确应用

  ```c
  PRIMASK = 0;/* PRIMASK 初始值为 0,表示没有关中断 */
  
  CPU_SR cpu_sr1 = (CPU_SR)0
  CPU_SR cpu_sr2 = (CPU_SR)0
  
  /* 临界段代码 */
  {
  	/* 临界段 1 开始 */
  	cpu_sr1 = CPU_SR_Save();/* 关中断,cpu_sr1=0,PRIMASK=1 */
  	{
  		/* 临界段 2 */
  		cpu_sr2 = CPU_SR_Save();/* 关中断,cpu_sr2=1,PRIMASK=1 */
  		{
  			
  		}
  		CPU_SR_Restore(cpu_sr2);/* 开中断,cpu_sr2=1,PRIMASK=1 */
  }
  	/* 临界段 1 结束 */
  	CPU_SR_Restore(cpu_sr1);/* 开中断,cpu_sr1=0,PRIMASK=0 */
  }
  
  ```

  区别在于,`cpu_sr1`和`cpu_sr2`作为函数的形参传入开中断函数.

### 测量开关中断时间

- 系统会在每次关中断前开始测量,开中断后结束测量,测量功能保存了2个方面的测量值,总的关中断时间与最近一次关中断的时间.
- 过设置cpu_cfg.h文件中的宏定义`CPU_CFG_INT_DIS_MEAS_EN` 为1就表示启用该功能.

1. 测量中断时间初始化

   关中断之前要用`CPU_IntDisMeasInit()`函数进行初始化

   ```c
   #ifdef CPU_CFG_INT_DIS_MEAS_EN
   static void CPU_IntDisMeasInit(void){
   	CPU_TS_TMR time_meas_tot_cnts;
   	CPU_INT16U i;
   	CPU_SR_ALLOC();
       CPU_IntDisMeasCtr = 0u;
   	CPU_IntDisNestCtr = 0u;
   	CPU_IntDisMeasStart_cnts = 0u;
   	CPU_IntDisMeasStop_cnts = 0u;
   	CPU_IntDisMeasMaxCur_cnts = 0u;
   	CPU_IntDisMeasMax_cnts = 0u;
   	CPU_IntDisMeasOvrhd_cnts = 0u;
   	
   	time_meas_tot_cnts = 0u;
   	CPU_INT_DIS(); /* 关中断 */
   	for (i = 0u; i < CPU_CFG_INT_DIS_MEAS_OVRHD_NBR; i++)
   	{
   		CPU_IntDisMeasMaxCur_cnts = 0u;
   		CPU_IntDisMeasStart();/* 执行多个连续的开始/停止时间测量 */
   		CPU_IntDisMeasStop();
   		time_meas_tot_cnts += CPU_IntDisMeasMaxCur_cnts; /* 计算总的时间 */
   	}
   	//测量结果保存在CPU_IntDisMeasOvrhd_cnts
   	CPU_IntDisMeasOvrhd_cnts = (time_meas_tot_cnts +(CPU_CFG_INT_DIS_MEAS_OVRHD_NBR/2u))/CPU_CFG_INT_DIS_MEAS_OVRHD_NBR;
   	/* 得到平均值,就是每一次测量额外消耗的时间 */
   	CPU_IntDisMeasMaxCur_cnts = 0u;
   	CPU_IntDisMeasMax_cnts = 0u;
   	CPU_INT_EN();
   }
   #endif
   
   ```

2. 测量最大关中断时间

   启用了`CPU_CFG_INT_DIS_MEAS_EN` 这个宏定义,那么系统在关中断的时候会调用了开始测量关中断最大时间的函数 `CPU_IntDisMeasStart()`,开中断的时候调用停止测量关中断最大时间的函数 `CPU_IntDisMeasStop()`.

   ```c
   /* 开始测量关中断时间 */
   #ifdef CPU_CFG_INT_DIS_MEAS_EN
   void CPU_IntDisMeasStart(void){
       CPU_IntDisMeasCtr++;
   	if (CPU_IntDisNestCtr == 0u) /* 嵌套层数为 0 */
   	{
       	CPU_IntDisMeasStart_cnts = CPU_TS_TmrRd(); /* 保存时间戳 */
   	}
   	CPU_IntDisNestCtr++;
   }
   #endif
   
   /* 停止测量关中断时间 */
   #ifdef CPU_CFG_INT_DIS_MEAS_EN
   void CPU_IntDisMeasStop(void){
   	CPU_TS_TMR time_ints_disd_cnts;
   	CPU_IntDisNestCtr--;
   	if(CPU_IntDisNestCtr == 0u) /* 嵌套层数为 0*/
   	{
   		CPU_IntDisMeasStop_cnts = CPU_TS_TmrRd(); /* 保存时间戳 */
           
   		time_ints_disd_cnts = CPU_IntDisMeasStop_cnts -
   		CPU_IntDisMeasStart_cnts;/* 得到关中断时间 */
   		/* 更新最大关中断时间 */
   		if(CPU_IntDisMeasMaxCur_cnts < time_ints_disd_cnts)
   		{
   			CPU_IntDisMeasMaxCur_cnts = time_ints_disd_cnts;
   		}
   		if(CPU_IntDisMeasMax_cnts < time_ints_disd_cnts)
   		{
   			CPU_IntDisMeasMax_cnts = time_ints_disd_cnts;
   		}
   	}
   }
   #endif
       
   ```

   在关中断且嵌套层数`OSSchedLockNestingCtr`为0的时候保存下时间戳,如果嵌套层数不为0,肯定不是刚刚进入中断,退出中断且嵌套层数为0的时候,这个时候才算是真正的退出中断,把测得的时间戳减去一次测量额外消耗的时间,便得到这次关中断的时间,再将这个时间跟历史保存下的最大的关中断的时间对比.

3. 获取最大关中断时间

   μC/OS提供了三个与获取关中断时间有关的函数,分别是:

   - `CPU_IntDisMeasMaxCurGet()`

     获取整个程序运行过程中最大的关中断时间.

     ```c
     #ifdef CPU_CFG_INT_DIS_MEAS_EN//如果启用了关中断时间测量
     //获取测量的程序段的最大关中断时间
     CPU_TS_TMR CPU_IntDisMeasMaxCurGet(void){
         CPU_TS_TMR time_tot_cnts;
     	CPU_TS_TMR time_max_cnts;
     	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
     	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
     	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
     	CPU_INT_DIS(); //关中断
     	time_tot_cnts = CPU_IntDisMeasMaxCur_cnts;
     	//获取未处理的程序段最大关中断时间
     	CPU_INT_EN(); //开中断
     	time_max_cnts = CPU_IntDisMeasMaxCalc(time_tot_cnts);
     	//获取减去测量时间后的最大关中断时间
     	
     	return (time_max_cnts); //返回程序段的最大关中断时间
     }
     #endif
     
     ```

   - `CPU_IntDisMeasMaxCurReset()`和`CPU_IntDisMeasMaxGet()`

     在程序前调用`CPU_IntDisMeasMaxCurReset() `函数将`CPU_IntDisMeasMaxCur_cnts`变量清0,在程序结束的时候调用`CPU_IntDisMeasMaxCurGet() `函数即可.

     ```c
     #ifdef CPU_CFG_INT_DIS_MEAS_EN//如果启用了关中断时间测量
     //初始化(复位)测量程序段的最大关中断时间
     CPU_TS_TMR CPU_IntDisMeasMaxCurReset(void)
     {
     	CPU_TS_TMR time_max_cnts;
     	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
     	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
     	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
     	time_max_cnts=CPU_IntDisMeasMaxCurGet();//获取复位前的程序段最大关中断时间
     	CPU_INT_DIS(); //关中断
     	CPU_IntDisMeasMaxCur_cnts = 0u; //清零程序段的最大关中断时间
     	CPU_INT_EN(); //开中断
     	
     	return (time_max_cnts); //返回复位前的程序段最大关中断时间
     }
     #endif
     
     #ifdef CPU_CFG_INT_DIS_MEAS_EN//如果启用了关中断时间测量
     //获取整个程序目前最大的关中断时间
     CPU_TS_TMR CPU_IntDisMeasMaxGet(void){
     	CPU_TS_TMR time_tot_cnts;
     	CPU_TS_TMR time_max_cnts;
     	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
     	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
     	//SR(临界段关中断只需保存SR),开中断时将该值还原
     	CPU_INT_DIS(); //关中断
     	time_tot_cnts = CPU_IntDisMeasMax_cnts;
     	//获取尚未处理的最大关中断时间
     	CPU_INT_EN(); //开中断
     	time_max_cnts = CPU_IntDisMeasMaxCalc(time_tot_cnts);
     	//获取减去测量时间后的最大关中断时间
         
     	return (time_max_cnts); //返回目前最大关中断时间
     }
     #endif
     
     ```

     

## 就绪列表

- 在μC/OS中使用`OSTaskCreate()`函数创建任务后,任务的TCB会被放入就绪列表中.
- 就序列表包含一个表示任务优先级的优先表,一个储存任务TCB的TCB双向链表.

### 优先级表

- 优先级表是在os_prio.c文件开头定义的一个数组.

- ```c
  CPU_DATA   OSPrioTbl[OS_PRIO_TBL_SIZE];
  
  #define OS_PRIO_TBL_SIZE ((OS_CFG_PRIO_MAX - 1u) / (DEF_INT_CPU_NBR_BITS) + 1u)
  
  ```

  `OS_CFG_PRIO_MAX`(os_cfg.h)表示支持多少个优先级.

  `DEF_INT_CPU_NBR_BITS`定义CPU整型数据有多少位,Cortex-M为32位.

  默认情况下`OS_CFG_PRIO_MAX`为32,经计算`OS_PRIO_TBL_SIZE`的值为1,由此可知优先级表只需要一个成员就可以表示32个优先级.

- 优先级表示意图

  ![优先级表](.\Image\优先级表.png)

  创建一个优先级为Prio的任务,那么就在`OSPrioTbl[0]`的位[31-prio]置1即可.

- 判断单片机是低位优先还是高位优先

  1. 以Byte为最小单位

     ```c
     /*
     int i=10;
     内存从低到高(低位优先)
     00001010 00000000 00000000 00000000
     内存从高到低(高位优先)
     00000000 00000000 00000000 00001010
     */
     void Sequence_validation(void){
       	int a=10;
         short b;
         memcpy(&b,&a,2);
         printf("%d\n",b);
     }
     /*
     如果输出结果为10,储存是低位优先
     如果输出结果为0,储存是高位优先
     */
     ```

     实测:![低位优先](.\Image\低位优先.png)

  2. 以bit为最小单位

     数据都是左高右低.

### 优先级表函数

| 函数名称            | 函数作用               |
| ------------------- | ---------------------- |
| OS_PrioInit()       | 初始化优先级表         |
| OS_PrioInsert()     | 设置优先级表中相应的位 |
| OS_PrioRemove()     | 清除优先级表中相应的位 |
| OS_PrioGetHighest() | 查找最高的优先级       |

- `OS_PrioInit()`函数

  ```c
  /* 初始化优先级表 */
  void OS_PrioInit( void ){
      CPU_DATA i;
  	/* 默认全部初始化为 0 */
  	for ( i=0u; i<OS_PRIO_TBL_SIZE; i++ )
  	{
      OSPrioTbl[i] = (CPU_DATA)0;
  	}
  }
  
  ```

- `OS_PrioInsert()`函数

  ```c
  /* 置位优先级表中相应的位 */
  void OS_PrioInsert(OS_PRIO prio){
      CPU_DATA bit;
  	CPU_DATA bit_nbr;
  	OS_PRIO ix;
  	/* 求模操作,获取优先级表数组的下标索引 */
  	ix = prio / DEF_INT_CPU_NBR_BITS;
  
  	/* 求余操作,将优先级限制在 DEF_INT_CPU_NBR_BITS 之内 */
  	bit_nbr = (CPU_DATA)prio & (DEF_INT_CPU_NBR_BITS - 1u);
  
  	/* 获取优先级在优先级表中对应的位的位置 */
  	bit = 1u;
  	bit <<= (DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr;
  
  	/* 将优先级在优先级表中对应的位置 1 */
  	OSPrioTbl[ix] |= bit;
  }
  
  ```

- `OS_PrioRemove()`函数

  ```c
  /* 清除优先级表中相应的位 */
  void OS_PrioRemove(OS_PRIO prio){
      CPU_DATA bit;
  	CPU_DATA bit_nbr;
  	OS_PRIO ix;
  	/* 求模操作,获取优先级表数组的下标索引 */
  	ix = prio / DEF_INT_CPU_NBR_BITS;
  
  	/* 求余操作,将优先级限制在 DEF_INT_CPU_NBR_BITS 之内 */
  	bit_nbr = (CPU_DATA)prio & (DEF_INT_CPU_NBR_BITS - 1u);
  
  	/* 获取优先级在优先级表中对应的位的位置 */
  	bit = 1u;
  	bit <<= (DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr;
  
  	/* 将优先级在优先级表中对应的位清零 */
  	OSPrioTbl[ix] &= ~bit;
  }
  
  ```

- `OS_PrioGetHighest()`函数

  ```c
  /* 获取最高的优先级 */
  OS_PRIO OS_PrioGetHighest(void){
      CPU_DATA *p_tbl;
  	OS_PRIO prio;
  	prio = (OS_PRIO)0;
      /* 获取优先级表首地址 */
  	p_tbl = &OSPrioTbl[0];
  
  	/* 找到数值不为0的数组成员 */
  	while (*p_tbl == (CPU_DATA)0){
  		prio += DEF_INT_CPU_NBR_BITS;
  		p_tbl++;
  	}
  
  	/* 找到优先级表中最高的优先级(横向) */
  	prio += (OS_PRIO)CPU_CntLeadZeros(*p_tbl);
  	return (prio);
  }
  
  ```

  `CPU_CntLeadZeros`函数是由高位向低位查找第一个1前面有几个0.

  `OS_PrioGetHighest()`函数的大致原理:prio=优先级表数(纵向)*32+第一个1前面有几个0(横向).

### 就绪列表(函数级)

准备好运行的任务的TCB都会被放到就绪列表中,系统可随时调度任务运行.

- 就序列表是在os.h文件开头定义的一个数组.

  ```c
  OS_EXT	OS_RDY_LIST	OSRdyList[OS_CFG_PRIO_MAX];
  
  typedef	struct	os_rdy_list	OS_RDY_LIST;
  struct os_rdy_list {
  	OS_TCB	*HeadPtr;
      OS_TCB	*TailPtr;
  #if (OS_CFG_DBG_EN == DEF_ENABLED)
      OS_OBJ_QTY	NbrEntries;
  #endif
  };
  
  ```

  `OSRdyList[]`的成员数与任务的最大优先级相同,同一个优先级的多个任务会以双向链表的形式存在于`OSRdyList[]`的同一个索引下,`HeadPtr`用于指向链表的头节点,`TailPtr`用于指向链表的尾节点,该优先级下的索引成员的地址称为该优先级下双向链表的根节点,知道根节点的地址就可以查到该链表下的每一个节点.

  `NbrEntries`表示`OSRdyList[]`同一个索引下有多少个任务.

  初始化空的就绪列表时,`OSRdyList[]`索引下的成员都为0.

### 就序列表相关函数

就绪列表相关的所有函数都在 os_core.c 实现,这些函数都是以`OS_`开头,表示是OS的内部函数,用户不能调用.

| 函数名称                   | 函数作用                      |
| :------------------------- | ----------------------------- |
| OS_RdyListInit()           | 初始化就绪列表为空            |
| OS_RdyListInsert()         | 插入一个TCB到就绪列表         |
| OS_RdyListInsertHead ()    | 插入一个TCB到就绪列表的头部   |
| OS_RdyListInsertTail()     | 插入一个TCB到就绪列表的尾部   |
| OS_RdyListMoveHeadToTail() | 将TCB从就绪列表的头部移到尾部 |
| OS_RdyListRemove ()        | 将TCB从就绪列表中移除         |

```c
//相关结构体和变量
struct os_tcb{
    CPU_STK *StkPtr;
	CPU_STK_SIZE StkSize;
	/* 任务延时周期个数 */
	OS_TICK TaskDelayTicks;
	/* 任务优先级 */
	OS_PRIO Prio;

	/* 就绪列表双向链表的下一个指针 */
	OS_TCB *NextPtr;
	/* 就绪列表双向链表的前一个指针 */
	OS_TCB *PrevPtr;
};

/* 在 os.h 中定义 */
OS_EXT OS_PRIO OSPrioCur; /* 当前优先级 */
OS_EXT OS_PRIO OSPrioHighRdy; /* 最高优先级 */

```

- `OS_RdyListInit()`函数

  ```c
  //将就绪列表OSRdyList[]初始化为空
  void OS_RdyListInit(void){
      OS_PRIO i;
  	OS_RDY_LIST *p_rdy_list;
  	/* 循环初始化,所有成员都初始化为 0 */
  	for ( i=0u; i<OS_CFG_PRIO_MAX; i++ ) {
          p_rdy_list = &OSRdyList[i];
  	p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
  	p_rdy_list->HeadPtr = (OS_TCB *)0;
  	p_rdy_list->TailPtr = (OS_TCB *)0;
  	}
  }
  
  ```

  

- `OS_RdyListInsert()`函数

  ```c
  //将任务的TCB插入就绪列表
  void OS_RdyListInsert(OS_TCB *p_tcb){
      /* 将优先级插入优先级表 */
  	OS_PrioInsert(p_tcb->Prio);
  	if (p_tcb->Prio == OSPrioCur){
  	    /* 如果是当前优先级则插入链表尾部 */
  		OS_RdyListInsertTail(p_tcb);
  	}else{
  	/* 否则插入链表头部 */
  	OS_RdyListInsertHead(p_tcb);
  	}
  }
  
  ```

  

- `OS_RdyListInsertHead()`函数

  ```c
  //在链表头部插入一个TCB节点
  void OS_RdyListInsertHead(OS_TCB *p_tcb){
      OS_RDY_LIST *p_rdy_list;
  	OS_TCB *p_tcb2;
  	/* 获取链表根部 */
  	p_rdy_list = &OSRdyList[p_tcb->Prio];
  	/* CASE 0: 链表是空链表 */
  	if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) {
  		p_rdy_list->NbrEntries = (OS_OBJ_QTY)1;
  		p_tcb->NextPtr = (OS_TCB *)0;
  		p_tcb->PrevPtr = (OS_TCB *)0;
  		p_rdy_list->HeadPtr = p_tcb;
  		p_rdy_list->TailPtr = p_tcb;
  	}
  	/* CASE 1: 链表已有节点 */
  	else {
  		p_rdy_list->NbrEntries++;
  		p_tcb->NextPtr = p_rdy_list->HeadPtr;
  		p_tcb->PrevPtr = (OS_TCB *)0;
  		p_tcb2 = p_rdy_list->HeadPtr;
  		p_tcb2->PrevPtr = p_tcb;
  		p_rdy_list->HeadPtr = p_tcb;
  	}
  }
  
  ```

  链表的节点操作在这里不详细叙事(μC/OS源码该函数下有说明).

- `OS_RdyListInsertTail()`函数

  ```c
  //在链表尾部插入一个TCB节点
  void OS_RdyListInsertTail(OS_TCB *p_tcb){
      OS_RDY_LIST *p_rdy_list;
  	OS_TCB *p_tcb2;
  	/* 获取链表根部 */
  	p_rdy_list = &OSRdyList[p_tcb->Prio];
  	/* CASE 0: 链表是空链表 */
  	if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) {
  		p_rdy_list->NbrEntries = (OS_OBJ_QTY)1;
  		p_tcb->NextPtr = (OS_TCB *)0;
  		p_tcb->PrevPtr = (OS_TCB *)0;
  		p_rdy_list->HeadPtr = p_tcb;
  		p_rdy_list->TailPtr = p_tcb;
  	}
  	/* CASE 1: 链表已有节点 */
  	else {
  		p_rdy_list->NbrEntries++;
  		p_tcb->NextPtr = (OS_TCB *)0;
  		p_tcb2 = p_rdy_list->TailPtr;
  		p_tcb->PrevPtr = p_tcb2;
  		p_tcb2->NextPtr = p_tcb;
  		p_rdy_list->TailPtr = p_tcb;
  	}
  }
  
  ```

  链表的节点操作在这里不详细叙事(μC/OS源码该函数下有说明).

- `OS_RdyListMoveHeadToTail()`函数

  ```c
  //将节点从链表头部移动到尾部
  void OS_RdyListMoveHeadToTail(OS_RDY_LIST *p_rdy_list){
      OS_TCB *p_tcb1;
  	OS_TCB *p_tcb2;
  	OS_TCB *p_tcb3;
  	switch (p_rdy_list->NbrEntries) {
  		case 0:
  		case 1:
  		break;
  		case 2:
              p_tcb1 = p_rdy_list->HeadPtr;
  			p_tcb2 = p_rdy_list->TailPtr;
  			p_tcb1->PrevPtr = p_tcb2;
  			p_tcb1->NextPtr = (OS_TCB *)0;
  			p_tcb2->PrevPtr = (OS_TCB *)0;
  			p_tcb2->NextPtr = p_tcb1;
  			p_rdy_list->HeadPtr = p_tcb2;
  			p_rdy_list->TailPtr = p_tcb1;
  			break;
  		default:
  			p_tcb1 = p_rdy_list->HeadPtr;
  			p_tcb2 = p_rdy_list->TailPtr;
  			p_tcb3 = p_tcb1->NextPtr;
  			p_tcb3->PrevPtr = (OS_TCB *)0;
  			p_tcb1->NextPtr = (OS_TCB *)0;
  			p_tcb1->PrevPtr = p_tcb2;
  			p_tcb2->NextPtr = p_tcb1;
  			p_rdy_list->HeadPtr = p_tcb3;
  			p_rdy_list->TailPtr = p_tcb1;
  			break;
  	}
  }
  
  ```

  链表的节点操作在这里不详细叙事(μC/OS源码该函数下有说明).

- `OS_RdyListRemove()`函数

  ```c
  //表中移除一个节点
  void OS_RdyListRemove(OS_TCB *p_tcb){
      OS_RDY_LIST *p_rdy_list;
  	OS_TCB *p_tcb1;
  	OS_TCB *p_tcb2;
  	p_rdy_list = &OSRdyList[p_tcb->Prio];
  	/* 保存要删除的 TCB 节点的前一个和后一个节点 */
  	p_tcb1 = p_tcb->PrevPtr;
  	p_tcb2 = p_tcb->NextPtr;
  	
  	/* 要移除的 TCB 节点是链表中的第一个节点 */
  	if (p_tcb1 == (OS_TCB *)0){
  		/* 且该链表中只有一个节点 */
  		if (p_tcb2 == (OS_TCB *)0){
  			/* 根节点全部初始化为 0 */
  			p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
  			p_rdy_list->HeadPtr = (OS_TCB *)0;
  			p_rdy_list->TailPtr = (OS_TCB *)0;
      
  			/* 清除在优先级表中相应的位 */
  			OS_PrioRemove(p_tcb->Prio);
  		}
  		/* 该链表中不止一个节点 */
  		else{
  			/* 节点减 1 */
  			p_rdy_list->NbrEntries--;
  			p_tcb2->PrevPtr = (OS_TCB *)0;
  			p_rdy_list->HeadPtr = p_tcb2;
  		}
  	}
  	/* 要移除的 TCB 节点不是链表中的第一个节点 */
  	else{
  		p_rdy_list->NbrEntries--;
  		p_tcb1->NextPtr = p_tcb2;
  		
  		/* 如果要删除的节点的下一个节点是 0,即要删除的节点是最后一个节点 */
  		if (p_tcb2 == (OS_TCB *)0)
  		{
  			p_rdy_list->TailPtr = p_tcb1;
  		}else{
  			p_tcb2->PrevPtr = p_tcb1;
  		}
  	}
  	/* 复位从就绪列表中删除的 TCB 的 PrevPtr 和 NextPtr 这两个指针 */
  	p_tcb->PrevPtr = (OS_TCB *)0;
  	p_tcb->NextPtr = (OS_TCB *)0;
  }
  
  ```

  链表的节点操作在这里不详细叙事(μC/OS源码该函数下有说明).

## 支持多优先级

数字优先级越小,逻辑优先级越高.

### 优先级相关全局变量

os.h文件中定义了优先级相关的两个全局变量.

```c
OS_EXT            OS_PRIO                   OSPrioCur;//当前优先级
OS_EXT            OS_PRIO                   OSPrioHighRdy;//最高优先级
```

### OSInit()函数

OS中定义的所有的全局变量都是在`OSInit()`中初始化的,其中也包括优先级相关的全局变量.

```c
void OSInit (OS_ERR *p_err){
    /* 配置 OS 初始状态为停止态 */
	OSRunning = OS_STATE_OS_STOPPED;
	/* 初始化两个全局 TCB,这两个 TCB 用于任务切换 */
    OSTCBCurPtr = (OS_TCB *)0;
    OSTCBHighRdyPtr = (OS_TCB *)0;
	/* 初始化优先级变量 */
	OSPrioCur = (OS_PRIO)0;
	OSPrioHighRdy = (OS_PRIO)0;
    
	/* 初始化优先级表 */
	OS_PrioInit();
	
	/* 初始化就绪列表 */
	OS_RdyListInit();
	
	/* 初始化空闲任务 */
	OS_IdleTaskInit(p_err);
	if (*p_err != OS_ERR_NONE) {
		return;
	}
}

```

### 任务控制块

优先级`Prio`的数据类型为`OS_PRIO`,宏展开后是8位的整型,所以只支持255个优先级.

```c
struct os_tcb{
    ...
    //任务优先级
   	OS_PRIO	Prio;
    ...
}
```

### OSTaskCreate()函数

```c
void  OSTaskCreate (OS_TCB        *p_tcb,
                    CPU_CHAR      *p_name,
                    OS_TASK_PTR    p_task,
                    void          *p_arg,
                    OS_PRIO        prio,
                    CPU_STK       *p_stk_base,
                    CPU_STK_SIZE   stk_limit,
                    CPU_STK_SIZE   stk_size,
                    OS_MSG_QTY     q_size,
                    OS_TICK        time_quanta,
                    void          *p_ext,
                    OS_OPT         opt,
                    OS_ERR        *p_err){	
    CPU_STK *p_sp;
	CPU_SR_ALLOC();
	
	/* 初始化 TCB 为默认值 */
	OS_TaskInitTCB(p_tcb);
	
	/* 初始化栈 */
	p_sp = OSTaskStkInit( p_task,
						  p_arg,
                          p_stk_base,
	                      stk_size );
    
	p_tcb->Prio = prio;
    
	p_tcb->StkPtr = p_sp;
	p_tcb->StkSize = stk_size;
	
	/* 进入临界段 */
	OS_CRITICAL_ENTER();
    
	/* 将任务添加到就绪列表 */
	OS_PrioInsert(p_tcb->Prio);
	OS_RdyListInsertTail(p_tcb);
    
	/* 退出临界段 */
	OS_CRITICAL_EXIT();
    
	*p_err = OS_ERR_NONE;
}  

void OS_TaskInitTCB(OS_TCB *p_tcb){
    p_tcb->StkPtr = (CPU_STK *)0;
    p_tcb->StkSize = (CPU_STK_SIZE )0u;
    p_tcb->TaskDelayTicks = (OS_TICK )0u;
    p_tcb->Prio = (OS_PRIO )OS_PRIO_INIT;
	p_tcb->NextPtr = (OS_TCB *)0;
	p_tcb->PrevPtr = (OS_TCB *)0;
}

```

`OS_PRIO_INIT`(os.h)是任务TCB初始化的时候给的默认的一个优先级,宏展开等于`OS_CFG_PRIO_MAX`,这是一个不会被OS使用到的优先级.

### OS_IdleTaskInit()函数

```c
/* 空闲任务初始化 */
void OS_IdleTaskInit(OS_ERR *p_err){
    /* 初始化空闲任务计数器 */
	OSIdleTaskCtr = (OS_IDLE_CTR)0;
    /* 创建空闲任务 */
	OSTaskCreate( (OS_TCB *)&OSIdleTaskTCB,
	(OS_TASK_PTR )OS_IdleTask,
	(void *)0,
	(OS_PRIO)(OS_CFG_PRIO_MAX - 1u),
	(CPU_STK *)OSCfg_IdleTaskStkBasePtr,
	(CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
	(OS_ERR *)p_err );
}
    
```

空闲任务的优先级最低,为OS_CFG_PRIO_MAX - 1u

### OSStart()函数

```c
/* 启动 RTOS,将不再返回 */
2 void OSStart(OS_ERR *p_err){
    if ( OSRunning == OS_STATE_OS_STOPPED ) {
#if 0
		/* 手动配置任务 1 先运行 */
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
#endif
		/* 寻找最高的优先级 */
		OSPrioHighRdy = OS_PrioGetHighest();
		OSPrioCur = OSPrioHighRdy;
	
		/* 找到最高优先级的 TCB */
		OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;
		OSTCBCurPtr = OSTCBHighRdyPtr;
    
		/* 标记 OS 开始运行 */
		OSRunning = OS_STATE_OS_RUNNING;
    
		/* 启动任务切换,不会返回 */
		OSStartHighRdy();
    
		/* 不会运行到这里,运行到这里表示发生了致命的错误 */
		*p_err = OS_ERR_FATAL_RETURN;
	} else {
		*p_err = OS_STATE_OS_RUNNING;
	}
}

```

`OS_PrioGetHighest()`函数从全局变量优先级表`OSPrioTbl[]`中获取最高的优先级存入`OSPrioHighRdy`这个全局变量中,然后再赋值给`OSPrioCur`这个全局变量.这两个变量在任务切换的时候需要用到.同样可以找到最高优先级的TCB以供后续使用.

### OS_CPU_PendSVHandler()函数

```assembly
OS_CPU_PendSVHandler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer
    STMFD   R0!, {R4-R11}                                       ; Save remaining regs r4-11 on process stack

    MOV32   R5, OSTCBCurPtr                                     ; OSTCBCurPtr->OSTCBStkPtr = SP;
    LDR     R6, [R5]
    STR     R0, [R6]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
    MOV     R4, LR                                              ; Save LR exc_return value
    BL      OSTaskSwHook                                        ; OSTaskSwHook();

    MOV32   R0, OSPrioCur                                       ; OSPrioCur   = OSPrioHighRdy;
    MOV32   R1, OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    MOV32   R1, OSTCBHighRdyPtr                                 ; OSTCBCurPtr = OSTCBHighRdyPtr;
    LDR     R2, [R1]
    STR     R2, [R5]

    ORR     LR, R4, #0x04                                       ; Ensure exception return uses process stack
    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    LDMFD   R0!, {R4-R11}                                       ; Restore r4-11 from new process stack
    MSR     PSP, R0                                             ; Load PSP with new process SP
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context

    END
```



### OSTimeDly()函数

```c
/* 阻塞延时 */
void OSTimeDly(OS_TICK dly){
#if 0
	/* 设置延时时间 */
	OSTCBCurPtr->TaskDelayTicks = dly;
	/* 进行任务调度 */
	OSSched();
#endif

	CPU_SR_ALLOC();

    /* 进入临界区 */
	OS_CRITICAL_ENTER();
	
	/* 设置延时时间 */
	OSTCBCurPtr->TaskDelayTicks = dly;
    
	/* 从就绪列表中移除 */
	//OS_RdyListRemove(OSTCBCurPtr);
	OS_PrioRemove(OSTCBCurPtr->Prio);
    
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
    
	/* 任务调度 */
	OSSched();
}

```

任务处于阻塞态,将任务从就绪列表移除,这里只需将任务在优先级表中对应的位清除即可,暂时不需要把任务TCB从`OSRdyList[]`中移除,因为接下来`OSTimeTick()`函数还是通过扫描`OSRdyList[]`来判断任务的延时时间是否到期.当我们加入了时基列表之后,当任务调用`OSTimeDly()`函数进行延时,就可以把任务的TCB从就绪列表删除,然后把任务TCB插入时基列表,`OSTimeTick()`函数判断任务的延时是否到期只需通过扫描时基列表即可.所以这里暂时不能把TCB从就绪列表中删除,只是将任务优先级在优先级表中对应的位清除来达到任务不处于就绪态的目的.

### OSSched()函数

根据优先级来调度任务

```c
void OSSched(void){
#if 0
/* 如果当前任务是空闲任务,那么就去尝试执行任务 1 或者任务 2, 5 看看他们的延时时间是否结束,如果任务的延时时间均没有到期,那就返回继续执行空闲任务 */
	if ( OSTCBCurPtr == &OSIdleTaskTCB ){
        if (OSRdyList[0].HeadPtr->TaskDelayTicks == 0){
			OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
		}else if (OSRdyList[1].HeadPtr->TaskDelayTicks == 0){
			OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
		}else{
			return; /* 任务延时均没有到期则返回,继续执行空闲任务 */
		}
	}else{
		 /* 如果是 task1 或者 task2 的话,检查下另外一个任务,
		如果另外的任务不在延时中,就切换到该任务,
		否则,判断下当前任务是否应该进入延时状态,
		如果是的话,就切换到空闲任务.否则就不进行任何切换 */
		if (OSTCBCurPtr == OSRdyList[0].HeadPtr){
			if (OSRdyList[1].HeadPtr->TaskDelayTicks == 0){
				OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
			} else if (OSTCBCurPtr->TaskDelayTicks != 0){
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			} else {
				/* 返回,不进行切换,因为两个任务都处于延时中 */
				return;
			}
		}else if (OSTCBCurPtr == OSRdyList[1].HeadPtr){
			if (OSRdyList[0].HeadPtr->TaskDelayTicks == 0){
				OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
			}else if (OSTCBCurPtr->TaskDelayTicks != 0){
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			}else{
				/* 返回,不进行切换,因为两个任务都处于延时中 */
				return;
			}
		}
	}
	
	/* 任务切换 */
	OS_TASK_SW();
#endif
	
	CPU_SR_ALLOC();(1)
	
	/* 进入临界区 */
	OS_CRITICAL_ENTER();(2)
	
	/* 查找最高优先级的任务 */
	OSPrioHighRdy = OS_PrioGetHighest();
	OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;
   	
	/* 如果最高优先级的任务是当前任务则直接返回,不进行任务切换 */
	if (OSTCBHighRdyPtr == OSTCBCurPtr){
		/* 退出临界区 */
		OS_CRITICAL_EXIT();
	
		return;
	}
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
	
	/* 任务切换 */
	OS_TASK_SW();
}

```

判断最高优先级任务是不是当前任务,如果是则直接返回,否则将继续往下执行,最后执行任务切换.

### OSTimeTick()函数

`OSTimeTick()`函数在`SysTick`中断服务函数中被调用,是一个周期函数,具体用于扫描就绪列表`OSRdyList[]`,判断任务的延时时间是否到期,如果到期则将任务在优先级表中对应的位置位.

```c
void OSTimeTick(void){
    unsigned int i;
	CPU_SR_ALLOC();
    /* 进入临界区 */
	OS_CRITICAL_ENTER();
/* 扫描就绪列表中所有任务的 TaskDelayTicks,如果不为 0,则减 1 */
#if 0
	for (i=0; i<OS_CFG_PRIO_MAX; i++){
		if (OSRdyList[i].HeadPtr->TaskDelayTicks > 0){
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
		}
	}
#endif
	
	for (i=0; i<OS_CFG_PRIO_MAX; i++){
		if (OSRdyList[i].HeadPtr->TaskDelayTicks > 0){
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
			if (OSRdyList[i].HeadPtr->TaskDelayTicks == 0){
				/* 为 0 则表示延时时间到,让任务就绪 */
				//OS_RdyListInsert (OSRdyList[i].HeadPtr);
				OS_PrioInsert(i);
			}
		}
	}
	
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
	
	/* 任务调度 */
	
	OSSched();
}

```

扫描就绪列表`OSRdyList[]`判断任务的延时时间是否到期,如果到期则将任务在优先级表中对应的位置位.

## 时基列表

- 时基列表是跟时间相关的,处于延时的任务和等待事件有超时限制的任务都会从就绪列表中移除,然后插入时基列表.
- 时基列表在`OSTimeTick()`函数中更新.

### μC/OS如何实现时基列表

#### 时基列表变量

时基列表在代码层面上由全局数组`OSCfg_TickWheel[]`和全局变量`OSTickCtr`构成.//这里不对

![时基列表](.\Image\时基列表.png)

#### 任务控制块TCB

时基列表`OSCfg_TickWheel[]`的每个成员都包含一条单向链表,被插入该条链表的TCB会按照延时时间做升序排列.

![时基列表和TCB的关系](.\Image\时基列表和TCB的关系.png)

```c
struct os_tcb{
    ...
    //时基列表相关代码段
    OS_TCB	*TickNextPtr;
    OS_TCB	*TickPrevPtr;
    OS_TICK_SPOKE	*TickSpokePtr;
    OS_TICK	TickCtrMatch;
    OS_TICK TickRemain;
    ...
}
```

- `TickNextPtr`用于指向链表中的下一个TCB节点.
- `TickPrevPtr`用于指向链表中的上一个TCB节点.
- `TickRemain`用于设置任务还需要等待多少个时钟周期,每到来一个时钟周期,该值会递减.
- `TickCtrMatch`的值等于时基计数器`OSTickCtr`的值加上`TickRemain`的值,当`TickCtrMatch`的值等于`OSTickCtr`的值的时候,表示等待到期,TCB 会从链表中删除.
- 每个被插入链表的TCB都包含一个字段`TickSpokePtr`,用于回指到链表的根部.

#### 时列表相关函数

| 函数名称            | 函数作用                                                  |
| ------------------- | --------------------------------------------------------- |
| OS_TickListInit()   | 初始化时基列表.                                           |
| OS_TickListInsert() | 向时基列表中插入一个任务TCB.                              |
| OS_TickListRemove() | 从时基列表删除一个指定的TCB节点.                          |
| OS_TickListUpdate() | 更新时基计数器OSTickCtr,扫描时基列表中的任务延时是否到期. |
| OS_TaskRdy()        | 将任务从时基列表删除,插入就绪列表.                        |

- `OS_TickListInit()`函数

  ```c
  //初始化时基列表的数据域
  void OS_TickListInit(void){
      OS_TICK_SPOKE_IX i;
  	OS_TICK_SPOKE *p_spoke;
  	for (i = 0u; i < OSCfg_TickWheelSize; i++) {
  		p_spoke = (OS_TICK_SPOKE *)&OSCfg_TickWheel[i];
  		p_spoke->FirstPtr = (OS_TCB *)0;
  		p_spoke->NbrEntries = (OS_OBJ_QTY )0u;
  		p_spoke->NbrEntriesMax = (OS_OBJ_QTY )0u;
  	}
  }
  
  ```

  都初始化为0.

- `OS_TickListInsert()`函数

  ![时基列表插TCB](.\Image\时基列表插TCB.png)

  ```c
  //将一个任务插入时基列表,根据延时时间的大小升序排列
  void OS_TickListInsert(OS_TCB *p_tcb,OS_TICK time){
      OS_TICK_SPOKE_IX spoke;
  	OS_TICK_SPOKE *p_spoke;
  	OS_TCB *p_tcb0;
  	OS_TCB *p_tcb1;
  	p_tcb->TickCtrMatch = OSTickCtr + time;
  	p_tcb->TickRemain = time;
  	
  	spoke = (OS_TICK_SPOKE_IX)(p_tcb->TickCtrMatch % OSCfg_TickWheelSize);
  
  	p_spoke = &OSCfg_TickWheel[spoke];
  	
  	/* 插入 OSCfg_TickWheel[spoke] 的第一个节点 */
  	if (p_spoke->NbrEntries == (OS_OBJ_QTY)0u){
  		p_tcb->TickNextPtr = (OS_TCB *)0;
  		p_tcb->TickPrevPtr = (OS_TCB *)0;
  		p_spoke->FirstPtr = p_tcb;
  		p_spoke->NbrEntries = (OS_OBJ_QTY)1u;
  	}
  	/* 如果插入的不是第一个节点,则按照 TickRemain 大小升序排列 */
  	else {
  		/* 获取第一个节点指针 */
  		p_tcb1 = p_spoke->FirstPtr;
  		while (p_tcb1 != (OS_TCB *)0){
  			/* 计算比较节点的剩余时间 */
  			p_tcb1->TickRemain = p_tcb1->TickCtrMatch - OSTickCtr;
  			
  			/* 插入比较节点的后面 */
  			if (p_tcb->TickRemain > p_tcb1->TickRemain){
  				if (p_tcb1->TickNextPtr != (OS_TCB *)0){
  					/* 寻找下一个比较节点 */
  					p_tcb1 = p_tcb1->TickNextPtr;
  				} else { /* 在最后一个节点插入 */
  					p_tcb->TickNextPtr = (OS_TCB *)0;
  					p_tcb->TickPrevPtr = p_tcb1;
  					p_tcb1->TickNextPtr = p_tcb;
  					p_tcb1 = (OS_TCB *)0;
  				}
  			} else {/* 插入比较节点的前面 */
  				/* 在第一个节点插入 */
  				if (p_tcb1->TickPrevPtr == (OS_TCB *)0) {
  					p_tcb->TickPrevPtr = (OS_TCB *)0;
  					p_tcb->TickNextPtr = p_tcb1;
  					p_tcb1->TickPrevPtr = p_tcb;
  					p_spoke->FirstPtr = p_tcb;
   				}else{
  					/* 插入两个节点之间 */
  					p_tcb0 = p_tcb1->TickPrevPtr;
  					p_tcb->TickPrevPtr = p_tcb0;
  					p_tcb->TickNextPtr = p_tcb1;
  					p_tcb0->TickNextPtr = p_tcb;
  					p_tcb1->TickPrevPtr = p_tcb;
  				}
  				/* 跳出 while 循环 */
  				p_tcb1 = (OS_TCB *)0;(8)
  			}
  		}
  	
  		/* 节点成功插入 */
  		p_spoke->NbrEntries++;(9)
  	}
  	
  	/* 刷新 NbrEntriesMax 的值 */
  	if (p_spoke->NbrEntriesMax < p_spoke->NbrEntries){
  		p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
  	}
  	
  	/* 任务 TCB 中的 TickSpokePtr 回指根节点 */
  	p_tcb->TickSpokePtr = p_spoke;(11)
  }
  
  ```

  

- `OS_TickListRemove()`函数

  ```c
  //从时基列表中移除一个任务
  2 void OS_TickListRemove(OS_TCB *p_tcb){
      OS_TICK_SPOKE *p_spoke;
  	OS_TCB *p_tcb1;
  	OS_TCB *p_tcb2;
  	/* 获取任务 TCB 所在链表的根指针 */
  	p_spoke = p_tcb->TickSpokePtr;(1)
  	
  	/* 确保任务在链表中 */
  	if (p_spoke != (OS_TICK_SPOKE *)0) {
  		/* 将剩余时间清零 */
  		p_tcb->TickRemain = (OS_TICK)0u;
  		
  		/* 要移除的刚好是第一个节点 */
  		if (p_spoke->FirstPtr == p_tcb) {
  			/* 更新第一个节点,原来的第一个节点需要被移除 */
  			p_tcb1 = (OS_TCB *)p_tcb->TickNextPtr;
  			p_spoke->FirstPtr = p_tcb1;
  			if (p_tcb1 != (OS_TCB *)0) {
  				p_tcb1->TickPrevPtr = (OS_TCB *)0;
  			}
  		} else {/* 要移除的不是第一个节点 */
      		/* 保存要移除的节点的前后节点的指针 */
  			p_tcb1 = p_tcb->TickPrevPtr;
  			p_tcb2 = p_tcb->TickNextPtr;
  			
  			/* 节点移除,将节点前后的两个节点连接在一起 */
  			p_tcb1->TickNextPtr = p_tcb2;
  			if (p_tcb2 != (OS_TCB *)0) {
  				p_tcb2->TickPrevPtr = p_tcb1;
  			}
  		}
  		
  		/* 复位任务 TCB 中时基列表相关的字段成员 */
  		p_tcb->TickNextPtr = (OS_TCB *)0;
  		p_tcb->TickPrevPtr = (OS_TCB *)0;
  		p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0;
  		p_tcb->TickCtrMatch = (OS_TICK )0u;
  		
  		/* 节点减 1 */
  		p_spoke->NbrEntries--;(5)
  	}
  }
  
  ```

  

- `OS_TickListUpdate()`函数

  ```c
  void OS_TickListUpdate(void){
      OS_TICK_SPOKE_IX spoke;
  	OS_TICK_SPOKE *p_spoke;
  	OS_TCB *p_tcb;
  	OS_TCB *p_tcb_next;
  	CPU_BOOLEAN done;
  
  	CPU_SR_ALLOC();
  	/* 进入临界段 */
  	OS_CRITICAL_ENTER();
      
  	/* 时基计数器 ++ */
  	OSTickCtr++;
      
  	spoke = (OS_TICK_SPOKE_IX)(OSTickCtr % OSCfg_TickWheelSize);(2)
  	p_spoke = &OSCfg_TickWheel[spoke];]
      
  	p_tcb = p_spoke->FirstPtr;
  	done = DEF_FALSE;
  	
  	while (done == DEF_FALSE) {
  		if (p_tcb != (OS_TCB *)0) {
  			p_tcb_next = p_tcb->TickNextPtr;
  			
  			p_tcb->TickRemain = p_tcb->TickCtrMatch - OSTickCtr;
  			
  			/* 节点延时时间到 */
  			if (OSTickCtr == p_tcb->TickCtrMatch) {
  				/* 让任务就绪 */
  				OS_TaskRdy(p_tcb);
   			} else {
  				/* 如果第一个节点延时期未满,则退出 while 循环
  				因为链表是根据升序排列的,第一个节点延时期未满,那后面的肯定未满 */
  				done = DEF_TRUE;
  			}
  			/* 如果第一个节点延时期满,则继续遍历链表,看看还有没有延时期满的任务
  			如果有,则让它就绪 */
  			p_tcb = p_tcb_next;(7)
  		} else {
  			done = DEF_TRUE;(8)
  		}
  	}
  	
  	/* 退出临界段 */
  	OS_CRITICAL_EXIT();
  }
  
  ```

  

- `OS_TaskRdy()`函数

  ![时基列表删除TCB](.\Image\时基列表删除TCB.png)

  ```c
  //将任务从时基列表删除,插入就列表
  void OS_TaskRdy(OS_TCB *p_tcb){
      /* 从时基列表删除 */
  	OS_TickListRemove(p_tcb);
  	/* 插入就绪列表 */
  	OS_RdyListInsert(p_tcb);
  }
  
  ```

### **OSTimeDly()** 函数

```c
void OSTimeDly(OS_TICK dly){
    CPU_SR_ALLOC();
	/* 进入临界区 */
	OS_CRITICAL_ENTER();
#if 0
	/* 设置延时时间 */
	OSTCBCurPtr->TaskDelayTicks = dly;
	
	/* 从就绪列表中移除 */
	//OS_RdyListRemove(OSTCBCurPtr);
	OS_PrioRemove(OSTCBCurPtr->Prio);
#endif
	
	/* 插入时基列表 */
	OS_TickListInsert(OSTCBCurPtr, dly);
	
	/* 从就绪列表移除 */
	OS_RdyListRemove(OSTCBCurPtr);
	
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
    
	/* 任务调度 */
	OSSched();
}

```



### **OSTimeTick()** 函数

```c
void OSTimeTick(void){
#if 0
	unsigned int i;
	CPU_SR_ALLOC();

    /* 进入临界区 */
	OS_CRITICAL_ENTER();
    
	for (i=0; i<OS_CFG_PRIO_MAX; i++) {
		if (OSRdyList[i].HeadPtr->TaskDelayTicks > 0) {
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
			if (OSRdyList[i].HeadPtr->TaskDelayTicks == 0) {
				/* 为 0 则表示延时时间到,让任务就绪 */
				//OS_RdyListInsert (OSRdyList[i].HeadPtr);
				OS_PrioInsert(i);
			}
		}
    }
	
	/* 退出临界区 */
	OS_CRITICAL_EXIT();
    
#endif
	
	/* 更新时基列表 */
	OS_TickListUpdate();
	
    /* 任务调度 */
	OSSched();
}

```



## 时间片

### μC/OS如何实现时间片

#### 任务TCB

```c
struct os_tcb{
    ...
    /* 时间片相关字段 */
	OS_TICK TimeQuanta;
	OS_TICK TimeQuantaCtr;
    ...
}
```

- `TimeQuanta`表示任务需要多少个时间片,单位为系统时钟周期Tick.

- `TimeQuantaCtr`表示任务还剩下多少个时间,每到来一个系统时钟周期`TimeQuantaCtr`会减一,当 `TimeQuantaCtr`等于0的时候,表示时间片用完,任务的TCB会从就绪列表链表的头部移动到尾部,好让下一个任务共享时间片.

#### 时间片调度函数

```c
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void OS_SchedRoundRobin(OS_RDY_LIST *p_rdy_list){
    OS_TCB *p_tcb;
	CPU_SR_ALLOC();
	/* 进入临界段 */
	CPU_CRITICAL_ENTER();
	
	p_tcb = p_rdy_list->HeadPtr;
	
	/* 如果 TCB 节点为空,则退出 */
	if (p_tcb == (OS_TCB *)0) {
		CPU_CRITICAL_EXIT();
		return;
	}
	
	/* 如果是空闲任务,也退出 */
	if (p_tcb == &OSIdleTaskTCB) {
		CPU_CRITICAL_EXIT();
		return;
	}
	
	/* 时间片自减 */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) {
		p_tcb->TimeQuantaCtr--;
	}
	
	/* 时间片没有用完,则退出 */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) {
		CPU_CRITICAL_EXIT();
		return;
	}
	
	/* 如果当前优先级只有一个任务,则退出 */
	if (p_rdy_list->NbrEntries < (OS_OBJ_QTY)2) {
		CPU_CRITICAL_EXIT();
		return;
	}
    
	/* 时间片耗完,将任务放到链表的最后一个节点 */
	OS_RdyListMoveHeadToTail(p_rdy_list);
	
	/* 重新获取任务节点 */
	p_tcb = p_rdy_list->HeadPtr;
	/* 重载默认的时间片计数值 */
	p_tcb->TimeQuantaCtr = p_tcb->TimeQuanta;
	
	/* 退出临界段 */
	CPU_CRITICAL_EXIT();
}
#endif/* OS_CFG_SCHED_ROUND_ROBIN_EN > 0u */

```

- 时 间 片 是 一 个 可 选 的 功 能, 是否选择由`OS_CFG_SCHED_ROUND_ROBIN_EN`控制,该宏在os_cfg.h定义.
- 时间片用完,如果该优先级下有两个以上任务,则将刚刚消耗完时间片的节点移到链表的末尾,此时位于末尾的任务的TCB字段中的`TimeQuantaCtr`是等于0的,只有等他下一次运行的时候值才会重置为 `TimeQuanta`.

### OSTimeTick()函数

任务的时间片的单位在每个系统时钟周期到来的时候被更新,时间片调度函数则由时基周期处理函数`OSTimeTick()`调用.

```c
void OSTimeTick(void){
    /* 更新时基列表 */
	OS_TickListUpdate();
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	/* 时间片调度 */
	OS_SchedRoundRobin(&OSRdyList[OSPrioCur]);
#endif
	
	/* 任务调度 */
	OSSched();
}

```

### OSTaskCreate()函数

任务的时间片在函数创建的时候被指定.

```c
void OSTaskCreate (OS_TCB *p_tcb,
				   OS_TASK_PTR p_task,
				   void *p_arg,
     		       OS_PRIO prio,
				   CPU_STK *p_stk_base,
				   CPU_STK_SIZE stk_size,
				   OS_TICK time_quanta,
			       OS_ERR *p_err) {
	
    CPU_STK *p_sp;
	CPU_SR_ALLOC();
	
	/* 初始化 TCB 为默认值 */
	OS_TaskInitTCB(p_tcb);
	
	/* 初始化栈 */
	p_sp = OSTaskStkInit( p_task,
	p_arg,
	p_stk_base,
	stk_size );
	
	p_tcb->Prio = prio;
	
	p_tcb->StkPtr = p_sp;
	p_tcb->StkSize = stk_size;
	
	/* 时间片相关初始化 */
	p_tcb->TimeQuanta = time_quanta;
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	p_tcb->TimeQuantaCtr = time_quanta;
#endif
	
	/* 进入临界段 */
	OS_CRITICAL_ENTER();
	
	/* 将任务添加到就绪列表 */
	OS_PrioInsert(p_tcb->Prio);
	OS_RdyListInsertTail(p_tcb);
	
	/* 退出临界段 */
	OS_CRITICAL_EXIT();
    
	*p_err = OS_ERR_NONE;
}

```

- 初始化时间片计数器`TimeQuantaCtr`的值等于`TimeQuanta`,每经过一个系统时钟周期,该值会递减,如果该值为0,则表示时间片耗完.

### **OS_IdleTaskInit()** 函数

```c
void OS_IdleTaskInit(OS_ERR *p_err){
    /* 初始化空闲任务计数器 */
	OSIdleTaskCtr = (OS_IDLE_CTR)0;
    /* 创建空闲任务 */
	OSTaskCreate( (OS_TCB *)&OSIdleTaskTCB,
	(OS_TASK_PTR )OS_IdleTask,
	(void *)0,
	(OS_PRIO)(OS_CFG_PRIO_MAX - 1u),
	(CPU_STK *)OSCfg_IdleTaskStkBasePtr,
	(CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
	(OS_TICK )0,
	(OS_ERR *)p_err );
}

```



## 任务的挂起和恢复

### μC/OS如何实现任务的挂起和恢复

#### 任务的状态

任务实现挂起和恢复的时候,根据任务的状态来操作,任务的状态不同,操作也不同.

os.h

```c
/* ---------- 任务的状态 -------*/
#define OS_TASK_STATE_BIT_DLY (OS_STATE)(0x01u)/* 挂起位 */
#define OS_TASK_STATE_BIT_PEND (OS_STATE)(0x02u)/* 等待位 */
#define OS_TASK_STATE_BIT_SUSPENDED (OS_STATE)(0x04u)/* 延时/超时位 */
#define OS_TASK_STATE_RDY (OS_STATE)( 0u)/* 0 0 0 就 绪 */
#define OS_TASK_STATE_DLY (OS_STATE)( 1u)/* 0 0 1 延时或者超时 */
#define OS_TASK_STATE_PEND (OS_STATE)( 2u)/* 0 1 0 等 待 */
#define OS_TASK_STATE_PEND_TIMEOUT (OS_STATE)( 3u)/* 0 1 1 等 待 + 超时 */
#define OS_TASK_STATE_SUSPENDED (OS_STATE)( 4u)/* 1 0 0 挂 起 */
#define OS_TASK_STATE_DLY_SUSPENDED (OS_STATE)( 5u)/* 1 0 1 挂 起 + 延时或者超时 */
#define OS_TASK_STATE_PEND_SUSPENDED (OS_STATE)( 6u)/* 1 1 0 挂 起 + 等待 */
#define OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED (OS_STATE)( 7u)/* 1 1 1 挂 起 + 等待 + 超时 */
#define OS_TASK_STATE_DEL (OS_STATE)(255u)
```

#### 任务控制块TCB

```c
struct os_tcb{
    ...
	OS_STATE TaskState;
	
#if OS_CFG_TASK_SUSPEND_EN > 0u
	/* 任务挂起函数 OSTaskSuspend() 计数器 */
	OS_NESTING_CTR SuspendCtr;
#endif
}

```

- 任务挂起计数器,任务每被挂起一次,`SuspendCtr`递增一次,一个任务挂起多少次就要被恢复多少次才能重新运行.

#### 任务挂起和恢复函数

- `OSTaskSuspend()`函数

  ```c
  //任务挂起函数
  #if OS_CFG_TASK_SUSPEND_EN > 0u
  void OSTaskSuspend(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC();
  #if 0/* 屏蔽开始 */
  #ifdef OS_SAFETY_CRITICAL
  	/* 安全检查,OS_SAFETY_CRITICAL_EXCEPTION() 函数需要用户自行编写 */
  	if (p_err == (OS_ERR *)0) {
  		OS_SAFETY_CRITICAL_EXCEPTION();
  		return;
  	}
  #endif
  	
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	/* 不能在 ISR 程序中调用该函数 */
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) {
  		*p_err = OS_ERR_TASK_SUSPEND_ISR;
  		return;
  	}
  #endif
  	
  	/* 不能挂起空闲任务 */
  	if (p_tcb == &OSIdleTaskTCB) {
  		*p_err = OS_ERR_TASK_SUSPEND_IDLE;
  		return;
  	}
  	
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u
  	/* 不能挂起中断处理任务 */
  	if (p_tcb == &OSIntQTaskTCB) {
  		*p_err = OS_ERR_TASK_SUSPEND_INT_HANDLER;
  		return;
  	}
  #endif
  	
  #endif/* 屏蔽结束 */
  	
  	CPU_CRITICAL_ENTER();
  	
  	/* 是否挂起自己 */
  	if (p_tcb == (OS_TCB *)0) {
  		p_tcb = OSTCBCurPtr;
  	}
  	
  	if (p_tcb == OSTCBCurPtr) {
  		/* 如果调度器锁住则不能挂起自己 */
  		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {
  			CPU_CRITICAL_EXIT();
  			*p_err = OS_ERR_SCHED_LOCKED;
  			return;
  		}
  	}
  	
  	*p_err = OS_ERR_NONE;
  	
  	/* 根据任务的状态来决定挂起的动作 */
  	switch (p_tcb->TaskState) {
  		case OS_TASK_STATE_RDY:
  			OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT();
  			p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
  			OS_RdyListRemove(p_tcb);
  			OS_CRITICAL_EXIT_NO_SCHED();
  			break;
  		case OS_TASK_STATE_DLY:
  			p_tcb->TaskState = OS_TASK_STATE_DLY_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
  			CPU_CRITICAL_EXIT();
  			break;
  		case OS_TASK_STATE_PEND:
  			p_tcb->TaskState = OS_TASK_STATE_PEND_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
  			CPU_CRITICAL_EXIT();
  			break;
  		case OS_TASK_STATE_PEND_TIMEOUT:
  			p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
  			CPU_CRITICAL_EXIT();
  			break;
  		case OS_TASK_STATE_SUSPENDED:
  		case OS_TASK_STATE_DLY_SUSPENDED:
  		case OS_TASK_STATE_PEND_SUSPENDED:
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
  			p_tcb->SuspendCtr++;
  			CPU_CRITICAL_EXIT();
  			break;
  		default:
  			CPU_CRITICAL_EXIT();
  			*p_err = OS_ERR_STATE_INVALID;
  			return;
  	}
  	
  	/* 任务切换 */
  	OSSched();
  }
  #endif
  
  ```

  - 这部分代码是为了程序的健壮性写的代码,加了各种判断,避免用户的误操作.

- OSTaskResume()函数

  ```c
  //恢复被挂起的函数(除了自己)
  #if OS_CFG_TASK_SUSPEND_EN > 0u
  void OSTaskResume(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC();
  #if 0/* 屏蔽开始 */
  #ifdef OS_SAFETY_CRITICAL
  	/* 安全检查,OS_SAFETY_CRITICAL_EXCEPTION() 函数需要用户自行编写 */
  	if (p_err == (OS_ERR *)0) {
  		OS_SAFETY_CRITICAL_EXCEPTION();
  		return;
  	}
  #endif
  	
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	/* 不能在 ISR 程序中调用该函数 */
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) {
  		*p_err = OS_ERR_TASK_RESUME_ISR;
  		return;
  	}
  #endif
      
  	CPU_CRITICAL_ENTER();
  #if OS_CFG_ARG_CHK_EN > 0u
  	/* 不能自己恢复自己 */
  	if ((p_tcb == (OS_TCB *)0)||(p_tcb == OSTCBCurPtr)) {
  		CPU_CRITICAL_EXIT();
  		*p_err = OS_ERR_TASK_RESUME_SELF;
  		return;
  	}
  #endif
  	
  #endif/* 屏蔽结束 */
      
  	*p_err = OS_ERR_NONE;
  	/* 根据任务的状态来决定挂起的动作 */
  	switch (p_tcb->TaskState) {
          case OS_TASK_STATE_RDY:
          case OS_TASK_STATE_DLY:
          case OS_TASK_STATE_PEND:
          case OS_TASK_STATE_PEND_TIMEOUT:
              CPU_CRITICAL_EXIT();
              *p_err = OS_ERR_TASK_NOT_SUSPENDED;
              break;
          case OS_TASK_STATE_SUSPENDED:
              OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT();
              p_tcb->SuspendCtr--;
              if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                  p_tcb->TaskState = OS_TASK_STATE_RDY;
                  OS_TaskRdy(p_tcb);
              }
              OS_CRITICAL_EXIT_NO_SCHED();
              break;
          case OS_TASK_STATE_DLY_SUSPENDED:
              p_tcb->SuspendCtr--;
              if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                  p_tcb->TaskState = OS_TASK_STATE_DLY;
              }
              CPU_CRITICAL_EXIT();
              break;
          case OS_TASK_STATE_PEND_SUSPENDED:
              p_tcb->SuspendCtr--;
              if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                  p_tcb->TaskState = OS_TASK_STATE_PEND;
              }
              CPU_CRITICAL_EXIT();
              break;
          case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
              p_tcb->SuspendCtr--;
              if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                  p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;
              }
              CPU_CRITICAL_EXIT();
              break;
          default:
              CPU_CRITICAL_EXIT();
              *p_err = OS_ERR_STATE_INVALID;
              return;
  	}
  	
  	/* 任务切换 */
  	OSSched();
  }
  #endif
  
  ```

  - 这部分代码是为了程序的健壮性写的代码,加了各种判断,避免用户的误操作.

## 任务的删除

### μC/OS如何实现任务的删除

#### 任务删除函数

- OSTaskDel()函数

  ```c
  //删除一个指定的任务(包括自身)
  #if OS_CFG_TASK_DEL_EN > 0u
  void OSTaskDel(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC();
  	/* 不允许删除空闲任务 */
      if (p_tcb == &OSIdleTaskTCB) {
  		*p_err = OS_ERR_TASK_DEL_IDLE;
  		return;
  	}
  	/* 删除自己 */
  	if (p_tcb == (OS_TCB *)0) {
  		CPU_CRITICAL_ENTER();
  		p_tcb = OSTCBCurPtr;
  		CPU_CRITICAL_EXIT();
  	}
  	
  	OS_CRITICAL_ENTER();
  	
  	/* 根据任务的状态来决定删除的动作 */
  	switch (p_tcb->TaskState) {
  		case OS_TASK_STATE_RDY:
  			OS_RdyListRemove(p_tcb);
  			break;
  		case OS_TASK_STATE_SUSPENDED:
  			break;
  		/* 任务只是在延时,并没有在任何等待列表 */
  		case OS_TASK_STATE_DLY:
  		case OS_TASK_STATE_DLY_SUSPENDED:
  			OS_TickListRemove(p_tcb);
  			break;
  		case OS_TASK_STATE_PEND:
  		case OS_TASK_STATE_PEND_SUSPENDED:
  		case OS_TASK_STATE_PEND_TIMEOUT:
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
  			OS_TickListRemove(p_tcb);
              
  #if 0/* 目前我们还没有实现等待列表,暂时先把这部分代码注释 */
  	/* 看看在等待什么 */
              switch (p_tcb->PendOn) {
                  case OS_TASK_PEND_ON_NOTHING:
              /* 任务信号量和队列没有等待队列,直接退出 */
              case OS_TASK_PEND_ON_TASK_Q:
              case OS_TASK_PEND_ON_TASK_SEM:
                  break;
              /* 从等待列表移除 */
              case OS_TASK_PEND_ON_FLAG:
              case OS_TASK_PEND_ON_MULTI:
              case OS_TASK_PEND_ON_MUTEX:
              case OS_TASK_PEND_ON_Q:
              case OS_TASK_PEND_ON_SEM:
                  OS_PendListRemove(p_tcb);
                  break;
              default:
                  break;
              }
  			break;
  #endif
  		default:
  			OS_CRITICAL_EXIT();
  			*p_err = OS_ERR_STATE_INVALID;
  			return;
  	}
  	
  	/* 初始化 TCB 为默认值 */
  	OS_TaskInitTCB(p_tcb);
  	/* 修改任务的状态为删除态,即处于休眠 */
  	p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;
      
  	OS_CRITICAL_EXIT_NO_SCHED();
  	/* 任务切换,寻找最高优先级的任务 */
  	OSSched();
  	
  	*p_err = OS_ERR_NONE;
  }
  #endif/* OS_CFG_TASK_DEL_EN > 0u */
  
  ```

  - 任务删除是一个可选功能,由`OS_CFG_TASK_DEL_EN`控制(os_cfg.h).
  - 空闲任务不能被删除.系统必须至少有一个任务在运行,当没有其他用户任务运行的时候,系统就会运行空闲任务.

## 任务管理

### 任务管理的基本概念

- 从系统的角度看,任务是竞争系统资源的最小运行单元.μC/OS是一个支持多任务的操作系统.在μC/OS中,任务可以使用或等待CPU,使用内存空间等系统资源,并独立于其他任务运行,任何数量的任务可以共享同一个优先级,处于就绪态的多个相同优先级任务将会以时间片切换的方式共享处理器.
- μC/OS的任务可认为是一系列独立任务的集合,每个任务在自己的环境中运行.在任何时刻,只有一个任务得到运行,μC/OS调度器决定运行哪个任务.调度器会不断的启动,轮换每一个任务,宏观看上去所有的任务都在同时在执行.任务不需要对调度器的活动有所了解,在任务切入切出时保存上下文环境(寄存器值,栈内容)是调度器主要的职责.为了实现这点,μC/OS中每个任务都需要有自己的栈空间.当任务切出时,它的执行环境会被保存在该任务的栈空间中,这样当任务再次运行时,就能从栈中正确的恢复上次的运行环境,任务越多,需要的栈空间就越大,而一个系统能运行多少个任务,取决于系统的可用的SRAM.
- μC/OS的可以给用户提供多个任务单独享有独立的栈空间.系统可用决定任务的状态,决定任务是否可以运行,同时还能运用内核的IPC通信资源,实现了任务之间的通信,帮助用户管理业务程序流程.这样用户可以将更多的精力投入到业务功能的实现中.
- μC/OS中的任务是抢占式调度机制,高优先级的任务可打断低优先级任务,低优先级任务必须在高优先级任务阻塞或结束后才能得到调度.同时μC/OS也支持时间片轮转调度方式,只不过时间片的调度是不允许抢占任务的CPU使用权.
- 任务通常会运行在一个死循环中,也不会退出,如果一个任务不再需要,可以调用μC/OS中的任务删除API函数接口显式地将其删除.

### 任务调度器的基本概念

- μC/OS中提供的任务调度器是基于优先级的全抢占式调度:在系统中除了中断处理函数,调度器上锁部分的代码和禁止中断的代码是不可抢占的之外,系统的其他部分都是可以抢占的.系统理论上可以支持无数个优先级 (0-N,优先级数值越大的任务优先级越低,`(OS_CFG_PRIO_MAX - 1u)`为最低优先级,分配给空闲任务使用,一般不建议用户来使用这个优先级.一般系统默认的最大可用优先级数目为 32.在一些资源比较紧张的系统中,用户可以根据实际情况选择只支持8个或自定义个数优先级的系统配置.在系统中,当有比当前任务优先级更高的任务就绪时,当前任务将立刻被切出,高优先级任务抢占处理器运行.
- 一个操作系统如果只是具备了高优先级任务能够"立即"获得处理器并得到执行的特点,那么它仍然不算是实时操作系统.因为这个查找最高优先级任务的过程决定了调度时间是否具有确定性,例如一个包含n个就绪任务的系统中,如果仅仅从头找到尾,那么这个时间将直接和n相关,而下一个就绪任务抉择时间的长短将会极大的影响系统的实时性.
- μC/OS内核中采用两种方法寻找最高优先级的任务,第一种是通用的方法,因为μC/OS防止CPU平台不支持前导零指令,就采用C语言模仿前导零指令的效果实现了快速查找到最高优先级任务的方法.而第二种方法则是特殊方法,利用硬件计算前导零指令CLZ,这样子一次就能知道哪一个优先级任务能够运行,这种调度算法比普通方法更快捷,但受限于平台(在STM32中我们就使用这种方法).
- μC/OS内核中也允许创建相同优先级的任务.相同优先级的任务采用时间片轮转方式进行调度(也就是通常说的分时调度器),时间片轮转调度仅在当前系统中无更高优先级就绪任务存在的情况下才有效.为了保证系统的实时性,系统尽最大可能地保证高优先级的任务得以运行.任务调度的原则是一旦任务状态发生了改变,并且当前运行的任务优先级小于优先级队列组中任务最高优先级时,立刻进行任务切换(除非当前系统处于中断处理程序中或禁止任务切换的状态).

### 任务状态迁移

![状态迁移图](.\Image\状态迁移图.png)

- 任务状态迁移图①:创建任务→就绪态(Ready):任务创建完成后进入就绪态,表明任务已准备就绪,随时可以运行,只等待调度器进行调度.

- 任务状态迁移图②:就绪态→运行态(Running):发生任务切换时,就绪列表中最高优先级的任务被执行,从而进入运行态.

- 任务状态迁移图③:运行态→就绪态:有更高优先级任务创建或者恢复后,会发生任务调度,此刻就绪列表中最高优先级任务变为运行态,那么原先运行的任务由运行态变为就绪态,依然在就绪列表中,等待最高优先级的任务运行完毕继续运行原来的任务(此处可以看作CPU使用权被更高优先级的任务抢占了).
- 任务状态迁移图④:运行态→阻塞态(或者称为挂起态Suspended):正在运行的任务发生阻塞(挂起,延时,读信号量等待)时,该任务会从就绪列表中删除,任务状态由运行态变成阻塞态,然后发生任务切换,运行就绪列表中当前最高优先级任务.

- 任务状态迁移图⑤:阻塞态→就绪态:阻塞的任务被恢复后(任务恢复,延时时间超时,读信号量超时或读到信号量等),此时被恢复的任务会被加入就绪列表,从而由阻塞态变成就绪态;如果此时被恢复任务的优先级高于正在运行任务的优先级,则会发生任务切换,将该任务将再次转换任务状态,由就绪态变成运行态.

- 任务状态迁移图⑥⑦⑧:就绪态,阻塞态,运行态→删除态(Delete):任务可以通过调用`OSTaskDel()`API函数都可以将处于任何状态的任务删除,被删除后的任务将不能再次使用,关于任务的资源都会被系统回收.

- 任务状态迁移图⑨:删除态→就绪态:这就是创建任务的过程,一个任务将会从无到有,创建成功的任务可以参与系统的调度.

ps:此处的任务状态只是大致的任务状态而并非μC/OS的所有任务状态.

### μC/OS任务状态

μC/OS系统中的每一任务都有多种运行状态.系统初始化完成后,创建的任务就可以在系统中竞争一定的资源,由内核进行调度.

μC/OS的任务状态通常分为以下几种:

- 就绪`OS_TASK_STATE_RDY`:该任务在就绪列表中,就绪的任务已经具备执行的能力,只等待调度器进行调度,新创建的任务会初始化为就绪态.
- 延时`OS_TASK_STATE_DLY`:该任务处于延时调度状态.
- 等待`OS_TASK_STATE_PEND`:任务调用`OSQPend()`,`OSSemPend()`这类等待函数,系统就会设置一个超时时间让该任务处于等待状态,如果超时时间设置为0,任务的状态,无限期等下去,直到事件发生.如果超时时间为N(N>0),在N个时间内任务等待的事件或信号都没发生,就退出等待状态转为就绪状态.
- 运行`Running`:该状态表明任务正在执行,此时它占用处理器,ΜC/OS调度器选择运行的永远是处于最高优先级的就绪态任务,当任务被运行的一刻,它的任务状态就变成了运行态,其实运行态的任务也是处于就绪列表中的.
- 挂起`OS_TASK_STATE_SUSPENDED`:任务通过调用`OSTaskSuspend()`函数能够挂起自己或其他任务,调用`OSTaskResume()`是使被挂起的任务回复运行的唯一的方法.挂起一任务意味着该任务再被恢复运行以前不能够取得CPU的使用权,类似强行暂停一个任务.
- 延时+挂起`OS_TASK_STATE_DLY_SUSPENDED`:任务先产生一个延时,延时没结束的时候被其他任务挂起,挂起的效果叠加,当且仅当延时结束并且挂起被恢复了,该任务才能够再次运行.
-  等待+挂起`OS_TASK_STATE_PEND_SUSPENDED`:任务先等待一个事件或信号的发生(无限期等待),还没等待到就被其他任务挂起,挂起的效果叠加,当且仅当任务等待到事件或信号并且挂起被恢复了,该任务才能够再次运行.
- 超时等待+挂起`OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED`:任务在指定时间内等待事件或信号的产生,但是任务已经被其他任务挂起.
- 删除`OS_TASK_STATE_DEL`:任务被删除后的状态,任务被删除后将不再运行,除非重新创建任务.

### 常用任务函数

| 函数名称         | 函数作用 |
| ---------------- | -------- |
| OS_TaskSuspend() | 任务挂起 |
| OSTaskResume()   | 任务恢复 |
| OSTaskDel()      | 任务删除 |
| OSTimeDly()      | 任务延时 |
| OSTimeDlyHMSM()  | 任务延时 |

- OS_TaskSuspend()

  被挂起的任务绝不会得到CPU的使用权,不管该任务具有什么优先级.任务可以通过调用 `vTaskSuspend()`函数都可以将处于任何状态的任务挂起,被挂起的任务得不到CPU的使用权,也不会参与调度,它相对于调度器而言是不可见的,除非它从挂起态中解除.任务挂起是我们经常使用的一个函数,想要使用的就必须将宏定义`OS_CFG_TASK_SUSPEND_EN`启用,这样在编译的时候才会包含`OS_TaskSuspend()`函数.

  ```c
  #if OS_CFG_TASK_SUSPEND_EN > 0u//如果启用了函数 OSTaskSuspend()
  void OS_TaskSuspend(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC();//使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  	//SR(临界段关中断只需保存SR),开中断时将该值还原.
  	CPU_CRITICAL_ENTER(); //关中断
  	if (p_tcb == (OS_TCB *)0) {
          //如果 p_tcb 为空
  		p_tcb = OSTCBCurPtr; //挂起自身
  	}
  	
  	if (p_tcb == OSTCBCurPtr) {
          //如果是挂起自身
  		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  			return; //返回,停止执行
  		}
  	}
      *p_err = OS_ERR_NONE; //错误类型为"无错误"
  	switch (p_tcb->TaskState) {//根据 p_tcb 的任务状态分类处理
  		case OS_TASK_STATE_RDY://如果是就绪状态
  			OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,重开中断
  			p_tcb->TaskState = OS_TASK_STATE_SUSPENDED; //任务状态改为"挂起状态"
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1; //挂起前套数为 1
  			OS_RdyListRemove(p_tcb); //将任务从就绪列表移除
  			OS_CRITICAL_EXIT_NO_SCHED(); //开调度器,不进行调度
  			break; //跳出
  		
          case OS_TASK_STATE_DLY: (7)//如果是延时状态将改为"延时中被挂起"
  			p_tcb->TaskState = OS_TASK_STATE_DLY_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1; //挂起前套数为 1
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		case OS_TASK_STATE_PEND://如果是无期限等待状态将改为"无期限等待中被挂起"
  			p_tcb->TaskState = OS_TASK_STATE_PEND_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1; //挂起前套数为 1
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		case OS_TASK_STATE_PEND_TIMEOUT://如果是有期限等待将改为"有期限等待中被挂起"
  			p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED;
  			p_tcb->SuspendCtr = (OS_NESTING_CTR)1; //挂起前套数为 1
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		case OS_TASK_STATE_SUSPENDED://如果状态中有挂起状态
  		case OS_TASK_STATE_DLY_SUSPENDED:
  		case OS_TASK_STATE_PEND_SUSPENDED:
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
  			p_tcb->SuspendCtr++; //挂起嵌套数加 1
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		default://如果任务状态超出预期
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_STATE_INVALID; //错误类型为"状态非法"
  			return; //返回,停止执行
  	}
  		
  	OSSched();//调度任务
  }
  #endif
  
  ```

  ps:任务可以调用`OS_TaskSuspend()`这个函数来挂起任务自身,但是在挂起自身的时候会进行一次任务上下文切换,需要挂起自身就将任务控制块指针设置为NULL或0传递进来即可.无论任务是什么状态都可以被挂起,只要调用了`OS_TaskSuspend()`这个函数就会挂起成功,不论是挂起其他任务还是挂起任务自身.任务的挂起与恢复函数在很多时候都是很有用的,比如我们想暂停某个任务运行一段时间,但是我们又需要在其恢复的时候继续工作,那么删除任务是不可能的,因为删除了任务的话,任务的所有的信息都是不可能恢复的了,删除是完完全全删除了,里面的资源都被系统释放掉,但是挂起任务就不会这样子,调用挂起任务函数,仅仅是将任务进入挂起态,其内部的资源都会保留下来,同时也不会参与系统中任务的调度,当调用恢复函数的时候,整个任务立即从挂起态进入就绪态,并且参与任务的调度,如果该任务的优先级是当前就绪态优先级最高的任务,那么立即会按照挂起前的任务状态继续执行该任务,从而达到我们需要的效果,注意,是继续执行,也就是说,挂起任务之前是什么状态,都会被系统保留下来,在恢复的瞬间,继续执行.这个任务函数的使用方法是很简单的,只需把任务句柄传递进来即可,`OS_TaskSuspend()`会根据任务句柄的信息将对应的任务挂起,任务的设计要点.

- OSTaskResume()函数

  任务恢复就是让挂起的任务重新进入就绪状态,恢复的任务会保留挂起前的状态信息,在恢复的时候根据挂起时的状态继续运行.如果被恢复任务在所有就绪态任务中,处于最高优先级列表的第一位,那么系统将进行任务上下文的切换.

  ```c
  #if OS_CFG_TASK_SUSPEND_EN > 0u//如果启用了函数 OSTaskResume()
  void OSTaskResume(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
  	#ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) { //如果 p_err 为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  	//如果禁用了中断延迟发布和中断中非法调用检测
  #if (OS_CFG_ISR_POST_DEFERRED_EN == 0u)&&(OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u)
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果在中断中调用该函数
  		*p_err = OS_ERR_TASK_RESUME_ISR; //错误类型为"在中断中恢复任务"
  		return; //返回,停止执行
  	}
  #endif
  	CPU_CRITICAL_ENTER(); //关中断
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if ((p_tcb == (OS_TCB *)0)||(p_tcb == OSTCBCurPtr)) { //如果被恢复任务为空或是自身
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_TASK_RESUME_SELF; //错误类型为"恢复自身"
  		return; //返回,停止执行
  	}
  #endif
  	CPU_CRITICAL_EXIT(); //关中断
  	
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
  		OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_RESUME,
                      (void      *)p_tcb,
                      (void      *)0,
                      (OS_MSG_SIZE)0,
                      (OS_FLAGS   )0,
                      (OS_OPT     )0,
                      (CPU_TS 	)0,
                      (OS_ERR    *)p_err);//把恢复任务命令发布到中断消息队列
  		return; //返回,停止执行
  	}
  #endif
  	/* 如果禁用了中断延迟发布或不是在中断中调用该函数,直接调用`OS_TaskResume()`函数恢复任务*/
  	OS_TaskResume(p_tcb, p_err); //直接将任务 p_tcb 恢复
  }
  #endif
      
  ```

  `OS_TaskResume()`函数

  ```c
  #if OS_CFG_TASK_SUSPEND_EN > 0u//如果启用了函数OSTaskResume()
  void OS_TaskResume(OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
  	CPU_CRITICAL_ENTER(); //关中断
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	switch (p_tcb->TaskState) {//根据 p_tcb 的任务状态分类处理
  		case OS_TASK_STATE_RDY: //如果状态中没有挂起状态
  		case OS_TASK_STATE_DLY:
  		case OS_TASK_STATE_PEND:
  		case OS_TASK_STATE_PEND_TIMEOUT:
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_TASK_NOT_SUSPENDED;//错误类型为"任务未被挂起"
  			break; //跳出
  		case OS_TASK_STATE_SUSPENDED://如果是"挂起状态"
  			OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,重开中断
  			p_tcb->SuspendCtr--;//任务的挂起嵌套数减 1
  			if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) { //如果挂起前套数为 0
  				p_tcb->TaskState = OS_TASK_STATE_RDY; //修改状态为"就绪状态"
  				OS_TaskRdy(p_tcb); //把 p_tcb 插入就绪列表
  			}
  			OS_CRITICAL_EXIT_NO_SCHED(); //开调度器,不调度任务
  			break; //跳出
  		case OS_TASK_STATE_DLY_SUSPENDED://如果是"延时中被挂起"
  			p_tcb->SuspendCtr--; //任务的挂起嵌套数减 1
  			if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) { //如果挂起前套数为 0
  				p_tcb->TaskState = OS_TASK_STATE_DLY; //修改状态为"延时状态"
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		case OS_TASK_STATE_PEND_SUSPENDED://如果是"无期限等待中被挂起"
  			p_tcb->SuspendCtr--; //任务的挂起嵌套数减 1
  			if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) { //如果挂起前套数为 0
  				p_tcb->TaskState = OS_TASK_STATE_PEND; //修改状态为"无期限等待状态"
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:(7)//如果是"有期限等待中被挂起"
  			p_tcb->SuspendCtr--; //任务的挂起嵌套数减 1
  			if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) { //如果挂起前套数为0
  				p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			break; //跳出
  		default: //如果 p_tcb 任务状态超出预期
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_STATE_INVALID; //错误类型为"状态非法"
  			return;//跳出
  	}
  	
  	OSSched();//调度任务
  }
  #endif
  
  ```

  ps:`OSTaskResume()`函数用于恢复挂起的任务.任务在挂起时候调用过多少次的`OS_TaskSuspend()` 函数,那么就需要调用多少次`OSTaskResume()`函数才能将任务恢复运行.

- OSTaskDel()函数

  当一个任务删除另外一个任务时,形参为要删除任务创建时返回的任务句柄,如果是删除自身,则形参为NULL.要想使用该函数必须在os_cfg.h中把`OS_CFG_TASK_DEL_EN`宏定义配置为1,删除的任务将从所有就绪,阻塞,挂起和事件列表中删除.

  ```c
  #if OS_CFG_TASK_DEL_EN > 0u//如果启用了函数 OSTaskDel()
  void OSTaskDel (OS_TCB *p_tcb,OS_ERR *p_err){
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) { //如果 p_err 为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  	
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u(1)//如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
  		*p_err = OS_ERR_TASK_DEL_ISR; //错误类型为"在中断中删除任务"
  		return; //返回,停止执行
  	}
  #endif
  	if (p_tcb == &OSIdleTaskTCB) {//如果目标任务是空闲任务
  		*p_err = OS_ERR_TASK_DEL_IDLE; //错误类型为"删除空闲任务"
  		return; //返回,停止执行
  	}
  	
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
  	if (p_tcb == &OSIntQTaskTCB) { //如果目标任务是中断延迟提交任务
  		*p_err = OS_ERR_TASK_DEL_INVALID; //错误类型为"非法删除任务"
  		return; //返回,停止执行
  	}
  #endif
  	
  	if (p_tcb == (OS_TCB *)0) {//如果 p_tcb 为空
  		CPU_CRITICAL_ENTER(); //关中断
  		p_tcb = OSTCBCurPtr; //目标任务设为自身
  		CPU_CRITICAL_EXIT(); //开中断
  	}
  	
  	OS_CRITICAL_ENTER(); //进入临界段
  	switch (p_tcb->TaskState) { //根据目标任务的任务状态分类处理
  		case OS_TASK_STATE_RDY: //如果是就绪状态
  			OS_RdyListRemove(p_tcb); //将任务从就绪列表移除
  			break; //跳出
          case OS_TASK_STATE_SUSPENDED: //如果是挂起状态
  			break; //直接跳出
  		case OS_TASK_STATE_DLY: //如果包含延时状态
  		case OS_TASK_STATE_DLY_SUSPENDED:
  			OS_TickListRemove(p_tcb); //将任务从节拍列表移除
              break; //跳出
  		case OS_TASK_STATE_PEND: //如果包含等待状态
  		case OS_TASK_STATE_PEND_SUSPENDED:
  		case OS_TASK_STATE_PEND_TIMEOUT:
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
  			OS_TickListRemove(p_tcb);//将任务从节拍列表移除
  			switch (p_tcb->PendOn) { //根据任务的等待对象分类处理
  				case OS_TASK_PEND_ON_NOTHING: //如果没在等待内核对象
  				case OS_TASK_PEND_ON_TASK_Q: //如果等待的是任务消息队列
  				case OS_TASK_PEND_ON_TASK_SEM://如果等待的是任务信号量
  					break; //直接跳出
  				case OS_TASK_PEND_ON_FLAG: //如果等待的是事件
  				case OS_TASK_PEND_ON_MULTI: //如果等待多个内核对象
  				case OS_TASK_PEND_ON_MUTEX: //如果等待的是互斥量
  				case OS_TASK_PEND_ON_Q: //如果等待的是消息队列
  				case OS_TASK_PEND_ON_SEM: //如果等待的是信号量
  					OS_PendListRemove(p_tcb);//将任务从等待列表移除
  					break; //跳出
  				default: //如果等待对象超出预期
  					break; //直接跳出
  			}
  			break; //跳出
  		default: //如果目标任务状态超出预期
  			OS_CRITICAL_EXIT(); //退出临界段
  			*p_err = OS_ERR_STATE_INVALID; //错误类型为"状态非法"
  			return; //返回,停止执行
  	}
  	
  #if OS_CFG_TASK_Q_EN > 0u//如果启用了任务消息队列
  	(void)OS_MsgQFreeAll(&p_tcb->MsgQ); //释放任务的所有任务消息
  #endif
  
      OSTaskDelHook(p_tcb); //调用用户自定义的钩子函数
  
  #if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
  	OS_TLS_TaskDel(p_tcb); /* Call TLSk ␣ ,→ */
  #endif
  
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  	OS_TaskDbgListRemove(p_tcb); //将任务从任务调试双向列表移除
  #endif
  	OSTaskQty--; //任务数目减 1
  	
  	OS_TaskInitTCB(p_tcb); //初始化任务控制块
  	p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;//标定任务已被删除
  	
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	
  	OSSched(); //调度任务
  }
   #endif
  
  ```

  ps:删除任务是说任务将返回并处以删除(休眠)状态,任务的代码不再被μC/OS调用,删除任务不是删除代码,删除任务和挂起任务有些相似,其实有着本质的区别,根本来说,最大的不同就是删除任务队任务控制块的操作,我们知道在任务创建的时候,需要给每个任务分配一个任务控制块,这个任务控制块存储有关这个任务重要的信息,对任务间有至关重要的作用,挂起任务根本不会动任务控制块,但删除任务就会把任务控制块进行初始化,这样子关于任务的任何信息都被抹去.注意了,删除任务并不会释放任务的栈空间.

- `OSTimeDly()`函数

  `OSTimeDly()`函数在任务设计中用的非常之多,每个任务都必须是死循环,并且是必须要有阻塞的情况,否则低优先级的任务就无法被运行了,`OSTimeDly()`函数常用于停止当前任务进行的运行,延时一段时间后再运行.

  ```c
  void OSTimeDly (OS_TICK dly,OS_OPT opt,OS_ERR *p_err){
      CPU_SR_ALLOC();
  	//使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和定义一个局部变
  	//量,用于保存关中断前的 CPU 状态寄存器 SR(临界段关中断只需保存 SR)
      //开中断时将该值还原.
  
  #ifdef OS_SAFETY_CRITICAL(4)//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不执行延时操作
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0u){//如果该延时函数是在中断中被调用
  		*p_err = OS_ERR_TIME_DLY_ISR; //错误类型为"在中断函数中延时"
  		return; //返回,不执行延时操作
  	}
  #endif
  	/* 当调度器被锁时任务不能延时 */
  	if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0u) { //如果调度器被锁
  		*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  		return; //返回,不执行延时操作
  	}
  	
  	switch (opt) {//根据延时选项参数 opt 分类操作
  		case OS_OPT_TIME_DLY: //如果选择相对时间(从现在起延时多长时间)
  		case OS_OPT_TIME_TIMEOUT: //如果选择超时(实际同上)
  		case OS_OPT_TIME_PERIODIC: //如果选择周期性延时
  			if (dly == (OS_TICK)0u) {//如果参数 dly 为 0(0 意味不延时)
  				*p_err = OS_ERR_TIME_ZERO_DLY; //错误类型为"0 延时"
  				return; //返回,不执行延时操作
  			}
  			break;
  		case OS_OPT_TIME_MATCH:
  			//如果选择绝对时间(匹配系统开始运行(OSStart())后的时钟节拍数)
  			break;
  		default://如果选项超出范围
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return; //返回,不执行延时操作
  	}
  	
  	OS_CRITICAL_ENTER(); //进入临界段
  	OSTCBCurPtr->TaskState = OS_TASK_STATE_DLY; //修改当前任务的任务状态为延时状态
  	OS_TickListInsert(OSTCBCurPtr,dly,opt,p_err); //将当前任务插入节拍列表
  	if (*p_err != OS_ERR_NONE) { //如果当前任务插入节拍列表时出现错误
  		OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  		return; //返回,不执行延时操作
  	}
  	OS_RdyListRemove(OSTCBCurPtr); //从就绪列表移除当前任务
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	OSSched(); //任务切换
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  任务延时OPT

  ```c
  #define OS_OPT_TIME_DLY DEF_BIT_NONE
  2 #define OS_OPT_TIME_TIMEOUT ((OS_OPT)DEF_BIT_01)
  3 #define OS_OPT_TIME_MATCH ((OS_OPT)DEF_BIT_02)
  4 #define OS_OPT_TIME_PERIODIC ((OS_OPT)DEF_BIT_03)
  ```

  - `OS_OPT_TIME_DLY`:dly为相对时间,就是从现在起延时多长时间,到时钟节拍总计数 `OSTickCtr = OSTickCtr` 当前+dly时延时结束.
  - `OS_OPT_TIME_TIMEOUT`:跟`OS_OPT_TIME_DLY`的作用情况一样.
  - `OS_OPT_TIME_MATCH`:dly为绝对时间,就是从系统开始运行(调用`OSStart()`)时到节拍总计数`OSTickCtr = dly`时延时结束.
  - `OS_OPT_TIME_PERIODIC`:周期性延时,跟`OS_OPT_TIME_DLY`的作用差不多,如果是长时间延时,该选项更精准一些.

  `OS_TickListInsert()`函数

  ```c
  void OS_TickListInsert (OS_TCB *p_tcb,OS_TICK time,OS_OPT opt,OS_ERR *p_err){
      OS_TICK tick_delta;
  	OS_TICK tick_next;
  	OS_TICK_SPOKE *p_spoke;
  	OS_TCB *p_tcb0;
  	OS_TCB *p_tcb1;
  	OS_TICK_SPOKE_IX spoke;
  	
  	if (opt == OS_OPT_TIME_MATCH){ //如果 time 是个绝对时间
  		tick_delta = time - OSTickCtr - 1u; //计算离到期还有多长时间
          if (tick_delta > OS_TICK_TH_RDY){ //如果延时时间超过了门限
              p_tcb->TickCtrMatch = (OS_TICK )0u; //将任务的时钟节拍的匹配变量置0
              p_tcb->TickRemain = (OS_TICK )0u; //将任务的延时还需时钟节拍数置0
              p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0; //该任务不插入节拍列表
              *p_err = OS_ERR_TIME_ZERO_DLY; //错误类型相当于"0 延时"
              return; //返回,不将任务插入节拍列表
          }
  		p_tcb->TickCtrMatch = time; //任务等待的匹配点为 OSTickCtr = time
  		p_tcb->TickRemain = tick_delta + 1u; //计算任务离到期还有多长时间
  	} else if (time > (OS_TICK)0u){ //如果 time > 0
  		if (opt == OS_OPT_TIME_PERIODIC) { //如果 time 是周期性时间
  			tick_next = p_tcb->TickCtrPrev + time;
  			//计算任务接下来要匹配的时钟节拍总计数
  			tick_delta = tick_next - OSTickCtr - 1u; //计算任务离匹配还有个多长时间
  			if (tick_delta < time){ //如果 p_tcb->TickCtrPrev<OSTickCtr+1
  				p_tcb->TickCtrMatch = tick_next; //将 p_tcb->TickCtrPrev +time 设为时钟节拍匹配点
  			} else { //如果 p_tcb->TickCtrPrev >= OSTickCtr + 1
  				p_tcb->TickCtrMatch = OSTickCtr + time; //将 OSTickCtr +time 设为时钟节拍匹配点
  			}
  			p_tcb->TickRemain = p_tcb->TickCtrMatch - OSTickCtr; //计算任务离到期还有多长时间
  			p_tcb->TickCtrPrev = p_tcb->TickCtrMatch; //保存当前匹配值为下一周期延时用
  		} else { //如果 time 是相对时间
  			p_tcb->TickCtrMatch = OSTickCtr + time; //任务等待的匹配点为 ,OSTickCtr + time
  			p_tcb->TickRemain = time; //计算任务离到期的时间就是 time
  		}
  	
  	} else { //如果 time = 0
  		p_tcb->TickCtrMatch = (OS_TICK )0u; //将任务的时钟节拍的匹配变量置0
  		p_tcb->TickRemain = (OS_TICK )0u; //将任务的延时还需时钟节拍数置 0
  		p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0; //该任务不插入节拍列表
  		*p_err = OS_ERR_TIME_ZERO_DLY; //错误类型为"0 延时"
  		return; //返回,不将任务插入节拍列表
  	}
  	
  	spoke = (OS_TICK_SPOKE_IX)(p_tcb->TickCtrMatch % OSCfg_TickWheelSize);
      //使用哈希算法(取余)来决定任务存于数组
  	p_spoke = &OSCfg_TickWheel[spoke];
  	//OSCfg_TickWheel 的哪个元素(组织一个节拍列表),
  	//与更新节拍列表相对应,可方便查找到期任务.
  	if (p_spoke->NbrEntries == (OS_OBJ_QTY)0u) { //如果当前节拍列表为空
  		p_tcb->TickNextPtr = (OS_TCB *)0;
  		//任务中指向节拍列表中下一个任务的指针置空
  		p_tcb->TickPrevPtr = (OS_TCB *)0;
  		//任务中指向节拍列表中前一个任务的指针置空
  		p_spoke->FirstPtr = p_tcb;
  		//当前任务被列为该节拍列表的第一个任务
  		p_spoke->NbrEntries = (OS_OBJ_QTY)1u; //节拍列表中的元素数目为 1
  	} else { //如果当前节拍列表非空
  		p_tcb1 = p_spoke->FirstPtr; //获取列表中的第一个任务
  		while (p_tcb1 != (OS_TCB *)0) { //如果该任务存在
  			p_tcb1->TickRemain = p_tcb1->TickCtrMatch - OSTickCtr;//计算该任务的剩余等待时间
  			if (p_tcb->TickRemain > p_tcb1->TickRemain) {
  				//如果当前任务的剩余等待时间大于该任务的
  				if (p_tcb1->TickNextPtr != (OS_TCB *)0) {//如果该任务不是列表的最后一个元素
  					p_tcb1 = p_tcb1->TickNextPtr;
  					//让当前任务继续与该任务的下一个任务作比较
  				} else { //如果该任务是列表的最后一个元素
  					p_tcb->TickNextPtr = (OS_TCB *)0; //当前任务为列表的最后一个元素
  					p_tcb->TickPrevPtr = p_tcb1; //该任务是当前任务的前一个元素
  					p_tcb1->TickNextPtr = p_tcb; //当前任务是该任务的后一个元素
  					p_tcb1 = (OS_TCB *)0; //插入完成,退出 while␣ ,→循环
  				}
  			} else { //如果当前任务的剩余等待时间不大于该任务的
  				if (p_tcb1->TickPrevPtr == (OS_TCB *)0) {//如果该任务是列表的第一个元素
  					p_tcb->TickPrevPtr = (OS_TCB *)0; //当前任务就作为列表的第一个元素
  					p_tcb->TickNextPtr = p_tcb1; //该任务是当前任务的后一个元素
  					p_tcb1->TickPrevPtr = p_tcb; //当前任务是该任务的前一个元素
  					p_spoke->FirstPtr = p_tcb; //当前任务是列表的第一个元素
  				} else { //如果该任务也不是是列表的第一个元素
  					p_tcb0 = p_tcb1->TickPrevPtr; // p_tcb0 暂存该任务的前一个任务
  					p_tcb->TickPrevPtr = p_tcb0;
  					//该任务的前一个任务作为当前任务的前一个任务
  					p_tcb->TickNextPtr = p_tcb1; //该任务作为当前任务的后一个任务
  					p_tcb0->TickNextPtr = p_tcb; // p_tcb0 暂存的任务的下一个任务改为当前任务
  					p_tcb1->TickPrevPtr = p_tcb; // 该任务的前一个任务也改为当前任务
  				}
  				p_tcb1 = (OS_TCB *)0; //插入完成,退出 while 循环
  			}
  		}
  	p_spoke->NbrEntries++; //节拍列表中的元素数目加 1
  	} //更新节拍列表的元素数目的最大记录
  	if (p_spoke->NbrEntriesMax < p_spoke->NbrEntries) {
  		p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
  	}
  	p_tcb->TickSpokePtr = p_spoke; //记录当前任务存放于哪个节拍列表
  	*p_err = OS_ERR_NONE;//错误类型为"无错误"
  }
  
  ```

  任务的延时在实际中运用特别多,因为需要暂停一个任务,让任务放弃 CPU,延时结束后再继续运行该任务,如果任务中没有阻塞的话,比该任务优先级低的任务则无法得到 CPU 的使用权,就无法运行

- OSTimeDlyHMSM()函数

  `OSTimeDlyHMSM()`函数与`OSTimeDly()`函数的功能类似,也是用于停止当前任务进行的运行,延时一段时间后再运行,但是`OSTimeDlyHMSM()`函数会更加直观,延时多少个小时,分钟,秒,毫秒.但是,用户若要使用`OSTimeDlyHMSM()`函数,必须将宏 `OS_CFG_TIME_DLY_HMSM_EN`(os_cfg.h)设为1

  ```c
  #if OS_CFG_TIME_DLY_HMSM_EN > 0u//如果启用(默认启用)了OSTimeDlyHMSM()函数
  2 void OSTimeDlyHMSM (CPU_INT16U hours,//小时数
                        CPU_INT16U minutes,//分钟数
                        CPU_INT16U seconds,//秒数
  					  CPU_INT32U milli,//毫秒数
                        OS_OPT opt,//选项
                        OS_ERR *p_err)){
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测功能
  	CPU_BOOLEAN opt_invalid; //声明变量用于参数检测
  	CPU_BOOLEAN opt_non_strict;
  #endif
  	OS_OPT opt_time;
  	OS_RATE_HZ tick_rate;
  	OS_TICK ticks;
  	CPU_SR_ALLOC();
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不执行延时操作
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0u){//如果该延时函数是在中断中被调用
  		*p_err = OS_ERR_TIME_DLY_ISR; //错误类型为"在中断函数中延时"
  		return; //返回,不执行延时操作
  	}
  #endif
  	/* 当调度器被锁时任务不能延时 */
  	if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0u) {//如果调度器被锁
  		*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  		return; //返回,不执行延时操作
  	}
      
  	opt_time = opt & OS_OPT_TIME_MASK; //检测除选项中与延时时间性质有关的位
  	switch (opt_time) { //根据延时选项参数 opt 分类操作
  		case OS_OPT_TIME_DLY: //如果选择相对时间(从现在起延时多长时间)
  		case OS_OPT_TIME_TIMEOUT: //如果选择超时(实际同上)
  		case OS_OPT_TIME_PERIODIC: //如果选择周期性延时
  			if (milli == (CPU_INT32U)0u) { //如果毫秒数为 0
  				if (seconds == (CPU_INT16U)0u) { //如果秒数为 0
  					if (minutes == (CPU_INT16U)0u) { //如果分钟数为 0
  						if (hours == (CPU_INT16U)0u) { //如果小时数为 0
  							*p_err = OS_ERR_TIME_ZERO_DLY; //错误类型为"0 延时"
  							return;//返回,不执行延时操作
  						}
  					}
  				}
  			}
  			break;
  		case OS_OPT_TIME_MATCH:
  			//如果选择绝对时间(把系统开始运行(OSStart() 时做为起点)
  			break;
  		default: //如果选项超出范围
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return; //返回,不执行延时操作
  	}
  
  #if OS_CFG_ARG_CHK_EN > 0u
  	//如果启用(默认启用)了参数检测功能
  	opt_invalid = DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK);
  	//检测除选项位以后其他位是否被置位
  	if (opt_invalid == DEF_YES) {//如果除选项位以后其他位有被置位的
  		*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  		return; //返回,不执行延时操作
  	}
      
  	opt_non_strict = DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT);
      //检测有关时间参数取值范围的选项位
  	if (opt_non_strict != DEF_YES) {//如果选项选择了 OS_OPT_TIME_HMSM_STRICT
  		if (milli > (CPU_INT32U)999u) { //如果毫秒数>999
  			*p_err = OS_ERR_TIME_INVALID_MILLISECONDS; //错误类型为"毫秒数不可用"
  			return; //返回,不执行延时操作
  		}
  		if (seconds > (CPU_INT16U)59u) { //如果秒数>59
  			*p_err = OS_ERR_TIME_INVALID_SECONDS; //错误类型为"秒数不可用"
  			return; //返回,不执行延时操作
  		}
  		if (minutes > (CPU_INT16U)59u) { //如果分钟数>59
  			*p_err = OS_ERR_TIME_INVALID_MINUTES; //错误类型为"分钟数不可用"
  			return; //返回,不执行延时操作
  		}
  		if (hours > (CPU_INT16U)99u) { //如果小时数>99
  			*p_err = OS_ERR_TIME_INVALID_HOURS; //错误类型为"小时数不可用"
  			return; //返回,不执行延时操作
  		}
  	} else { //如果选项选择了 OS_OPT_TIME_HMSM_NON_STRICT
  		if (minutes > (CPU_INT16U)9999u) { //如果分钟数>9999
  			*p_err = OS_ERR_TIME_INVALID_MINUTES; //错误类型为"分钟数不可用"
  			return; //返回,不执行延时操作
  		}
  		if (hours > (CPU_INT16U)999u) {  //如果小时数>999
  			*p_err = OS_ERR_TIME_INVALID_HOURS; //错误类型为"小时数不可用"
  			return; //返回,不执行延时操作
  		}
  	}
  #endif
  
  	/* 将延时时间转换成时钟节拍数 */
  	tick_rate = OSCfg_TickRate_Hz; //获取时钟节拍的频率
  	ticks = ((OS_TICK)hours * (OS_TICK)3600u + (OS_TICK)minutes *
  	(OS_TICK)60u + (OS_TICK)seconds) * tick_rate + (tick_rate * ((OS_TICK)milli + (OS_TICK)500u / tick_rate)) / (OS_TICK)1000u;//将延时时间转换成时钟节拍数
  	
  	if (ticks > (OS_TICK)0u) { //如果延时节拍数>0
  		OS_CRITICAL_ENTER(); //进入临界段
  		OSTCBCurPtr->TaskState = OS_TASK_STATE_DLY; //修改当前任务的任务状态为延时状态
  		OS_TickListInsert(OSTCBCurPtr, //将当前任务插入节拍列表
                            ticks,
                            opt_time,
                            p_err);
  		if(*p_err != OS_ERR_NONE) { //如果当前任务插入节拍列表时出现错误
  			OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  			return; //返回,不执行延时操作
  		}
  		OS_RdyListRemove(OSTCBCurPtr); //从就绪列表移除当前任务
  		OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  		OSSched(); //任务切换
  		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	} else { //如果延时节拍数 =0
  	*p_err = OS_ERR_TIME_ZERO_DLY; //错误类型为"0 延时"
  	}
  }
  #endif
  
  ```

  任务延时的可选选项opt

  ```c
  #define OS_OPT_TIME_DLY DEF_BIT_NONE
  2 #define OS_OPT_TIME_TIMEOUT ((OS_OPT)DEF_BIT_01)
  3 #define OS_OPT_TIME_MATCH ((OS_OPT)DEF_BIT_02)
  4 #define OS_OPT_TIME_PERIODIC ((OS_OPT)DEF_BIT_03)
  56 #define OS_OPT_TIME_HMSM_STRICT ((OS_OPT)DEF_BIT_NONE)
  7 #define OS_OPT_TIME_HMSM_NON_STRICT ((OS_OPT)DEF_BIT_04)
  ```

  - `OS_OPT_TIME_DLY`:dly为相对时间,就是从现在起延时多长时间,到时钟节拍总计数 `OSTickCtr = OSTickCtr`当前+dly时延时结束.

  - `OS_OPT_TIME_TIMEOUT`:跟`OS_OPT_TIME_DLY`的作用情况一样.

  - `OS_OPT_TIME_MATCH`:dly为绝对时间,就是从系统开始运行(调用`OSStart()`)时到节拍总计数`OSTickCtr = dly`时延时结束.

  - `OS_OPT_TIME_PERIODIC`:周期性延时, 跟`OS_OPT_TIME_DLY`的作用差不多,如果是长时间延时,该选项更精准一些.

  - 延时时间取值比较严格:

    **–** 小时数hours:(0-99)

    **–** 分钟数minutes:(0-59)

    **–** 秒数seconds:(0-59)

    **–** 毫秒数milliseconds:(0-999)

  - 延时时间取值比较宽松.

    **–** 小时数hours:(0-999)

    **–** 分钟数minutes:(0-9999)

    **–** 秒数seconds:(0-65535)

    **–** 毫秒数milliseconds:(0-4294967295)

### 任务的设计要点

- 我们要对自己设计的嵌入式系统要了如指掌,任务的优先级信息,任务与中断的处理,任务的运行时间,逻辑,状态等都要知道,才能设计出好的系统,所以,在设计的时候需要根据需求制定框架.在设计之初就应该考虑任务运行的上下文环境,任务的执行时间的合理性.

- μC/OS中程序运行的上下文包括:

  - 中断服务函数.
  - 普通任务.
  - 空闲任务.

  1. 中断服务函数

     中断服务函数是一种需要特别注意的上下文环境,它运行在非任务的执行环境下(一般为芯片的一种特殊运行模式(也被称作特权模式)),在这个上下文环境中不能使用挂起当前任务的操作,不允许调用任何会阻塞运行的API函数接口.另外需要注意的是,中断服务程序最好保持精简短小,快进快出,一般在中断服务函数中只做标记事件的发生,然后通知任务,让对应任务去执行相关处理,因为中断服务函数的优先级高于任何优先级的任务,如果中断处理时间过长,将会导致整个系统的任务无法正常运行.所以在设计的时候必须考虑中断的频率,中断的处理时间等重要因素,以便配合对应中断处理任务的工作.

     μC/OS支持中断延迟发布,使得原本在中断中发布的信息变成任务级发布,这样子会使得中断服务函数的处理更加快速,屏蔽中断的时间更短,这样子能快速响应其他的中断,真正称得上实时操作系统.

  2. 任务

     任务看似没有什么限制程序执行的因素,似乎所有的操作都可以执行.但是做为一个优先级明确的实时系统,如果一个任务中的程序出现了死循环操作(此处的死循环是指没有阻塞机制的任务循环体),那么比这个任务优先级低的任务都将无法执行,当然也包括了空闲任务,因为死循环的时候,任务不会主动让出CPU,低优先级的任务是不可能得到CPU的使用权的,而高优先级的任务就可以抢占CPU.这个情况在实时操作系统中是必须注意的一点,所以在任务中不允许出现死循环.如果一个任务只有就绪态而无阻塞态,势必会影响到其他低优先级任务的执行,所以在进行任务设计时,就应该保证任务在不活跃的时候,任务可以进入阻塞态以交出CPU使用权,这就需要我们自己明确知道什么情况下让任务进入阻塞态,保证低优先级任务可以正常运行.在实际设计中,一般会将紧急的处理事件的任务优先级设置得高一些.

  3. 空闲任务

     空闲任务(idle 任务)是μC/OS系统中没有其他工作进行时自动进入的系统任务.因为处理器总是需要代码来执行,所以至少要有一个任务处于运行态.μC/OS为了保证这一点,当调用`OSInit()`函数进行系统初始化时,系统会自动创建一个空闲任务,空闲任务是一个非常短小的循环.用户可以通过空闲任务钩子方式,在空闲任务上钩入自己的功能函数.通常这个空闲任务钩子能够完成一些额外的特殊功能,例如系统运行状态的指示,系统省电模式等.空闲任务是唯一一个不允许出现阻塞情况的任务,因为μC/OS需要保证系统永远都有一个可运行的任务.

     对于空闲任务钩子上挂接的空闲钩子函数,它应该满足以下的条件:

     - 永远不会挂起空闲任务.
     - 不应该陷入死循环,需要留出部分时间用于统计系统的运行状态等.

  4. 任务的执行时间

     任务的执行时间一般是指两个方面,一是任务从开始到结束的时间,二是任务的周期.在系统设计的时候这两个时间候我们都需要考虑,例如,对于事件A对应的服务任务Ta,系统要求的实时响应指标是10ms,而,Ta 的最大运行时间是1ms,那么10ms就是任务Ta的周期了,1ms则是任务的运行时间,简单来说任务Ta在10ms内完成对事件A的响应即可.此时,系统中还存在着以50ms为周期的另一任务Tb,它每次运行的最大时间长度是,100us.在这种情况下,即使把任务 Tb的优先级设置比Ta更高,对系统的实时性指标也没什么影响,因为即使在Ta的运行过程中,Tb抢占了Ta 的资源,等到Tb执行完毕,消耗的时间也只不过是100us,还是在事件A规定的响应时间内(10ms),Ta能够安全完成对事件A的响应.但是假如系统中还存在任务Tc,其运行时间为20ms,假如将Tc的优先级设置比Ta更高,那么在Ta运行的时候,突然间被Tc打断,等到Tc 执行完毕,那Ta已经错过对事件A(10ms)的响应了,这是不允许的.所以在我们设计的时候,必须考虑任务的时间,一般来说处理时间更短的任务优先级应设置更高一些.



## 消息队列

### 消息队列的基本概念

- 队列又称消息队列,是一种常用于任务间通信的数据结构,队列可以在任务与任务间,中断和任务间传递信息,实现了任务接收来自其他任务或中断的不固定长度的消息,任务能够从队列里面读取消息,当队列中的消息是空时,读取消息的任务将被阻塞,用户还可以指定阻塞的任务时间timeout,在这段时间中,如果队列为空,该任务将保持阻塞状态以等待队列数据有效.当队列中有新消息时,被阻塞的任务会被唤醒并处理新消息;当等待的时间超过了指定的阻塞时间,即使队列中尚无有效数据,任务也会自动从阻塞态转为就绪态.
- 消息队列是一种异步的通信方式.
- 通过消息队列服务,任务或中断服务程序可以将消息放入消息队列中.同样,一个或多个任务可以从消息队列中获得消息.当有多个消息发送到消息队列时,通常是将先进入消息队列的消息先传给任务,也就是说,任务先得到的是最先进入消息队列的消息,即先进先出原则(FIFO),但是μC/OS也支持后进先出原则(LIFO).

### 消息队列的工作过程

- 在μC/OSIII中定义了一个数组`OSCfg_MsgPool[OS_CFG_MSG_POOL_SIZE]`,因为在使用消息队列的时候存取消息比较频繁,在系统初始化的时候就将这个大数组的各个元素串成单向链表,组成我们说的消息池,而这些元素我们称之为消息,为什么这里是单向链表而不是我们之前在各种列表中看到的双向链表?因为消息的存取并不需要从链表中间,只需在链表的首尾存取即可,单向链表即够用,使用双向链表反而更复杂.消息池的大小`OS_CFG_MSG_POOL_SIZE`(os_cfg_app.h)由用户自己定义.
- 消息队列的处理速度很快,同时共用了资源,系统中所有被创建的队列都可以从消息池中取出消息,挂载到自身的队列上,以表示消息队列拥有消息,当消息使用完毕,则又会被释放回到消息池中,其他队列也可以从中取出消息,这样子的消息资源是能被系统所有的消息队列反复使用.

#### 消息池初始化

- 在运行到`OSInit()`系统初始化函数时,系统就会将消息池进行初始化,其中`OS_MsgPoolInit()`(定义在os_msg.c中)就是消息池初始化函数.

  ```c
  void OS_MsgPoolInit(OS_ERR *p_err){
      OS_MSG *p_msg1;
      OS_MSG *p_msg2;
      OS_MSG_QTY i;
      OS_MSG_QTY loops;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if (OSCfg_MsgPoolBasePtr == (OS_MSG *)0) {//如果消息池不存在
  		*p_err = OS_ERR_MSG_POOL_NULL_PTR; //错误类型为"消息池指针为空"
  		return; //返回,停止执行
  	}
  	if (OSCfg_MsgPoolSize == (OS_MSG_QTY)0) { //如果消息池不能存放消息
  		*p_err = OS_ERR_MSG_POOL_EMPTY; //错误类型为"消息池为空"
  		return; //返回,停止执行
  	}
  #endif
  /* 将消息池里的消息逐条串成单向链表,方便管理 */
      p_msg1 = OSCfg_MsgPoolBasePtr;
      p_msg2 = OSCfg_MsgPoolBasePtr;
      p_msg2++;
      loops = OSCfg_MsgPoolSize - 1u;
      for (i = 0u; i < loops; i++) { //初始化每一条消息
          p_msg1->NextPtr = p_msg2;
          p_msg1->MsgPtr = (void *)0;
          p_msg1->MsgSize = (OS_MSG_SIZE)0u;
          p_msg1->MsgTS = (CPU_TS )0u;
          p_msg1++;
          p_msg2++;
      }
      p_msg1->NextPtr = (OS_MSG *)0; //最后一条消息
      p_msg1->MsgPtr = (void *)0;
      p_msg1->MsgSize = (OS_MSG_SIZE)0u;
      p_msg1->MsgTS = (CPU_TS )0u;
      /* 初始化消息池数据 */
      OSMsgPool.NextPtr = OSCfg_MsgPoolBasePtr;
      OSMsgPool.NbrFree = OSCfg_MsgPoolSize;
      OSMsgPool.NbrUsed = (OS_MSG_QTY)0;
      OSMsgPool.NbrUsedMax = (OS_MSG_QTY)0;
      *p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  - 如果启用了安全检测`OS_SAFETY_CRITICAL`这个宏定义,那么在编译代码的时候就会包含安全检测,如果p_err的指针为空,系统会执行安全检测异常函数`OS_SAFETY_CRITICAL_EXCEPTION()`,然后退出.

  - 如果启用了参数检测`OS_CFG_ARG_CHK_EN`这个宏定义,那么在编译代码的时候会包含参数检测,如果消息池不存在,系统会返回错误类型为"消息池指针为空"的错误代码,然后退出,不执行初始化操作;如果消息池不能存放消息,系统会返回错误类型为"消息池为空"的错误代码,然后退出,也不执行初始化操作.

  - 每个消息有四个元素

    1. NextPtr:指向下一个可用的消息.
    2. MsgPtr:指向实际的消息.
    3.  MsgSize:记录消息的大小(以字节为单位).
    4. MsgTS:记录发送消息时的时间戳.

    ![OS_MSG](.\Image\OS_MSG.png)
    
  - OSMsgPool是个全局变量,用来管理内存池的存取操作,它包含以下四个元素.
  
    1. NextPtr :指向下一个可用的消息.
    2. NbrFree :记录消息池中可用的消息个数.
    3. NbrUsed:记录已用的消息个数.
    4.  NbrUsedMax:记录使用的消息峰值数量.
  
    ![OSMsgPool](.\Image\OSMsgPool.png)

#### 消息队列的运作机制

- μC/OS的消息队列控制块由多个元素组成,当消息队列被创建时,编译器会静态为消息队列分配对应的内存空间(因为我们需要自己定义一个消息队列控制块),用于保存消息队列的一些信息如队列的名字,队列可用的最大消息个数,入队指针,出队指针等.在创建成功的时候,这些内存就被占用了,创建队列的时候用户指定队列的最大消息个数,无法再次更改,每个消息空间可以存放任意类型的数据.
- 任务或者中断服务程序都可以给消息队列发送消息,当发送消息时,如果队列未满,μC/OS会将从消息池中取出一个消息,将消息挂载到队列的尾部,消息中的成员变量`MsgPtr`指向要发送的消息.如果队列已满,则返回错误代码,入队失败.
- μC/OS还支持发送紧急消息,也就是我们所说的后进先出(LIFO排队,其过程与发送消息几乎一样,唯一的不同是,当发送紧急消息时,发送的消息会挂载到队列的队头而非队尾,这样,接收者就能够优先接收到紧急消息,从而及时进行消息处理.
- 当某个任务试图读一个队列时,可以指定一个阻塞超时时间.在这段时间中,如果队列为空,该任务将保持阻塞状态以等待队列数据有效.当其他任务或中断服务程序往其等待的队列中写入了数据,该任务将自动由阻塞态转移为就绪态.当等待的时间超过了指定的阻塞时间,即使队列中尚无有效数据,任务也会自动从阻塞态转移为就绪态.
- 当消息队列不再被使用时,可以对它进行删除操作,一旦删除操作完成,消息队列将被永久性的删除,所有关于队列的信息会被清空,知道再次创建才可使用.

![消息队列的运作过程](.\Image\消息队列的运作过程.png)

### 消息队列的阻塞机制

- 我们使用的消息队列一般不是属于某个任务的队列,在很多时候,我们创建的队列,是每个任务都可以去对他进行读写操作的,但是为了保护每个任务对它进行读操作的过程(μC/OS队列的写操作是没有阻塞的),我们必须要有阻塞机制,在某个任务对它读操作的时候,必须保证该任务能正常完成读操作,而不受后来的任务干扰.
- μC/OS消息队列这种先来后到的机制,每个对消息队列读的函数,都有这种机制,我称之为阻塞机制.假设有一个任务A对某个队列进行读操作的时候(也就是我们所说的出队),发现它没有消息,那么此时任务A有3个选择:第一个选择,任务A扭头就走,既然队列没有消息,那我也不等了,干其他事情去,这样子任务A不会进入阻塞态;第二个选择,任务A还是在这里等等吧,可能过一会队列就有消息,此时任务A会进入阻塞状态,在等待着消息的道来,而任务A的等待时间就由我们自己定义,比如设置1000个系统时钟节拍tick的等待,在这1000个tick到来之前任务A都是处于阻塞态,当阻塞的这段时间任务A等到了队列的消息,那么任务A就会从阻塞态变成就绪态,如果此时任务A比当前运行的任务优先级还高,那么,任务A就会得到消息并且运行;假如 1000个tick都过去了,队列还没消息,那任务A就不等了,从阻塞态中唤醒,返回一个没等到消息的错误代码,然后继续执行任务A的其他代码;第三个选择,任务A死等,不等到消息就不走了,这样子任务A就会进入阻塞态,直到完成读取队列的消息.
- 假如有多个任务阻塞在一个消息队列中,那么这些阻塞的任务将按照任务优先级进行排序,优先级高的任务将优先获得队列的访问权.
- 如果发送消息的时候用户选择广播消息,那么在等待中的任务都会收到一样的消息.

### 消息队列的应用场景

- 消息队列可以应用于发送不定长消息的场合,包括任务与任务间的消息交换,队列是μC/OS中任务与任务间,中断与任务间主要的通讯方式,发送到队列的消息是通过引用方式实现的,这意味着队列存储的是数据的地址,我们可以通过这个地址将这个数据读取出来,这样子,无论数据量是多大,其操作时间都是一定的,只是一个指向数据地址指针.

### 消息队列的结构

- μC/OS的消息队列由多个元素组成,在信号量被创建时,需要由我们自己定义消息队列(也可以称之为消息队列句柄),因为它是用于保存消息队列的一些信息的,其数据结构OS_Q除了队列必须的一些基本信息外,还有PendList链表与MsgQ,为的是方便系统来管理消息队列.![消息队列的结构](.\Image\消息队列的结构.png)

- ```c
  struct os_q{
  /* ------------------ GENERIC MEMBERS ------------------ */
      OS_OBJ_TYPE Type;
      CPU_CHAR *NamePtr;
      OS_PEND_LIST PendList;
  #if OS_CFG_DBG_EN > 0u
      OS_Q *DbgPrevPtr;
      OS_Q *DbgNextPtr;
      CPU_CHAR *DbgNamePtr;
  #endif
  /* ------------------ SPECIFIC MEMBERS ------------------ */
      OS_MSG_Q MsgQ;
  };
  
  ```

  1. `Type`:消息队列的类型.
  2. `*NamePtr`:消息队列的名字.
  3. `PendList`:等待消息队列的任务列表.
  4. `MsgQ`:消息列表.

- 消息队列`OS_MSG_Q`的结构

  ```c
  struct os_msg_q{ /* OS_MSG_Q */
      OS_MSG *InPtr;/* 指向要插入队列的下一个 OS_MSG 的指针 */
      OS_MSG *OutPtr;/* 指向要从队列中提取的下一个 OS_MSG 的指针
      */
      OS_MSG_QTY NbrEntriesSize;/* 队列中允许的最大消息个数 */
      OS_MSG_QTY NbrEntries;/* 队列中当前的消息个数 */
      OS_MSG_QTY NbrEntriesMax;/* 队列中的消息个数峰值 */
  };
  
  ```

  1. `*InPtr`,`*OutPtr`:队列中消息也是用单向链表串联起来的,但存取消息不像消息池只是从固定的一端.队列存取消息有两种方式,一种是 FIFO模式,即先进先出,这个时候消息的存取是在单向链表的两端,一个头一个尾,存取位置可能不一样就产生了这两个输入指针和输出指针,具体见图FIFO模式.另一种是LIFO模式,后进先出,这个时候消息的存取都是在单向链表的一端,仅仅用OutPtr就足够指示存取的位置,具体见图LIFO模式.当队列中已经存在比较多的消息没有处理,这个时候有个紧急的消息需要马上传送到其他任务去的时候就可以在发布消息的时候选择LIFO模式.
  2. `NbrEntries`:记录消息队列中当前的消息个数,每发送一个消息,若没有任务在等待该消息队列的消息,那么新发送的消息被插入此消息队列后此值加1,`NbrEntries`的大小不能超过`NbrEntriesSize`.

![消息队列a](.\Image\消息队列a.png)

![消息队列b](.\Image\消息队列b.png)

### 消息队列常用函数

| 函数名称        | 函数作用                                                     |
| --------------- | ------------------------------------------------------------ |
| OSQCreate()     | 创建消息队列                                                 |
| OS_MsgQInit()   | 初始化消息队列                                               |
| OSQDel()        | 删除消息队列                                                 |
| OS_PendObjDel() | 将阻塞在内核对象(如信号量)上的任务从阻塞态恢复               |
| OS_QDel()       | 删除消息队列                                                 |
| OSQPost()       | 给消息队列发送消息                                           |
| OS_QPost()      | 给消息队列发送消息                                           |
| OS_MsgQPut()    | 将消息放入队列中,执行完毕就退出                              |
| OS_Post()       | 在发送消息完成的时候,进行一次任务调度                        |
| OSQPend()       | 获取消息队列                                                 |
| OS_MsgQGet()    | 从消息队列获取一个消息                                       |
| OS_Pend()       | 将当前任务脱离就序列表,并根据用户指定的阻塞时间插入节拍列表和队列等待列表,打开调度器,但不进行调度 |

- 创建消息队列函数OSQCreate()

  使用消息队列必须先声明和创建消息队列,队列是一种数据结构,用于任务间的数据的传递.每创建一个新的队列都需要为其分配RAM,在创建的时候我们需要自己定义一个消息队列结构体,其内存是由编译器自动分配的.

  ```c
  void OSQCreate (OS_Q *p_q, //消息队列指针
  				CPU_CHAR *p_name, //消息队列名称
  				OS_MSG_QTY max_qty, //消息队列大小(不能为 0)
  				OS_ERR *p_err){ //返回错误类型
  	CPU_SR_ALLOC();//使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  //定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  // SR(临界段关中断只需保存 SR),开中断时将该值还原
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  
  #ifdef OS_SAFETY_CRITICAL_IEC61508 //如果启用了安全关键
  	//如果在调用 OSSafetyCriticalStart() 后创建
  	if (OSSafetyCriticalStartFlag == DEF_TRUE) {
  		*p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME; //错误类型为"非法创建内核对象"
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数是在中断中被调用
  		*p_err = OS_ERR_CREATE_ISR; //错误类型为"在中断中创建对象"
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_q == (OS_Q *)0) { //如果 p_q 为空
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"创建对象为空"
  		return; //返回,停止执行
  	}
  	if (max_qty == (OS_MSG_QTY)0) { //如果 max_qty = 0
  		*p_err = OS_ERR_Q_SIZE; //错误类型为"队列空间为 0"
  		return; //返回,停止执行
  	}
  #endif
  
  	OS_CRITICAL_ENTER(); //进入临界段
  	p_q->Type = OS_OBJ_TYPE_Q; //标记创建对象数据结构为消息队列
  	p_q->NamePtr = p_name; //标记消息队列的名称
      OS_MsgQInit(&p_q->MsgQ, //初始化消息队列
      max_qty); 
      OS_PendListInit(&p_q->PendList); //初始化该消息队列的等待列表
  
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
      OS_QDbgListAdd(p_q); //将该队列添加到消息队列双向调试链表
  #endif
      OSQQty++; //消息队列个数加 1
      OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
      *p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  ps:消息队列创建完成的示意图

  ![消息队列创建完成](.\Image\消息队列创建完成.png)

  在创建消息队列的时候,是需要用户自己定义消息队列的句柄的,但是注意了,定义了队列的句柄并不等于创建了队列,创建队列必须是调用消息队列创建函数进行创建,否则,以后根据队列句柄使用消息队列的其他函数的时候会发生错误,用户通过消息队列句柄就可使用消息队列进行发送与获取消息的操作,用户可以根据返回的错误代码进行判断消息队列是否创建成功,消息队列创建函数`OSQCreate()`使用实例具如下

  ```c
  OS_Q queue; //声明消息队列
  OS_ERR err;
  
  /* 创建消息队列 queue */
  OSQCreate ((OS_Q *)&queue, //指向消息队列的指针
  		   (CPU_CHAR *)"Queue For Test", //队列的名字
  		   (OS_MSG_QTY )20, //最多可存放消息的数目
  		   (OS_ERR *)&err); //返回错误类型
  
  ```

  

- 初始化消息队列函数OS_MsgQInit()

  ```c
  void OS_MsgQInit(OS_MSG_Q *p_msg_q, //消息队列指针
  				OS_MSG_QTY size){ //消息队列空间
      p_msg_q->NbrEntriesSize = (OS_MSG_QTY)size; //消息队列可存放消息数目
  	p_msg_q->NbrEntries = (OS_MSG_QTY)0; //消息队列目前可用消息数
  	p_msg_q->NbrEntriesMax = (OS_MSG_QTY)0; //可用消息数的最大历史记录
  	p_msg_q->InPtr = (OS_MSG *)0; //队列的入队指针
  	p_msg_q->OutPtr = (OS_MSG *)0; //队列的出队指针
  }
  
  ```

- 消息队列删除函数OSQDel()

  队列删除函数是根据队列结构(队列句柄)直接删除的,删除之后这个消息队列的所有信息都会被系统清空,而且不能再次使用这个消息队列了,但是需要注意的是,如果某个消息队列没有被定义,那也是无法被删除的.想要使用消息队列删除函数就必须将`OS_CFG_Q_DEL_EN`宏定义配置为1.

  ```c
  #if OS_CFG_Q_DEL_EN > 0u//如果启用了 OSQDel() 函数
  OS_OBJ_QTY OSQDel(OS_Q *p_q,//消息队列指针
     				  OS_OPT opt, //选项
      			  OS_ERR *p_err){ //返回错误类型
      OS_OBJ_QTY cnt;
      OS_OBJ_QTY nbr_tasks;
      OS_PEND_DATA *p_pend_data;
      OS_PEND_LIST *p_pend_list;
      OS_TCB *p_tcb;
   	CPU_TS ts;
  
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  //定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  // SR(临界段关中断只需保存 SR),开中断时将该值还原.
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  }
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
  *p_err = OS_ERR_DEL_ISR; //错误类型为"在中断中中止等待"
  return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  if (p_q == (OS_Q *)0) { //如果 p_q 为空
  *p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"对象为空"
  return ((OS_OBJ_QTY)0u); //返回 0(有错误),停止执行
  }
  switch (opt) { (7)//根据选项分类处理
  case OS_OPT_DEL_NO_PEND: //如果选项在预期内
  case OS_OPT_DEL_ALWAYS:
  break; //直接跳出
  
  default:
  *p_err = OS_ERR_OPT_INVALID; //如果选项超出预期
  return ((OS_OBJ_QTY)0u); //返回 0(有错误),停止执行
  }
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  if (p_q->Type != OS_OBJ_TYPE_Q) { //如果 p_q 不是消息队列类型
  *p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型有误"
  return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  }
  #endif
  
  CPU_CRITICAL_ENTER(); //关中断
  p_pend_list = &p_q->PendList; //获取消息队列的等待列表
  cnt = p_pend_list->NbrEntries; //获取等待该队列的任务数
  nbr_tasks = cnt; //按照任务数目逐个处理
  switch (opt) { //根据选项分类处理
  case OS_OPT_DEL_NO_PEND: //如果只在没有任务等待的情况下删除队列
  if (nbr_tasks == (OS_OBJ_QTY)0) { //如果没有任务在等待该队列
  #if OS_CFG_DBG_EN > 0u //如果启用了调试代码和变量
  OS_QDbgListRemove(p_q); //将该队列从消息队列调试列表移除
  #endif
  OSQQty--; //消息队列数目减 1
  OS_QClr(p_q); //清除该队列的内容
  CPU_CRITICAL_EXIT(); //开中断
   *p_err = OS_ERR_NONE; //错误类型为"无错误"
  } else { //如果有任务在等待该队列
  CPU_CRITICAL_EXIT(); //开中断
  *p_err = OS_ERR_TASK_WAITING; //错误类型为"有任务在等待该队列"
  }
  break;
  
  case OS_OPT_DEL_ALWAYS: //如果必须删除信号量
  OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  
  ts = OS_TS_GET(); //获取时间戳
  while (cnt > 0u) { //逐个移除该队列等待列表中的任务
  p_pend_data = p_pend_list->HeadPtr;
  p_tcb = p_pend_data->TCBPtr;
  OS_PendObjDel((OS_PEND_OBJ *)((void *)p_q),
  p_tcb,
  ts);
  cnt--;
  }
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  OS_QDbgListRemove(p_q); //将该队列从消息队列调试列表移除
  #endif
  OSQQty--; //消息队列数目减 1
  OS_QClr(p_q); //清除消息队列内容
  OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  OSSched(); //调度任务
  *p_err = OS_ERR_NONE; //错误类型为"无错误"
  break; //跳出
  
  default://如果选项超出预期
  CPU_CRITICAL_EXIT(); //开中断
  *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  break; //跳出
  }
  return (nbr_tasks); //返回删除队列前等待其的任务数
  }
  #endif
  
  ```

- 阻塞恢复函数OS_PendObjDel()

  ```c
  void OS_PendObjDel (OS_PEND_OBJ *p_obj, //被删除对象的类型
  					OS_TCB *p_tcb,  //任务控制块指针
  					CPU_TS ts){ //信号量被删除时的时间戳
  
  	switch (p_tcb->TaskState) //根据任务状态分类处理
  	{
  		case OS_TASK_STATE_RDY: //如果任务是就绪状态
  		case OS_TASK_STATE_DLY: //如果任务是延时状态
  		case OS_TASK_STATE_SUSPENDED: //如果任务是挂起状态
  		case OS_TASK_STATE_DLY_SUSPENDED: //如果任务是在延时中被挂起
  			break; //这些情况均与等待无关,直接跳出
  
      	case OS_TASK_STATE_PEND: //如果任务是无期限等待状态
  		case OS_TASK_STATE_PEND_TIMEOUT: //如果任务是有期限等待状态
              if (p_tcb->PendOn == OS_TASK_PEND_ON_MULTI)//如果任务在等待多个信号量或消息队列
              {
                  OS_PendObjDel1(p_obj, //强制解除任务对某一对象的等待
  
                                 p_tcb,
                                 ts); 
              }
      #if (OS_MSG_EN > 0u)//如果启用了任务队列或消息队列
              p_tcb->MsgPtr = (void *)0; //清除(复位)任务的消息域
              p_tcb->MsgSize = (OS_MSG_SIZE)0u;
      #endif
              p_tcb->TS = ts; 
              //保存等待被中止时的时间戳到任务控制块
              OS_PendListRemove(p_tcb); //将任务从所有等待列表中移除
              OS_TaskRdy(p_tcb); //让任务进准备运行
              p_tcb->TaskState = OS_TASK_STATE_RDY; //修改任务状态为就绪状态
              p_tcb->PendStatus = OS_STATUS_PEND_DEL;//标记任务的等待对象被删除
              p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;//标记任务目前没有等待任何对象
              break; //跳出
  		case OS_TASK_STATE_PEND_SUSPENDED: //如果任务在无期限等待中被挂起
  		case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED://如果任务在有期限等待中被挂起
  			if (p_tcb->PendOn == OS_TASK_PEND_ON_MULTI)//如果任务在等待多个信号量或消息队列
  			{
  				OS_PendObjDel1(p_obj, //强制解除任务对某一对象的等待
  							   p_tcb,
  							   ts);
  			}
  #if (OS_MSG_EN > 0u) //如果启用了任务队列或消息队列
  			p_tcb->MsgPtr = (void *)0;//清除(复位)任务的消息域
  			p_tcb->MsgSize = (OS_MSG_SIZE)0u;
  #endif
  			p_tcb->TS = ts; //保存等待被中止时的时间戳到任务控制块
  
              OS_TickListRemove(p_tcb); //让任务脱离节拍列表
              OS_PendListRemove(p_tcb); //将任务从所有等待列表中移除
              p_tcb->TaskState = OS_TASK_STATE_SUSPENDED; //修改任务状态为挂起状态
              p_tcb->PendStatus = OS_STATUS_PEND_DEL; //标记任务的等待对象被删除
              p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING; //标记任务目前没有等待任何对象
              break; //跳出
          default: //如果任务状态超出预期
  	        break; //不需处理,直接跳出
      }
  }
  
  ```

- 消息队列删除函数OSQDel()

  系统只应删已创建的消息队列.如果删除消息队列时,有任务正在等待消息,则不应该进行删除操作,删除之后的消息队列就不可用了.实例如下

  ```c
  OS_Q queue; //声明消息队列
  
  OS_ERR err;
  
  /* 删除消息队列 queue */
  OSQDel ((OS_Q *)&queue, //指向消息队列的指针
  		 OS_OPT_DEL_NO_PEND,
  		(OS_ERR *)&err); //返回错误类型
  
  ```

- 消息队列发送函数OSQPost()

  任务或者中断服务程序都可以给消息队列发送消息,当发送消息时,如果队列未满,就说明运行信息入队.μC/OS会从消息池中取出一个消息,挂载到消息队列的末尾(FIFO发送方式),如果是LIFO发送方式,则将消息挂载到消息队列的头部,然后将消息中MsgPtr成员变量指向要发送的消息(此处可以理解为添加要发送的信息到消息(块)中),如果系统有任务阻塞在消息队列中,那么在发送了消息队列的时候,会将任务解除阻塞.

  ```c
  void OSQPost(OS_Q *p_q, //消息队列指针
  			 void *p_void, //消息指针
  			 OS_MSG_SIZE msg_size, //消息大小(单位:字节)
  			 OS_OPT opt, //选项
  			 OS_ERR *p_err){
  	CPU_TS ts;
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if (p_q == (OS_Q *)0) { //如果 p_q 为空
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"内核对象为空"
  		return; //返回,停止执行
  	}
      switch (opt) { //根据选项分类处理
          case OS_OPT_POST_FIFO: //如果选项在预期内
          case OS_OPT_POST_LIFO:
          case OS_OPT_POST_FIFO | OS_OPT_POST_ALL:
          case OS_OPT_POST_LIFO | OS_OPT_POST_ALL:
          case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
          case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
          case OS_OPT_POST_FIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
          case OS_OPT_POST_LIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
          break; //直接跳出
  
          default: //如果选项超出预期
          *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
          return; //返回,停止执行
      }
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_q->Type != OS_OBJ_TYPE_Q) { //如果 p_q 不是消息队列类型
  	    *p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型错误"
     		return; //返回,停止执行
  	}
  #endif
  
  	ts = OS_TS_GET(); //获取时间戳
  
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
  		OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_Q, //将该消息发布到中断消息队列
  					(void *)p_q,
  					(void *)p_void,
  					(OS_MSG_SIZE)msg_size,
  					(OS_FLAGS )0,
                      (OS_OPT )opt,
                      (CPU_TS )ts,
                      (OS_ERR *)p_err);
  		return; //返回(尚未发布),停止执行
  	}
  #endif
  
  	OS_QPost(p_q, //将消息按照普通方式
  	         p_void,
       	     msg_size,
   	         opt,
       	     ts,
  	         p_err);
  }
  
  ```

  ps:发送消息的选项

  ```c
  #define OS_OPT_POST_FIFO (OS_OPT)(0x0000u)/* 默认采用 FIFO 方式发送 */
  #define OS_OPT_POST_LIFO (OS_OPT)(0x0010u)/* 采用 LIFO 方式发送消息 */
  #define OS_OPT_POST_1 (OS_OPT)(0x0000u)/* 将消息发布到最高优先级的等待任务 */
  #define OS_OPT_POST_ALL (OS_OPT)(0x0200u)/* 向所有等待的任务广播消息 */
  #define OS_OPT_POST_NO_SCHED (OS_OPT)(0x8000u)/* 发送消息但是不进行任务调度 */
  
  ```

  **OSQPend()使用实例**

  ```c
  OS_Q queue; //声明消息队列
  OS_ERR err;
  OS_MSG_SIZE msg_size;
  
  /* 获取消息队列 queue 的消息 */
  pMsg = OSQPend ((OS_Q *)&queue, //消息变量指针
  				(OS_TICK )0, //等待时长为无限
  				(OS_OPT )OS_OPT_PEND_BLOCKING, //如果没有获取到信号量就等待
  				(OS_MSG_SIZE *)&msg_size, //获取消息的字节大小
  				(CPU_TS *)0, //获取任务发送时的时间戳
  				(OS_ERR *)&err); //返回错误
  
  ```

  

- 消息发送函数OS_QPost()

  如果不是在中断中调用`OSQPost()`函数,或者未启用中断延迟发布,则可直接调用`OS_QPost()`函数进行消息的发送.

  ```c
  void OS_QPost (OS_Q *p_q, //消息队列指针
  			   void *p_void, //消息指针
  			   OS_MSG_SIZE msg_size, //消息大小(单位:字节)
  			   OS_OPT opt, //选项
  			   CPU_TS ts, //消息被发布时的时间戳
  			   OS_ERR *p_err){
  	OS_OBJ_QTY cnt;
      OS_OPT post_type;
      OS_PEND_LIST *p_pend_list;
      OS_PEND_DATA *p_pend_data;
      OS_PEND_DATA *p_pend_data_next;
      OS_TCB *p_tcb;
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  //定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  // SR(临界段关中断只需保存 SR),开中断时将该值还原.
  
  	OS_CRITICAL_ENTER(); //进入临界段
  	p_pend_list = &p_q->PendList; //取出该队列的等待列表
  	if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) //如果没有任务在等待该队列
  	{
  		if ((opt & OS_OPT_POST_LIFO) == (OS_OPT)0)//把消息发布到队列的末端
  		{
  			post_type = OS_OPT_POST_FIFO;
          } else {//把消息发布到队列的前端
  			post_type = OS_OPT_POST_LIFO;
  		}
  
  		OS_MsgQPut(&p_q->MsgQ, //把消息放入消息队列
  				   p_void,
  				   msg_size,
  				   post_type,
  				   ts,
    				   p_err); 
  		OS_CRITICAL_EXIT(); //退出临界段
  		return; //返回,执行完毕
  	}
  	/* 如果有任务在等待该队列 */
  	if ((opt & OS_OPT_POST_ALL) != (OS_OPT)0) //如果要把消息发布给所有等待任务
  	{
  		cnt = p_pend_list->NbrEntries; //获取等待任务数目
  	} else {//如果要把消息发布给一个等待任务
  		cnt = (OS_OBJ_QTY)1; //要处理的任务数目为 1
  	}
  	p_pend_data = p_pend_list->HeadPtr; //获取等待列表的头部(任务)
  	while (cnt > 0u) //根据要发布的任务数目逐个发布
  	{
  		p_tcb = p_pend_data->TCBPtr;
  		p_pend_data_next = p_pend_data->NextPtr;
  		OS_Post((OS_PEND_OBJ *)((void *)p_q), //把消息发布给任务
  				 p_tcb,
  				 p_void,
  				 msg_size,
   				 ts);
  		p_pend_data = p_pend_data_next;
  		cnt--;
  	}
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0)//如果没选择"发布完不调度任务"
  	{
  		OSSched(); //任务调度
      }
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

- 消息入队函数OS_MsgQPut()

  将消息放入队列中,执行完毕就退出

  ```c
  void OS_MsgQPut(OS_MSG_Q *p_msg_q, //消息队列指针
  				void *p_void, //消息指针
  				OS_MSG_SIZE msg_size, //消息大小(单位:字节)
  				OS_OPT opt, //选项
  				CPU_TS ts, //消息被发布时的时间戳
  				OS_ERR *p_err){
  	OS_MSG *p_msg;
  	OS_MSG *p_msg_in;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  
  	if (p_msg_q->NbrEntries >= p_msg_q->NbrEntriesSize) //如果消息队列已没有可用空间
  	{
  		*p_err = OS_ERR_Q_MAX; //错误类型为"队列已满"
  		return; //返回,停止执行
  	}
  	if (OSMsgPool.NbrFree == (OS_MSG_QTY)0) //如果消息池没有可用消息
  	{
  		*p_err = OS_ERR_MSG_POOL_EMPTY; //错误类型为"消息池没有消息"
  		return; //返回,停止执行
  	}
  	/* 从消息池获取一个消息(暂存于 p_msg )*/
  	p_msg = OSMsgPool.NextPtr; //将消息控制块从消息池移除
  	OSMsgPool.NextPtr = p_msg->NextPtr; //指向下一个消息(取走首个消息)
  	OSMsgPool.NbrFree--; //消息池可用消息数减 1
  	OSMsgPool.NbrUsed++; //消息池被用消息数加 1
  	if (OSMsgPool.NbrUsedMax < OSMsgPool.NbrUsed) //更新消息被用最大数目的历史记录
  	{
  		OSMsgPool.NbrUsedMax = OSMsgPool.NbrUsed;
  	}
  	/* 将获取的消息插入消息队列 */
  	if (p_msg_q->NbrEntries == (OS_MSG_QTY)0) //如果消息队列目前没有消息
  	{
  		p_msg_q->InPtr = p_msg; //将其入队指针指向该消息
  		p_msg_q->OutPtr = p_msg; //出队指针也指向该消息
  		p_msg_q->NbrEntries = (OS_MSG_QTY)1; //队列的消息数为 1
  		p_msg->NextPtr = (OS_MSG *)0; //该消息的下一个消息为空
  	} else {//如果消息队列目前已有消息
  		if ((opt & OS_OPT_POST_LIFO) == OS_OPT_POST_FIFO) //如果用 FIFO 方式插入队列
  		{
  	        p_msg_in = p_msg_q->InPtr;//将消息插入入队端,入队
  			p_msg_in->NextPtr = p_msg; //指针指向该消息.
              p_msg_q->InPtr = p_msg;
              p_msg->NextPtr = (OS_MSG *)0;
          } else {//如果用 LIFO 方式插入队列,
  			p_msg->NextPtr = p_msg_q->OutPtr; //将消息插入出队端,出队
          	p_msg_q->OutPtr = p_msg; //指针指向该消息.
  		}
          	p_msg_q->NbrEntries++; //消息队列的消息数目加 1
  		}
  	if (p_msg_q->NbrEntriesMax < p_msg_q->NbrEntries) //更新改消息队列的最大消息
  	{
  		p_msg_q->NbrEntriesMax = p_msg_q->NbrEntries; //数目的历史记录.
  	}
  	p_msg->MsgPtr = p_void; //给该消息填写消息内容
      p_msg->MsgSize = msg_size; //给该消息填写消息大小
      p_msg->MsgTS = ts; //填写发布该消息时的时间戳
      *p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

- 发送完消息的任务调度函数OS_Post()

  在发送消息完成的时候,进行一次任务调度

  ```c
  void OS_Post(OS_PEND_OBJ *p_obj, //内核对象类型指针
  			 OS_TCB *p_tcb, //任务控制块
  			 void *p_void, //消息
  			 OS_MSG_SIZE msg_size, //消息大小
  			 CPU_TS ts){ //时间戳
  	switch (p_tcb->TaskState) //根据任务状态分类处理
  	{
  		case OS_TASK_STATE_RDY: //如果任务处于就绪状态
  		case OS_TASK_STATE_DLY: //如果任务处于延时状态
  		case OS_TASK_STATE_SUSPENDED: //如果任务处于挂起状态
  		case OS_TASK_STATE_DLY_SUSPENDED:
  //如果任务处于延时中被挂起状态
  		break; //不用处理,直接跳出
  		case OS_TASK_STATE_PEND: //如果任务处于无期限等待状态
  		case OS_TASK_STATE_PEND_TIMEOUT: //如果任务处于有期限等待状态
  		if (p_tcb->PendOn == OS_TASK_PEND_ON_MULTI)
  //如果任务在等待多个信号量或消息队列
  		{
  			OS_Post1(p_obj, //标记哪个内核对象被发布
  					 p_tcb,
  					 p_void,
  					 msg_size,
  					 ts);
  		} else {//如果任务不是在等待多个信号量或消息队列
  #if (OS_MSG_EN > 0u)
  	//如果启用了任务队列或消息队列
  			p_tcb->MsgPtr = p_void; //保存消息到等待任务
  			p_tcb->MsgSize = msg_size;
  #endif
  			p_tcb->TS = ts; //保存时间戳到等待任务
  		}
  		if (p_obj != (OS_PEND_OBJ *)0) //如果内核对象不为空
  		{
  			OS_PendListRemove(p_tcb); //从等待列表移除该等待任务
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  			OS_PendDbgNameRemove(p_obj, //移除内核对象的调试名
  								 p_tcb);
  #endif
  		}
  		OS_TaskRdy(p_tcb); //让该等待任务准备运行
  		p_tcb->TaskState = OS_TASK_STATE_RDY; //任务状态改为就绪状态
  		p_tcb->PendStatus = OS_STATUS_PEND_OK; //清除等待状态
  		p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING; //标记不再等待
  		break;
  	case OS_TASK_STATE_PEND_SUSPENDED:
  	//如果任务在无期限等待中被挂起
  	case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
  	//如果任务在有期限等待中被挂起
          if (p_tcb->PendOn == OS_TASK_PEND_ON_MULTI) //如果任务在等待多个信号量或消息队列
          {
              OS_Post1(p_obj, //标记哪个内核对象被发布
                       p_tcb,
                       p_void,
                       msg_size,
                       ts);
  		} else {//如果任务不在等待多个信号量或消息队列
  #if (OS_MSG_EN > 0u)//如果启用了调试代码和变量
  			p_tcb->MsgPtr = p_void; //保存消息到等待任务
  			p_tcb->MsgSize = msg_size;
  #endif
  			p_tcb->TS = ts; //保存时间戳到等待任务
  		}
  		OS_TickListRemove(p_tcb); //从节拍列表移除该等待任务
  		if (p_obj != (OS_PEND_OBJ *)0) //如果内核对象为空
  		{
  			OS_PendListRemove(p_tcb); //从等待列表移除该等待任务
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  			OS_PendDbgNameRemove(p_obj, //移除内核对象的调试名
  								 p_tcb);
  #endif
  		}
  		p_tcb->TaskState = OS_TASK_STATE_SUSPENDED; //任务状态改为被挂起状态
  		p_tcb->PendStatus = OS_STATUS_PEND_OK; //清除等待状态
  		p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING; //标记不再等待
  		break;
  	default: //如果任务状态超出预期
  		break; //直接跳出
  }
  
  ```

- 消息队列获取函数OSQPend()

  当任务试图从队列中的获取消息时,用户可以指定一个阻塞超时时间,当且仅当消息队列中有消息的时候,任务才能获取到消息.在这段时间中,如果队列为空,该任务将保持阻塞状态以等待队列消息有效.当其他任务或中断服务程序往其等待的队列中写入了数据,该任务将自动由阻塞态转为就绪态.当任务等待的时间超过了用户指定的阻塞时间,即使队列中尚无有效消息,任务也会自动从阻塞态转为就绪态.

  ```c
  void *OSQPend(OS_Q *p_q, //消息队列指针
  			  OS_TICK timeout, //等待期限(单位:时钟节拍)
  			  OS_OPT opt, //选项
  			  OS_MSG_SIZE *p_msg_size, //返回消息大小(单位:字节)
  			  CPU_TS *p_ts, //获取等到消息时的时间戳
  			  OS_ERR *p_err){
  	OS_PEND_DATA pend_data;
  	void *p_void;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和
  	//定义一个局部变量,用于保存关中断前的 CPU 状态寄存器
  	// SR(临界段关中断只需保存 SR),开中断时将该值还原.
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_PEND_ISR; //错误类型为"在中断中中止等待"
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_q == (OS_Q *)0) //如果 p_q 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"对象为空"
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  	if (p_msg_size == (OS_MSG_SIZE *)0) //如果 p_msg_size 为空
  	{
  		*p_err = OS_ERR_PTR_INVALID; //错误类型为"指针不可用"
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_PEND_BLOCKING: //如果选项在预期内
          case OS_OPT_PEND_NON_BLOCKING:
          break; //直接跳出
          default: //如果选项超出预期
          *p_err = OS_ERR_OPT_INVALID; //返回错误类型为"选项非法"
          return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_q->Type != OS_OBJ_TYPE_Q) //如果 p_q 不是消息队列类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型有误"
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  	if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  	{
  		*p_ts = (CPU_TS )0; //初始化(清零)p_ts,待用于返回时间
  	}
  
  	CPU_CRITICAL_ENTER(); //关中断
  	p_void = OS_MsgQGet(&p_q->MsgQ, //从消息队列获取一个消息
  						p_msg_size,
  						p_ts,
  						p_err);
  	if (*p_err == OS_ERR_NONE) //如果获取消息成功
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		return (p_void); //返回消息内容
  	}
  	/* 如果获取消息不成功 */
  	if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"等待渴求阻塞"
  		return ((void *)0); //返回 0(有错误),停止执行
  	} else {//如果选择了阻塞任务
  		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)//如果调度器被锁
  		{
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  			return ((void *)0); //返回 0(有错误),停止执行
  		}
  	}
  	/* 如果调度器未被锁 */
  	OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,重开中断
  	OS_Pend(&pend_data,//阻塞当前任务,等待消息队列,
  			(OS_PEND_OBJ *)((void *)p_q), //将当前任务脱离就绪列表,并
  
  			OS_TASK_PEND_ON_Q, //插入节拍列表和等待列表.
  			timeout);
  	OS_CRITICAL_EXIT_NO_SCHED(); //开调度器,但不进行调度
  	OSSched();
  	//找到并调度最高优先级就绪任务
  	/* 当前任务(获得消息队列的消息)得以继续运行 */
  	CPU_CRITICAL_ENTER(); //关中断
  	switch (OSTCBCurPtr->PendStatus)
  	//根据当前运行任务的等待状态分类处理
  	{
  		case OS_STATUS_PEND_OK: //如果等待状态正常
  		p_void = OSTCBCurPtr->MsgPtr;
  		//从(发布时放于)任务控制块提取消息
  		*p_msg_size = OSTCBCurPtr->MsgSize; //提取消息大小
  		if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  		{
  			*p_ts = OSTCBCurPtr->TS; //获取任务等到消息时的时间戳
  		}
  		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  		break; //跳出
          case OS_STATUS_PEND_ABORT: //如果等待被中止
  			p_void = (void *)0; //返回消息内容为空
  			*p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //获取等待被中止时的时间戳
  			}
  			*p_err = OS_ERR_PEND_ABORT; //错误类型为"等待被中止"
  			break; //跳出
  
  		case OS_STATUS_PEND_TIMEOUT: //如果等待超时
  			p_void = (void *)0; //返回消息内容为空
  			*p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = (CPU_TS )0; //清零 p_ts
  			}
  			*p_err = OS_ERR_TIMEOUT; //错误类型为"等待超时"
  			break; //跳出
  		case OS_STATUS_PEND_DEL: //如果等待的内核对象被删除
  			p_void = (void *)0; //返回消息内容为空
  			*p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
  				*p_ts = OSTCBCurPtr->TS; //获取对象被删时的时间戳
              }
  			*p_err = OS_ERR_OBJ_DEL; //错误类型为"等待对象被删"
  			break; //跳出
  		default: //如果等待状态超出预期
  			p_void = (void *)0; //返回消息内容为空
  			*p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
  			*p_err = OS_ERR_STATUS_INVALID; //错误类型为"状态非法"
  			break; //跳出
  	}
  	CPU_CRITICAL_EXIT(); //开中断
  	return(p_void); //返回消息内容
  }
  
  ```

- 消息获取函数OS_MsgQGet()

  ```c
  void *OS_MsgQGet (OS_MSG_Q *p_msg_q, //消息队列
  				  OS_MSG_SIZE *p_msg_size, //返回消息大小
  				  CPU_TS *p_ts, //返回某些操作的时间戳
  				  OS_ERR *p_err){ //返回错误类型
  	OS_MSG *p_msg;
  	void *p_void;
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((void *)0); //返回空消息,停止执行
  	}
  #endif
  
  	if (p_msg_q->NbrEntries == (OS_MSG_QTY)0) //如果消息队列没有消息
  	{
          *p_msg_size = (OS_MSG_SIZE)0; //返回消息长度为 0
          if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  		{
  			*p_ts = (CPU_TS )0; //清零 p_ts
  		}
  		*p_err = OS_ERR_Q_EMPTY; //错误类型为"队列没消息"
  		return ((void *)0); //返回空消息,停止执行
  	}
          /* 如果消息队列有消息 */
      p_msg = p_msg_q->OutPtr; //从队列的出口端提取消息
      p_void = p_msg->MsgPtr; //提取消息内容
      *p_msg_size = p_msg->MsgSize; //提取消息长度
      if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
      {
  	    *p_ts = p_msg->MsgTS; //获取消息被发布时的时间戳
      }
      p_msg_q->OutPtr = p_msg->NextPtr; //修改队列的出队指针
      if (p_msg_q->OutPtr == (OS_MSG *)0) //如果队列没有消息了
      {
      	p_msg_q->InPtr = (OS_MSG *)0; //清零出队指针
  	    p_msg_q->NbrEntries = (OS_MSG_QTY)0; //清零消息数
      } else {//如果队列还有消息
      	p_msg_q->NbrEntries--; //队列的消息数减 1
      }
      /* 从消息队列提取完消息信息后,将消息释放回消息池供继续使用 */
      p_msg->NextPtr = OSMsgPool.NextPtr; //消息插回消息池
      OSMsgPool.NextPtr = p_msg;
      OSMsgPool.NbrFree++; //消息池的可用消息数加 1
      OSMsgPool.NbrUsed--; //消息池的已用消息数减 1
      *p_err = OS_ERR_NONE; //错误类型为"无错误"
  
      return (p_void); //返回消息内容
  }
  
  ```

- 任务脱离就序列表函数OS_Pend()

  将当前任务脱离就绪列表,并根据用户指定的阻塞时间插入节拍列表和队列等待列表,然后打开调度器,但不进行调度.

  ```c
  void OS_Pend(OS_PEND_DATA *p_pend_data, //待插入等待列表的元素
  			 OS_PEND_OBJ *p_obj, //等待的内核对象
  			 OS_STATE pending_on, //等待哪种对象内核
  			 OS_TICK timeout){ //等待期限
      OS_PEND_LIST *p_pend_list;
      OSTCBCurPtr->PendOn = pending_on; //资源不可用,开始等待
      OSTCBCurPtr->PendStatus = OS_STATUS_PEND_OK; //正常等待中
      OS_TaskBlock(OSTCBCurPtr,timeout); //阻塞当前运行任务,如果 timeout 非0,把任务插入的节拍列表
      if (p_obj != (OS_PEND_OBJ *)0) //如果等待对象非空
  	{
          p_pend_list = &p_obj->PendList; //获取对象的等待列表到
          p_pend_list
          p_pend_data->PendObjPtr = p_obj; //保存要等待的对象
          OS_PendDataInit((OS_TCB *)OSTCBCurPtr, //初始化 p_pend_data(待插入等待列表)
          (OS_PEND_DATA *)p_pend_data,
          (OS_OBJ_QTY )1);
          //按优先级将 p_pend_data 插入等待列表
          OS_PendListInsertPrio(p_pend_list,
      					      p_pend_data);
  	} else {//如果等待对象为空
  		OSTCBCurPtr->PendDataTblEntries = (OS_OBJ_QTY )0; //清零当前任务的等待域数据
  		OSTCBCurPtr->PendDataTblPtr = (OS_PEND_DATA *)0;
  	}
  #if OS_CFG_DBG_EN > 0u //如果启用了调试代码和变量
  	OS_PendDbgNameAdd(p_obj, //更新信号量的 DbgNamePtr 元素为其等待
  	OSTCBCurPtr);//列表中优先级最高的任务的名称.
  #endif
  }
  
  ```

  

### 消息队列使用注意事项

在使用μC/OS提供的消息队列函数的时候,需要了解以下几点:

1. 用`OSQPend()`,`OSQPost()`等这些函数之前应先创建需消息队列,并根据队列句柄(队列控制块)进行操作.

2. 列读取采用的是先进先出(FIFO)模式,会先读取先存储在队列中的数据.当然μC/OS也支持后进先出(LIFO)模式,那么读取的时候就会读取到后进队列的数据.

3. 无论是发送或者是接收消息都是以数据引用的方式进行.

4. 队列是具有自己独立权限的内核对象,并不属于任何任务.所有任务都可以向同一队列写入和读出.一个队列由多任务或中断写入是经常的事,但由多个任务读出倒是用的比较少.

5. 消息的传递实际上只是传递传送内容的指针和传送内容的字节大小.这在使用消息队列的时候就要注意了,获取消息之前不能释放存储在消息中的指针内容,比如中断定义了一个局部变量,然后将其地址放在消息中进行传递,中断退出之前消息并没有被其他任务获取,退出中断的时候CPU已经释放了中断中的这个局部变量,后面任务获取这个地址的内容就会出错.所以一定要保证在获取内容地址之前不能释放内容这个内存单元.

   有三种方式可以避免这种情况:

   - 将变量定义为静态变量,即在其前面加上static,这样内存单元就不会被释放.
   - 将变量定义为全局变量.
   - 将要传递的内容当做指针传递过去.比如地址0x12345678存放一个变量的值为5.常规是把0x12345678这个地址传递给接收消息的任务,任务接收到这个消息后,取出这个地址的内容5.但是如果我们把5当做"地址"传递给任务,最后接收消息的任务直接拿着这个"地址"当做内容去处理即可.不过这种方法不能传递结构体等比较复杂的数据结构,因为消息中存放地址的变量内存大小是有限的(一个指针大小)

### 消息队列实验

创建两个任务`AppTaskPost()`和`AppTaskPend()`,任务`AppTaskPost()`用于发送消息,任务`AppTaskPend()` 用于接收消息,两个任务独立运行,把接收到的消息通过串口输出.

1. 声明消息队列,并定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![消息队列1_1](.\Image\消息队列1_1.png)

   ![消息队列1_3](.\Image\消息队列1_3.png)

   ![消息队列1_2](.\Image\消息队列1_2.png)

2. 定义任务函数

   ![消息队列2_1](.\Image\消息队列2_1.png)

3. 任务启动函数编写

   ![消息队列2_2](.\Image\消息队列3.png)

4. 结果现象

   ![消息队列实验现象](.\Image\消息队列实验现象.png)

## 信号量

### 信号量的基本概念

- 信号量(Semaphore)是一种实现任务间通信的机制,可以实现任务之间同步或临界资源的互斥访问,常用于协助一组相互竞争的任务来访问临界资源.在多任务系统中,各任务之间需要同步或互斥实现临界资源的保护,信号量功能可以为用户提供这方面的支持.
- 信号量是一个非负整数,所有获取它的任务都会将该整数减一(获取它当然是为了使用资源),当该整数值为零时,所有试图获取它的任务都将处于阻塞状态.通常一个信号量的计数值用于对应有效的资源数,表示剩下可被占用的临界资源数,其值的含义分两种情况:
  1. 0:表示没有积累下来的释放信号量操作,且有可能有在此信号量上阻塞的任务
  2. 正值:表示有一个或多个释放信号量操作

ps:μC/OS的信号量并没有二值信号量与计数信号量之分(下面是为了解释清楚信号量而已)

### 二值信号量

- 二值信号量既可以用于临界资源访问也可以用于同步功能.
- 二值信号量和互斥信号量(以下使用互斥量表示互斥信号量)非常相似,但是有一些细微差别:互斥量有优先级继承机制,二值信号量则没有这个机制.这使得二值信号量更偏向应用于同步功(任务与任务间的同步或任务和中断间同步),而互斥量更偏向应用于临界资源的互斥访问.
- 用作同步时,信号量在创建后应被置为空,任务1获取信号量而进入阻塞,任务2在某种条件发生后,释放信号量,于是任务1获得信号量得以进入就绪态,如果任务1的优先级是最高的,那么就会立即切换任务,从而达到了两个任务间的同步.同样的,在中断服务函数中释放信号量,任务1也会得到信号量,从而达到任务与中断间的同步.

### 计数信号量

- 计数信号量用于计数,在实际的使用中,我们常将计数信号量用于事件计数与资源管理.每当某个事件发生时,任务或者中断将释放一个信号量(信号量计数值加 1),当处理被事件时(一般在任务中处理),处理任务会取走该信号量(信号量计数值减1),信号量的计数值则表示还有多少个事件没被处理.此外,系统还有很多资源,我们也可以使用计数信号量进行资源管理,信号量的计数值表示系统中可用的资源数目,任务必须先获取到信号量才能获取资源访问权,当信号量的计数值为零时表示系统没有可用的资源,但是要注意,在使用完资源的时候必须归还信号量,否则当计数值为0的时候任务就无法访问该资源了.
- 计数型信号量允许多个任务对其进行操作,但限制了任务的数量.我们的信号量在使用之后需要减一,当信号量为0时,后面的任务对资源的访问便无法进行,只有当我们释放了这个资源,后面的任务才能对这个资源进行访问.

### 信号量的使用场景

- 在嵌入式操作系统中二值信号量是任务间,任务与中断间同步的重要手段,信号量使用最多的一般都是二值信号量与互斥量.为什么叫二值信号量呢?因为信号量资源被获取了,信号量值就是0,信号量资源被释放,信号量值就是1,把这种只有0和1两种情况的信号量称之为二值信号量.

- 在多任务系统中,我们经常会使用这个二值信号量,比如,某个任务需要等待一个标记,那么任务可以在轮询中查询这个标记有没有被置位,但是这样子做,就会很消耗CPU资源并且妨碍其他任务执行,更好的做法是任务的大部分时间处于阻塞状态(允许其他任务执行),直到某些事件发生该任务才被唤醒去执行.可以使用二进制信号量实现这种同步,当任务取信号量时,因为此时尚未发生特定事件,信号量为空,任务会进入阻塞状态;当事件的条件满足后,任务/中断便会释放信号量,告知任务这个事件发生了,任务取得信号量便被唤醒去执行对应的操作,任务执行完毕并不需要归还信号量,这样子的CPU的效率可以大大提高,而且实时响应也是最快的.

- 某个任务使用信号量在等中断的标记的发生,在这之前任务已经进入了阻塞态,在等待着中断的发生,当在中断发生之后,释放一个信号量,也就是我们常说的标记,当它退出中断之后,操作系统会进行任务的调度,如果这个任务能够运行,系统就会去执行这个任务,这样子就大大提高了我们的效率.

- 二值信号量在任务与任务中同步的应用场景:假设我们有一个温湿度的传感器,假设是1s采集一次数据,那么我们让他在液晶屏中显示数据出来,这个周期也是要1s一次的,如果液晶屏刷新的周期是100ms更新一次,那么此时的温湿度的数据还没更新,液晶屏根本无需刷新,只需要在1s后温湿度数据更新的时候刷新即可,否则CPU就是白白做了多次的无效数据更新,CPU的资源就被刷新数据这个任务占用了大半,造成CPU资源浪费,如果液晶屏刷新的周期是10s更新一次,那么温湿度的数据都变化了10次,液晶屏才来更新数据,那拿这个产品有啥用,根本就是不准确的,所以,还是需要同步协调工作,在温湿度采集完毕之后,进行液晶屏数据的刷新,这样子,才是最准确的,并且不会浪费 CPU 的资源.

- 二值信号量在任务与中断同步的应用场景:我们在串口接收中,我们不知道啥时候有数据发送过来,有一个任务是做接收这些数据处理,总不能在任务中每时每刻都在任务查询有没有数据到来,那样会浪费CPU资源,所以在这种情况下使用二值信号量是很好的办法,当没有数据到来的时候,任务就进入阻塞态,不参与任务的调度,等到数据到来了,释放一个二值信号量,任务就立即从阻塞态中解除,进入就绪态,然后运行的时候处理数据,这样子系统的资源就会很好的被利用起来.

- 计数信号量用于资源统计,比如当前任务来了很多个消息,但是这些消息都放在缓冲区中,尚未处理,这时候就可以利用计数信号量对这些资源进行统计,每来一个消息就加一,每处理完个消息就减一,这样子系统就知道有多少资源未处理的

### 二值信号量运作机制

- 创建信号量时,系统会为创建的信号量对象分配内存,并把可用信号量初始化为用户自定义的个数,二值信号量的最大可用信号量个数为1.

- 二值信号量获取,任何任务都可以从创建的二值信号量资源中获取一个二值信号量,获取成功则返回正确,否则任务会根据用户指定的阻塞超时时间来等待其他任务/中断释放信号量.在等待这段时间,系统将任务变成阻塞态,任务将被挂到该信号量的阻塞等待列表中.

- 在二值信号量无效的时候,假如此时有任务获取该信号量的话,那么任务将进入阻塞状态.

  ![信号量无效时候获取](.\Image\信号量无效时候获取.png)

- 假如某个时间中断/任务释放了信号量,其过程具体见图中断/任务释放信号量,那么,由于获取无效信号量而进入阻塞态的任务将获得信号量并且恢复为就绪态.

  ![二值信号量运作机制a](.\Image\二值信号量运作机制a.png)

  ![二值信号量运作机制b](.\Image\二值信号量运作机制b.png)

### 计数信号量运作机制

- 计数信号量可以用于资源管理,允许多个任务获取信号量访问共享资源,但会限制任务的最大数目.访问的任务数达到可支持的最大数目时,会阻塞其他试图获取该信号量的任务,直到有任务释放了信号量.这就是计数型信号量的运作机制,虽然计数信号量允许多个任务访问同一个资源,但是也有限定,比如某个资源限定只能有3个任务访问,那么第4个任务访问的时候,会因为获取不到信号量而进入阻塞,等到有任务(比如任务1)释放掉该资源的时候,第4个任务才能获取到信号量从而进行资源的访问.

  ![计数信号量运作示机制](.\Image\计数信号量运作示机制.png)

### 信号量控制块

- μC/OS的信号量由多个元素组成,在信号量被创建时,需要由我们自己定义信号量控制块(也可以称之为信号量句柄),因为它是用于保存信号量的一些信息的,其数据结构`OS_SEM`除了信号量必须的一些基本信息外,还有`PendList`链表与`Ctr`,为的是方便系统来管理信号量.

  ![信号量控制块结构](.\Image\信号量控制块结构.png)

  ```c
  struct os_sem{
  OS_OBJ_TYPE Type; //信号量的类型
  CPU_CHAR *NamePtr; //信号量的名字
  OS_PEND_LIST PendList; //等待信号量的任务列表
  #if OS_CFG_DBG_EN > 0u
  OS_SEM *DbgPrevPtr;
  OS_SEM *DbgNextPtr;
  CPU_CHAR *DbgNamePtr;
  #endif
  OS_SEM_CTR Ctr; //可用信号量的个数,如果为0则表示无可用信号量.
  CPU_TS TS; //用于记录时间戳
  };
  
  ```

### 信号量函数接口

| 函数名称      | 函数作用   |
| ------------- | ---------- |
| OSSemCreate() | 创建信号量 |
| OSSemDel()    | 删除信号量 |
| OSSemPost()   | 释放信号量 |
| OSSemPend()   | 获取信号量 |

- 创建信号量函数

  在定义完信号量结构体变量后就可以调用`OSSemCreate()`函数进行创建一个信号量.内核对象使用之前一定要先创建,这个创建过程必须要保证在所有可能使用内核对象的任务之前,所以一般我们都是在创建任务之前就创建好系统需要的内核对象(如信号量等).

  | 参数    | 含义                                                         |
  | ------- | ------------------------------------------------------------ |
  | *p_sem  | 指向信号量变量的指针.                                        |
  | *p_name | 指向信号量变量名字字符串的指针.                              |
  | cnt     | 信号量的初始值,用作资源保护的信号量这个值通常跟资源的数量相同,用做标志事件发生的信号量这个值设置为0,标志事情还没有发生. |
  | *p_err  | 指向返回错误类型的指针.                                      |

  | 错误类型                       | 含义                                                         |
  | ------------------------------ | ------------------------------------------------------------ |
  | OS_ERR_CREATE_ISR              | 在中断中创建信号量是不被允许的,返回错误.                     |
  | OS_ERR_ILLEGAL_CREATE_RUN_TIME | 在定义OSSafetyCriticalStartFlag 为DEF_TRUE后就不运行创建任何内核对象. |
  | OS_ERR_NAME                    | 参数p_name是个空指针.                                        |
  | OS_ERR_OBJ_CREATED             | 信号量已经被创建.                                            |
  | OS_ERR_OBJ_PTR_NULL            | 参数p_sem是个空指针.                                         |
  | OS_ERR_OBJ_TYPE                | 参数p_sem被初始化为别的内核对象了.                           |
  | OS_ERR_NONE                    | 无错误.                                                      |

  ```c
  void OSSemCreate(OS_SEM *p_sem, //信号量控制块指针
  CPU_CHAR *p_name, //信号量名称
  OS_SEM_CTR cnt, //资源数目或事件是否发生标志
  OS_ERR *p_err){
  	CPU_SR_ALLOC();//使用到临界段(在关/开中断时)时必须用到该宏,该宏声明和定义一个局部变
  			       //量,用于保存关中断前的 CPU 状态寄存器 SR(临界段关中断只需保存 SR)
  				   //,开中断时将该值还原.
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不继续执行
  	}
  #endif
      
  #ifdef OS_SAFETY_CRITICAL_IEC61508 //如果启用(默认禁用)了安全关键
  	//如果是在调用 OSSafetyCriticalStart() 后创建该信号量
  	if (OSSafetyCriticalStartFlag == DEF_TRUE)
  	{
  		*p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME; //错误类型为"非法创建内核对象"
  		return; //返回,不继续执行
  	}
  #endif
      
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		*p_err = OS_ERR_CREATE_ISR; //错误类型为"在中断函数中创建对象"
  		return; //返回,不继续执行
  	}
  #endif
      
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测
  	if (p_sem == (OS_SEM *)0) //如果参数 p_sem 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"信号量对象为空"
  		return; //返回,不继续执行
  	}
  #endif
      
      OS_CRITICAL_ENTER(); //进入临界段
      p_sem->Type = OS_OBJ_TYPE_SEM; //初始化信号量指标
      p_sem->Ctr = cnt;
      p_sem->TS = (CPU_TS)0;
      p_sem->NamePtr = p_name;
      OS_PendListInit(&p_sem->PendList); //初始化该信号量的等待列表
      
  #if OS_CFG_DBG_EN > 0u//如果启用(默认启用)了调试代码和变量
  	OS_SemDbgListAdd(p_sem); //将该定时添加到信号量双向调试链表
  #endif
  	OSSemQty++; //信号量个数加 1
      
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  使用实例

  ```c
  OS_SEM SemOfKey; //标志 KEY1 是否被按下的信号量
  
  /* 创建信号量 SemOfKey */
  OSSemCreate((OS_SEM *)&SemOfKey, //指向信号量变量的指针
  			(CPU_CHAR *)"SemOfKey", //信号量的名字
  			(OS_SEM_CTR )0,//信号量这里是指示事件发生,所以赋值为 0,表示事件还没有发生
  			(OS_ERR *)&err); //错误类型
  
  ```

  

- 信号量删除函数

  信号量删除函数是根据信号量结构(信号量句柄)直接删除的,删除之后这个信号量的所有信息都会被系统清空,而且不能再次使用这个信号量了,但是需要注意的是,如果某个信号量没有被定义,那也是无法被删除的,如果有任务阻塞在该信号量上,那么尽量不要删除该信号量.想要使用互斥量删除函数就必须将`OS_CFG_SEM_DEL_EN`宏定义配置为1.

  | 参数   | 含义                    |
  | ------ | ----------------------- |
  | *p_sem | 指向信号量变量的指针.   |
  | opt    | 删除信号量时候的选项.   |
  | *p_err | 指向返回错误类型的指针. |

  | 选项               | 功能                                                    |
  | ------------------ | ------------------------------------------------------- |
  | OS_OPT_DEL_NO_PEND | 当信号量的等待列表上面没有相应的任务的时候才删除信号量. |
  | OS_OPT_DEL_ALWAYS  | 不管信号量的等待列表是否有相应的任务都删除信号量.       |

  | 错误类型            | 含义                                                         |
  | ------------------- | ------------------------------------------------------------ |
  | OS_ERR_DEL_ISR      | 企图在中断中删除信号量.                                      |
  | OS_ERR_OBJ_PTR_NULL | 参数p_sem是空指针.                                           |
  | OS_ERR_OBJ_TYPE     | 参数p_sem指向的内核变量类型不是信号量.                       |
  | OS_ERR_OPT_INVALID  | opt在给出的选项之外.                                         |
  | OS_ERR_TASK_WAITING | 在选项opt是OS_OPT_DEL_NO_PEND的时候,并且信号量等待列表上有等待的任务. |
  | OS_ERR_NONE         | 无错误.                                                      |

  ```c
  #if OS_CFG_SEM_DEL_EN > 0u //如果启用了 OSSemDel() 函数
  OS_OBJ_QTY OSSemDel(OS_SEM *p_sem, //信号量指针
  					OS_OPT opt, //选项
  					OS_ERR *p_err){
      OS_OBJ_QTY cnt;
      OS_OBJ_QTY nbr_tasks;
      OS_PEND_DATA *p_pend_data;
      OS_PEND_LIST *p_pend_list;
      OS_TCB *p_tcb;
      CPU_TS ts;
      CPU_SR_ALLOC();
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return ((OS_OBJ_QTY)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_DEL_ISR; //返回错误类型为"在中断中删除"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_sem == (OS_SEM *)0) //如果 p_sem 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //返回错误类型为"内核对象为空"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),不继续执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_DEL_NO_PEND: //如果选项在预期之内
  		case OS_OPT_DEL_ALWAYS:
  			break; //直接跳出
  		default: //如果选项超出预期
  		*p_err = OS_ERR_OPT_INVALID; //返回错误类型为"选项非法"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),不继续执行
  	}
  #endif
      
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_sem->Type != OS_OBJ_TYPE_SEM) //如果 p_sem 不是信号量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //返回错误类型为"内核对象类型错误"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
      CPU_CRITICAL_ENTER(); //关中断
      p_pend_list = &p_sem->PendList; //获取信号量的等待列表到 p_pend_list
      cnt = p_pend_list->NbrEntries; //获取等待该信号量的任务数
      nbr_tasks = cnt;
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_DEL_NO_PEND:
  			//如果只在没有任务等待的情况下删除信号量
  			if (nbr_tasks == (OS_OBJ_QTY)0) //如果没有任务在等待该信号量
  			{
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  				OS_SemDbgListRemove(p_sem); //将该信号量从信号量调试列表移除
  #endif
                  OSSemQty--; //信号量数目减 1
                  OS_SemClr(p_sem); //清除信号量内容
                  CPU_CRITICAL_EXIT(); //开中断
                  *p_err = OS_ERR_NONE; //返回错误类型为"无错误"
  			} else {//如果有任务在等待该信号量
  				CPU_CRITICAL_EXIT(); //开中断
  				*p_err = OS_ERR_TASK_WAITING;//返回错误类型为"有任务在等待该信号量"
  			}
  			break;
  		case OS_OPT_DEL_ALWAYS: //如果必须删除信号量
  			OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,并开中断
  			ts = OS_TS_GET(); //获取时间戳
  			while (cnt > 0u)//逐个移除该信号量等待列表中的任务
  			{
  				p_pend_data = p_pend_list->HeadPtr;
  				p_tcb = p_pend_data->TCBPtr;
  				OS_PendObjDel((OS_PEND_OBJ *)((void *)p_sem),
  							  p_tcb,
  							  ts);
  				cnt--;
  			}
  #if OS_CFG_DBG_EN > 0u //如果启用了调试代码和变量
  			OS_SemDbgListRemove(p_sem);//将该信号量从信号量调试列表移除
  #endif
  			OSSemQty--; //信号量数目减 1
  			OS_SemClr(p_sem); //清除信号量内容
  			OS_CRITICAL_EXIT_NO_SCHED(); //减锁调度器,但不进行调度
  			OSSched();//任务调度,执行最高优先级的就绪任务
  			*p_err = OS_ERR_NONE; //返回错误类型为"无错误"
  			break;
  		default: //如果选项超出预期
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_OPT_INVALID; //返回错误类型为"选项非法"
  			break;
  		}
  	return ((OS_OBJ_QTY)nbr_tasks);//返回删除信号量前等待其的任务数
  }
  #endif
  
  ```

  使用实例

  ```c
  OS_SEM SemOfKey;; //声明信号量
  
  OS_ERR err;
  
  /* 删除信号量 sem*/
  OSSemDel ((OS_SEM *)&SemOfKey, //指向信号量的指针
  		  OS_OPT_DEL_NO_PEND,
  		  OS_ERR *)&err);
  
  ```

  

- 信号量释放函数OSSemPost()

  - 信号量的释放可以在任务,中断中使用.

  - 当信号量有效的时候,任务才能获取信号量,什么函数可以使得信号量变得有效?有两个方式:
    1. 在创建的时候进行初始化,将它可用的信号量个数设置一个初始值;如果该信号量用作二值信号量,那么我们在创建信号量的时候其初始值的范围 0~1,假如初始值为 1 个可用的信号量的话,被获取一次就变得无效了,那就需要我们释放信号量,μC/OS 提供了信号量释放函数,每调用一次该函数就释放一个信号量.但是有个问题,能不能一直释放?很显然如果用作二值信号量的话,一直释放信号量就达不到同步或者互斥访问的效果,虽然说 μC/OS 的信号量是允许一直释放的,但是,信号量的范围还需我们用户自己根据需求进行决定,当用作二值信号量的时候,必须确保其可用值在 0~1 范围内;而用作计数信号量的话,其范围是由用户根据实际情况来决定的,在写代码的时候,要注意代码的严谨性.
    2. 

  | 参数   | 含义                                 |
  | ------ | ------------------------------------ |
  | *p_sem | 指向要提交的信号量的指针.            |
  | opt    | 发布信号时的选项,可能有以下几个选项. |
  | *p_err | 指向返回错误类型的指针.              |

  | 选项                 | 功能                                                         |
  | -------------------- | ------------------------------------------------------------ |
  | OS_OPT_POST_1        | 发布给信号量等待列表中优先级最高的任务.                      |
  | OS_OPT_POST_ALL      | 发布给信号量**等待列表**中所有的任务.                        |
  | OS_OPT_POST_NO_SCHED | 提交信号量之后要不要进行任务调度,默认是要进行任务调度的,选择该选项可能的原因是想继续运行当前任务,因为发布信号量可能让那些等待信号量的任务就绪,这个选项没有进行任务调度,发布完信号量当前任务还是继续运行.当任务想发布多个信号量,最后同时调度的话也可以用这个选项.可以跟上面两个选项之一相与做为参数. |

  | 错误类型       | 含义                                                         |
  | -------------- | ------------------------------------------------------------ |
  | OS_ERR_SEM_OVF | 信号量计数值已经达到最大范围了,这次提交会引起信号量计数值溢出. |
  | OS_ERR_NONE    | 无错误.                                                      |

  ```c
  OS_SEM_CTR OSSemPost(OS_SEM *p_sem, //信号量控制块指针
  					 OS_OPT opt, //选项
  					 OS_ERR *p_err){
  	OS_SEM_CTR ctr;
  	CPU_TS ts;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
      
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测功能
  	if (p_sem == (OS_SEM *)0) //如果 p_sem 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //返回错误类型为"内核对象指针为空"
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  	switch (opt) //根据选项情况分类处理
  	{
  		case OS_OPT_POST_1: //如果选项在预期内,不处理
  		case OS_OPT_POST_ALL:
  		case OS_OPT_POST_1 | OS_OPT_POST_NO_SCHED:
  		case OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
  			break;
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //返回错误类型为"选项非法"
  			return ((OS_SEM_CTR)0u); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用了对象类型检测
  	if (p_sem->Type != OS_OBJ_TYPE_SEM) //如果 p_sem 的类型不是信号量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //返回错误类型为"对象类型错误"
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
  	ts = OS_TS_GET(); //获取时间戳
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_SEM,//将该信号量发布到中断消息队列
  					(void *)p_sem,
  					(void *)0,
  					(OS_MSG_SIZE)0,
  					(OS_FLAGS )0,
  					(OS_OPT )opt,
  					(CPU_TS )ts,
  					(OS_ERR *)p_err);
  
  		return ((OS_SEM_CTR)0); //返回 0(尚未发布),不继续执行
  	}
  #endif
  
  	ctr = OS_SemPost(p_sem, //将信号量按照普通方式处理
  					 opt,
  					 ts,
  					 p_err);
  
  	return (ctr); //返回信号的当前计数值
  }
  
  ```

  使用实例

  ```c
  OS_SEM SemOfKey; //标志 KEY1 是否被按下的信号量
  OSSemPost((OS_SEM *)&SemOfKey, //发布 SemOfKey
  		  (OS_OPT )OS_OPT_POST_ALL, //发布给所有等待任务
  		  (OS_ERR *)&err); //返回错误类型
  
  ```

  

-  信号量获取函数OSSemPend()

  - 信号量的获取可以在任务中使用.
  - 与释放信号量对应的是获取信号量,我们知道,当信号量有效的时候,任务才能获取信号量,当任务获取了某个信号量的时候,该信号量的可用个数就减一,当它减到0的时候,任务就无法再获取了,并且获取的任务会进入阻塞态(假如用户指定了阻塞超时时间的话).如果某个信号量中当前拥有1个可用的信号量的话,被获取一次就变得无效了,那么此时另外一个任务获取该信号量的时候,就会无法获取成功,该任务便会进入阻塞态,阻塞时间由用户指定.
  - μC/OS支持系统中多个任务获取同一个信号量,假如信号量中已有多个任务在等待,那么这些任务会按照优先级顺序进行排列,如果信号量在释放的时候选择只释放给一个任务,那么在所有等待任务中最高优先级的任务优先获得信号量,而如果信号量在释放的时候选择释放给所有任务,则所有等待的任务都会获取到信号量.

  | 参数    | 含义                                                         |
  | ------- | ------------------------------------------------------------ |
  | *p_sem  | 指向要获取的信号量变量的指针.                                |
  | timeout | 可能是以下几个选项之一.                                      |
  | opt     | 这个参数是设置的是获取不到信号量的时候等待的时间.如果这个值为0,表示一直等待下去,如果这个值不为0,则最多等待timeout 个时钟节拍. |
  | *p_ts   | 指向等待的信号量被删除,等待被强制停止,等待超时等情况时的时间戳的指针. |
  | *p_err  | 指向返回错误类型的指针.                                      |
  
  | 选项                     | 功能                                         |
  | ------------------------ | -------------------------------------------- |
  | OS_OPT_PEND_BLOCKING     | 如果不能即刻获得信号量,选项表示要继续等待.   |
  | OS_OPT_PEND_NON_BLOCKING | 如果不能即刻获得信号量,选项表示不等待信号量. |
  
  | 错误                    | 含义                                                     |
  | ----------------------- | -------------------------------------------------------- |
  | OS_ERR_OBJ_DEL          | 信号量已经被删除了.                                      |
  | OS_ERR_OBJ_PTR_NULL     | 输入的信号量变量指针是空类型.                            |
  | OS_ERR_OBJ_TYPE         | p_sem指向的变量内核对象类型不是信号量.                   |
  | OS_ERR_OPT_INVALID      | 参数opt不符合要求.                                       |
  | OS_ERR_PEND_ABORT       | 等待过程,其他的任务调用了函数OSSemPendAbort强制取消等待. |
  | OS_ERR_PEND_ISR         | 企图在中断中等待信号量.                                  |
  | OS_ERR_PEND_WOULD_BLOCK | 开始获取不到信号量,且没有要求等待.                       |
  | OS_ERR_SCHED_LOCKED     | 调度器被锁住.                                            |
  | OS_ERR_STATUS_INVALID   | 系统出错,导致任务控制块的元素PendStatus不在可能的范围内. |
  | OS_ERR_TIMEOUT          | 等待超时.                                                |
  | OS_ERR_NONE             | 无错误.                                                  |
  
  ```c
  OS_SEM_CTR OSSemPend(OS_SEM *p_sem, //信号量指针
  					 OS_TICK timeout, //等待超时时间
  					 OS_OPT opt, //选项
  					 CPU_TS *p_ts, //等到信号量时的时间戳
  					 OS_ERR *p_err){
  	OS_SEM_CTR ctr;
  	OS_PEND_DATA pend_data;
  	CPU_SR_ALLOC();
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION();//执行安全检测异常函数
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_PEND_ISR; //返回错误类型为"在中断中等待"
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_sem == (OS_SEM *)0) //如果 p_sem 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //返回错误类型为"内核对象为空"
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_PEND_BLOCKING: //如果选择"等待不到对象进行阻塞"
  		case OS_OPT_PEND_NON_BLOCKING: //如果选择"等待不到对象不进行阻塞"
  			break; //直接跳出,不处理
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //返回错误类型为"选项非法"
  			return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_sem->Type != OS_OBJ_TYPE_SEM) //如果 p_sem 不是信号量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //返回错误类型为"内核对象类型错误"
  		return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  #endif
  
  	if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  	{
  		*p_ts = (CPU_TS)0;//初始化(清零)p_ts,待用于返回时间戳
  	}
  	CPU_CRITICAL_ENTER(); //关中断
  	if (p_sem->Ctr > (OS_SEM_CTR)0) //如果资源可用
  	{
  		p_sem->Ctr--; //资源数目减 1
  		if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  		{
  			*p_ts = p_sem->TS; //获取该信号量最后一次发布的时间戳
  		}
  		ctr = p_sem->Ctr; //获取信号量的当前资源数目
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_NONE; //返回错误类型为"无错误"
  		return (ctr);//返回信号量的当前资源数目,不继续执行
  	}
  
  	if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0)//如果没有资源可用,而且选择了不阻塞任务
  	{
  		ctr = p_sem->Ctr; //获取信号量的资源数目到 ctr
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_PEND_WOULD_BLOCK;//返回错误类型为"等待渴求阻塞"
  		return (ctr);//返回信号量的当前资源数目,不继续执行
  	} else {//如果没有资源可用,但选择了阻塞任务
  		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  		{
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_SCHED_LOCKED;//返回错误类型为"调度器被锁"
  			return ((OS_SEM_CTR)0);//返回 0(有错误),不继续执行
  		}
  	}
  
  	OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,并重开中断
  	OS_Pend(&pend_data, //阻塞等待任务,将当前任务脱离就绪列表,
  			(OS_PEND_OBJ *)((void *)p_sem), //并插入节拍列表和等待列表.
  			OS_TASK_PEND_ON_SEM,
              timeout);
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //开调度器,但不进行调度
  
  	OSSched();//找到并调度最高优先级就绪任务
  	/* 当前任务(获得信号量)得以继续运行 */
  	CPU_CRITICAL_ENTER(); //关中断
  	switch (OSTCBCurPtr->PendStatus)//根据当前运行任务的等待状态分类处理
  	{
  		case OS_STATUS_PEND_OK: //如果等待状态正常
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
                  *p_ts = OSTCBCurPtr->TS; //获取信号被发布的时间戳
              }
              *p_err = OS_ERR_NONE; //返回错误类型为"无错误"
  			break;
  		case OS_STATUS_PEND_ABORT: //如果等待被终止中止
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //获取等待被中止的时间戳
  			}
  			*p_err = OS_ERR_PEND_ABORT; //返回错误类型为"等待被中止"
  			break;
  		case OS_STATUS_PEND_TIMEOUT: //如果等待超时
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = (CPU_TS )0; //清零 p_ts
  			}
  			*p_err = OS_ERR_TIMEOUT; //返回错误类型为"等待超时"
  			break;
  		case OS_STATUS_PEND_DEL: //如果等待的内核对象被删除
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //获取内核对象被删除的时间戳
  			}
  			*p_err = OS_ERR_OBJ_DEL;//返回错误类型为"等待对象被删除"
  			break;
          default: //如果等待状态超出预期
  			*p_err = OS_ERR_STATUS_INVALID;//返回错误类型为"等待状态非法"
  			CPU_CRITICAL_EXIT(); //开中断
  			return ((OS_SEM_CTR)0); //返回 0(有错误),不继续执行
  	}
  	ctr = p_sem->Ctr; //获取信号量的当前资源数目
  	CPU_CRITICAL_EXIT(); //开中断
  	return (ctr); //返回信号量的当前资源数目
  }
  
  ```
  
  使用实例
  
  ```c
  OSSemPend ((OS_SEM *)&SemOfKey, //等待该信号量被发布
  		   (OS_TICK )0, //无期限等待
  		   (OS_OPT )OS_OPT_PEND_BLOCKING, //如果没有信号量可用就等待
  		   (CPU_TS *)&ts_sem_post, //获取信号量最后一次被发布的时间戳
  		   (OS_ERR *)&err);
  
  ```

### 使用信号量的注意事项

- 信号量访问共享资源不会导致中断延迟.当任务在执行信号量所保护的共享资源时,ISR或高优先级任务可以抢占该任务.
- 应用中可以有任意个信号量用于保护共享资源.然而,推荐将信号量用于I/O端口的保护,而不是内存地址.
- 信号量经常会被过度使用.很多情况下,访问一个简短的共享资源时不推荐使用信号量,请求和释放信号量会消耗CPU时间.通过关/开中断能更有效地执行这些操作.假设两个任务共享一个32位的整数变量.第一个任务将这个整数变量加1,第二个任务将这个变量清零.考虑到执行这些操作用时很短,不需要使用信号量.执行这个操作前任务只需关中断,执行完毕后再开中断.但是若操作浮点数变量且处理器不支持硬件浮点操作时,就需要用到信号量.因为在这种情况下处理浮点数变量需较长时间.
- 信号量会导致一种严重的问题:优先级翻转.

### 信号量实验

- 在μC/OS中创建了两个任务,一个是获取信号量任务,一个是释放信号量任务,两个任务独立运行,获取信号量任务是一直在等待信号量,其等待时间是无期限等待,等到获取到信号量之后,任务开始执行任务代码,如此反复等待另外任务释放的信号量.

- 释放信号量任务在检测按键是否按下,如果按下则释放信号量,此时释放信号量会唤醒获取任务,获取任务开始运行,然后形成两个任务间的同步,LED 进行翻转,因为如果没按下按键,那么信号量就不会释放,只有当信号量释放的时候,获取信号量的任务才会被唤醒,如此一来就达到任务与任务的同步,同时程序的运行会在串口打印出相关信息.

1. 声明信号量,并定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![信号量实验1_1](.\Image\信号量实验1_1.png)

   ![信号量实验1_2](.\Image\信号量实验1_2.png)

   ![信号量实验1_3](.\Image\信号量实验1_3.png)

2. 定义任务函数

   ![信号量实验2](.\Image\信号量实验2.png)

3. 任务启动函数编写

   ![信号量试验3](.\Image\信号量试验3.png)

4. 结果现象

   按下WKUP键时LED1闪烁.

ps:μC/OS的信号量并没有二值信号量与计数信号量之分,在程序设计时可以通过对任务函数的编写来实现二值信号和技术信号.

## 互斥量

### 互斥量的基本概念

- 互斥量又称互斥信号量(本质也是一种信号量,不具备传递数据功能),是一种特殊的二值信号量,它和信号量不同的是,它支持互斥量所有权,递归访问以及防止优先级翻转的特性,用于实现对临界资源的独占式处理.任意时刻互斥量的状态只有两种,开锁或闭锁.当互斥量被任务持有时,该互斥量处于闭锁状态,这个任务获得互斥量的所有权.当该任务释放这个互斥量时,该互斥量处于开锁状态,任务失去该互斥量的所有权.当一个任务持有互斥量时,其他任务将不能再对该互斥量进行开锁或持有.持有该互斥量的任务也能够再次获得这个锁而不被挂起,这就是递归访问,也就是递归互斥量的特性,这个特性与一般的信号量有很大的不同,在信号量中,由于已经不存在可用的信号量,任务递归获取信号量时会发生主动挂起任务最终形成死锁.
- 如果想要用于实现同步(任务之间或者任务与中断之间),二值信号量或许是更好的选择,虽然互斥量也可以用于任务与任务间同步,但是互斥量更多的是用于保护资源的互锁.
- 用于互锁的互斥量可以充当保护资源的令牌,当一个任务希望访问某个资源时,它必须先获取令牌.当任务使用完资源后,必须还回令牌,以便其他任务可以访问该资源.是不是很熟悉,在我们的二值信号量里面也是一样的,用于保护临界资源,保证多任务的访问井然有序.当任务获取到信号量的时候才能开始使用被保护的资源,使用完就释放信号量,下一个任务才能获取到信号量从而可用使用被保护的资源.但是信号量会导致的另一个潜在问题,那就是任务优先级翻转.而μC/OS提供的互斥量可以通过优先级继承算法,可以降低优先级翻转问题产生的影响,所以,用于临界资源的保护一般建议使用互斥量.

### 互斥量的优先级继承机制

- 在 μC/OS 操作系统中为了降低优先级翻转问题利用了优先级继承算法.                                                                     优先级继承算法是指,暂时提高某个占有某种资源的低优先级任务的优先级,使之与在所有等待该资源的任务中优先级最高那个任务的优先级相等,而当这个低优先级任务执行完毕释放该资源时,优先级重新回到初始设定值.                                                                     因此,继承优先级的任务避免了系统资源被任何中间优先级的任务抢占.                                                                     
- 互斥量与二值信号量最大的不同是:互斥量具有优先级继承机制,而信号量没有.                                                                     也就是说,某个临界资源受到一个互斥量保护,如果这个资源正在被一个低优先级任务使用,那么此时的互斥量是闭锁状态,也代表了没有任务能申请到这个互斥量,如果此时一个高优先级任务想要对这个资源进行访问,去申请这个互斥量,那么高优先级任务会因为申请不到互斥量而进入阻塞态,那么系统会将现在持有该互斥量的任务的优先级临时提升到与高优先级任务的优先级相同(防止优先级较低的任务在使用资源时被较高优先级的任务打断,导致更高优先级的任务要花费更长的时间来等待资源的释放),这个优先级提升的过程叫作优先级继承.                                                                     这个优先级继承机制确保高优先级任务进入阻塞状态的时间尽可能短(更高优先级任务的阻塞时间仅仅是低优先级占用资源时的执行时间),将已经出现的"优先级翻转"危害降低到最小.                                                                     

### 互斥量使用场景

- 互斥量的使用比较单一,因为它是信号量的一种,并且它是以锁的形式存在.在初始化的时候,互斥量处于开锁的状态,而被任务持有的时候则立刻转为闭锁的状态.互斥量更适合于:
  - 可能会引起优先级翻转的情况.                                                                     
  - 任务可能会多次获取互斥量的情况下,这样可以避免同一任务多次递归持有而造成死锁的问题.                                                                     
- 多任务环境下往往存在多个任务竞争同一临界资源的应用场景,互斥量可被用于对临界资源的保护从而实现独占式访问.                                                                     另外,互斥量可以降低信号量存在的优先级翻转问题带来的影响
- 互斥量不能在中断服务函数中使用,因为其特有的优先级继承机制只在任务起作用,而在中断的上下文环境中毫无意义

### 互斥量运作机制

- 多任务环境下会存在多个任务访问同一个临界资源的场景,该资源会被任务单独处理.其他任务在资源被占用的情况下不允许对该临界资源进行访问,这时候就需要用到μC/OS的互斥量来进行资源保护.


- 用互斥量处理不同任务对临界资源的同步访问时,任务想要获得互斥量才能进行资源访问,如果一旦有任务成功获得了互斥量,则互斥量立即变为闭锁状态,此时其他任务会因为获取不到互斥量,而不能访问这个资源,任务会根据用户自定义的等待时间进行等待,直到互斥量被持有的任务释放后,其他任务才能获取互斥量从而得以访问该临界资源,此时互斥量再次上锁,如此一来就可以确保每个时刻只有一个任务正在访问这个临界资源,保证了临界资源操作的安全性.

  ![互斥量运作机制](.\Image\互斥量运作机制.png)

  ①:因为互斥量具有优先级继承机制,一般选择使用互斥量对资源进行保护,如果资源被占用的时候,无论是什么优先级的任务想要使用该资源都会被阻塞.                                                                     

  ②:假如正在使用该资源的任务1比阻塞中的任务2的优先级还低,那么任务1将被系统临时提升到与高优先级任务2相等的优先级(任务1的优先级从L变成H).

  ③:当任务1使用完资源之后,释放互斥量,此时任务1的优先级会从H变回原来的L.

  ④-⑤:任务2此时可以获得互斥量,然后进行资源的访问,当任务2访问了资源的时候,该互斥量的状态又为闭锁状态,其他任务无法获取互斥量.

### 互斥量控制块

- μC/OS的互斥量由多个元素组成,在互斥量被创建时,需要由我们自己定义互斥量(也可以称之为互斥量句柄),因为它是用于保存互斥量的一些信息的,其数据结构`OS_MUTEX`除了互斥量必须的一些基本信息外,还有指向任务控制块的指针`OwnerTCBPtr`,任务优先级变量`OwnerOriginalPrio`,`PendList` 链表与 `OwnerNestingCtr`变量等,为的是方便系统来管理互斥量.

  ![互斥量的控制块数据结构](.\Image\互斥量的控制块数据结构.png)

  ```c
  struct os_mutex
  {
      /* ------------------ GENERIC MEMBERS ------------------ */
      OS_OBJ_TYPE Type; //互斥量的类型
      CPU_CHAR *NamePtr; //互斥量的名字
      OS_PEND_LIST PendList; //等待互斥量的任务代码
  
  #if OS_CFG_DBG_EN > 0u
      OS_MUTEX *DbgPrevPtr;
      OS_MUTEX *DbgNextPtr;
      CPU_CHAR *DbgNamePtr;
  #endif
      /* ------------------ SPECIFIC MEMBERS ------------------ */
      OS_TCB *OwnerTCBPtr; //指向持有互斥量任务控制块的指针,如果任务占用这个mutex,那么该变量OwnerTCBPtr会指向占用这个mutex的任务的OS_TCB.
      OS_PRIO OwnerOriginalPrio; //用于记录持有互斥量任务的优先级,如果任务占用这个mutex,那么该变量OwnerOriginalPrio中存放着任务的原优先级,当占用mutex任务的优先级被提升时就会用到这个变量.
      OS_NESTING_CTR OwnerNestingCtr; //表示互斥量是否可用,当该值为0的时候表示互斥量处于开锁状态,互斥量可用.μC/OS允许任务递归调用同一个mutex多达 256次,每递归调用一次mutex该值就会加一,但也需要释放相同次数才能真正释放掉这个mutex.
      CPU_TS TS; //mutex中的变量TS用于保存该mutex最后一次被释放的时间戳.当mutex被释放,读取时基计数值并存放到该变量中.
  };
  
  ```

  PS:用户代码不能直接访问这个结构体,必须通过μC/OS提供的API访问.

### 互斥量函数接口讲解

| 函数名称        | 函数作用   |
| --------------- | ---------- |
| OSMutexCreate() | 创建互斥量 |
| OSMutexDel()    | 删除互斥量 |
| OSMutexPend()   | 获取互斥量 |
| OSMutexPost()   | 释放互斥量 |

- 创建互斥量函数OSMutexCreate()

  在定义完互斥量结构体变量后就可以调用 OSMutexCreate() 函数进行创建一个互斥量,跟信号量

  的创建差不多,我们知道,其实这里的"创建互斥量"指的就是对内核对象(互斥量)的一些初

  始化.要特别注意的是内核对象使用之前一定要先创建,这个创建过程必须要保证在所有可能使

  用内核对象的任务之前,所以一般我们都是在创建任务之前就创建好系统需要的内核对象(如互

  斥量等).

  | 参数     | 作用                           |
  | -------- | ------------------------------ |
  | *p_mutex | 指向mutex变量的指针.           |
  | *p_name  | 指向mutex变量名字字符串的指针. |
  | *p_err   | 指向返回错误类型指针.          |

  | 错误类型            | 含义                                                         |
  | ------------------- | ------------------------------------------------------------ |
  | OS_ERR_CREATE_ISR   | 任务企图在中断中引用mutex创建函数.                           |
  | OS_ERR_CREATE_ISR   | 参数p_name是空指针.                                          |
  | OS_ERR_OBJ_CREATED  | mutex变量已经被创建,但是OSMutexCreate函数中没有涉及这个错误的相关代码. |
  | OS_ERR_OBJ_PTR_NULL | 变量p_mutex是空指针.                                         |
  | OS_ERR_NONE         | 无错误.                                                      |

  ```c
  void OSMutexCreate(OS_MUTEX *p_mutex, //互斥量指针
  CPU_CHAR *p_name, //取互斥量的名称
  OS_ERR *p_err){
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不继续执行
  	}
  #endif
  
  #ifdef OS_SAFETY_CRITICAL_IEC61508//如果启用(默认禁用)了安全关键
  	//如果是在调用 OSSafetyCriticalStart() 后创建
  	if (OSSafetyCriticalStartFlag == DEF_TRUE)
  	{
          *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME; //错误类型为"非法创建内核对象"
          return; //返回,不继续执行
      }
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
      //如果启用(默认启用)了中断中非法调用检测
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
      {
          *p_err = OS_ERR_CREATE_ISR; //错误类型为"在中断函数中定时"
          return; //返回,不继续执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用(默认启用)了参数检测
      if (p_mutex == (OS_MUTEX *)0) //如果参数 p_mutex 为空
      {
          *p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"创建对象为空"
          return; //返回,不继续执行
      }
  #endif
  
  	OS_CRITICAL_ENTER(); //进入临界段,初始化互斥量指标
  	//标记创建对象数据结构为互斥量
      p_mutex->Type = OS_OBJ_TYPE_MUTEX;
      p_mutex->NamePtr = p_name;
      p_mutex->OwnerTCBPtr = (OS_TCB *)0;
      p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)0;
      p_mutex->TS = (CPU_TS )0;
      p_mutex->OwnerOriginalPrio = OS_CFG_PRIO_MAX;
      OS_PendListInit(&p_mutex->PendList); //初始化该互斥量的等待列表
  
  #if OS_CFG_DBG_EN > 0u//如果启用(默认启用)了调试代码和变量
  	OS_MutexDbgListAdd(p_mutex); //将该互斥量添加到互斥量双向调试链表
  #endif
  	OSMutexQty++; //互斥量个数加 1
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  互斥量创建函数的使用实例

  ```c
  OS_MUTEX mutex; //声明互斥量
  /* 创建互斥量 mutex */
  OSMutexCreate ((OS_MUTEX *)&mutex, //指向互斥量变量的指针
  			   (CPU_CHAR *)"Mutex For Test", //互斥量的名字
  			   (OS_ERR *)&err);
  
  ```

- 删除互斥量函数OSMutexDel()

  互斥量删除函数是根据互斥量结构(互斥量句柄)直接删除的,删除之后这个互斥量的所有信息都会被系统清空,而且不能再次使用这个互斥量了,但是需要注意的是,如果某个互斥量没有被定义,那也是无法被删除的,如果有任务阻塞在该互斥量上,那么尽量不要删除该互斥量.想要使用互斥量删除函数就必须将`OS_CFG_MUTEX_DEL_EN`宏定义配置为1.

  | 参数     | 作用                    |
  | -------- | ----------------------- |
  | *p_mutex | 指向mutex变量指针.      |
  | opt      | 选项可分为以下两种.     |
  | *p_err   | 指向返回错误类型的指针. |

  | 选项               | 功能                                                  |
  | ------------------ | ----------------------------------------------------- |
  | OS_OPT_DEL_NO_PEND | 要在mutex等待列表上没有等待任务的时候才可以删除mutex. |
  | OS_OPT_DEL_ALWAYS  | 不管mutex等待列表上是否有等待的任务都直接删除mutex.   |

  | 错误类型             | 含义                                                         |
  | -------------------- | ------------------------------------------------------------ |
  | OS_ERR_TASK_WAITING  | 参数opt是OS_OPT_DEL_NO_PEND,但是mutex等待列表上有等待的任务. |
  | OS_ERR_STATE_INVALID | 在还原拥有mutex任务优先级的时候是根据任务的状态来的,如果检测到任务的状态超出范围即返回这个错误. |
  | OS_ERR_DEL_ISR       | 从中断中调用删除函数.                                        |
  | OS_ERR_NONE          | 无错误                                                       |

  ```c
  #if OS_CFG_MUTEX_DEL_EN > 0u //如果启用了 OSMutexDel()
  OS_OBJ_QTY OSMutexDel(OS_MUTEX *p_mutex, //互斥量指针
  					  OS_OPT opt, //选项
  					  OS_ERR *p_err){
  	OS_OBJ_QTY cnt;
      OS_OBJ_QTY nbr_tasks;
      OS_PEND_DATA *p_pend_data;
      OS_PEND_LIST *p_pend_list;
      OS_TCB *p_tcb;
      OS_TCB *p_tcb_owner;
      CPU_TS ts;
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
      
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
      if (p_err == (OS_ERR *)0) //如果错误类型实参为空
      {
  	    OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
      	return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
      }
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
      {
          *p_err = OS_ERR_DEL_ISR; //错误类型为"在中断中中止等待"
          return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_mutex == (OS_MUTEX *)0) //如果 p_mutex 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"对象为空"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_DEL_NO_PEND: //如果选项在预期内
  		case OS_OPT_DEL_ALWAYS:
  			break; //直接跳出
  		default: //如果选项超出预期
  
  		*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) //如果 p_mutex 非互斥量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型错误"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  	OS_CRITICAL_ENTER(); //进入临界段
  	p_pend_list = &p_mutex->PendList; //获取互斥量的等待列表
  	cnt = p_pend_list->NbrEntries; //获取等待该互斥量的任务数
  	nbr_tasks = cnt;
  	switch (opt) //根据选项分类处理
  	{
  	case OS_OPT_DEL_NO_PEND: //如果只在没任务等待时删除互斥量
  		if (nbr_tasks == (OS_OBJ_QTY)0) //如果没有任务在等待该互斥量
  		{
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  			OS_MutexDbgListRemove(p_mutex);//将该互斥量从互斥量调试列表移除
  #endif
  			OSMutexQty--; //互斥量数目减 1
  			OS_MutexClr(p_mutex); //清除互斥量内容
  			OS_CRITICAL_EXIT(); //退出临界段
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  		} else {//如果有任务在等待该互斥量
  			OS_CRITICAL_EXIT(); //退出临界段
  			*p_err = OS_ERR_TASK_WAITING; //错误类型为"有任务正在等待"
  		}
  		break; //跳出
  	case OS_OPT_DEL_ALWAYS: //如果必须删除互斥量
  		p_tcb_owner = p_mutex->OwnerTCBPtr; //获取互斥量持有任务
  		if ((p_tcb_owner != (OS_TCB *)0)&&(p_tcb_owner->Prio != p_mutex->OwnerOriginalPrio))//如果持有任务存在,而且优先级被提升过.
  		{
              switch (p_tcb_owner->TaskState) //根据其任务状态处理
              {
                  case OS_TASK_STATE_RDY: //如果是就绪状态
                      OS_RdyListRemove(p_tcb_owner); //将任务从就绪列表移除
                      p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//还原任务的优先级
                      OS_PrioInsert(p_tcb_owner->Prio); //将该优先级插入优先级表格
                      OS_RdyListInsertTail(p_tcb_owner); //将任务重插入就绪列表
                      break; //跳出
                  case OS_TASK_STATE_DLY: //如果是延时状态
                  case OS_TASK_STATE_SUSPENDED: //如果是被挂起状态
                  case OS_TASK_STATE_DLY_SUSPENDED: //如果是延时中被挂起状态
                      p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//还原任务的优先级
                      break;
                  case OS_TASK_STATE_PEND: //如果是无期限等待状态
                  case OS_TASK_STATE_PEND_TIMEOUT: //如果是有期限等待状态
                  case OS_TASK_STATE_PEND_SUSPENDED://如果是无期等待中被挂状态
                  case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED://如果是有期等待中被挂状态
                      //改变任务在等待列表的位置
                      OS_PendListChangePrio(p_tcb_owner,p_mutex->OwnerOriginalPrio);
                      break;
                  default: //如果状态超出预期
                      OS_CRITICAL_EXIT();
                      *p_err = OS_ERR_STATE_INVALID;
                      //错误类型为"任务状态非法"
                      return ((OS_OBJ_QTY)0);
                      //返回 0(有错误),停止执行
              }
          }
  
          ts = OS_TS_GET(); //获取时间戳
          while(cnt > 0u) //移除该互斥量等待列表中的所有任务.
          {
              p_pend_data = p_pend_list->HeadPtr;
              p_tcb = p_pend_data->TCBPtr;
              OS_PendObjDel((OS_PEND_OBJ *)((void *)p_mutex),
                            p_tcb,
                            ts);
              cnt--;
          }
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  		OS_MutexDbgListRemove(p_mutex); //将互斥量从互斥量调试列表移除
  #endif
              
          OSMutexQty--; //互斥量数目减 1
          OS_MutexClr(p_mutex); //清除互斥量内容
          OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段,但不调度
          OSSched(); //调度最高优先级任务运行
          *p_err = OS_ERR_NONE; //错误类型为"无错误"
          break; //跳出
      default: //如果选项超出预期
          OS_CRITICAL_EXIT(); //退出临界段
          *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
          break; //跳出
      }
  	return (nbr_tasks); //返回删除前互斥量等待列表中的任务数
  }
  #endif
  
  ```

  删除互斥量函数的使用实例

  ```c
  OS_SEM mutex;; //声明互斥量
  OS_ERR err;
  
  /* 删除互斥量 mutex*/
  OSMutexDel((OS_MUTEX *)&mutex, //指向互斥量的指针
  		   OS_OPT_DEL_NO_PEND,
  		   (OS_ERR *)&err);
  
  ```

- 获取互斥量函数OSMutexPend()

  当互斥量处于开锁的状态,任务才能获取互斥量成功,当任务持有了某个互斥量的时候,其他任务就无法获取这个互斥量,需要等到持有互斥量的任务进行释放后,其他任务才能获取成功,任务通过互斥量获取函数来获取互斥量的所有权.任务对互斥量的所有权是独占的,任意时刻互斥量只能被一个任务持有,如果互斥量处于开锁状态,那么获取该互斥量的任务将成功获得该互斥量,并拥有互斥量的使用权;如果互斥量处于闭锁状态,获取该互斥量的任务将无法获得互斥量,任务将被挂起,在任务被挂起之前,会进行优先级继承,如果当前任务优先级比持有互斥量的任务优先级高,那么将会临时提升持有互斥量任务的优先级.

  | 参数     | 作用                                                         |
  | -------- | ------------------------------------------------------------ |
  | *p_mutex | 指向mutex 变量指针.                                          |
  | timeout  | 如果任务一开始获取不到信号量等待的节拍数.0表示无限期地等待.  |
  | opt      | 选项有两种.                                                  |
  | *p_ts    | 指向等待的信号量被删除,等待被强制停止,等待超时等情况时的时间戳的指针.这个参数输入空指针表示不想获取时间戳. |
  | *p_err   | 指向返回错误类型的指针.                                      |

  | 选项                     | 功能                                                         |
  | ------------------------ | ------------------------------------------------------------ |
  | OS_OPT_PEND_BLOCKING     | 一开始获取不到信号量就阻塞任务,阻塞的时候根据参数timeout 决定. |
  | OS_OPT_PEND_NON_BLOCKING | 一开始获取不到信号量就退出函数,继续运行任务.                 |

  | 错误类型                | 含义                                                         |
  | ----------------------- | ------------------------------------------------------------ |
  | OS_ERR_MUTEX_OWNER      | 信号量已经被占用.                                            |
  | OS_ERR_PEND_ABORT       | 返回的时候不是因为获得mutex,而是被强制解除等待状态.          |
  | OS_ERR_PEND_ISR         | 从中断中调用等待函数.                                        |
  | OS_ERR_PEND_WOULD_BLOCK | 没有获取到mutex,输入的参数选项是OS_OPT_PEND_NON_BLOCKING的时候. |
  | OS_ERR_SCHED_LOCKED     | 调度器被锁住了.                                              |
  | OS_ERR_TIMEOUT          | 等待mutex超时了                                              |
  | OS_ERR_NONE             | 无错误                                                       |

  ```c
  void OSMutexPend(OS_MUTEX *p_mutex, //互斥量指针
  				 OS_TICK timeout, //超时时间(节拍)
  		 		 OS_OPT opt, //选项
  				 CPU_TS *p_ts, //时间戳
  				 OS_ERR *p_err){
  	OS_PEND_DATA pend_data;
  	OS_TCB *p_tcb;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
      
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_PEND_ISR; //错误类型为"在中断中等待"
  		return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if (p_mutex == (OS_MUTEX *)0) //如果 p_mutex 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //返回错误类型为"内核对象为空"
  		return; //返回,不继续执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_PEND_BLOCKING: //如果选项在预期内
  		case OS_OPT_PEND_NON_BLOCKING:
  			break;
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用了对象类型检测
  	if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) //如果 p_mutex 非互斥量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"内核对象类型错误"
  		return; //返回,不继续执行
  		}
  #endif
  
  	if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  	{
  		*p_ts = (CPU_TS )0; //初始化(清零)p_ts,待用于返回时间戳
  	}
  
  	CPU_CRITICAL_ENTER(); //关中断
  	if (p_mutex->OwnerNestingCtr == (OS_NESTING_CTR)0) //如果互斥量可用
  	{
  
      	p_mutex->OwnerTCBPtr = OSTCBCurPtr; (7)//让当前任务持有互斥量
  	    p_mutex->OwnerOriginalPrio = OSTCBCurPtr->Prio;  //保存持有任务的优先级
      	p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)1; //开始嵌套
  	    if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  		{
  			*p_ts = p_mutex->TS; //返回互斥量的时间戳记录
  		}
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  		return; //返回,不继续执行
  	}
  	/* 如果互斥量不可用 */
  	if (OSTCBCurPtr == p_mutex->OwnerTCBPtr) //如果当前任务已经持有该互斥量
  	{
  		p_mutex->OwnerNestingCtr++; //互斥量嵌套数加 1
  		if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  		{
  			*p_ts = p_mutex->TS; //返回互斥量的时间戳记录
  		}
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_MUTEX_OWNER; //错误类型为"任务已持有互斥量"
  		return; //返回,不继续执行
  	}
  	/* 如果当前任务非持有该互斥量 */
  	if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"渴求阻塞"
  		return; //返回,不继续执行
  	} else {//如果选择了阻塞任务
  		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  		{
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  			return; //返回,不继续执行
  		}
  	}
  	/* 如果调度器未被锁 */
  	OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,并重开中断
  	p_tcb = p_mutex->OwnerTCBPtr; //获取互斥量持有任务
  	if (p_tcb->Prio > OSTCBCurPtr->Prio)
  	//如果持有任务优先级低于当前任务
  	{
          switch (p_tcb->TaskState)
          //根据持有任务的任务状态分类处理
          {
              case OS_TASK_STATE_RDY: //如果是就绪状态
                  OS_RdyListRemove(p_tcb); //从就绪列表移除持有任务
                  p_tcb->Prio = OSTCBCurPtr->Prio; //提升持有任务的优先级到当前任务
                  OS_PrioInsert(p_tcb->Prio); //将该优先级插入优先级表格
                  OS_RdyListInsertHead(p_tcb); //将持有任务插入就绪列表
                  break; //跳出
              case OS_TASK_STATE_DLY: //如果是延时状态
              case OS_TASK_STATE_DLY_SUSPENDED: //如果是延时中被挂起状态
              case OS_TASK_STATE_SUSPENDED: //如果是被挂起状态
                  p_tcb->Prio = OSTCBCurPtr->Prio; //提升持有任务的优先级到当前任务
                  break; //跳出
              case OS_TASK_STATE_PEND: //如果是无期限等待状态
              case OS_TASK_STATE_PEND_TIMEOUT: //如果是有期限等待状态
              case OS_TASK_STATE_PEND_SUSPENDED: //如果是无期限等待中被挂起状态
              case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED: //如果是有期限等待中被挂起状态
                  OS_PendListChangePrio(p_tcb,OSTCBCurPtr->Prio); //改变持有任务在等待列表的位置
                  break; //跳出
              default: (24)//如果任务状态超出预期
                  OS_CRITICAL_EXIT(); //开中断
                  *p_err = OS_ERR_STATE_INVALID; //错误类型为"任务状态非法"
                  return; //返回,不继续执行
          }
  	}
  /*阻塞任务,将当前任务脱离就绪列表,并插入节拍列表和等待列表.*/
  	OS_Pend(&pend_data,
  		    (OS_PEND_OBJ *)((void *)p_mutex),
  			OS_TASK_PEND_ON_MUTEX,
  			timeout);
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //开调度器,但不进行调度
  
  	OSSched(); (26)//调度最高优先级任务运行
  
  	CPU_CRITICAL_ENTER(); //开中断
  	switch (OSTCBCurPtr->PendStatus) //根据当前运行任务的等待状态分类处理
  	{
  		case OS_STATUS_PEND_OK: //如果等待正常(获得互斥量)
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
                  *p_ts = OSTCBCurPtr->TS; //返回互斥量最后一次被释放的时间戳
              }
              *p_err = OS_ERR_NONE; //错误类型为"无错误"
              break; //跳出
  		case OS_STATUS_PEND_ABORT: //如果等待被中止
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //返回等待被中止时的时间戳
  			}
  			*p_err = OS_ERR_PEND_ABORT; //错误类型为"等待被中止"
  			break; //跳出
  		case OS_STATUS_PEND_TIMEOUT: //如果超时内为获得互斥量
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
  	            *p_ts = (CPU_TS )0; //清零 p_ts
              }
      	        *p_err = OS_ERR_TIMEOUT; //错误类型为"超时"
          	    break; //跳出
          case OS_STATUS_PEND_DEL: //如果互斥量已被删除
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
              *p_ts = OSTCBCurPtr->TS; //返回互斥量被删除时的时间戳
              }
              *p_err = OS_ERR_OBJ_DEL; //错误类型为"对象被删除"
              break; //跳出
          default: //根据等待状态超出预期
              *p_err = OS_ERR_STATUS_INVALID;//错误类型为"状态非法"
              break; //跳出
      }
  CPU_CRITICAL_EXIT(); //开中断
  }
  
  ```

  PS:如果任务获取互斥量成功,那么在使用完毕需要立即释放,否则很容易造成其他任务无法获取互斥量,因为互斥量的优先级继承机制是只能将优先级危害降低,而不能完全消除.同时还需注意的是,互斥量是不允许在中断中操作的,因为互斥量特有的优先级继承机制在中断是毫无意义的.

  互斥量获取函数的使用实例

  ```C
  OS_MUTEX mutex; //声明互斥量
  OS_ERR err;
  
  OSMutexPend ((OS_MUTEX *)&mutex, //申请互斥量 mutex
  			 (OS_TICK )0, //无期限等待
  			 (OS_OPT )OS_OPT_PEND_BLOCKING, //如果不能申请到互斥量就阻塞任务
  			 (CPU_TS *)0, //不想获得时间戳
  			 (OS_ERR *)&err);
  
  ```

- 释放互斥量函数OSMutexPost()

  - 任务想要访问某个资源的时候,需要先获取互斥量,然后进行资源访问,在任务使用完该资源的时候,必须要及时归还互斥量,这样别的任务才能对资源进行访问.在前面的讲解中,我们知道,当互斥量有效的时候,任务才能获取互斥量,那么,是什么函数使得互斥量变得有效呢?μC/OS给我们提供了互斥量释放函数 `OSMutexPost()`,任务可以调用该函数进行释放互斥量,表示我已经用完了,别人可以申请使用,但是要注意的是,互斥量的释放只能在任务中,不允许在中断中释放互斥量.
  - 使用该函数接口时,只有已持有互斥量所有权的任务才能释放它,当任务调用`OSMutexPost()`函数时会释放一次互斥量,当互斥量的成员变量`OwnerNestingCtr`为 0 的时候,互斥量状态才会成为开锁状态,等待获取该互斥量的任务将被唤醒.如果任务的优先级被互斥量的优先级翻转机制临时提升,那么当互斥量被完全释放后,任务的优先级将恢复为原本设定的优先级.

  | 参数     | 作用                              |
  | -------- | --------------------------------- |
  | *p_mutex | 指向Mutex变量的指针.              |
  | opt      | 提交Mutex时的选项,可以为以下两种. |
  | *p_err   | 指向返回错误类型指针.             |
  
  | 选项                 | 功能                                      |
  | -------------------- | ----------------------------------------- |
  | OS_OPT_POST_NO_SCHED | 发布mutex后不要进行调度.                  |
  | OS_OPT_POST_NONE     | 这个是默认的选项,发布mutex后进行任务调度. |
  
  | 错误                   | 含义                                                         |
  | ---------------------- | ------------------------------------------------------------ |
  | OS_ERR_MUTEX_NESTING   | Mutex被发布后,仍然处于嵌套中,即前面提到的元素OwnerNestingCtr还不为0,Mutex还不能为其他任务占有. |
  | OS_ERR_MUTEX_NOT_OWNER | 只有拥有mutex的任务才可以释放这个mutex.当在一个不是拥有mutex 的任务中释放mutex的时候,返回这个错误. |
  | OS_ERR_OBJ_PTR_NULL    | 参数p_mutex是空指针.                                         |
  | OS_ERR_OBJ_TYPE        | 参数p_mutex指向的内核变量类型不是mutex.                      |
  | OS_ERR_POST_ISR        | ISR不是任务,不可能拥有一个mutex,所以也不能在中断中提交mutex. |
  | OS_ERR_NONE            | 无错误.                                                      |
  
  ```C
  void OSMutexPost(OS_MUTEX *p_mutex, //互斥量指针
  				 OS_OPT opt, //选项
  				 OS_ERR *p_err){
  	OS_PEND_LIST *p_pend_list;
  	OS_TCB *p_tcb;
  	CPU_TS ts;
  	CPU_SR_ALLOC();//使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_POST_ISR; //错误类型为"在中断中等待"
  		return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_mutex == (OS_MUTEX *)0) //如果 p_mutex 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"内核对象为空"
  		return; //返回,不继续执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_POST_NONE: //如果选项在预期内,不处理
  		case OS_OPT_POST_NO_SCHED:
  			break;
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return; //返回,不继续执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u(7)//如果启用了对象类型检测
  	if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) //如果 p_mutex 的类型不是互斥量类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //返回,不继续执行
  		return;
  	}
  #endif
  
  	CPU_CRITICAL_ENTER(); //关中断
  	if(OSTCBCurPtr != p_mutex->OwnerTCBPtr) //如果当前运行任务不持有该互斥量
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_MUTEX_NOT_OWNER; //错误类型为"任务不持有该互斥量"
  		return; //返回,不继续执行
  	}
  
  	OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,开中断
  	ts = OS_TS_GET(); //获取时间戳
  	p_mutex->TS = ts;
  	//存储互斥量最后一次被释放的时间戳
  	p_mutex->OwnerNestingCtr--; //互斥量的嵌套数减 1
  	if (p_mutex->OwnerNestingCtr > (OS_NESTING_CTR)0) //如果互斥量仍被嵌套
  	{
  		OS_CRITICAL_EXIT(); //解锁调度器
  		*p_err = OS_ERR_MUTEX_NESTING; //错误类型为"互斥量被嵌套"
  		return; //返回,不继续执行
  	}
  	/* 如果互斥量未被嵌套,已可用 */
  	p_pend_list = &p_mutex->PendList; //获取互斥量的等待列表
  	if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) //如果没有任务在等待该互斥量
  	{
  		p_mutex->OwnerTCBPtr = (OS_TCB *)0; //清空互斥量持有者信息
  		p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)0;
  		OS_CRITICAL_EXIT(); //解锁调度器
  		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  		return; //返回,不继续执行
  	}
  	/* 如果有任务在等待该互斥量 */
  	if (OSTCBCurPtr->Prio != p_mutex->OwnerOriginalPrio) //如果当前任务的优先级被改过
  	{
  		OS_RdyListRemove(OSTCBCurPtr); //从就绪列表移除当前任务
  		OSTCBCurPtr->Prio = p_mutex->OwnerOriginalPrio; //还原当前任务的优先级
  		OS_PrioInsert(OSTCBCurPtr->Prio); //在优先级表格插入这个优先级
  		OS_RdyListInsertTail(OSTCBCurPtr); //将当前任务插入就绪列表尾端
  		OSPrioCur = OSTCBCurPtr->Prio; //更改当前任务优先级变量的值
  	}
  
  	p_tcb = p_pend_list->HeadPtr->TCBPtr; //获取等待列表的首端任务
  	p_mutex->OwnerTCBPtr = p_tcb; //将互斥量交给该任务
  	p_mutex->OwnerOriginalPrio = p_tcb->Prio;
  	p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)1; //开始嵌套
  	/* 释放互斥量给该任务 */
  	OS_Post((OS_PEND_OBJ *)((void *)p_mutex),
  	(OS_TCB *)p_tcb,
  	(void *)0,
  	(OS_MSG_SIZE )0,
  	(CPU_TS )ts);
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //减锁调度器,但不执行任务调度
  
  	if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) //如果 opt 没选择"发布时不调度任务"
  	{
  		OSSched(); //任务调度
  	}
  
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```
  
  已经获取到互斥量的任务拥有互斥量的所有权,能重复获取同一个互斥量,但是任务获取了多少次互斥量就要释放多少次互斥量才能彻底释放掉互斥量,互斥量的状态才会变成开锁状态,否则在此之前互斥量都处于无效状态,别的任务就无法获取该互斥量.使用该函数接口时,只有已持有互斥量所有权的任务才能释放它,每释放一次该互斥量,它的`OwnerNestingCtr`成员变量就减1.当该互斥量的`OwnerNestingCtr`成员变量为0时(即持有任务已经释放所有的持有操作),互斥量则变为开锁状态,等待在该互斥量上的任务将被唤醒.如果任务的优先级被互斥量的优先级翻转机制临时提升,那么当互斥量被释放后,任务的优先级将恢复为原本设定的优先级.
  
  互斥量释放函数使用实例
  
  ```c
  OS_MUTEX mutex; //声明互斥互斥量
  OS_ERR err;
  OSMutexPost ((OS_MUTEX *)&mutex, //释放互斥互斥量 mutex
  			 (OS_OPT )OS_OPT_POST_NONE, //进行任务调度
  			 (OS_ERR *)&err);
  
  ```

### 实验

#### 模拟优先级翻转实验

模拟优先级翻转实验是在μC/OS中创建了三个任务与一个二值信号量,任务分别是高优先级任务 `AppTaskLed3`,中优先级任务`AppTaskLed2`,低优先级任务`AppTaskLed1`,用于模拟产生优先级翻转.低优先级任务在获取信号量的时候,被中优先级打断,中优先级的任务开始执行,因为低优先级还未释放信号量,那么高优先级任务就无法取得信号量继续运行,此时就发生了优先级翻转,任务在运行中,使用串口打印出相关信息.

1. 声明信号量,并定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![优先级翻转1_1](.\Image\优先级翻转1_1.png)

   ![优先级翻转1_2](.\Image\优先级翻转1_2.png)

   ![优先级翻转1_3](.\Image\优先级翻转1_3.png)

2. 定义任务函数

   ![优先级翻转2](.\Image\优先级翻转2.png)

3. 任务启动函数编写

   ![优先级翻转3](.\Image\优先级翻转3.png)

4. 结果现象

   ![优先级翻转4](.\Image\优先级翻转4.png)

   低优先级的LED1任务被较高优先级LED2打断,导致高优先级的LED3无法继续运行.

#### 互斥量实验

互斥量实验是基于优先级翻转实验进行修改的,将信号量改为互斥量,目的是为了测试互斥量的优先级继承机制是否有效.

1. 声明互斥信号量,并定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![互斥量实验1_1](.\Image\互斥量实验1_1.png)

   ![互斥量实验1_2](.\Image\互斥量实验1_2.png)

   ![互斥量实验1_3](.\Image\互斥量实验1_3.png)

2. 定义任务函数

   ![互斥量实验2](.\Image\互斥量实验2.png)

3. 任务启动函数编写

   ![互斥量实验3](.\Image\互斥量实验3.png)

4. 结果现象

   ![互斥量实验4](.\Image\互斥量实验4.png)

   低优先级的LED1任务经优先级继承后并未被较高优先级的LED2任务打断.

## 事件

### 事件的基本概念

- 事件是一种实现任务间通信的机制,主要用于实现多任务间的同步,但事件通信只能是事件类型的通信,无数据传输.与信号量不同的是,它可以实现一对多,多对多的同步.即一个任务可以等待多个事件的发生:可以是任意一个事件发生时唤醒任务进行事件处理;也可以是几个事件都发生后才唤醒任务进行事件处理.同样,也可以是多个任务同步多个事件.
- 每一个事件组只需要很少的RAM空间来保存事件组的状态.事件组存储在一个`OS_FLAGS`类型的`Flags`变量中,该变量在事件结构体中定义.而变量的宽度由我们自己定义,可以是8位,16位,32位的变量,取决于os_type.h中的 OS_FLAGS的位数.在STM32中,我们一般将其定义为32位的变量,有32个位用来实现事件标志组.每一位代表一个事件,任务通过"逻辑与"或"逻辑或"与一个或多个事件建立关联,形成一个事件组.事件的"逻辑或"也被称作是独立型同步,指的是任务感兴趣的所有事件任一件发生即可被唤醒;事件"逻辑与"则被称为是关联型同步,指的是任务感兴趣的若干事件都发生时才被唤醒,并且事件发生的时间可以不同步.
- 多任务环境下,任务,中断之间往往需要同步操作,一个事件发生会告知等待中的任务,即形成一个任务与任务,中断与任务间的同步.事件可以提供一对多,多对多的同步操作.一对多同步模型:一个任务等待多个事件的触发,这种情况是比较常见的;多对多同步模型:多个任务等待多个事件的触发.
- 任务可以通过设置事件位来实现事件的触发和等待操作.μC/OS的事件仅用于同步,不提供数据传输功能.
- μC/OS提供的事件具有如下特点
  - 事件只与任务相关联,事件相互独立,一个32位(数据宽度由用户定义)的事件集合用于标识该任务发生的事件类型,其中每一位表示一种事件类型(0表示该事件类型未发生,1表示该事件类型已经发生),一共 32 种事件类型.
  - 事件仅用于同步,不提供数据传输功能.
  - 事件无排队性,即多次向任务设置同一事件(如果任务还未来得及读走),等效于只设置一次.
  - 允许多个任务对同一事件进行读写操作.
  - 支持事件等待超时机制.
  - 支持显式清除事件.
- 在μC/OS的等待事件中,用户可以选择感兴趣的事件,并且选择等待事件的选项,它有4个属性,分别是逻辑与,逻辑或,等待所有事件清除或者等待任意事件清除.当任务等待事件同步时,可以通过任务感兴趣的事件位和事件选项来判断当前获取的事件是否满足要求,如果满足则说明任务等待到对应的事件,系统将唤醒等待的任务;否则,任务会根据用户指定的阻塞超时时间继续等待下去.

### 事件的应用场景

- μC/OS的事件用于事件类型的通讯,无数据传输,也就是说,我们可以用事件来做标志位,判断某些事件是否发生了,然后根据结果做处理,那很多人又会问了,为什么我不直接用变量做标志呢,岂不是更好更有效率?非也非也,若是在裸机编程中,用全局变量是最为有效的方法,这点我不否认,但是在操作系统中,使用全局变量就要考虑以下问题了:

  - 如何对全局变量进行保护呢,如何处理多任务同时对它进行访问?
  - 如何让内核对事件进行有效管理呢?使用全局变量的话,就需要在任务中轮询查看事件是否发送,这简直就是在浪费 CPU 资源啊,还有等待超时机制,使用全局变量的话需要用户自己去实现.

  在操作系统中,使用操作系统给我们提供的通信机制就可,简单方便还实用.

- 在某些场合,可能需要多个时间发生了才能进行下一步操作,比如一些危险机器的启动,需要检查各项指标,当指标不达标的时候,无法启动,但是检查各个指标的时候,不能一下子检测完毕啊,所以,需要事件来做统一的等待,当所有的事件都完成了,那么机器才允许启动,这只是事件的其中一个应用.

- 事件可使用于多种场合,它能够在一定程度上替代信号量,用于任务与任务间,中断与任务间的同步.一个任务或中断服务例程发送一个事件给事件对象,而后等待的任务被唤醒并对相应的事件进行处理.但是它与信号量不同的是,事件的发送操作是不可累计的,而信号量的释放动作是可累计的.事件另外一个特性是,接收任务可等待多种事件,即多个事件对应一个任务或多个任务.同时按照任务等待的参数,可选择是"逻辑或"触发还是"逻辑与"触发.这个特性也是信号量等所不具备的,信号量只能识别单一同步动作,而不能同时等待多个事件的同步.

- 各个事件可分别发送或一起发送给事件对象,而任务可以等待多个事件,任务仅对感兴趣的事件进行关注.当有它们感兴趣的事件发生时并且符合感兴趣的条件,任务将被唤醒并进行后续的处理动作.

### 事件运作机制

- 等待(接收)事件时,可以根据感兴趣的参事件类型等待事件的单个或者多个事件类型.事件等待成功后,必须使用`OS_OPT_PEND_FLAG_CONSUME`选项来清除已接收到的事件类型,否则不会清除已接收到的事件,这样就需要用户显式清除事件位.用户可以自定义通过传入opt选项来选择读取模式,是等待所有感兴趣的事件还是等待感兴趣的任意一个事件.

- 设置事件时,对指定事件写入指定的事件类型,设置事件集合的对应事件位为1,可以一次同时写多个事件类型,设置事件成功可能会触发任务调度.

- 清除事件时,根据入参数事件句柄和待清除的事件类型,对事件对应位进行清零操作.

- 事件不与任务相关联,事件相互独立,一个32位的变量就是事件的集合,用于标识该任务发生的事件类型,其中每一位表示一种事件类型(0表示该事件类型未发生,1表示该事件类型已经发生),一共32种事件类型.

  ![事件集合Flags](.\Image\事件集合Flags.png)

- 事件唤醒机制,当任务因为等待某个或者多个事件发生而进入阻塞态,当事件发生的时候会被唤醒.

  ![事件唤醒任务](.\Image\事件唤醒任务.png)

  任务1对事件3或事件5感兴趣(逻辑或),当发生其中的某一个事件都会被唤醒,并且执行相应操作.而任务2对事件3与事件5感兴趣(逻辑与),当且仅当事件3与事件5都发生的时候,任务2才会被唤醒,如果只有一个其中一个事件发生,那么任务还是会继续等待事件发生.如果在接收事件函数中设置了清除事件位选项 `OS_OPT_PEND_FLAG_CONSUME`,那么当任务唤醒后将把事件3和事件5的事件标志清零,否则事件标志将依然存在.

### 事件控制块

- 理论上用户可以创建任意个事件(仅限制于处理器的 RAM 大小).通过设置os_cfg.h中的宏定义`OS_CFG_FLAG_EN`为1即可开启事件功能.事件是一个内核对象,由数据类型`OS_FLAG_GRP`定义,该数据类型由`os_flflag_grp`定义(在 os.h 文件).

- μC/OS的事件由多个元素组成,在事件被创建时,需要由我们自己定义事件(也可以称之为事件句柄),因为它是用于保存事件的一些信息的,其数据结构`OS_FLAG_GRP`除了事件必须的一些基本信息外,还有`PendList` 链表与一个32位的事件组变量`Flags`等,为的是方便系统来管理事件.

- 事件控制块数据结构

  ![事件控制块数据结构](.\Image\事件控制块数据结构.png)

  ```c
  struct os_flag_grp
  {
  /* ------------------ GENERIC MEMBERS ------------------ */
      OS_OBJ_TYPE Type; //事件的类型
      CPU_CHAR *NamePtr; //事件的名称
      OS_PEND_LIST PendList; //因为可以有多个任务同时等待系统中的事件,所以事件中包含了一个用于控制挂起任务列表的结构体,用于记录阻塞在此事件上的任务
  
  #if OS_CFG_DBG_EN > 0u
      OS_FLAG_GRP *DbgPrevPtr;
      OS_FLAG_GRP *DbgNextPtr;
      CPU_CHAR *DbgNamePtr;
  #endif
  /* ------------------ SPECIFIC MEMBERS ------------------ */
      OS_FLAGS Flags; //事件中包含了很多标志位,Flags这个变量中保存了当前这些标志位的状态.这个变量可以为8位,16位或32位.
      CPU_TS TS; //事件中的变量TS用于保存该事件最后一次被释放的时间戳.当事件被释放时,读取时基计数值并存放到该变量中.
  };
  
  ```

  PS:用户代码不能直接访问这个结构体,必须通过μC/OS提供的API访问.

### 事件函数接口

| 函数名称       | 函数作用                                                |
| -------------- | ------------------------------------------------------- |
| OSFlagCreate() | 创建事件                                                |
| OSFlagDel()    | 删除事件                                                |
| OSFlagPost()   | 设置事件                                                |
| OSFlagPend()   | 等待事件                                                |
| OS_FlagPost()  | 如果没有启用中断延迟发布,则直接将该事件对应的标志位置位 |

- 事件创建函数OSFlagCreate()

  事件创建函数,顾名思义,就是创建一个事件,与其他内核对象一样,都是需要先创建才能使用的资源,μC/OS给我们提供了一个创建事件的函数`OSFlagCreate()`,当创建一个事件时,系统会对我们定义的事件控制块进行基本的初始化.在使用创建函数之前,需要先定义一个事件控制块(句柄).

  | 参数    | 含义                           |
  | ------- | ------------------------------ |
  | *p_grp  | 指向事件标志组变量指针         |
  | *p_name | 指向事件标志组名字字符串的指针 |
  | flags   | 标志初始值                     |
  | *p_err  | 指向返回错误类型的指针         |

  ```c
  void OSFlagCreate (OS_FLAG_GRP *p_grp, //事件指针
  				   CPU_CHAR *p_name, //命名事件
  				   OS_FLAGS flags, //标志初始值
  				   OS_ERR *p_err){
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL //如果启用了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,停止执行
  	}
  #endif
  
  #ifdef OS_SAFETY_CRITICAL_IEC61508 //如果启用了安全关键
  	if (OSSafetyCriticalStartFlag == DEF_TRUE) //如果afetyCriticalStart()后创建
  	{
  		*p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME; //错误类型为"非法创建内核对象"
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		*p_err = OS_ERR_CREATE_ISR; //错误类型为"在中断中创建对象"
  		return; //返回,停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_grp == (OS_FLAG_GRP *)0) //如果 p_grp 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"创建对象为空"
  		return; //返回,停止执行
  	}
  #endif
  
  	OS_CRITICAL_ENTER(); //进入临界段
  	p_grp->Type = OS_OBJ_TYPE_FLAG; //标记创建对象数据结构为事件
  	p_grp->NamePtr = p_name; //标记事件的名称
  	p_grp->Flags = flags; //设置标志初始值
  	p_grp->TS = (CPU_TS)0; //清零事件的时间戳
  	OS_PendListInit(&p_grp->PendList); //初始化该事件的等待列表
  
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  	OS_FlagDbgListAdd(p_grp); //将该事件添加到事件双向调试链表
  #endif
  	OSFlagQty++; //事件个数加1
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  事件创建函数的使用实例

  ```c
  OS_FLAG_GRP flag_grp; //声明事件
  OS_ERR err;
  
  /* 创建事件 flag_grp */
  OSFlagCreate ((OS_FLAG_GRP *)&flag_grp, //指向事件的指针
  			  (CPU_CHAR *)"FLAG For Test", //事件的名字
  			  (OS_FLAGS )0, //事件的初始值
  			  (OS_ERR *)&err);
  
  ```

  

- 事件删除函数OSFlagDel()

  | 参数   | 含义                   |
  | ------ | ---------------------- |
  | *p_grp | 指向事件变量的指针     |
  | opt    | 删除事件量时候的选项   |
  | *p_err | 指向返回错误类型的指针 |

  | 选项               | 功能                                                         |
  | ------------------ | ------------------------------------------------------------ |
  | OS_OPT_DEL_NO_PEND | 当事件的等待列表上面没有相应的任务的时候才删除事件           |
  | OS_OPT_DEL_ALWAYS  | 不管事件的等待列表是否有相应的任务都删除事件,删除之前,系统会把所有阻塞在该事件上的任务恢复 |

  | 错误类型            | 含义                                                         |
  | ------------------- | ------------------------------------------------------------ |
  | OS_ERR_DEL_ISR      | 企图在中断中删除事件                                         |
  | OS_ERR_OBJ_PTR_NULL | 参数p_grp是空指针                                            |
  | OS_ERR_OBJ_TYPE     | 参数p_grp指向的内核变量类型不是事件                          |
  | OS_ERR_OPT_INVALID  | opt在给出的选项之外                                          |
  | OS_ERR_TASK_WAITING | 在选项opt是OS_OPT_DEL_NO_PEND的时候,并且事件的等待列表上有等待的任务 |
  | OS_ERR_NONE         | 无错误                                                       |

  ```c
  #if OS_CFG_FLAG_DEL_EN > 0u//如果启用了 OSFlagDel() 函数
  OS_OBJ_QTY OSFlagDel(OS_FLAG_GRP *p_grp, //事件指针
  					 OS_OPT opt, //选项
  					 OS_ERR *p_err){
  	OS_OBJ_QTY cnt;
  	OS_OBJ_QTY nbr_tasks;
  	OS_PEND_DATA *p_pend_data;
  	OS_PEND_LIST *p_pend_list;
  	OS_TCB *p_tcb;
  	CPU_TS ts;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u //如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_DEL_ISR; //错误类型为"在中断中删除对象"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用了参数检测
  	if (p_grp == (OS_FLAG_GRP *)0) //如果 p_grp 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"对象为空"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_DEL_NO_PEND: //如果选项在预期内
  		case OS_OPT_DEL_ALWAYS:
  			break; //直接跳出
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u //如果启用了对象类型检测
  	if (p_grp->Type != OS_OBJ_TYPE_FLAG) //如果 p_grp 不是事件类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型有误"
  		return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  	OS_CRITICAL_ENTER(); //进入临界段
  	p_pend_list = &p_grp->PendList; //获取消息队列的等待列表
  	cnt = p_pend_list->NbrEntries; //获取等待该队列的任务数
  	nbr_tasks = cnt; //按照任务数目逐个处理
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_DEL_NO_PEND: //如果只在没任务等待时进行删除
  			if (nbr_tasks == (OS_OBJ_QTY)0) //如果没有任务在等待该事件
  			{
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  				OS_FlagDbgListRemove(p_grp); //将该事件从事件调试列表移除
  #endif
  				OSFlagQty--; //事件数目减1
  				OS_FlagClr(p_grp); //清除该事件的内容
  	
  				OS_CRITICAL_EXIT(); //退出临界段
  				*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			} else {
  				OS_CRITICAL_EXIT(); //退出临界段
  				*p_err = OS_ERR_TASK_WAITING; //错误类型为"有任务在等待事件"
  			}
  			break; //跳出
  		case OS_OPT_DEL_ALWAYS: //如果必须删除事件
  			ts = OS_TS_GET(); //获取时间戳
  			while (cnt > 0u) //逐个移除该事件等待列表中的任务
  			{
  				p_pend_data = p_pend_list->HeadPtr;
  				p_tcb = p_pend_data->TCBPtr;
  				OS_PendObjDel((OS_PEND_OBJ *)((void *)p_grp),
  							  p_tcb,
  							  ts);
  				cnt--;
  			}
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
  			OS_FlagDbgListRemove(p_grp); //将该事件从事件调试列表移除
  #endif
  			OSFlagQty--; //事件数目减 1
  			OS_FlagClr(p_grp); //清除该事件的内容
  			OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  			OSSched(); //调度任务
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			break; //跳出
          default: //如果选项超出预期
              OS_CRITICAL_EXIT(); //退出临界段
              *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
              break; //跳出
      }
  	return (nbr_tasks); //返回删除事件前等待其的任务数
  }
  #endif
  
  ```

  删除事件函数OSFlagDel()的使用实例

  ```c
  OS_FLAG_GRPflag_grp;; //声明事件句柄
  OS_ERR err;
  
  /* 删除事件 */
  OSFlagDel((OS_FLAG_GRP*)&flag_grp, //指向事件的指针
  		  OS_OPT_DEL_NO_PEND,
  		  (OS_ERR *)&err);
  
  ```

  事件删除函数`OSFlagDel()`使用时传入要删除的事件的句柄与选项并保存返回的错误类型即可,调用函数时,系统将删除这个事件.需要注意的是在调用删除事件函数前,系统应存在已创建的事件.如果删除事件时,系统中有任务正在等待该事件,则不应该进行删除操作.

- 事件设置函数OSFlagPost()

  `OSFlagPost()`用于设置事件组中指定的位,当位被置位之后,并且满足任务的等待事件,那么等待在事件该标志位上的任务将会被恢复.使用该函数接口时,通过参数指定的事件标志来设置事件的标志位,然后遍历等待在事件对象上的事件等待列表,判断是否有任务的事件激活要求与当前事件对象标志值匹配,如果有,则唤醒该任务.简单来说,就是设置自己定义的事件标志位为1,并且看看有没有任务在等待这个事件,有的话就唤醒它.

  | 参数   | 含义                   |
  | ------ | ---------------------- |
  | *p_grp | 指向要提交的事件的指针 |
  | flags  | 想要设置的标志位       |
  | opt    | 置位选项               |
  | *p_err | 指向返回错误类型的指针 |

  | 选项                 | 功能                         |
  | -------------------- | ---------------------------- |
  | OS_OPT_POST_FLAG_SET | 参数flags选定的标志位全部置1 |
  | OS_OPT_POST_FLAG_CLR | 参数flags选定的标志位全部置0 |

  | 错误类型            | 含义                                  |
  | ------------------- | ------------------------------------- |
  | OS_ERR_OBJ_PTR_NULL | 参数p_grp是空指针                     |
  | OS_ERR_OBJ_TYPE     | 参数p_grp指向的变量类型不是事件标志组 |
  | OS_ERR_OPT_INVALID  | 参数opt不是指定的选项之一             |
  | OS_ERR_NONE         | 无错误                                |

  ```c
  OS_FLAGS OSFlagPost (OS_FLAG_GRP *p_grp, //事件指针
  					 OS_FLAGS flags, //选定要操作的标志位
  					 OS_OPT opt, //选项
  					 OS_ERR *p_err){
  	OS_FLAGS flags_cur;
  	CPU_TS ts;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测
  	if (p_grp == (OS_FLAG_GRP *)0) //如果参数 p_grp 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"事件对象为空"
  		return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_POST_FLAG_SET: //如果选项在预期之内
  		case OS_OPT_POST_FLAG_CLR:
  		case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
  		case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
  			break; //直接跳出
          default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用了对象类型检测
  	if (p_grp->Type != OS_OBJ_TYPE_FLAG) //如果 p_grp 不是事件类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型"对象类型有误"
  		return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  #endif
  
  	ts = OS_TS_GET(); //获取时间戳
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u //如果启用了中断延迟发布
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_FLAG,//将该事件发布到中断消息队列
  					(void *)p_grp,
  					(void *)0,
  					(OS_MSG_SIZE)0,
  					(OS_FLAGS )flags,
  					(OS_OPT )opt,
  					(CPU_TS )ts,
  				(OS_ERR *)p_err);
  		return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  #endif
  /* 如果没有启用中断延迟发布 */
  	flags_cur = OS_FlagPost(p_grp, //将事件直接发布
  							flags,
  							opt,
  							ts,
  							p_err);
  	return (flags_cur); //返回当前标志位的值
  }
  
  ```

  事件设置函数OSFlagPost()使用实例

  ```c
  #define EVENT1 (0x01<<0) //设置事件掩码的位0
  #define EVENT2 (0x01<<1) //设置事件掩码的位1
  
  OS_FLAG_GRP flag_grp; //声明事件标志组
  
  void AppTaskPost(void * p_arg){
      OS_ERR err;
      (void)p_arg;
      while(DEF_TRUE){
          if("EVENT1 has occurred"){
              OSFlagPost ((OS_FLAG_GRP *)&flag_grp, //将标志组的 BIT0 置 1
  						(OS_FLAGS )EVENT1,
  						(OS_OPT )OS_OPT_POST_FLAG_SET,
  						(OS_ERR *)&err);
          } else if("EVENT1 has occurred"){
               OSFlagPost ((OS_FLAG_GRP *)&flag_grp, //将标志组的 BIT0 置 1
  						(OS_FLAGS )EVENT2,
  						(OS_OPT )OS_OPT_POST_FLAG_SET,
  						(OS_ERR *)&err);
          } else {}
          OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_DLY,&err); //每20ms检测一次
      }
  }
  
  ```

- 标志位置位函数OS_FlagPost()

  OS_FlagPost()函数可直接将该事件对应的标志位置位.

  | 参数   | 含义                   |
  | ------ | ---------------------- |
  | *p_grp | 指向要提交的事件的指针 |
  | flags  | 想要设置的标志位       |
  | opt    | 置位选项               |
  | ts     | 时间戳                 |
  | *p_err | 指向返回错误类型的指针 |

  ```c
  OS_FLAGS OS_FlagPost (OS_FLAG_GRP *p_grp, //事件指针
  					  OS_FLAGS flags, //选定要操作的标志位
  					  OS_OPT opt, //选项
  					  CPU_TS ts, //时间戳
  					  OS_ERR *p_err){
  	OS_FLAGS flags_cur;
  	OS_FLAGS flags_rdy;
  	OS_OPT mode;
  	OS_PEND_DATA *p_pend_data;
  	OS_PEND_DATA *p_pend_data_next;
  	OS_PEND_LIST *p_pend_list;
  	OS_TCB *p_tcb;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  	
  	CPU_CRITICAL_ENTER(); //关中断
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_POST_FLAG_SET: //如果要求将选定位置 1
  		case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
  			p_grp->Flags |= flags; //将选定位置 1
  			break; //跳出
  		case OS_OPT_POST_FLAG_CLR: //如果要求将选定位请 0
  		case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
  			p_grp->Flags &= ~flags; //将选定位请 0
  			break; //跳出
  		default: //如果选项超出预期
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return ((OS_FLAGS)0); //返回 0,停止执行
  	}
  	p_grp->TS = ts; //将时间戳存入事件
  	p_pend_list = &p_grp->PendList; //获取事件的等待列表
  	if (p_pend_list->NbrEntries == 0u) /如果没有任务在等待事件
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  		return (p_grp->Flags); //返回事件的标志值
  	}
  	/* 如果有任务在等待事件 */
  	OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  	p_pend_data = p_pend_list->HeadPtr; //获取等待列表头个等待任务
  	p_tcb = p_pend_data->TCBPtr;
  	while (p_tcb != (OS_TCB *)0)//从头至尾遍历等待列表的所有任务
  	{
  		p_pend_data_next = p_pend_data->NextPtr;
  		mode = p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK; //获取任务的标志选项
  		switch (mode) //根据任务的标志选项分类处理
  		{
  			case OS_OPT_PEND_FLAG_SET_ALL: //如果要求任务等待的标志位都得置 1
  				flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend);
  				if (flags_rdy == p_tcb->FlagsPend) //如果任务等待的标志位都置 1 了
  				{
  					OS_FlagTaskRdy(p_tcb, //让该任务准备运行
  								   flags_rdy,
  								   ts);
  				}
  				break; //跳出
  			case OS_OPT_PEND_FLAG_SET_ANY://如果要求任务等待的标志位有 1 位置 1 即可
  				flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend);
  				if (flags_rdy != (OS_FLAGS)0) //如果任务等待的标志位有置 1 的
  				{
  					OS_FlagTaskRdy(p_tcb, //让该任务准备运行
  								   flags_rdy,
  								   ts);
  				}
  				break; //跳出
  
  #if OS_CFG_FLAG_MODE_CLR_EN > 0u //如果启用了标志位清零触发模式
  			case OS_OPT_PEND_FLAG_CLR_ALL: //如果要求任务等待的标志位都得请 0
  				flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
  				if (flags_rdy == p_tcb->FlagsPend) //如果任务等待的标志位都请 0 了
  				{
  					OS_FlagTaskRdy(p_tcb, //让该任务准备运行
  								   flags_rdy,
  								   ts);
  				}
  				break; //跳出
  			case OS_OPT_PEND_FLAG_CLR_ANY://如果要求任务等待的标志位有 1 位请 0 即可
  				flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
  				if (flags_rdy != (OS_FLAGS)0) //如果任务等待的标志位有请 0 的
  				{
  					OS_FlagTaskRdy(p_tcb, //让该任务准备运行
  								   flags_rdy,
  								   ts);
  				}
  				break; //跳出
  #endif
  			default: //如果标志选项超出预期
  				OS_CRITICAL_EXIT(); //退出临界段
  				*p_err = OS_ERR_FLAG_PEND_OPT; //错误类型为"标志选项非法"
  				return ((OS_FLAGS)0); //返回 0,停止运行
  			}
  		p_pend_data = p_pend_data_next; //准备处理下一个等待任务
  		if (p_pend_data != (OS_PEND_DATA *)0) //如果该任务存在
  		{
  			p_tcb = p_pend_data->TCBPtr; //获取该任务的任务控制块
  		} else {//如果该任务不存在
  			p_tcb = (OS_TCB *)0; (30)//清空 p_tcb,退出 while 循环
  		}
  	}
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  
  	if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) //如果 opt 没选择"发布时不调度任务"
  	{
  		OSSched(); //任务调度
  	}
  
  	CPU_CRITICAL_ENTER(); //关中断
  	flags_cur = p_grp->Flags; //获取事件的标志值
  	CPU_CRITICAL_EXIT(); //开中断
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	return (flags_cur);//返回事件的当前标志值
  }
  
  ```

-  事件等待函数OSFlagPend()

  既然标记了事件的发生,那么我们怎么知道他到底有没有发生,这也是需要一个函数来获取事件是否已经发生,μC/OS提供了一个等待指定事件的函数`OSFlagPend()`,通过这个函数,任务可以知道事件标志组中的哪些位,有什么事件发生了,然后通过"逻辑与","逻辑或"等操作对感兴趣的事件进行获取,并且这个函数实现了等待超时机制,当且仅当任务等待的事件发生时,任务才能获取到事件信息.在这段时间中,如果事件一直没发生,该任务将保持阻塞状态以等待事件发生.当其他任务或中断服务程序往其等待的事件设置对应的标志位,该任务将自动由阻塞态转为就绪态.当任务等待的时间超过了指定的阻塞时间,即使事件还未发生,任务也会自动从阻塞态转移为就绪态.这样子很有效的体现了操作系统的实时性,如果事件正确获取(等待到)则返回对应的事件标志位,由用户判断再做处理,因为在事件超时的时候也可能会返回一个不能确定的事件值,所以最好判断一下任务所等待的事件是否真的发生.

  | 参数    | 含义                                                         |
  | ------- | ------------------------------------------------------------ |
  | *p_grp  | 指向要提交的事件的指针                                       |
  | flags   | 想要设置的标志位                                             |
  | timeout | 这个参数是设置的是获取不到等待事件时候等待的时间.如果这个值为0,表示一直等待下去,如果这个值不为0,则最多等待timeout个时钟节拍 |
  | opt     | 置位选项                                                     |
  | *p_ts   | 指向等待的事件被删除,等待被强制停止,等待超时等情况时的时间戳的指针 |
  | *p_err  | 指向返回错误类型的指针                                       |

  | 选项                     | 功能                                                         |
  | ------------------------ | ------------------------------------------------------------ |
  | OS_OPT_PEND_FLAG_CLR_ALL | 等待上面flags选定的位都被清0                                 |
  | OS_OPT_PEND_FLAG_CLR_ANY | 等待上面flags选定的位任意一位被清0                           |
  | OS_OPT_PEND_FLAG_SET_ALL | 等待上面flags选定的位都被置1                                 |
  | OS_OPT_PEND_FLAG_SET_ANY | 等待上面flags选定的位任意一位被置1                           |
  | OS_OPT_PEND_FLAG_CONSUME | 这个选项主要是获取到任务指定的位都被设置好后对这些位进行置反操作,注意不是清零.这个选项可以起到在满足条件后自动复位的作用.上面4个任意选项之一可以与上这个选项 |
  | OS_OPT_PEND_NON_BLOCKING | 如果一开始判断指定位的设置情况不满足不继续等待,直接退出函数继续运行任务 |
  | OS_OPT_PEND_BLOCKING     | 如果一开始判断指定位的设置情况不满足不继续等待,直接退出函数继续运行任务 |

  | 错误类型                | 含义                                                     |
  | ----------------------- | -------------------------------------------------------- |
  | OS_ERR_OBJ_DEL          | 事件已经被删除了                                         |
  | OS_ERR_OBJ_PTR_NULL     | 输入的事件变量指针是空类型                               |
  | OS_ERR_OBJ_TYPE         | p_grp指向的变量内核对象类型不是事件                      |
  | OS_ERR_OPT_INVALID      | 参数opt不符合要求                                        |
  | OS_ERR_PEND_ABORT       | 等待过程,其他的任务调用了函数OSFlagPendAbort强制取消等待 |
  | OS_ERR_PEND_ISR         | 企图在中断中等待事件                                     |
  | OS_ERR_PEND_WOULD_BLOCK | 开始获取不到事件,且没有要求等待                          |
  | OS_ERR_SCHED_LOCKED     | 调度器被锁住                                             |
  | OS_ERR_STATUS_INVALID   | 系统出错,导致任务控制块的元素PendStatus不在可能的范围内  |
  | OS_ERR_TIMEOUT          | 等待超时                                                 |
  | OS_ERR_NONE             | 无错误                                                   |

  ```C
  OS_FLAGS OSFlagPend (OS_FLAG_GRP *p_grp, //事件指针
  					 OS_FLAGS flags, //选定要操作的标志位
  					 OS_TICK timeout, //等待期限(单位:时钟节拍)
  					 OS_OPT opt, //选项
  					 CPU_TS *p_ts, //返回等到事件标志时的时间戳
  					 OS_ERR *p_err){
  	CPU_BOOLEAN consume;
  	OS_FLAGS flags_rdy;
  	OS_OPT mode;
  	OS_PEND_DATA pend_data;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
  	{
  		*p_err = OS_ERR_PEND_ISR; //错误类型为"在中断中中止等待"
  		return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if (p_grp == (OS_FLAG_GRP *)0) //如果 p_grp 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"对象为空"
  		return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_PEND_FLAG_CLR_ALL: //如果选项在预期内
  		case OS_OPT_PEND_FLAG_CLR_ANY:
  		case OS_OPT_PEND_FLAG_SET_ALL:
  		case OS_OPT_PEND_FLAG_SET_ANY:
  		case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
  		case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
  		case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
  		case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
  		case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
  		case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
  			break; //直接跳出
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID;//错误类型为"选项非法"
  			return ((OS_OBJ_QTY)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用了对象类型检测
  	if (p_grp->Type != OS_OBJ_TYPE_FLAG) //如果 p_grp 不是事件类型
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型有误"
  		return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  	if ((opt & OS_OPT_PEND_FLAG_CONSUME) != (OS_OPT)0) //选择了标志位匹配后自动取反
  	{
  		consume = DEF_TRUE;
  	} else {//未选择标志位匹配后自动取反
  		consume = DEF_FALSE;
  	}
  	if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  	{
  		*p_ts = (CPU_TS)0; //初始化(清零)p_ts,待用于返回时间戳
  	}
  
  	mode = opt & OS_OPT_PEND_FLAG_MASK; //从选项中提取对标志位的要求
  	CPU_CRITICAL_ENTER(); //关中断
  	switch (mode) //根据事件触发模式分类处理
  	{
  		case OS_OPT_PEND_FLAG_SET_ALL: //如果要求所有标志位均要置 1
  			flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //提取想要的标志位的值
  			if (flags_rdy == flags) //如果该值与期望值匹配
  			{
  				if (consume == DEF_TRUE)(15)//如果要求将标志位匹配后取反
  				{
  					p_grp->Flags &= ~flags_rdy; //清零事件的相关标志位
  				}
  				OSTCBCurPtr->FlagsRdy = flags_rdy; //保存让任务脱离等待的标志值
  				if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  				{
  					*p_ts = p_grp->TS; //获取任务等到事件时的时间戳
  				}
  				CPU_CRITICAL_EXIT(); //开中断
  				*p_err = OS_ERR_NONE; //错误类型为"无错误"
  				return (flags_rdy); //返回让任务脱离等待的标志值
  			} else {//如果想要标志位的值与期望值不匹配
  				if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
  				{
  					CPU_CRITICAL_EXIT(); //关中断
  					*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"渴求阻塞"
  					return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  				} else {//如果选择了阻塞任务
  					if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  					{
  						CPU_CRITICAL_EXIT(); //关中断
  						*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  						return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  					}
  				}
  				/* 如果调度器未被锁 */
  				OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  				OS_FlagBlock(&pend_data, //阻塞当前运行任务,等待事件
  					 		 p_grp,
  							 flags,
  					 		 opt,
  							 timeout);
  				OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  			}
  			break; //跳出
  		case OS_OPT_PEND_FLAG_SET_ANY: //如果要求有标志位被置 1 即可
  			flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //提取想要的标志位的值
  			if (flags_rdy != (OS_FLAGS)0) //如果有位被置 1
  			{
  				if (consume == DEF_TRUE) //如果要求将标志位匹配后取反
  				{
  					p_grp->Flags &= ~flags_rdy; //清零事件的相关标志位
  				}
  				OSTCBCurPtr->FlagsRdy = flags_rdy; //保存让任务脱离等待的标志值
  				if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  				{
  					*p_ts = p_grp->TS; //获取任务等到事件时的时间戳
  				}
  				CPU_CRITICAL_EXIT(); //开中断
  				*p_err = OS_ERR_NONE; //错误类型为"无错误"
  				return (flags_rdy); //返回让任务脱离等待的标志值
  			} else {//如果没有位被置 1
  				if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果没设置阻塞任务
  				{
  					CPU_CRITICAL_EXIT(); //关中断
  					*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"渴求阻塞"
  					return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  				} else {//如果设置了阻塞任务
  					if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  					{
  						CPU_CRITICAL_EXIT(); //关中断
  						*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  						return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  					}
  				}
  				/* 如果调度器没被锁 */
  				OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  				OS_FlagBlock(&pend_data, //阻塞当前运行任务,等待事件
  							 p_grp,
  							 flags,
  							 opt,
  							 timeout);
  				OS_CRITICAL_EXIT_NO_SCHED(); //退出中断(无调度)
  			}
  			break; //跳出
  
  #if OS_CFG_FLAG_MODE_CLR_EN > 0u
  			//如果启用了标志位清零触发模式
  		case OS_OPT_PEND_FLAG_CLR_ALL: //如果要求所有标志位均要清零
  			flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//提取想要的标志位的值
  			if (flags_rdy == flags)//如果该值与期望值匹配
  			{
  				if(consume == DEF_TRUE) //如果要求将标志位匹配后取反
  			{
  				p_grp->Flags |= flags_rdy; //置 1 事件的相关标志位
  			}
  				OSTCBCurPtr->FlagsRdy = flags_rdy; //保存让任务脱离等待的标志值
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = p_grp->TS; //获取任务等到事件时的时间戳
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			return (flags_rdy); //返回 0(有错误),停止执行
  			} else {//如果想要标志位的值与期望值不匹配
  				if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
  				{
  					CPU_CRITICAL_EXIT(); //关中断
  					*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"渴求阻塞"
  					return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  				} else {//如果选择了阻塞任务
  					if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  					{
  						CPU_CRITICAL_EXIT(); //关中断
  						*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  						return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  					}
  				}
  				/* 如果调度器未被锁 */
  				OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  				OS_FlagBlock(&pend_data, //阻塞当前运行任务,等待事件
  							 p_grp,
  							 flags,
  							 opt,
  							 timeout);
  				OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  			}
  			break; //跳出
  		case OS_OPT_PEND_FLAG_CLR_ANY: //如果要求有标志位被清零即可
  			flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//提取想要的标志位的值
  			if (flags_rdy != (OS_FLAGS)0) //如果有位被清零
  			{
  				if (consume == DEF_TRUE) //如果要求将标志位匹配后取反
  				{
  					p_grp->Flags |= flags_rdy; //置 1 事件的相关标志位
  				}
  				OSTCBCurPtr->FlagsRdy = flags_rdy; //保存让任务脱离等待的标志值
  				if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  				{
  					*p_ts = p_grp->TS; //获取任务等到事件时的时间戳
  				}
  				CPU_CRITICAL_EXIT(); //开中断
  				*p_err = OS_ERR_NONE; //错误类型为"无错误"
  				return (flags_rdy); //返回 0(有错误),停止执行
  			} else {//如果没有位被清零
  				if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果没设置阻塞任务
  				{
  					CPU_CRITICAL_EXIT(); //开中断
  					*p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"渴求阻塞"
  					return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  				} else {//如果设置了阻塞任务
  					if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
  					{
  						CPU_CRITICAL_EXIT(); //开中断
  						*p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
  						return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  					}
  				}
  				/* 如果调度器没被锁 */
  				OS_CRITICAL_ENTER_CPU_EXIT(); //进入临界段,重开中断
  				OS_FlagBlock(&pend_data, //阻塞当前运行任务,等待事件
  							 p_grp,
  							 flags,
  							 opt,
  							 timeout);
  				OS_CRITICAL_EXIT_NO_SCHED(); //退出中断(无调度)
  			}
  			break; //跳出
  #endif
  		default: //如果要求超出预期
  			CPU_CRITICAL_EXIT();
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  
  	OSSched(); //任务调度
  	/* 任务等到了事件后得以继续运行 */
  	CPU_CRITICAL_ENTER(); //关中断
  	switch (OSTCBCurPtr->PendStatus) //根据运行任务的等待状态分类处理
  	{
  		case OS_STATUS_PEND_OK: //如果等到了事件
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //返回等到事件时的时间戳
  			}
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			break; //跳出
  		case OS_STATUS_PEND_ABORT: //如果等待被中止
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //返回等待被中止时的时间戳
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_PEND_ABORT; //错误类型为"等待被中止"
  			break; //跳出
  		case OS_STATUS_PEND_TIMEOUT: //如果等待超时
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = (CPU_TS )0; //清零 p_ts
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_TIMEOUT; //错误类型为"超时"
  			break; //跳出
  		case OS_STATUS_PEND_DEL: //如果等待对象被删除
  			if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
  			{
  				*p_ts = OSTCBCurPtr->TS; //返回对象被删时的时间戳
  			}
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_OBJ_DEL; //错误类型为"对象被删"
  			break; //跳出
  		default: //如果等待状态超出预期
  			CPU_CRITICAL_EXIT(); //开中断
  			*p_err = OS_ERR_STATUS_INVALID; //错误类型为"状态非法"
  			break; //跳出
  	}
  	if (*p_err != OS_ERR_NONE) //如果有错误存在
  	{
  		return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  	}
  	/* 如果没有错误存在 */
  	flags_rdy = OSTCBCurPtr->FlagsRdy; //读取让任务脱离等待的标志值
  	if (consume == DEF_TRUE)
  	//如果需要取反触发事件的标志位
  	{
  		switch (mode)//根据事件触发模式分类处理
  		{
  			case OS_OPT_PEND_FLAG_SET_ALL: //如果是通过置 1 来标志事件的发生
  			case OS_OPT_PEND_FLAG_SET_ANY:
  				p_grp->Flags &= ~flags_rdy; //清零事件里触发事件的标志位
  				break; //跳出
  
  #if OS_CFG_FLAG_MODE_CLR_EN > 0u//如果启用了标志位清零触发模式
  			case OS_OPT_PEND_FLAG_CLR_ALL: //如果是通过清零来标志事件的发生
  			case OS_OPT_PEND_FLAG_CLR_ANY:
  				p_grp->Flags |= flags_rdy; //置 1 事件里触发事件的标志位
  				break; //跳出
  #endif
  			default: //如果触发模式超出预期
  				CPU_CRITICAL_EXIT(); //开中断
  				*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  				return ((OS_FLAGS)0); //返回 0(有错误),停止执行
  		}
  	}
  	CPU_CRITICAL_EXIT(); //开中断
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	return (flags_rdy); //返回让任务脱离等待的标志值
  }
  
  ```

  事件等待函数OSFlagPend()使用实例

  ```C
  #define EVENT1 (0x01<<0) //设置事件掩码的位0
  #define EVENT2 (0x01<<1) //设置事件掩码的位1
  
  OS_FLAG_GRP	flag_grp;
  
  void AppTaskPend(void * p_arg){
      OS_ERR err;
      (void)p_arg;
      
      while(DEF_TRUE){
          //等待标志组的BIT0和BIT1位均被置1
          OSFlagPend((OS_FLAG_GRP *)&flag_grp,
          		   (OS_FLAGS )(EVENT1 | EVENT2),
  				   (OS_TICK )0,
  				   (OS_OPT )OS_OPT_PEND_FLAG_SET_ALL |OS_OPT_PEND_BLOCKING,
  				   (CPU_TS *)0,
  				   (OS_ERR *)&err);
          
          //等待标志组的BIT0和BIT1有一个被清零
          OSFlagPend((OS_FLAG_GRP *)&flag_grp,
          		   (OS_FLAGS )(EVENT1 | EVENT2),
  				   (OS_TICK )0,
  				   (OS_OPT )OS_OPT_PEND_FLAG_CLR_ANY |OS_OPT_PEND_BLOCKING,
  				   (CPU_TS *)0,
  				   (OS_ERR *)&err);
      }
  }
  
  ```

  当用户调用这个函数接口时,系统首先根据用户指定参数和接收选项来判断它要等待的事件是否发生,如果已经发生,则根据等待选项来决定是否清除事件的相应标志位,并且返回事件标志位的值,但是这个值可能不是一个稳定的值,所以在等待到对应事件的时候,我们最好要判断事件是否与任务需要的一致;如果事件没有发生,则把任务添加到事件等待列表中,将当前任务阻塞,直到事件发生或等待时间超时.

### 事件实验

事件标志组实验是在μC/OS中创建了两个任务,一个是设置事件任务,一个是等待事件任务,两个任务独立运行,设置事件任务通过检测按键的按下情况设置不同的事件标志位,等待事件任务则获取这两个事件标志位,并且判断两个事件是否都发生,如果是则输出相应信息.等待事件任务一直在等待事件的发生,等待到事件之后清除对应的事件标记位.

1. 声明事件标志组,并定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![事件实验1_1](.\Image\事件实验1_1.png)

   ![事件实验1_2](.\Image\事件实验1_2.png)

2. 定义任务函数

   ![事件实验2](.\Image\事件实验2.png)

3. 任务启动函数编写

   ![事件实验3](.\Image\事件实验3.png)

4. 结果现象

   ![事件实验4](.\Image\事件实验4.png)

   每次按下WKUP都会进行任务同步.

## 软件定时器

### 软件定时器的基本概念

- 定时器,是指从指定的时刻开始,经过一个指定时间,然后触发一个超时事件,用户可以自定义定时器的周期与频率.类似生活中的闹钟,我们可以设置闹钟每天什么时候响,还能设置响的次数,是响一次还是每天都响.

- 定时器有硬件定时器和软件定时器之分:

  - 硬件定时器是芯片本身提供的定时功能.一般是由外部晶振提供给芯片输入时钟,芯片向软件模块提供一组配置寄存器,接受控制输入,到达设定时间值后芯片中断控制器产生时钟中断.硬件定时器的精度一般很高,可以达到纳秒级别,并且是中断触发方式.

  - 软件定时器,软件定时器是由操作系统提供的一类系统接口,它构建在硬件定时器基础之上,使系统能够提供不受硬件定时器资源限制的定时器服务,它实现的功能与硬件定时器也是类似的.

  - 使用硬件定时器时,每次在定时时间到达之后就会自动触发一个中断,用户在中断中处理信息;而使用软件定时器时,需要我们在创建软件定时器时指定时间到达后要调用的函数(也称超时函数/回调函数,为了统一,下文均用回调函数描述),在回调函数中处理信息.

    PS:软件定时器回调函数的上下文是任务,下文所说的定时器均为软件定时器.

  - 软件定时器在被创建之后,当经过设定的时钟计数值后会触发用户定义的回调函数.定时精度与系统时钟的周期有关.一般系统利用`SysTick`作为软件定时器的基础时钟,软件定时器的回调函数类似硬件的中断服务函数,所以,回调函数也要快进快出,而且回调函数中不能有任何阻塞任务运行的情况(软件定时器回调函数的上下文环境是任务),比如`OSTimeDly()`以及其他能阻塞任务运行的函数,两次触发回调函数的时间间隔period叫定时器的定时周期.

- μC/OS操作系统提供软件定时器功能,软件定时器的使用相当于扩展了定时器的数量,允许创建更多的定时业务.μC/OS软件定时器功能上支持:

  - 裁剪:能通过宏关闭软件定时器功能.
  - 软件定时器创建.
  - 软件定时器启动.
  - 软件定时器停止.
  - 软件定时器删除.

- μC/OS 提供的软件定时器支持单次模式和周期模式,单次模式和周期模式的定时时间到之后都

  会调用软件定时器的回调函数,用户可以在回调函数中加入要执行的工程代码.

  ![软件定时器的单次模式与周期模式](.\Image\软件定时器的单次模式与周期模式.png)

  - 单次模式:当用户创建了定时器并启动了定时器后,定时时间到了,只执行一次回调函数之后就

    将不再重复执行,当然用户还是可以调用软件定时器启动函数OSTmrStart()来启动一次软件定时

    器.

  - 周期模式:这个定时器会按照设置的定时时间循环执行回调函数,直到用户将定时器删除.

    μC/OS中软件定时器的周期模式也分为两种,一种是有初始化延迟的周期模式,另一种是

    无初始化延迟的周期模式,由OSTmrCreate()中的"dly"参数设置,这两种周期模式基本是一致

    的,但是有个细微的差别.

    - 有初始化延迟的周期模式:在软件定时器创建的时候,其第一个定时周期是由定时器中的dly参

      数决定,然后在运行完第一个周期后,其以后的定时周期均由period参数决定.

    - 无初始化延迟的周期模式:该定时器从始至终都按照周期运行.

      比如创建两个周期定时器,定时器1是无初始化延迟的定时器,周期为100个tick(时钟节拍),定时器 2是有初始化延迟的定时器,其初始化延迟的dly参数为150个tick,周期为100个tick,从tick为0的时刻就启动了两个软件定时器.定时器1从始至终都按照正常的周期运行,但是定时器2则在第一个周期中的运行周期为dly,从第二个运行周期开始按照正常的100个tick来运行.

    ![两种周期模式运行示意图](.\Image\两种周期模式运行示意图.png)

- μC/OS通过一个`OS_TmrTask`任务(也叫软件定时器任务)来管理软定时器,它是在系统初始化时(`OSInit()`函数中)自动创建的,为了满足用户定时需求.`TmrTask`任务会在定时器节拍到来的时候检查定时器列表,看看是否有定时器时间到了,如果到了就调用其回调函数.只有设置os_cfg.h 中的宏定义`OS_CFG_DBG_EN`设置为1 ,才会将软件定时器相关代码编译进来,才能正常使用软件定时器相关功能.

### 软件定时器应用场景

- 在很多应用中,我们需要一些定时器任务,硬件定时器受硬件的限制,数量上不足以满足用户的实际需求,无法提供更多的定时器,那么可以采用软件定时器来完成,由软件定时器代替硬件定时器任务.但需要注意的是软件定时器的精度是无法和硬件定时器相比的,因为在软件定时器的定时过程中是极有可能被其他中断所打断,因为软件定时器的执行上下文环境是任务.所以,软件定时器更适用于对时间精度要求不高的任务,一些辅助型的任务.

###  软件定时器的精度

- 在操作系统中,通常软件定时器以系统节拍为计时的时基单位.系统节拍是系统的心跳节拍,表示系统时钟的频率,就类似人的心跳,1s能跳动多少下,系统节拍配置为`OS_CFG_TICK_RATE_HZ`,该宏在os_app_cfg.h中有定义,默认是1000.那么系统的时钟节拍周期就为1ms(1s跳动1000下,每一下就为ms).

- μC/OS软件定时器的精度(分辨率) 决定于系统时基频率,也就是变量`OS_CFG_TMR_TASK_RATE_HZ`的值,它是以 Hz 为单位的.如果软件定时器任务的频率`OS_CFG_TMR_TASK_RATE_HZ`设置为10Hz,系统中所有软件定时器的精度为十分之一秒.事实上,这是用于软件定时器的推荐值,因为软件定时器常用于不精确时间尺度的任务.

- 而且定时器所定时的数值必须是这个定时器任务精度的整数倍,例如,定时器任务的频率为10HZ,那么上层软件定时器定时数值只能是100ms,200ms,1000ms等,而不能取值为150ms.由于系统节拍与软件定时器频率决定了系统中定时器能够分辨的精确度,用户可以根据实际CPU的处理能力和实时性需求设置合适的数值,软件定时器频率的值越大,精度越高,但是系统开销也将越大,因为这代表在1秒中系统进入定时器任务的次数也就越多.

  PS:定时器任务的频率`OS_CFG_TMR_TASK_RATE_HZ`的值不能大于系统时基频率`OS_CFG_TMR_TASK_RATE_HZ`的值.

### 软件定时器控制块

- μC/OS的软件定时器也属于内核对象,是一个可以裁剪的功能模块,同样在系统中由一个控制块管理其相关信息,软件定时器的控制块中包含创建的软件定时器基本信息,在使用定时器前我们需要通过 `OSTmrCreate()`函数创建一个软件定时器,但是在创建前需要我们定义一个定时器的句柄(控制块).

  ```c
  struct os_tmr
  {
      OS_OBJ_TYPE Type;
      CPU_CHAR *NamePtr;
      OS_TMR_CALLBACK_PTR CallbackPtr;
      void *CallbackPtrArg;
      OS_TMR *NextPtr;
      OS_TMR *PrevPtr;
      OS_TICK Match;
      OS_TICK Remain;
      OS_TICK Dly;
      OS_TICK Period;
      OS_OPT Opt;
      OS_STATE State;
  #if OS_CFG_DBG_EN > 0u
      OS_TMR *DbgPrevPtr;
      OS_TMR *DbgNextPtr;
  #endif
  };
  
  ```

  - Type:结构体开始于一个"Type"域,μC/OS可以通过这个域辨认它是个定时器(其他内核对象的结构体首部也有"Type").如果函数需传递一种内核对象,μC/OS会检测"Type"域是否为参数所需的类型.
  - *NamePtr:每个内核对象都可以被命名,以便于用户调试,这是一个指向内核对象名的指针.
  - CallbackPtr:指向函数的指针,被指向的函数称作回调函数,当定时器定时时间到达后,其指向的回调函数将被调用.如果定时器创建时该指针值为NULL,回调函数将不会被调用.
  - *CallbackPtrArg:当回调函数需要接受一个参数时(CallbackPtr不为NULL),这个参数通过该指针传递给回调函数,简单来说就是指向回调函数中的形参.
  - *NextPtr:指针指向下一个定时器.
  - *PrevPtr:指针指向上一个定时器,与NextPtr指针联合工作将定时器链接成一个双向链表.
  - Match:当定时器管理器中的变量OSTmrTickCtr的值等于定时器中的Match值时,表示定时器时间到了,Match也被称为匹配时间(唤醒时间).
  - Remain:中保存了距定时器定时时间到达还有多少个时基.
  - Dly:包含了定时器的初次定时值(可以看作是第一次延迟的
    值),这个值以定时器时基为最小单位.
  - Period:是定时器的定时周期(当被设置为周期模式时).这个值以
    定时器时基为最小单位.
  - Opt:定时器的选项,可选参数.
  - State:记录定时器的状态.

  ![软件定时器控制块](.\Image\软件定时器控制块.png)

### 软件定时器函数接口

| 函数名称       | 函数作用                                 |
| -------------- | ---------------------------------------- |
| OSTmrCreate()  | 创建软件定时器                           |
| OSTmrStart()   | 启动软件定时器                           |
| OSTmrStop()    | 停止定时器                               |
| SOTmrDel()     | 删除软件定时器                           |
| OS_TmrLink()   | 将软件定时器插入定时器列表               |
| OS_TmrUnlink() | 将运行中的定时器从原本的定时器列表中移除 |

- 创建软件定时器函数OSTmrCreate()

  软件定时器也是内核对象,与消息队列,信号量等内核对象一样,都是需要创建之后才能使用的资源,我们在创建的时候需要指定定时器延时初始值dly,定时器周期,定时器工作模式,回调函数等.每个软件定时器只需少许的RAM 空间,理论上μC/OS支持无限多个软件定时器,只要RAM足够即可.

  | 参数            | 作用                                                         |
  | --------------- | ------------------------------------------------------------ |
  | *p_tmr          | 用户自定义的定时器变量                                       |
  | *p_name         | 用户给定时器起的名字,方便调试                                |
  | opt             | 功能选项                                                     |
  | dly             | 如果上面的选项是OS_OPT_TMR_ONE_SHOT,那么dly就是这个定时器定时的时间.如果是OS_OPT_TMR_PERIODIC,而且dly不为0,则第一次延时的时间就是dly(为0为0延时初始化定时器,为其他则为延时对应时间启动) |
  | period          | 如果选项是OS_OPT_TMR_PERIODIC,那么第一次过后每次定时的时间都是period 这么长.而且如果选项是OS_OPT_TMR_PERIODIC且dly是0的话,那么定时器第一次延时的时间也是period |
  | p_callback      | 回调函数指针.定时器每次到期的时候都可以执行指定的回调函数,可以在创建的时候设置,回调函数跟钩子函数有很大的相似.回调函数一个很好的定义如下."回调函数是由用户撰写,而由操作系统调用的一类函数,回调函数可以把调用者和被调用者分开,调用者(例如操作系统)不需要关心被调用者到底是哪个函数,它所知道的就是有这么一类函数,这类满足相同的函数签名(函数原型,参数,返回值等),由用户书写完毕后在被调用就可以了.一般来讲:回调函数都是基于某种消息驱动,在获取相应消息时调用该函数你只要把你的函数写好并把地址传过去就行. |
  | *p_callback_arg | 回调函数传递的参数                                           |
  | *p_err          | 指向返回错误类型的指针,主要有以下几种类型                    |

  | 选项                | 功能                               |
  | ------------------- | ---------------------------------- |
  | OS_OPT_TMR_ONE_SHOT | 只调用一次回调函数就停止           |
  | OS_OPT_TMR_PERIODIC | 定时器到定时时间后重复调用回调函数 |

  | 错误类型                       | 含义                                                         |
  | ------------------------------ | ------------------------------------------------------------ |
  | OS_ERR_ILLEGAL_CREATE_RUN_TIME | 在定义OSSafetyCriticalStartFlag为DEF_TRUE后就不运行创建任何内核对象,包括软件定时器 |
  | OS_ERR_OBJ_CREATED             | 该软件定时器已经被创建过了,但是函数中没有这 个错误相关代码   |
  | OS_ERR_OBJ_PTR_NULL            | 参数p_tmr是个空指针                                          |
  | OS_ERR_OPT_INVALID             | 参数opt没有在规定的范围内                                    |
  | OS_ERR_TMR_INVALID_DLY         | 创建的是一次性定时器,但是参数dly为0                          |
  | OS_ERR_TMR_INVALID_PERIOD      | 创建的是周期性定时器,但是参数peroid为0                       |
  | OS_ERR_TMR_ISR                 | 在中断中创建定时器                                           |
  | OS_ERR_NONE                    | 无错误                                                       |

  ```c
  void OSTmrCreate(OS_TMR *p_tmr, //定时器控制块指针
  				 CPU_CHAR *p_name, //命名定时器,有助于调试
  				 OS_TICK dly, //初始定时节拍数
  				 OS_TICK period, //周期定时重载节拍数
  				 OS_OPT opt, //选项
  				 OS_TMR_CALLBACK_PTR p_callback, //定时到期时的回调函数
  				 void*p_callback_arg, //传给回调函数的参数
  				 OS_ERR *p_err){
  	CPU_SR_ALLOC();//使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return; //返回,不执行定时操作
  	}
  #endif
  
  #ifdef OS_SAFETY_CRITICAL_IEC61508//如果启用(默认禁用)了安全关键
  	//如果是在调用 OSSafetyCriticalStart() 后创建该定时器
  	if (OSSafetyCriticalStartFlag == DEF_TRUE)
  	{
  		*p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME; //错误类型为"非法创建内核对象"
  		return; //返回,不执行定时操作
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		*p_err = OS_ERR_TMR_ISR; //错误类型为"在中断函数中定时"
  		return; //返回,不执行定时操作
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测
  	if (p_tmr == (OS_TMR *)0) //如果参数 p_tmr 为空
  	{
  		*p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为"定时器对象为空"
  		return; //返回,不执行定时操作
  	}
  
  	switch (opt) //根据延时选项参数 opt 分类操作
  	{
  		case OS_OPT_TMR_PERIODIC: //如果选择周期性定时
  			if (period == (OS_TICK)0) //如果周期重载实参为 0
  			{
  				*p_err = OS_ERR_TMR_INVALID_PERIOD; //错误类型为"周期重载实参无效"
  				return; //返回,不执行定时操作
  			}
  			break;
  		case OS_OPT_TMR_ONE_SHOT: //如果选择一次性定时
  			if (dly == (OS_TICK)0) //如果定时初始实参为 0
  			{
  				*p_err = OS_ERR_TMR_INVALID_DLY; //错误类型为"定时初始实参无效"
  				return; //返回,不执行定时操作
  			}
  			break;
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return; //返回,不执行定时操作
  	}
  #endif
  
  	OS_CRITICAL_ENTER(); //进入临界段, 初始化定时器指标
  	p_tmr->State = (OS_STATE )OS_TMR_STATE_STOPPED;
  	p_tmr->Type = (OS_OBJ_TYPE )OS_OBJ_TYPE_TMR;
  	p_tmr->NamePtr = (CPU_CHAR *)p_name;
  	p_tmr->Dly = (OS_TICK )dly;
  	p_tmr->Match = (OS_TICK )0;
  	p_tmr->Remain = (OS_TICK )0;
  	p_tmr->Period = (OS_TICK )period;
  	p_tmr->Opt = (OS_OPT )opt;
  	p_tmr->CallbackPtr = (OS_TMR_CALLBACK_PTR)p_callback;
  	p_tmr->CallbackPtrArg = (void *)p_callback_arg;
  	p_tmr->NextPtr = (OS_TMR *)0;
  	p_tmr->PrevPtr = (OS_TMR *)0;
  
  #if OS_CFG_DBG_EN > 0u//如果启用(默认启用)了调试代码和变量
  	OS_TmrDbgListAdd(p_tmr); //将该定时添加到定时器双向调试链表
  #endif
  	OSTmrQty++; //定时器个数加 1
  
  	OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
  	*p_err = OS_ERR_NONE; //错误类型为"无错误"
  }
  
  ```

  软件定时器创建实例

  ```c
  OS_ERR err;
  OS_TMR my_tmr; //声明软件定时器对象
  
  /* 创建软件定时器 */
  OSTmrCreate ((OS_TMR *)&my_tmr, //软件定时器对象
  			 (CPU_CHAR *)"MySoftTimer", //命名软件定时器
  			 (OS_TICK )10,//定时器初始值,依 10Hz 时基计算,即为 1s
  			 (OS_TICK )10,//定时器周期重载值,依 10Hz 时基计算,即为 1s
  			 (OS_OPT )OS_OPT_TMR_PERIODIC, //周期性定时
  			 (OS_TMR_CALLBACK_PTR )TmrCallback, //回调函数
  			 (void *)"Timer Over!", //传递实参给回调函数
  			 (OS_ERR *)err);
  
  ```

- 启动软件定时器函数OSTmrStart()

  在系统初始化的时候,系统会帮我们自动创建一个软件定时器任务,在这个任务中,如果暂时没有运行中的定时器,任务会进入阻塞态等待定时器任务节拍的信号量.我们在创建一个软件定时器之后,如果没有启动它,该定时器就不会被添加到软件定时器列表中,那么在定时器任务就不会运行该定时器,而`OSTmrStart()`函数就是将已经创建的软件定时器添加到定时器列表中,这样子被创建的定时器就会被系统运行.

  | 参数   | 含义                   |
  | ------ | ---------------------- |
  | *p_tmr | 指向定时器对象的指针   |
  | *p_err | 指向返回错误类型的指针 |

  | 错误类型                 | 含义                                                         |
  | ------------------------ | ------------------------------------------------------------ |
  | OS_ERR_OBJ_TYPE          | 参数p_tmr没有指向一个定时器类型的变量                        |
  | OS_ERR_TMR_INVALID       | 参数p_tmr是空指针                                            |
  | OS_ERR_TMR_INACTIVE      | 使用定时器没有先初始化,即没有先创建定时器,也可能是定时器已经被删除 |
  | OS_ERR_TMR_INVALID_STATE | 定时器的状态不可用                                           |
  | OS_ERR_TMR_ISR           | 企图在中断中启动定时器. 创建完必须要再使用这个函数才可以运行定时器.如果是正在运行中的定时器再调用 启动函数,就会达到重启的效果.下面首先介绍下定时器的状态 |
  | OS_TMR_STATE_UNUSED      | 定时器已经被删除                                             |
  | OS_TMR_STATE_STOPPED     | 停止一个定时器,刚刚创建完也是这个状态                        |
  | OS_TMR_STATE_RUNNING     | 正常运行                                                     |
  | OS_TMR_STATE_COMPLETED   | 定时完成,一次性地定时器才会有这种状态,因为重复性的 定时器不可能自然地停止,用启动函数可以重新启动 |
  | OS_ERR_NONE              | 无错误                                                       |

  ```c
  CPU_BOOLEAN OSTmrStart (OS_TMR *p_tmr, //定时器控制块指针
  						OS_ERR *p_err){
  	OS_ERR err;
  	CPU_BOOLEAN success; //暂存函数执行结果
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		*p_err = OS_ERR_TMR_ISR; //错误类型为"在中断函数中定时"
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u //如果启用(默认启用)了参数检测
  	if(p_tmr == (OS_TMR *)0) //如果启用 p_tmr 的实参为空
  	{
  		*p_err = OS_ERR_TMR_INVALID; //错误类型为"无效的定时器"
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用(默认启用)了对象类型检测
  	if (p_tmr->Type != OS_OBJ_TYPE_TMR) //如果该定时器的对象类型有误
  	{
  		*p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型错误"
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  OSSchedLock(&err); //锁住调度器
  	switch (p_tmr->State) //根据定时器的状态分类处理
  	{
  		case OS_TMR_STATE_RUNNING: //如果定时器正在运行,则重启
  			OS_TmrUnlink(p_tmr); //从定时器列表里移除该定时器
  			OS_TmrLink(p_tmr, OS_OPT_LINK_DLY); //将该定时器重新插入定时器列表
  			OSSchedUnlock(&err); //解锁调度器
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			success = DEF_TRUE; //执行结果暂为 DEF_TRUE
  			break;
  		case OS_TMR_STATE_STOPPED: //如果定时器已被停止,则开启
  		case OS_TMR_STATE_COMPLETED: //如果定时器已完成了,则开启
  			OS_TmrLink(p_tmr, OS_OPT_LINK_DLY); //将该定时器重新插入定时器列表
  			OSSchedUnlock(&err); //解锁调度器
  			*p_err = OS_ERR_NONE; //错误类型为"无错误"
  			success = DEF_TRUE; //执行结果暂为 DEF_TRUE
  			break;
  		case OS_TMR_STATE_UNUSED: //如果定时器未被创建
  			OSSchedUnlock(&err); //解锁调度器
  			*p_err = OS_ERR_TMR_INACTIVE; //错误类型为"定时器未激活"
  			success = DEF_FALSE; //执行结果暂为 DEF_FALSE
  			break;
  		default: //如果定时器的状态超出预期
  			OSSchedUnlock(&err); //解锁调度器
  			*p_err = OS_ERR_TMR_INVALID_STATE; //错误类型为"定时器无效"
  			success = DEF_FALSE; //执行结果暂为 DEF_FALSE
  			break;
  	}
  	return (success); //返回执行结果
  }
  
  ```

  启动软件定时器函数使用实例

  ```c
  OS_TMR my_tmr; //声明软件定时器对象
  
  /* 启动软件定时器 */						 
  OSTmrStart ((OS_TMR   *)&my_tmr, //软件定时器对象
              (OS_ERR   *)err);
  
  ```

- 定时器列表插入函数OS_TmrLink()

  从定时器列表里移除该定时器之后需要将软件定时器重新按照周期插入定时器列表中,调用OS_TmrLink()函数即可将软件定时器插入定时器列表.

  ```c
  void OS_TmrLink (OS_TMR *p_tmr, //定时器控制块指针
  				 OS_OPT opt){
  	OS_TMR_SPOKE *p_spoke;
  	OS_TMR *p_tmr0;
  	OS_TMR *p_tmr1;
      OS_TMR_SPOKE_IX spoke;
  
  	//重置定时器为运行状态
  	p_tmr->State = OS_TMR_STATE_RUNNING;
  
  	if (opt == OS_OPT_LINK_PERIODIC)
  	{
  		//如果定时器是再次插入,匹配时间加个周期重载值
  		p_tmr->Match = p_tmr->Period + OSTmrTickCtr;
  	} else { //如果定时器是首次插入
  		if (p_tmr->Dly == (OS_TICK)0) //如果定时器的 Dly = 0,匹配时间加个周期重载值
  		{
  			p_tmr->Match = p_tmr->Period + OSTmrTickCtr;
  		} else { //如果定时器的 Dly != 0,匹配时间加个 Dly
  			p_tmr->Match = p_tmr->Dly + OSTmrTickCtr;
  		}
  	}
  	//通过哈希算法决定将该定时器插入定时器轮的哪个列表.
  	spoke = (OS_TMR_SPOKE_IX)(p_tmr->Match % OSCfg_TmrWheelSize);
  	p_spoke = &OSCfg_TmrWheel[spoke];
  
  	if (p_spoke->FirstPtr == (OS_TMR *)0)
  	{
  	//如果列表为空,直接将该定时器作为列表的第一个元素.
  		p_tmr->NextPtr = (OS_TMR *)0;
  		p_tmr->PrevPtr = (OS_TMR *)0;
  		p_spoke->FirstPtr = p_tmr;
  		p_spoke->NbrEntries = 1u;
  	} else { //如果列表非空,算出定时器 p_tmr 的剩余时间
  		p_tmr->Remain = p_tmr->Match - OSTmrTickCtr;
  		//取列表的首个元素到 p_tmr1
  		p_tmr1 = p_spoke->FirstPtr;
  		while (p_tmr1 != (OS_TMR *)0)
  		{
  			//如果 p_tmr1 非空,算出 p_tmr1 的剩余时间
  			p_tmr1->Remain = p_tmr1->Match - OSTmrTickCtr;
  			if (p_tmr->Remain > p_tmr1->Remain)
  			{
  				//如果 p_tmr 的剩余时间大于 p_tmr1 的
  				if (p_tmr1->NextPtr != (OS_TMR *)0)
  				{
                      //如果 p_tmr1 后面非空,取 p_tmr1 后一个定时器为新的 p_tmr1 进行下一次循环
                      p_tmr1 = p_tmr1->NextPtr;(11)
  				} else { //如果 p_tmr1 后面为空,将 p_tmr 插到 p_tmr1 的后面,结束循环
  					p_tmr->NextPtr = (OS_TMR *)0;
  					p_tmr->PrevPtr = p_tmr1;
  					p_tmr1->NextPtr = p_tmr;
  					p_tmr1 = (OS_TMR *)0;
  				}
  			} else { //如果 p_tmr 的剩余时间不大于 p_tmr1 的,
  				if (p_tmr1->PrevPtr == (OS_TMR *)0)
  				{
  					//将 p_tmr 插入 p_tmr1 的前一个,结束循环.
  					p_tmr->PrevPtr = (OS_TMR *)0;
  					p_tmr->NextPtr = p_tmr1;
  					p_tmr1->PrevPtr = p_tmr;
  					p_spoke->FirstPtr = p_tmr;
  				} else {
  					p_tmr0 = p_tmr1->PrevPtr;
  					p_tmr->PrevPtr = p_tmr0;
  					p_tmr->NextPtr = p_tmr1;
  					p_tmr0->NextPtr = p_tmr;
  					p_tmr1->PrevPtr = p_tmr;
  				}
  				p_tmr1 = (OS_TMR *)0;
  			}
  		}
  		//列表元素成员数加 1
  		p_spoke->NbrEntries++;
  	}
  	if (p_spoke->NbrEntriesMax < p_spoke->NbrEntries)
  	{
  		//更新列表成员数最大值历史记录
  		p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
  	                                                                                   }
  }
  
  ```

- 定时器列表删除函数OS_TmrUnlink()

  ```c
  void OS_TmrUnlink (OS_TMR *p_tmr //定时器控制块指针
  								 ){
  	OS_TMR_SPOKE *p_spoke;
  	OS_TMR *p_tmr1;
  	OS_TMR *p_tmr2;
  	OS_TMR_SPOKE_IX spoke;
  
  	spoke = (OS_TMR_SPOKE_IX)(p_tmr->Match % OSCfg_TmrWheelSize);
  	//与插入时一样,通过哈希算法找出
  	p_spoke = &OSCfg_TmrWheel[spoke];
  	//该定时器在定时器的哪个列表.
  
  	if (p_spoke->FirstPtr == p_tmr) //如果 p_tmr 是列表的首个元素
  	{
  		//取 p_tmr 后一个元素为 p_tmr1(可能为空)
  		p_tmr1 = (OS_TMR *)p_tmr->NextPtr;
  		p_spoke->FirstPtr = (OS_TMR *)p_tmr1; //表首改为 p_tmr1
  		if (p_tmr1 != (OS_TMR *)0) //如果 p_tmr1 确定非空
  		{
  			p_tmr1->PrevPtr = (OS_TMR *)0; //p_tmr1 的前面清空
  		}
  	} else {//如果 p_tmr 不是列表的首个元素
  		//将 p_tmr 从列表移除,并将 p_tmr 前后的两个元素连接在一起.
  		p_tmr1 = (OS_TMR *)p_tmr->PrevPtr;
  		p_tmr2 = (OS_TMR *)p_tmr->NextPtr;
  		p_tmr1->NextPtr = p_tmr2;
  		if (p_tmr2 != (OS_TMR *)0)
  		{
  			p_tmr2->PrevPtr = (OS_TMR *)p_tmr1;
  		}
  	}
  	p_tmr->State = OS_TMR_STATE_STOPPED; //复位 p_tmr 的指标
  	p_tmr->NextPtr = (OS_TMR *)0;
  	p_tmr->PrevPtr = (OS_TMR *)0;
  	p_spoke->NbrEntries--; //列表元素成员减 1
  }
  
  ```

- 软件定时器列表管理

  有些情况下,当系统中有多个软件定时器的时候,μC/OS可能要维护上百个定时器.使用定时器列表会大大降低更新定时器列表所占用的CPU时间,一个一个检测是否到期效率很低,有没有什么办法让系统快速查找到到期的软件定时器?μC/OS 对软件定时器列表的管理就跟时间节拍一样,采用哈希算法.`OS_TmrLink`将不同的定时器变量根据其对`OSCfg_TmrWheelSize`余数的不同插入数组`OSCfg_TmrWheel[OS_CFG_TMR_WHEEL_SIZE]`中去.

  ![软件定时器列表](.\Image\软件定时器列表.png)

  定时器列表中包含了`OS_CFG_TMR_WHEEL_SIZE`条记录,该值是一个宏定义,由用户指定,在

  os_cfg_app.h文件中.能记录定时器的多少仅限于处理器的RAM空间,推荐的设置值为定时器

  数/4.定时器列表的每个记录都由3部分组成:`NbrEntriesMax`表明该记录中有多少个定时器;

  `NbrEntriesMax`表明该记录中最大时存放了多少个定时器;`FirstPtr`指向当前记录的定时器链表.

- 停止定时器函数OSTmrStop()

  停止一个软件定时器.软件定时器被停掉之后可以调用 OSTmrStart() 函数重启,但是重启之后定时器是从头计时,而不是接着上次停止的时刻继续计时.

  ```c
  CPU_BOOLEAN OSTmrStop(OS_TMR *p_tmr, //定时器控制块指针
  					  OS_OPT opt, //选项
  					  void *p_callback_arg, //传给回调函数的新参数
  					  OS_ERR *p_err){
  	OS_TMR_CALLBACK_PTR p_fnct;
  	OS_ERR err;
  	CPU_BOOLEAN success; //暂存函数执行结果
  	
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	//如果启用(默认启用)了中断中非法调用检测
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
      {
          *p_err = OS_ERR_TMR_ISR; //错误类型为"在中断函数中定时"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测
      if (p_tmr == (OS_TMR *)0) //如果启用 p_tmr 的实参为空
      {
          *p_err = OS_ERR_TMR_INVALID; //错误类型为"无效的定时器"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用(默认启用)了对象类型检测
      if (p_tmr->Type != OS_OBJ_TYPE_TMR) //如果该定时器的对象类型有误
      {
          *p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型错误"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  #endif
  
  	OSSchedLock(&err); //锁住调度器
      switch (p_tmr->State) (5)
      {
          //根据定时器的状态分类处理
          case OS_TMR_STATE_RUNNING:
          //如果定时器正在运行
          OS_TmrUnlink(p_tmr);
          //从定时器轮列表里移除该定时器
          *p_err = OS_ERR_NONE;
          //错误类型为"无错误"
      switch (opt)
      {
      	//根据选项分类处理
  	    case OS_OPT_TMR_CALLBACK: //执行回调函数,使用创建定时器时的实参
  		    p_fnct = p_tmr->CallbackPtr; //取定时器的回调函数
  		    if (p_fnct != (OS_TMR_CALLBACK_PTR)0) //如果回调函数存在
  		    {
  			    (*p_fnct)((void *)p_tmr, p_tmr->CallbackPtrArg); //使用创建定时器时的实参执行回调函数
  		    } else { //如果回调函数不存在
  			    *p_err = OS_ERR_TMR_NO_CALLBACK;
  			    //错误类型为"定时器没有回调函数"
  		    }
  		    break;
          case OS_OPT_TMR_CALLBACK_ARG:
  		    //执行回调函数,使用 p_callback_arg 作为实参
  		    p_fnct = p_tmr->CallbackPtr;
  		    //取定时器的回调函数
  		    if (p_fnct != (OS_TMR_CALLBACK_PTR)0) //如果回调函数存在
  		    {
  			    (*p_fnct)((void *)p_tmr, p_callback_arg); //使用 p_callback_arg 作为实参执行回调函数
  		    } else { //如果回调函数不存在
  			    *p_err = OS_ERR_TMR_NO_CALLBACK; //错误类型为"定时器没有回调函数"
  		    }
  		    break;
  	    case OS_OPT_TMR_NONE: //只需停掉定时器
      		break;
  	    default: (10)//情况超出预期
  		    OSSchedUnlock(&err); //解锁调度器
  		    *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项无效"
  		    return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  	    OSSchedUnlock(&err);
      	success = DEF_TRUE;
  	    break;
  	case OS_TMR_STATE_COMPLETED: //如果定时器已完成第一次定时
      case OS_TMR_STATE_STOPPED: //如果定时器已被停止
  	    OSSchedUnlock(&err); //解锁调度器
      	*p_err = OS_ERR_TMR_STOPPED; //错误类型为"定时器已被停止"
  	    success = DEF_TRUE; //执行结果暂为 DEF_TRUE
      	break;
      case OS_TMR_STATE_UNUSED: //如果该定时器未被创建过
  	    OSSchedUnlock(&err); //解锁调度器
      	*p_err = OS_ERR_TMR_INACTIVE; //错误类型为"定时器未激活"
  	    success = DEF_FALSE; //执行结果暂为 DEF_FALSE
      	break;
      default: //如果定时器状态超出预期
  	    OSSchedUnlock(&err); //解锁调度器
      	*p_err = OS_ERR_TMR_INVALID_STATE;//错误类型为"定时器状态非法"
  	    success = DEF_FALSE; //执行结果暂为 DEF_FALSE
      	break;
      }
      return (success); //返回执行结果
  }
  
  ```

  定时器停止函数的使用实例

  ```c
  OS_ERR err;
  OS_TMR my_tmr; //声明软件定时器对象
  OSTmrStop ((OS_TMR *)&my_tmr, //定时器控制块指针
  		   (OS_OPT )OS_OPT_TMR_NONE, //选项
  		   (void *)"Timer Over!", //传给回调函数的新参数
  		   (OS_ERR *)err);
  
  ```

- 删除软件定时器函数OSTmrDel()

  删除一个已经被创建成功的软件定时器,删除之后就无法使用该定时器,并且定时器相应的信息也会被系清空.要想使用`OSTmrDel()`函数必须在头文件os_cfg.h中把宏`OS_CFG_TMR_DEL_EN`定义为1.

  ```c
  #if OS_CFG_TMR_DEL_EN > 0u//如果启用了 OSTmrDel() 函数
  CPU_BOOLEAN OSTmrDel(OS_TMR *p_tmr, //定时器控制块指针
  					 OS_ERR *p_err){
  	OS_ERR err;
  	CPU_BOOLEAN success; //暂存函数执行结果
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
  	//如果启用(默认启用)了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
          *p_err = OS_ERR_TMR_ISR; //错误类型为"在中断函数中定时"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测
      if (p_tmr == (OS_TMR *)0) //如果启用 p_tmr 的实参为空
      {
          *p_err = OS_ERR_TMR_INVALID; //错误类型为"无效的定时器"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  #endif
  
  #if OS_CFG_OBJ_TYPE_CHK_EN > 0u//如果启用(默认启用)了对象类型检测
      if (p_tmr->Type != OS_OBJ_TYPE_TMR) //如果该定时器的对象类型有误
      {
          *p_err = OS_ERR_OBJ_TYPE; //错误类型为"对象类型错误"
          return (DEF_FALSE); //返回 DEF_FALSE,不继续执行
      }
  #endif
  
  	OSSchedLock(&err); //锁住调度器
  #if OS_CFG_DBG_EN > 0u//如果启用(默认启用)了调试代码和变量
  	OS_TmrDbgListRemove(p_tmr); //将该定时从定时器双向调试链表移除
  #endif
  	OSTmrQty--; //定时器个数减 1
  
      switch (p_tmr->State) //根据定时器的状态分类处理
      {
          case OS_TMR_STATE_RUNNING: //如果定时器正在运行
         		OS_TmrUnlink(p_tmr); //从当前定时器列表列表移除定时器
  	        OS_TmrClr(p_tmr); //复位定时器的指标
      	    OSSchedUnlock(&err); //解锁调度器
      	    *p_err = OS_ERR_NONE; //错误类型为"无错误"
       	   success = DEF_TRUE; //执行结果暂为 DEF_TRUE
       	   break;
          case OS_TMR_STATE_STOPPED: //如果定时器已被停止
          case OS_TMR_STATE_COMPLETED: //如果定时器已完成第一次定时
      		OS_TmrClr(p_tmr); //复位定时器的指标
        		OSSchedUnlock(&err); //解锁调度器
        		*p_err = OS_ERR_NONE; //错误类型为"无错误"
  	       	success = DEF_TRUE; //执行结果暂为 DEF_TRUE
        		break;
          case OS_TMR_STATE_UNUSED: //如果定时器已被删除
          	OSSchedUnlock(&err); //解锁调度器
  	        *p_err = OS_ERR_TMR_INACTIVE; //错误类型为"定时器未激活"
      	    success = DEF_FALSE; //执行结果暂为 DEF_FALSE
          	break;
          default: //如果定时器的状态超出预期
  	        OSSchedUnlock(&err); //解锁调度器
      	    *p_err = OS_ERR_TMR_INVALID_STATE; //错误类型为"定时器无效"
          	success = DEF_FALSE; //执行结果暂为 DEF_FALSE
  	        break;
      }
  	return (success); //返回执行结果
  }
  #endif
  
  ```

  软件定时器删除函数使用实例

  ```c
  OS_ERR err;
  OS_TMR my_tmr; //声明软件定时器对象
  
  OSTmrDel ((OS_TMR *)&my_tmr, //软件定时器对象
  		  (OS_ERR *)err);
  
  ```

### 软件定时器任务

- 软件定时器的回调函数的上下文是在任务中,所有,系统中必须要一个任务来管理所有的软件定时器,等到定时时间到达后就调用定时器对应的回调函数,那么软件定时器任务又是一个什么东西呢,它是在系统初始化的时候系统就帮我们创建的一个任务.

  创建软件定时器任务

  ```c
  void OS_TmrInit(OS_ERR *p_err)
  {
  	OS_TMR_SPOKE_IX i;
  	OS_TMR_SPOKE *p_spoke;
  
  #ifdef OS_SAFETY_CRITICAL
      if (p_err == (OS_ERR *)0)
      {
          OS_SAFETY_CRITICAL_EXCEPTION();
          return;
      }
  #endif
  
  #if OS_CFG_DBG_EN > 0u
      OSTmrDbgListPtr = (OS_TMR *)0;
  #endif
  
      if (OSCfg_TmrTaskRate_Hz > (OS_RATE_HZ)0)
      {
      	OSTmrUpdateCnt = OSCfg_TickRate_Hz / OSCfg_TmrTaskRate_Hz;
      } else {
  	    OSTmrUpdateCnt = OSCfg_TickRate_Hz / (OS_RATE_HZ)10;
      }
      OSTmrUpdateCtr = OSTmrUpdateCnt;
  
      OSTmrTickCtr = (OS_TICK)0;
  
      OSTmrTaskTimeMax = (CPU_TS)0;
  
      for (i = 0u; i < OSCfg_TmrWheelSize; i++)
      {
          p_spoke = &OSCfg_TmrWheel[i];
          p_spoke->NbrEntries = (OS_OBJ_QTY)0;
          p_spoke->NbrEntriesMax = (OS_OBJ_QTY)0;
          p_spoke->FirstPtr = (OS_TMR *)0;
      }
  
      /* ---------------- CREATE THE TIMER TASK --------------- */
      if (OSCfg_TmrTaskStkBasePtr == (CPU_STK*)0)
      {
      	*p_err = OS_ERR_TMR_STK_INVALID;
      	return;
      }
  
      if (OSCfg_TmrTaskStkSize < OSCfg_StkSizeMin)
      {
      	*p_err = OS_ERR_TMR_STK_SIZE_INVALID;
      	return;
      }
  
      if (OSCfg_TmrTaskPrio >= (OS_CFG_PRIO_MAX - 1u))
      {
      	*p_err = OS_ERR_TMR_PRIO_INVALID;
      	return;
      }
  
      OSTaskCreate((OS_TCB *)&OSTmrTaskTCB, //创建OS_TmrTask任务
      			 (CPU_CHAR *)((void *)"μC/OS-III Timer Task"),
      			 (OS_TASK_PTR )OS_TmrTask,
      			 (void *)0,
      			 (OS_PRIO )OSCfg_TmrTaskPrio,
      			 (CPU_STK *)OSCfg_TmrTaskStkBasePtr,
      			 (CPU_STK_SIZE)OSCfg_TmrTaskStkLimit,
      			 (CPU_STK_SIZE)OSCfg_TmrTaskStkSize,
      			 (OS_MSG_QTY )0,
      			 (OS_TICK )0,
      			 (void *)0,
      			 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR|OS_OPT_TASK_NO_TLS),
      			 (OS_ERR *)p_err);
  }
  
  ```

  PS:正常来说定时器任务的执行频率`OSCfg_TmrTaskRate_Hz`是大于0 的,并且能被`OSCfg_TickRate_Hz`整除,才能比较准确得到定时器任务运行的频率.如果`OSCfg_TmrTaskRate_Hz`设置为大于0,就配置定时器任务的频率.否则就配置为系统时钟频率的十分之一(1/10).不过当设定的定时器的频率大于时钟节拍的执行频率的时候,定时器运行就会出错,但是这里没有进行判断,在写代码的时候需注意.

  OS_TmrTask()函数

  ```c
  void OS_TmrTask (void *p_arg)
  {
      CPU_BOOLEAN done;
      OS_ERR err;
      OS_TMR_CALLBACK_PTR p_fnct;
      OS_TMR_SPOKE *p_spoke;
      OS_TMR *p_tmr;
      OS_TMR *p_tmr_next;
      OS_TMR_SPOKE_IX spoke;
      CPU_TS ts;
      CPU_TS ts_start;
      CPU_TS ts_end;
  
      p_arg = p_arg;/* Not using 'p_arg', prevent compiler warning */
  
      while (DEF_ON)
      {
          /* 等待信号指示更新定时器的时间 */
          (void)OSTaskSemPend((OS_TICK )0,
          (OS_OPT )OS_OPT_PEND_BLOCKING,
          (CPU_TS *)&ts,
          (OS_ERR *)&err);
  
          OSSchedLock(&err);
          ts_start = OS_TS_GET();
          /* 增加当前定时器时间 */
          OSTmrTickCtr++;
  
          /* 通过哈希算法找到对应时间唤醒的列表 */
          spoke = (OS_TMR_SPOKE_IX)(OSTmrTickCtr % OSCfg_TmrWheelSize);
          p_spoke = &OSCfg_TmrWheel[spoke];
  
          /* 获取列表头部的定时器 */
          p_tmr = p_spoke->FirstPtr;
  
          done = DEF_FALSE;
          while (done == DEF_FALSE)
          {
  
              if (p_tmr != (OS_TMR *)0)
              {
                  /* 指向下一个定时器以进行更新,
                  因为可能当前定时器到时了会从列表中移除 */
                  p_tmr_next = (OS_TMR *)p_tmr->NextPtr;
  
                  /* 确认是定时时间到达 */
                  if (OSTmrTickCtr == p_tmr->Match)
                  {
                  	/* 先移除定时器 */
                 	 	OS_TmrUnlink(p_tmr);
  
            			/* 如果是周期定时器 */
              		if (p_tmr->Opt == OS_OPT_TMR_PERIODIC)
              		{
              			/* 重新按照唤醒时间插入定时器列表 */
              			OS_TmrLink(p_tmr,
              			OS_OPT_LINK_PERIODIC);
              		} else {
              			/* 定时器状态设置为已完成 */
              			p_tmr->State = OS_TMR_STATE_COMPLETED;
              		}
              		/* 执行回调函数(如果可用)*/
              		p_fnct = p_tmr->CallbackPtr;
              		if (p_fnct != (OS_TMR_CALLBACK_PTR)0)
              		{
              			(*p_fnct)((void *)p_tmr,
              			p_tmr->CallbackPtrArg);
              		}
              		/* 看看下一个定时器是否匹配 */
              		p_tmr = p_tmr_next;
          		} else {
          			done = DEF_TRUE;
          		}
              } else {
          		done = DEF_TRUE;
          	}
          }
          /* 测量定时器任务的执行时间 */
          ts_end = OS_TS_GET() - ts_start;
          OSSchedUnlock(&err);
          if (OSTmrTaskTimeMax < ts_end)
          {
          	OSTmrTaskTimeMax = ts_end;
          }
      }
  }
  
  ```

  调用`OSTaskSemPend()`函数在一直等待定时器节拍的信号量,等待到了才运行.系统的时钟节拍是基于SysTick定时器上的,μC/OS采用Tick任务(OS_TickTask)管理系统的时间节拍,而我们定时器节拍是由系统节拍分频而来,那么其发送信号量的地方当然也是在 SysTick 中断服务函数中,但是μC/OS支持采用中断延迟,如果使用了中断延迟,那么发送任务信号量的地方就会在中断发布任务中(OS_IntQTask),从代码中,我们可以看到当`OSTmrUpdateCtr`减到0的时候才会发送一次信号量.

### 软件定时器实验

软件定时器实验是在μC/OS中创建了一个应用任务`AppTaskTmr`,在该任务中创建一个软件定时器,周期性定时1s,每秒输出一行.

1. 定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![软件定时器1_1](.\Image\软件定时器1_1.png)

   ![软件定时器1_2](.\Image\软件定时器1_2.png)

2. 定义任务函数

   ![软件定时器2](.\Image\软件定时器2.png)

3. 任务启动函数编写

   ![软件定时器3](.\Image\软件定时器3.png)

4. 结果现象

   ![软件定时器4](.\Image\软件定时器4.png)

   经验证软件定时器每秒输出一行.

### 总结

- 定时器的创建,删除,启动,停止这些操作无非就是在操作定时器列表的双向列表和根据不同的设置进行定时器状态的转化以及相关的处理.至于检测定时器到期,系统将时间节拍进行分频得到定时器任务执行的频率,在定时器任务中,系统采用了哈希算法进行快速检测有没有定时器到期,然后执行其对应的回调函数等操作.软件定时器最核心的一点是底层的一个硬件定时器(SysTick内核定时器)上进行软件分频.
- μC/OS 允许用户建立任意数量的定时器(只限制于处理器的RAM大小).
- 回调函数是在定时器任务中被调用,所以回调函数的上下文环境是在任务中,并且运行回调函数时调度器处于被锁状态.一般我们编写的回调函数越简短越好,并且不能在回调函数中等待消息队列,信号量,事件等操作,否则定时器任务会被挂起,导致定时器任务崩溃,这是绝对不允许的.此外还需要注意几点:
  1. 回调函数是在定时器任务中被执行的,这意味着定时器任务需要有足够的栈空间供回调函数去执行.
  2. 回调函数是在根据定时器队列中依次存放的,所以在定时器时间到达后回调函数是依次被执行的.
  3. 定时器任务的执行时间决定于:有多少个定时器期满,执行定时器中的回调函数需多少时间.因为回调函数是用户提供,它可能很大程度上影响了定时器任务的执行时间.
  4. 回调函数被执行时会锁调度器,所以我们必须让回调函数尽可能地短,以便其他任务能正常运行.



## 任务信号量

###  任务信号量的基本概念

- μC/OS提供任务信号量这个功能,每个任务都有一个32位(用户可以自定义位宽,我们使用32位的CPU,此处就是32位)的信号量值SemCtr,这个信号量值是在任务控制块中包含的,是任务独有的一个信号量通知值,在大多数情况下,任务信号量可以替代内核对象的二值信号量,计数信号量等(而非内核对象信号量,如非特别说明,本章中的信号量都指的是内核对象信号量.前面所讲的信号量是单独的内核对象,是独立于任务存在的;本章要讲述的任务信号量是任务特有的属性,紧紧依赖于一个特定任务).
- 相对于前面使用μC/OS内核通信的资源,必须创建二进制信号量,计数信号量等情况,使用任务信号量显然更灵活.因为使用任务信号量比通过内核对象信号量通信方式解除阻塞的任务的速度要快,并且更加节省RAM内存空间,任务信号量的使用无需单独创建信号量.
- 通过对任务信号量的合理使用,可以在一定场合下替代μC/OS的信号量,用户只需向任务内部的信号量发送一个信号而不用通过外部的信号量进行发送,这样子处理就会很方便并且更加高效,当然,凡事都有利弊,不然的话μC/OS还要内核的IPC通信机制干嘛,任务信号量虽然处理更快,RAM开销更小,但也有限制:只能有一个任务接收任务信号量,因为必须指定接收信号量的任务,才能正确发送信号量;而内核对象的信号量则没有这个限制,用户在释放信号量,可以采用广播的方式,让所有等待信号量的任务都获取到信号量.
- 在实际任务间的通信中,一个或多个任务发送一个信号量给另一个任务是非常常见的,而一个任务给多个任务发送信号量的情况相对比较少.这种情况就很适合采用任务信号量进行传递信号,如果任务信号量可以满足设计需求,那么尽量不要使用普通信号量,这样子设计的系统会更加高效.
- 任务信号量的运作机制与普通信号量一样,没什么差别.

### 任务信号量的函数接口讲解

| 函数名称        | 函数作用         |
| --------------- | ---------------- |
| OSTaskSemPost() | 释放任务信号量   |
| OSTaskSemPend() | 获取一个任务信号 |

- 任务信号量释放函数OSTaskSemPost()

  释放任务信号量,虽然只有拥有任务信号量的任务才可以等待该任务信号量,但是其他所有的任务或者中断都可以向该任务释放信号量.

  | 参数   | 含义         |
  | ------ | ------------ |
  | *p_tcb | 目标任务TCB  |
  | opt    | 选项         |
  | *p_err | 返回错误类型 |
  
  | 选项                 | 功能           |
  | -------------------- | -------------- |
  | OS_OPT_POST_NONE     | 没有选项       |
  | OS_OPT_POST_NO_SCHED | 不调用调度程序 |
  
  | 错误类型              | 含义               |
  | --------------------- | ------------------ |
  | OS_ERR_INT_Q_FULL     | 延迟中断后队列已满 |
  | OS_ERR_OPT_INVALID    | 指定的选项无效     |
  | OS_ERR_OS_NOT_RUNNING | uC/OS尚未运行      |
  | OS_ERR_SEM_OVF        | 信号量计数溢出     |
  | OS_ERR_STATE_INVALID  | 任务处于无效状态   |
  | OS_ERR_NONE           | 无错误             |
  
  ```c
  OS_SEM_CTR OSTaskSemPost(OS_TCB *p_tcb, //目标任务
  						 OS_OPT opt, //选项
  						 OS_ERR *p_err){
  	OS_SEM_CTR ctr;
  	CPU_TS ts;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
  	if (p_err == (OS_ERR *)0) //如果 p_err 为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用(默认启用)了参数检测功能
  	switch (opt) //根据选项分类处理
  	{
  		case OS_OPT_POST_NONE: //如果选项在预期之内
  		case OS_OPT_POST_NO_SCHED:
  			break; //跳出
  		default: //如果选项超出预期
  			*p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  			return ((OS_SEM_CTR)0u); //返回 0(有错误),停止执行
  	}
  #endif
  
  	ts = OS_TS_GET(); //获取时间戳
  
  #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
  	{
  		OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_SIGNAL, //将该信号量发布到中断消息队列
  					(void *)p_tcb,
  					(void *)0,
                      (OS_MSG_SIZE)0,
                      (OS_FLAGS )0,
                      (OS_OPT )0,
                      (CPU_TS )ts,
                      (OS_ERR *)p_err);
  		return ((OS_SEM_CTR)0); //返回 0(尚未发布)
  	}
  #endif
  	
      //调用OS_TaskSemPost()函数将信号量发布到任务中
      ctr = OS_TaskSemPost(p_tcb, //将信号量按照普通方式处理
                           opt,
                           ts,
                           p_err);
  
  	return (ctr); //返回信号的当前计数值
  }
  
  ```
  
  释放任务信号量函数的使用实例.
  
  ```c
  OSTaskSemPost((OS_TCB *)&AppTaskPendTCB, //目标任务
  			  (OS_OPT )OS_OPT_POST_NONE, //没选项要求
  			  (OS_ERR *)&err);
  
  ```
  
  - 在释放任务信号量的时候,系统首先判断目标任务的状态,只有处于等待状态并且等待的是任务信号量那就调用`OS_Post()`函数让等待的任务就绪(如果内核对象信号量的话,还会让任务脱离等待列表),所以任务信号量的操作是非常高效的;如果没有处于等待状态或者等待的不是任务信号量,那就直接将任务控制块的元素`SemCtr`加1.最后返回任务信号量计数值.
  
  - 其实,不管是否启用了中断延迟发布,最终都是调用`OS_TaskSemPost()`函数进行释放任务信号量.只是启用了中断延迟发布的释放过程会比较曲折,中间会有许多插曲,这是中断管理范畴的内容,留到后面再作介绍.在`OS_TaskSemPost()`函数中,又会调用`OS_Post()`函数释放内核对象.`OS_Post()` 函数是一个底层的释放(发布)函数,它不仅仅用来释放(发布)任务信号量,还可以释放信号量,互斥信号量,消息队列,事件标志组或任务消息队列.
  
- 获取任务信号量函数OSTaskSemPend()

  用于获取一个任务信号量,参数中没有指定某个任务去获取信号量,实际上就是当前运行任务获取它自己拥有的任务信号量.

  | 参数    | 含义         |
  | ------- | ------------ |
  | timeout | 等待超时时间 |
  | opt     | 选项         |
  | *p_ts   | 时间戳       |
  | *p_err  | 返回错误类型 |

  | 选项                     | 功能   |
  | ------------------------ | ------ |
  | OS_OPT_PEND_BLOCKING     | 等待   |
  | OS_OPT_PEND_NON_BLOCKING | 不等待 |

  | 错误类型                | 含义              |
  | ----------------------- | ----------------- |
  | OS_ERR_PEND_ABORT       | 获取信号量已中止  |
  | OS_ERR_OPT_INVALID      | 指定选项无效      |
  | OS_ERR_OS_NOT_RUNNING   | uC/OS尚未运行     |
  | OS_ERR_PEND_ISR         | 从ISR调用了此函数 |
  | OS_ERR_PEND_WOULD_BLOCK | 任务处于无效状态  |
  | OS_ERR_SCHED_LOCKED     | 调度程序已锁定    |
  | OS_ERR_STATUS_INVALID   | 状态无效          |
  | OS_ERR_TIMEOUT          | 超时              |
  | OS_ERR_NONE             | 无错误            |

  ```c
  OS_SEM_CTR OSTaskSemPend (OS_TICK timeout, //等待超时时间
  						  OS_OPT opt, //选项
  						  CPU_TS *p_ts, //返回时间戳
  						  OS_ERR *p_err){
  	OS_SEM_CTR ctr;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
  	if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
      {
          *p_err = OS_ERR_PEND_ISR; //返回错误类型为"在中断中等待"
          return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
      switch (opt) //根据选项分类处理
      {
          case OS_OPT_PEND_BLOCKING: //如果选项在预期内
          case OS_OPT_PEND_NON_BLOCKING:
      		break; //直接跳出
          default: //如果选项超出预期
              *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
              return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
  	}
  #endif
  
      if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
      {
          *p_ts = (CPU_TS )0; //清零(初始化)p_ts
      }
  
      CPU_CRITICAL_ENTER(); //关中断
      if (OSTCBCurPtr->SemCtr > (OS_SEM_CTR)0) //如果任务信号量当前可用
      {
          OSTCBCurPtr->SemCtr--; //信号量计数器减 1
          ctr = OSTCBCurPtr->SemCtr; //获取信号量的当前计数值
          if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
          {
          *p_ts = OSTCBCurPtr->TS; //返回信号量被发布的时间戳
          }
  #if OS_CFG_TASK_PROFILE_EN > 0u
          OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS; //更新任务等待
          if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) //任务信号量的
          {
              OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime; //最长时间记录.
          }//如果启用任务统计的宏,计算任务信号量从被提交到获取所用时间及最大时间
  #endif
          CPU_CRITICAL_EXIT(); //开中断
          *p_err = OS_ERR_NONE; //错误类型为"无错误"
          return (ctr); //返回信号量的当前计数值
      }
      /* 如果任务信号量当前不可用 */
      if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
      {
          CPU_CRITICAL_EXIT(); //开中断
          *p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"缺乏阻塞"
          return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
      } else {//如果选择了阻塞任务
          if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
          {
              CPU_CRITICAL_EXIT(); //开中断
              *p_err = OS_ERR_SCHED_LOCKED;//错误类型为"调度器被锁"
              return ((OS_SEM_CTR)0); //返回 0(有错误),停止执行
          }
      }
      /* 如果调度器未被锁 */
      OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,重开中断
      OS_Pend((OS_PEND_DATA *)0, //阻塞任务,等待信号量.
      (OS_PEND_OBJ *)0, //不需插入等待列表.
      (OS_STATE )OS_TASK_PEND_ON_TASK_SEM,
      (OS_TICK )timeout);
      OS_CRITICAL_EXIT_NO_SCHED(); //开调度器(无调度)
  
      OSSched(); //调度任务
      /* 任务获得信号量后得以继续运行 */
      CPU_CRITICAL_ENTER(); //关中断
      switch (OSTCBCurPtr->PendStatus) //根据任务的等待状态分类处理
      {
          case OS_STATUS_PEND_OK: //如果任务成功获得信号量
              if (p_ts != (CPU_TS *)0) //返回信号量被发布的时间戳
              {
                  *p_ts = OSTCBCurPtr->TS;
  #if OS_CFG_TASK_PROFILE_EN > 0u//更新最长等待时间记录
                  OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
                  if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime)
                  {
                      OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;
                  }
  #endif
      		}
              *p_err = OS_ERR_NONE; //错误类型为"无错误"
              break; //跳出
      	case OS_STATUS_PEND_ABORT: //如果等待被中止
              if (p_ts != (CPU_TS *)0) //返回被终止时的时间戳
              {
                  *p_ts = OSTCBCurPtr->TS;
              }
              *p_err = OS_ERR_PEND_ABORT; //错误类型为"等待被中止"
              break; //跳出
      	case OS_STATUS_PEND_TIMEOUT: //如果等待超时
              if (p_ts != (CPU_TS *)0) //返回时间戳为 0
              {
              *p_ts = (CPU_TS )0;
              }
              *p_err = OS_ERR_TIMEOUT; //错误类型为"等待超时"
              break; //跳出
          default: //如果等待状态超出预期
              *p_err = OS_ERR_STATUS_INVALID; //错误类型为"状态非法"
              break; //跳出
      }
      ctr = OSTCBCurPtr->SemCtr; //获取信号量的当前计数值
      CPU_CRITICAL_EXIT(); //开中断
      return (ctr); //返回信号量的当前计数值
  }
  
  ```

  获取任务信号量函数的使用实例.

  ```c
  OSTaskSemPend ((OS_TICK )0, //无期限等待
  			   (OS_OPT )OS_OPT_PEND_BLOCKING, //如果信号量不可用就等待
  			   (CPU_TS *)&ts, //获取信号量被发布的时间戳
  			   (OS_ERR *)&err);
  
  ```

  在调用该函数的时候,系统先判断任务信号量是否可用,即检查任务信号量的计数值是否大于0,如果大于 0,即表示可用,这个时候获取信号量,即将计数值减1后直接返回.如果信号量不可用,且当调度器没有被锁住时,用户希望在任务信号量不可用的时候进行阻塞任务以等待任务信号量可用,那么系统就会调用`OS_Pend()`函数将任务脱离就绪列表,如果用户有指定超时时间,系统还要将该任务插入节拍列表.注意:此处系统并没有将任务插入等待列表.然后切换任务,处于就绪列表中最高优先级的任务通过任务调度获得CPU使用权,等到出现任务信号量被释放,任务等待任务信号量被强制停止,等待超时等情况,任务会从阻塞中恢复,等待任务信号量的任务重新获得CPU使用权,返回相关错误代码和任务信号量计数值,用户可以根据返回的错误知道任务退出等待状态的情况.

### 任务信号量实验

#### 任务信号量代替二值信号量

任务通知代替消息队列是在μC/OS中创建了两个任务,其中一个任务是用于接收任务信号量,另一个任务发送任务信号量.两个任务独立运行,发送任务信号量的任务是通过检测按键的按下情况发送,等待任务在任务信号量中没有可用的信号量之前就一直等待,获取到信号量以后就继续执行,这样子是为了代替二值信号量,任务同步成功则继续执行,然后在串口调试助手里将运行信息打印出来.

1. 定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![任务信号量代替二值信号量实验1_1](.\Image\任务信号量代替二值信号量实验1_1.png)

   ![任务信号量代替二值信号量实验1_2](.\Image\任务信号量代替二值信号量实验1_2.png)

2. 定义任务函数

   ![任务信号量代替二值信号量实验2](.\Image\任务信号量代替二值信号量实验2.png)

3. 任务启动函数编写

   ![任务信号量代替二值信号量实验3](.\Image\任务信号量代替二值信号量实验3.png)

4. 结果现象

   ![任务信号量代替二值信号量实验4](.\Image\任务信号量代替二值信号量实验4.png)

#### 任务信号量代替计数信号量

任务通知代替计数信号量是基于计数信号量实验修改而来,模拟停车场工作运行.并且在μC/OS中创建了两个任务:一个是获取信号量任务,一个是发送信号量任务,两个任务独立运行.按下WKUP按键计数信号量+1,每秒计数信号量-1.

1. 定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![任务信号量代替计数信号量实验1_1](.\Image\任务信号量代替计数信号量实验1_1.png)

   ![任务信号量代替计数信号量实验1_2](.\Image\任务信号量代替计数信号量实验1_2.png)

2. 定义任务函数

   ![任务信号量代替计数信号量实验2](.\Image\任务信号量代替计数信号量实验2.png)

3. 任务启动函数编写

   ![任务信号量代替计数信号量实验3](.\Image\任务信号量代替计数信号量实验3.png)

4. 结果现象

   ![任务信号量代替计数信号量实验4](.\Image\任务信号量代替计数信号量实验4.png)

## 任务消息队列

### 任务消息队列的基本概念

- 任务消息队列跟任务信号量一样,均隶属于某一个特定任务,不需单独创建,任务在则任务消息

  队列在,只有该任务才可以获取(接收)这个任务消息队列的消息,其他任务只能给这个任务消

  息队列发送消息,却不能获取.任务消息队列与前面讲解的(普通)消息队列极其相似,只是任

  务消息队列已隶属于一个特定任务,所以它不具有等待列表,在操作的过程中省去了等待任务插

  入和移除列表的动作,所以工作原理相对更简单一点,效率也比较高一些.

- 通过对任务消息队列的合理使用,可以在一定场合下替代μC/OS的消息队列,用户只需向任务内

  部的消息队列发送一个消息而不用通过外部的消息队列进行发送,这样子处理就会很方便并且

  更加高效,当然,凡事都有利弊,任务消息队列虽然处理更快,RAM开销更小,但也有限制:只

  能指定消息发送的对象,有且只有一个任务接收消息;而内核对象的消息队列则没有这个限制,

  用户在发送消息的时候,可以采用广播消息的方式,让所有等待该消息的任务都获取到消息.

- 在实际任务间的通信中,一个或多个任务发送一个消息给另一个任务是非常常见的,而一个任务

  给多个任务发送消息的情况相对比较少,前者就很适合采用任务消息队列进行传递消息,如果任

  务消息队列可以满足设计需求,那么尽量不要使用普通消息队列,这样子设计的系统会更加高

  效.

- 消息队列(内核对象)是用结构体OS_Q来管理的,包含了管理消息的元素MsgQ和管理等待列

  表的元素PendList等.而任务消息队列的结构体成员变量就少了PendList,因为等待任务消息队

  列只有拥有任务消息队列本身的任务才可以进行获取,故任务消息队列不需要等待列表的相关

  数据结构,

- 想要使用任务消息队列,就必须将`OS_CFG_TASK_Q_EN`宏定义配置为1,该宏定义位于os_cfg.h文件中.

- 任务消息队列数据结构

  ```c
  struct os_msg_q{
      OS_MSG *InPtr;
      OS_MSG *OutPtr; //*InPtr和*OutPtr是任务消息队列中进出消息指针
      OS_MSG_QTY NbrEntriesSize; //任务消息队列中最大可用的消息个数,在创建任务的时候由用户指定这个值的大小
      OS_MSG_QTY NbrEntries; //记录任务消息队列中当前的消息个数,每当发送一个消息到任务消息队列的时候,若任务没有在等待该消息,那么新发送的消息被插入任务消息队列后此值加1,NbrEntries的大小不能超过NbrEntriesSize
      OS_MSG_QTY NbrEntriesMax; //记录任务消息队列最多的时候拥有的消息个数
  };
  
  ```

### 任务消息队列的函数

| 函数名称      | 函数作用         |
| ------------- | ---------------- |
| OStaskQPost() | 发送任务消息队列 |
| OSTaskQpend() | 获取任务消息队列 |

- 任务消息队列发送函数OSTaskQPost()

  发送任务消息队列,参数中有指向消息要发送给的任务控制块的指针,任何任务都可以发送消息给拥有任务消息队列的任务(任务在被创建的时候,要设置参数 q_size大于 0).

  | 参数     | 含义         |
  | -------- | ------------ |
  | *p_tcb   | 目标任务TCB  |
  | *p_void  | 消息内容地址 |
  | msg_size | 消息长度     |
  | opt      | 选项         |
  | *p_err   | 返回错误类型 |

  | 选项                 | 功能                   |
  | -------------------- | ---------------------- |
  | OS_OPT_POST_FIFO     | 先进先出               |
  | OS_OPT_POST_LIFO     | 后进先出               |
  | OS_OPT_POST_NO_SCHED | 不在发布后运行调度程序 |

  | 错误类型              | 含义               |
  | --------------------- | ------------------ |
  | OS_ERR_INT_Q_FULL     | 延迟中断后队列已满 |
  | OS_ERR_MSG_POOL_EMPTY | 延迟中断后队列已满 |
  | OS_ERR_OPT_INVALID    | 栈已空             |
  | OS_ERR_OS_NOT_RUNNING | uC/OS尚未运行      |
  | OS_ERR_Q_MAX          | 消息队列已满       |
  | OS_ERR_STATE_INVALID  | 任务无效           |
  | OS_ERR_NONE           | 无错误             |

  ```c
  #if OS_CFG_TASK_Q_EN > 0u //如果启用了任务消息队列
  void OSTaskQPost(OS_TCB *p_tcb, //目标任务
                   void *p_void, //消息内容地址
                   OS_MSG_SIZE msg_size, //消息长度
                   OS_OPT opt, //选项
                   OS_ERR *p_err){
      CPU_TS ts;
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
      if (p_err == (OS_ERR *)0) //如果错误类型实参为空
      {
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return; //返回,停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
      switch (opt) //根据选项分类处理
      {
          case OS_OPT_POST_FIFO: //如果选项在预期内
          case OS_OPT_POST_LIFO:
          case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
          case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
      		break; //直接跳出
  		default: //如果选项超出预期
  		    *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
  		    return; //返回,停止执行
      }
  #endif
  
      ts = OS_TS_GET(); //获取时间戳
  
      #if OS_CFG_ISR_POST_DEFERRED_EN > 0u//如果启用了中断延迟发布
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
      {
          OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_MSG, //将消息先发布到中断消息队列
                      (void *)p_tcb,
                      (void *)p_void,
                      (OS_MSG_SIZE)msg_size,
                      (OS_FLAGS )0,
                      (OS_OPT )opt,
                      (CPU_TS )ts,
                      (OS_ERR *)p_err);
          return; //返回
      }
  #endif
  
      OS_TaskQPost(p_tcb, //将消息直接发布
                   p_void,
                   msg_size,
                   opt,
                   ts,
                   p_err);
  }
  #endif
  
  ```

  

- 任务消息队列获取函数OSTaskQPend()

  于获取一个任务消息队列,函数的参数中没有指定哪个任务获取任务消息,实际上就是当前执行的任务,当任务调用了这个函数就表明这个任务需要获取任务消息.

  | 参数        | 含义               |
  | ----------- | ------------------ |
  | timeout     | 等待期限           |
  | opt         | 选项               |
  | *p_msg_size | 返回消息长度       |
  | *p_ts       | 返回时间戳         |
  | *p_err      | 返回错误类型的指针 |

  | 选项                     | 功能   |
  | ------------------------ | ------ |
  | OS_OPT_PEND_BLOCKING     | 等待   |
  | OS_OPT_PEND_NON_BLOCKING | 不等待 |

  | 错误类型                | 含义              |
  | ----------------------- | ----------------- |
  | OS_ERR_OPT_INVALID      | 选项无效          |
  | OS_ERR_OS_NOT_RUNNING   | uC/OS尚未运行     |
  | OS_ERR_PEND_ABORT       | 获取信号量已终止  |
  | OS_ERR_PEND_ISR         | 从ISR调用了此函数 |
  | OS_ERR_PEND_WOULD_BLOCK | 任务处于无效状态  |
  | OS_ERR_PTR_INVALID      | p_msg_size为空    |
  | OS_ERR_SCHED_LOCKED     | 调度任务已锁定    |
  | OS_ERR_TIMEOUT          | 超时              |
  | OS_ERR_NONE             | 无错误            |

  ```c
  #if OS_CFG_TASK_Q_EN > 0u//如果启用了任务消息队列
  void *OSTaskQPend(OS_TICK timeout, //等待期限(单位:时钟节拍)
  				  OS_OPT opt, //选项
  				  OS_MSG_SIZE *p_msg_size, //返回消息长度
  				  CPU_TS *p_ts, //返回时间戳
  				  OS_ERR *p_err){
  	OS_MSG_Q *p_msg_q;
  	void *p_void;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用(默认禁用)了安全检测
      if (p_err == (OS_ERR *)0) //如果错误类型实参为空
      {
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return ((void *)0); //返回 0(有错误),停止执行
      }
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数在中断中被调用
      {
          *p_err = OS_ERR_PEND_ISR; //错误类型为"在中断中中止等待"
          return ((void *)0); //返回 0(有错误),停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
      if (p_msg_size == (OS_MSG_SIZE *)0) //如果 p_msg_size 为空
      {
          *p_err = OS_ERR_PTR_INVALID; //错误类型为"指针不可用"
          return ((void *)0); //返回 0(有错误),停止执行
      }
      switch (opt) //根据选项分类处理
      {
          case OS_OPT_PEND_BLOCKING: //如果选项在预期内
          case OS_OPT_PEND_NON_BLOCKING:
      		break; //直接跳出
  
          default: //如果选项超出预期
              *p_err = OS_ERR_OPT_INVALID; //错误类型为"选项非法"
              return ((void *)0); //返回 0(有错误),停止执行
      }
  #endif
  
      if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
      {
          *p_ts = (CPU_TS )0; //初始化(清零)p_ts,待用于返回时间戳
      }
  
      CPU_CRITICAL_ENTER(); //关中断
      p_msg_q = &OSTCBCurPtr->MsgQ;//获取当前任务的消息队列
      p_void = OS_MsgQGet(p_msg_q, //从队列里获取一个消息
      					p_msg_size,
                          p_ts,
                          p_err);
      if (*p_err == OS_ERR_NONE) //如果获取消息成功
      {
  #if OS_CFG_TASK_PROFILE_EN > 0u
  
      if (p_ts != (CPU_TS *)0)
      {
          OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - *p_ts;
          if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime)
          {
              OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;
          }
      }
  #endif
      CPU_CRITICAL_EXIT(); //开中断
      return (p_void); //返回消息内容
  }
      /* 如果获取消息不成功(队列里没有消息) */
      if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) //如果选择了不阻塞任务
      {
          *p_err = OS_ERR_PEND_WOULD_BLOCK; //错误类型为"缺乏阻塞"
          CPU_CRITICAL_EXIT(); //开中断
          return ((void *)0); //返回 0(有错误),停止执行
      } else {//如果选择了阻塞任务
          if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) //如果调度器被锁
          {
              CPU_CRITICAL_EXIT(); //开中断
              *p_err = OS_ERR_SCHED_LOCKED; //错误类型为"调度器被锁"
              return ((void *)0); //返回 0(有错误),停止执行
          }
      }
      /* 如果调度器未被锁 */
      OS_CRITICAL_ENTER_CPU_EXIT(); //锁调度器,重开中断
      OS_Pend((OS_PEND_DATA *)0, //阻塞当前任务,等待消息
      (OS_PEND_OBJ *)0,
      (OS_STATE )OS_TASK_PEND_ON_TASK_Q,
      (OS_TICK )timeout);
      OS_CRITICAL_EXIT_NO_SCHED(); //解锁调度器(无调度)
  
      OSSched(); //调度任务
      /* 当前任务(获得消息队列的消息)得以继续运行 */
      CPU_CRITICAL_ENTER(); //关中断
      switch (OSTCBCurPtr->PendStatus) //根据任务的等待状态分类处理
      {
          case OS_STATUS_PEND_OK: //如果任务已成功获得消息
              p_void = OSTCBCurPtr->MsgPtr; //提取消息内容地址
              *p_msg_size = OSTCBCurPtr->MsgSize; //提取消息长度
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
              	*p_ts = OSTCBCurPtr->TS; //获取任务等到消息时的时间戳
  #if OS_CFG_TASK_PROFILE_EN > 0u
  
      			OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
                  if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime)
                  {
                      OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;
                  }
  #endif
      		}
              *p_err = OS_ERR_NONE; //错误类型为"无错误"
              break; //跳出
  		case OS_STATUS_PEND_ABORT: //如果等待被中止
              p_void = (void *)0; //返回消息内容为空
              *p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
                  *p_ts = (CPU_TS )0; //清零 p_ts
              }
              *p_err = OS_ERR_PEND_ABORT; //错误类型为"等待被中止"
              break; //跳出
          case OS_STATUS_PEND_TIMEOUT: //如果等待超时
          default: //或者任务状态超出预
              p_void = (void *)0; //返回消息内容为空
              *p_msg_size = (OS_MSG_SIZE)0; //返回消息大小为 0
              if (p_ts != (CPU_TS *)0) //如果 p_ts 非空
              {
              *p_ts = OSTCBCurPtr->TS;
              }
              *p_err = OS_ERR_TIMEOUT; //错误类为"等待超时"
              break; //跳出
  	}
  	CPU_CRITICAL_EXIT(); //开中断
  	return (p_void); //返回消息内容地址
  }
  #endif
  
  ```

### 任务消息队列实验

任务通知代替消息队列是在μC/OS中创建了两个任务,其中一个任务是用于接收任务消息,另一个任务发送任务消息.两个任务独立运行,发送消息任务每秒发送一次任务消息,接收任务在就一直在等待消息,一旦获取到消息通知就通过串口发送出来.

1. 定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![任务消息队列实验1_1](.\Image\任务消息队列实验1_1.png)

   ![任务消息队列实验1_2](.\Image\任务消息队列实验1_2.png)

2. 定义任务函数

   ![任务消息队列实验2](.\Image\任务消息队列实验2.png)

3. 任务启动函数编写

   ![任务消息队列实验3](.\Image\任务消息队列实验3.png)

4. 结果现象

   ![任务消息队列实验4](.\Image\任务消息队列实验4.png)

## 内存管理

### 内存管理的基本概念

- 在计算系统中,变量,中间数据一般存放在系统存储空间中,只有在实际使用时才将它们从存储空间调入到中央处理器内部进行运算.通常存储空间可以分为两种:内部存储空间和外部存储空间.内部存储空间访问速度比较快,能够按照变量地址随机地访问,也就是我们通常所说的RAM(随机存储器),或计算机的内存;而外部存储空间内所保存的内容相对来说比较固定,即使掉电后数据也不会丢失,可以把它理解为计算机的硬盘.在这一章中我们主要讨论内部存储空间(RAM)的管理——内存管理.

- 在嵌入式系统设计中,内存分配应该是根据所设计系统的特点来决定选择使用动态内存分配还是静态内存分配算法,一些可靠性要求非常高的系统应选择使用静态的,而普通的业务系统可以使用动态来提高内存使用效率.静态可以保证设备的可靠性但是需要考虑内存上限,内存使用效率低,而动态则是相反.

- μC/OS的内存管理是采用内存池的方式进行管理,也就是创建一个内存池,静态划分一大块连续空间作为内存管理的空间,里面划分为很多个内存块,我们在使用的时候就从这个内存池中获取一个内存块,使用完毕的时候用户可以将其放回内存池中,这样子就不会导致内存碎片的产生.

- μC/OS内存管理模块管理用于系统中内存资源,它是操作系统的核心模块之一,主要包括内存池的创建,分配以及释放.

- 在计算机中我们可以用C语言标准库中的`malloc()`和`free()`这两个函数动态的分配内存和释放内存.但是,在嵌入式实时操作系统中,调用`malloc()`和`free()`却是危险的,原因有以下几点:

  - 这些函数在小型嵌入式系统中并不总是可用的,小型嵌入式设备中的RAM不足.
  - 它们的实现可能非常的复杂,占据了相当大的一块代码空间.
  - 他们几乎都不是安全的.
  - 它们并不是确定的,每次调用这些函数执行的时间可能都不一样.
  - 它们有可能产生碎片.
  - 这两个函数会使得链接器配置得复杂.
  - 如果允许堆空间的生长方向覆盖其他变量占据的内存,它们会成为debug的灾难.

  在一般的实时嵌入式系统中,由于实时性的要求,很少使用虚拟内存机制.所有的内存都需要用户参与分配,直接操作物理内存,所分配的内存不能超过系统的物理内存,所有的系统栈的管理,都由用户自己管理.

- 同时,在嵌入式实时操作系统中,对内存的分配时间要求更为苛刻,分配内存的时间必须是确定的.一般内存管理算法是根据需要存储的数据的长度在内存中去寻找一个与这段数据相适应的空闲内存块,然后将数据存储在里面,而寻找这样一个空闲内存块所耗费的时间是不确定的,因此对于实时系统来说,这就是不可接受的,实时系统必须要保证内存块的分配过程在可预测的确定时间内完成,否则实时任务对外部事件的响应也将变得不可确定.

- 在嵌入式系统中,内存是十分有限而且是十分珍贵的,用一块内存就少了一块内存,而在分配中随着内存不断被分配和释放,整个系统内存区域会产生越来越多的碎片,因为在使用过程中,申请了一些内存,其中一些释放了,导致内存空间中存在一些小的内存块,它们地址不连续,不能够作为一整块的大内存分配出去,所以一定会在某个时间,系统已经无法分配到合适的内存了,导致系统瘫痪.其实系统中实际是还有内存的,但是因为小块的内存的地址不连续,导致无法分配成功,所以我们需要一个优良的内存分配算法来避免这种情况的出现.所以μC/OS提供的内存分配算法是只允许用户分配固定大小的内存块,当使用完成就将其放回内存池中,这样的分配效率极高,时间复杂度是O(1),也就是一个固定的时间常数,并不会因为系统内存的多少而增加遍历内存块列表的时间,并且还不会导致内存碎片的出现,但是这样的内存分配机制会导致内存利用率的下降以及申请内存大小的限制.


### 内存管理的运作机制

- 内存池(Memory Pool)是一种用于分配大量大小相同的内存对象的技术,它可以极大加快内存分配/释放的速度.

- 在系统编译的时候,编译器就静态划分了一个大数组作为系统的内存池,然后在初始化的时候将其分成大小相等的多个内存块,内存块直接通过链表连接起来(此链表也称为空闲内存块列表).每次分配的时候,从空闲内存块列表中取出表头上第一个内存块,提供给申请者.物理内存中允许存在多个大小不同的内存池,每一个内存池又由多个大小相同的空闲内存块组成.我们必须先创建内存池才能去使用内存池里面的内存块,在创建的时候,我们必须定义一个内存池控制块,然后进行相关初始化,内存控制块的参数包括内存池名称,内存池起始地址,内存块大小,内存块数量等信息,在以后需要从内存池取出内存块或者释放内存块的时候,我们只需根据内存控制块的信息就能很轻易做到 .内存池一旦创建完成,其内部的内存块大小将不能再做调整.

- 内存控制块数据结构

  ```c
  struct os_mem
  {
      OS_OBJ_TYPE Type; //内核对象类型
      void *AddrPtr; //内存池的起始地址
      CPU_CHAR *NamePtr; //内存池名称
      void *FreeListPtr; //空闲内存块列表
      OS_MEM_SIZE BlkSize; //内存块大小
      OS_MEM_QTY NbrMax; //内存池中内存块的总数量
      OS_MEM_QTY NbrFree; //空闲内存块数量
  #if OS_CFG_DBG_EN > 0u
  	OS_MEM *DbgPrevPtr;
  	OS_MEM *DbgNextPtr;
  #endif
  };
  
  ```

- 静态内存示意图

  ![静态内存示意图](.\Image\静态内存示意图.png)

  PS:内存池中的内存块是通过单链表连接起来的,类似于消息池,内存池在创建的时候内存块地址是连续的,但是经过多次申请以及释放后,空闲内存块列表的内存块在地址上不一定是连续的.

### 内存管理的应用场景

在使用内存分配前,必须明白在做什么,这样做与其他的方法有什么不同,特别是会产生哪些负面影响,在产品面前,应当选择哪种分配策略.

- 内存管理的主要工作是动态划分并管理用户分配好的内存区间,主要是在用户需要使用大小不等的内存块的场景中使用,当用户需要分配内存时,可以通过操作系统的内存申请函数索取指定大小内存块,一旦使用完毕,通过动态内存释放函数归还所占用内存,使之可以重复使用(heap_1.c的内存管理除外).

  例如我们需要定义一个flfloat型数组:flfloatArr[];但是,在使用数组的时候,总有一个问题困扰着我们:数组应该有多大?在很多的情况下,你并不能确定要使用多大的数组,可能为了避免发生错误你就需要把数组定义得足够大.即使你知道想利用的空间大小,但是如果因为某种特殊原因空间利用的大小有增加或者减少,你又必须重新去修改程序,扩大数组的存储范围.这种分配固定大小的内存分配方法称之为静态内存分配.这种内存分配的方法存在比较严重的缺陷,在大多数情况下会浪费大量的内存空间,在少数情况下,当你定义的数组不够大时,可能引起下标越界错误,甚至导致严重后果.

- μC/OS将系统静态分配的大数组作为内存池,然后进行内存池的初始化,然后分配固定大小的内存块.

  PS:μC/OS也不能很好解决这种问题,因为内存块的大小是固定的,无法解决这种弹性很大的内存需求,只能按照最大的内存块进行分配.但是μC/OS的内存分配能解决内存利用率的问题,在不需要使用内存的时候,将内存释放到内存池中,让其他任务能正常使用该内存块.

### 内存管理函数

| 函数名称      | 函数作用             |
| ------------- | -------------------- |
| OSMemCreate() | 创建一个内存池       |
| OSMemGet()    | 申请固定大小的内存块 |
| OSMemPut()    | 进行内存的释放管理   |

- 内存池创建函数OSMemCreate()

  在使用内存池的时候首先要创建一个内存池,需要用户静态分配一个数组空间作为系统的内存池,且用户还需定义一个内存控制块.创建内存池后,任务才可以通过系统的内存申请、释放函数从内存池中申请或释放内存.

  | 参数     | 含义               |
  | -------- | ------------------ |
  | *p_mem   | 内存池控制块       |
  | *p_name  | 命名内存池         |
  | *p_addr  | 内存池首地址       |
  | n_blks   | 内存块数目         |
  | blk_size | 内存块大小         |
  | *p_err   | 指向错误类型的指针 |

  | 错误类型                       | 含义                                                 |
  | ------------------------------ | ---------------------------------------------------- |
  | OS_ERR_ILLEGAL_CREATE_RUN_TIME | 正在尝试创建内存分区                                 |
  | OS_ERR_MEM_CREATE_ISR          | 从ISR调用了此函数                                    |
  | OS_ERR_MEM_INVALID_BLKS        | 用户指定的块数无效                                   |
  | OS_ERR_MEM_INVALID_P_ADDR      | 如果为内存指定了无效地址(分区的存储或者块在指针边界) |
  | OS_ERR_MEM_INVALID_SIZE        | 用户指定的块大小无效                                 |
  | OS_ERR_NONE                    | 无错误                                               |

  ```c
  void OSMemCreate(OS_MEM *p_mem, //内存池控制块
  				 CPU_CHAR *p_name, //命名内存池
  				 void *p_addr, //内存池首地址
  				 OS_MEM_QTY n_blks, //内存块数目
  				 OS_MEM_SIZE blk_size, //内存块大小(单位:字节)
  				 OS_ERR *p_err){
  #if OS_CFG_ARG_CHK_EN > 0u
  	CPU_DATA align_msk;
  #endif
      OS_MEM_QTY i;
      OS_MEM_QTY loops;
      CPU_INT08U *p_blk;
      void **p_link; //二级指针,存放指针的指针
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
      if (p_err == (OS_ERR *)0) //如果错误类型实参为空
      {
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return; //返回,停止执行
      }
  #endif
  
  #ifdef OS_SAFETY_CRITICAL_IEC61508//如果启用了安全关键
      if (OSSafetyCriticalStartFlag == DEF_TRUE)
      {
          *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;//错误类型为“非法创建内核对象”
          return; //返回,停止执行
      }
  #endif
  
  #if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
      if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
      {
          *p_err = OS_ERR_MEM_CREATE_ISR; //错误类型为“在中断中创建对象”
          return; //返回,停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
      if (p_addr == (void *)0) //如果 p_addr 为空
      {
          *p_err = OS_ERR_MEM_INVALID_P_ADDR; //错误类型为“内存池地址非法”
          return; //返回,停止执行
  	}
      if (n_blks < (OS_MEM_QTY)2) //如果内存池的内存块数目少于 2
      {
          *p_err = OS_ERR_MEM_INVALID_BLKS; //错误类型为“内存块数目非法”
          return; //返回,停止执行
      }
      if (blk_size <sizeof(void *)) //如果内存块空间小于指针的
      {
          *p_err = OS_ERR_MEM_INVALID_SIZE; //错误类型为“内存空间非法”
          return; //返回,停止执行
      }
      align_msk = sizeof(void *) - 1u; //开始检查内存地址是否对齐
      if (align_msk > 0u)
      {
          if (((CPU_ADDR)p_addr & align_msk) != 0u) //如果首地址没对齐
          {
              *p_err = OS_ERR_MEM_INVALID_P_ADDR; //错误类型为“内存池地址非法”
              return; //返回,停止执行
          }
          if ((blk_size & align_msk) != 0u) //如果内存块地址没对齐
          {
              *p_err = OS_ERR_MEM_INVALID_SIZE; //错误类型为“内存块大小非法”
              return; //返回,停止执行
          }
  	}
  #endif
  /* 将空闲内存块串联成一个单向链表 */
      p_link = (void **)p_addr; //内存池首地址转为二级指针
      p_blk = (CPU_INT08U *)p_addr; //首个内存块地址
      loops = n_blks - 1u;
      for (i = 0u; i < loops; i++) //将内存块逐个串成单向链表
      {
          p_blk += blk_size; //下一内存块地址
          *p_link = (void *)p_blk;//在当前内存块保存下一个内存块地址
          p_link = (void **)(void *)p_blk;//下一个内存块的地址转为二级指针
      }
      *p_link = (void *)0; //最后一个内存块指向空
  
      OS_CRITICAL_ENTER(); //进入临界段
      p_mem->Type = OS_OBJ_TYPE_MEM; //设置对象的类型
      p_mem->NamePtr = p_name; //保存内存池的命名
      p_mem->AddrPtr = p_addr; //存储内存池的首地址
      p_mem->FreeListPtr = p_addr; //初始化空闲内存块池的首地址
      p_mem->NbrFree = n_blks; //存储空闲内存块的数目
      p_mem->NbrMax = n_blks; //存储内存块的总数目
      p_mem->BlkSize = blk_size; //存储内存块的空间大小
  
  #if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
      OS_MemDbgListAdd(p_mem); //将内存管理对象插入内存管理双向调试列表
  #endif
  
      OSMemQty++; //内存管理对象数目加 1
  
      OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段(无调度)
      *p_err = OS_ERR_NONE; //错误类型为“无错误”
  }
  
  ```

  内存池创建函数的使用实例

  ```c
  OS_MEM mem; //声明内存管理对象
  uint8_t ucArray [ 3 ] [ 20 ]; //声明内存池大小
  OS_ERR err;
  
  /* 创建内存管理对象 mem */
  OSMemCreate ((OS_MEM *)&mem, //指向内存管理对象
  (CPU_CHAR *)"Mem For Test", //命名内存管理对象
  (void *)ucArray, //内存池的首地址
  (OS_MEM_QTY )3, //内存池中内存块数目
  (OS_MEM_SIZE )20, //内存块的字节数目
  (OS_ERR *)&err);
  
  ```

- 内存申请函数OSMemGet()

  这个函数用于申请固定大小的内存块,从指定的内存池中分配一个内存块给用户使用,该内存块的大小在内存池初始化的时候就已经决定的.如果内存池中有可用的内存块,则从内存池的空闲内存块列表上取下一个内存块并且返回对应的内存地址；如果内存池中已经没有可用内存块,则返回0与对应的错误代码`OS_ERR_MEM_NO_FREE_BLKS`.

  | 参数   | 含义               |
  | ------ | ------------------ |
  | *p_mem | 内存管理对象       |
  | *p_err | 指向错误类型的指针 |

  | 错误类型                 | 含义                               |
  | ------------------------ | ---------------------------------- |
  | OS_ERR_MEM_INVALID_P_MEM | 传递了"p_mem"的NULL指针            |
  | OS_ERR_MEM_NO_FREE_BLKS  | 没有更多的可用内存块要分配给调用方 |
  | OS_ERR_OBJ_TYPE          | "p_mem"未指向内存分区              |
  | OS_ERR_NONE              | 无错误                             |

  ```c
  void *OSMemGet (OS_MEM *p_mem, //内存管理对象
  				 OS_ERR *p_err){
  	void *p_blk;
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
  	{
  		OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
  	if (p_mem == (OS_MEM *)0) //如果 p_mem 为空
  	{
  		*p_err = OS_ERR_MEM_INVALID_P_MEM; //错误类型为“内存池非法”
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  #endif
  
  	CPU_CRITICAL_ENTER(); //关中断
  	if (p_mem->NbrFree == (OS_MEM_QTY)0) //如果没有空闲的内存块
  	{
  		CPU_CRITICAL_EXIT(); //开中断
  		*p_err = OS_ERR_MEM_NO_FREE_BLKS; //错误类型为“没有空闲内存块”
  		return ((void *)0); //返回 0(有错误),停止执行
  	}
  	p_blk = p_mem->FreeListPtr; //如果还有空闲内存块,就获取它
  	p_mem->FreeListPtr = *(void **)p_blk; //调整空闲内存块指针
  	p_mem->NbrFree--; //空闲内存块数目减 1
  	CPU_CRITICAL_EXIT(); //开中断
  	*p_err = OS_ERR_NONE; //错误类型为“无错误”
  	return (p_blk); //返回获取到的内存块
  }
  
  ```

- 内存释放函数OSMemGet()

  嵌入式系统的内存对我们来说是十分珍贵的,任何内存块使用完后都必须被释放,否则会造成内存泄漏,导致系统发生致命错误.μC/OS提供了`OSMemPut()`函数进行内存的释放管理,使用该函数接口时,根据指定的内存控制块对象,将内存块插入内存池的空闲内存块列表中,然后增加该内存池的可用内存块数目.

  | 参数   | 含义               |
  | ------ | ------------------ |
  | *p_mem | 内存管理对象       |
  | *p_blk | 要退回的内存块     |
  | *p_err | 指向错位类型的指针 |

  | 错误类型 | 含义 |
  | -------- | ---- |
  |          |      |
  |          |      |
  |          |      |

  ```c
  void OSMemPut(OS_MEM *p_mem, //内存管理对象
  			  void *p_blk, //要退回的内存块
  			  OS_ERR *p_err){
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
  	if (p_err == (OS_ERR *)0) //如果错误类型实参为空
      {
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return; //返回,停止执行
      }
  #endif
  
  #if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
      if (p_mem == (OS_MEM *)0) //如果 p_mem 为空
      {
          *p_err = OS_ERR_MEM_INVALID_P_MEM; //错误类型为“内存池非法”
          return; //返回,停止执行
      }
      if (p_blk == (void *)0) //如果内存块为空
      {
          *p_err = OS_ERR_MEM_INVALID_P_BLK; //错误类型为" 内存块非法"
          return; //返回,停止执行
      }
  #endif
  
      CPU_CRITICAL_ENTER(); //关中断
      if (p_mem->NbrFree >= p_mem->NbrMax) //如果内存池已满
      {
          CPU_CRITICAL_EXIT(); //开中断
          *p_err = OS_ERR_MEM_FULL; //错误类型为“内存池已满”
          return; //返回,停止执行
      }
  	*(void **)p_blk = p_mem->FreeListPtr; //把内存块插入空闲内存块链表
      p_mem->FreeListPtr = p_blk; //内存块退回到链表的最前端
      p_mem->NbrFree++; //空闲内存块数目加 1
      CPU_CRITICAL_EXIT(); //开中断
      *p_err = OS_ERR_NONE; //错误类型为“无错误”
  }
  
  ```

  内存释放函数使用实例

  ```c
  OS_MEM mem; //声明内存管理对象
  OS_ERR err;
  
  /* 释放内存块 */
  OSMemPut ((OS_MEM *)&mem, //指向内存管理对象
  		  (void *)pMsg, //内存块的首地址
  		  (OS_ERR *)&err);
  
  ```

  PS:我们想要使用内存管理相关的函数时,需要将os_cfg.h中的`OS_CFG_MEM_EN`宏定义配置为1；`OSMemCreate()`只能在任务级被调用,但是`OSMemGet()`和`OSMemPut()`可以在中断中被调用.

### 内存管理实验

本次的实验例程采用消息队列进行发送与接收消息,只不过存放消息的地方是在内存块中,在获取完消息的时候,就进行释放内存块,反复使用内存块.

1. 声明内存管理对象和内存分区,定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![内存管理1_1](.\Image\内存管理1_1.png)

   ![内存管理1_2](.\Image\内存管理1_2.png)

2. 定义任务函数

   ![内存管理2](.\Image\内存管理2.png)

3. 任务启动函数编写

   ![内存管理3](.\Image\内存管理3.png)

4. 结果现象

   ![内存管理4](.\Image\内存管理4.png)

## 中断管理

### 异常与中断的基本概念

- 异常是导致处理器脱离正常运行转向执行特殊代码的任何事件,如果不及时进行处理,轻则系统出错,重则会导致系统毁灭性瘫痪.所以正确地处理异常,避免错误的发生是提高软件鲁棒性(稳定性)非常重要的一环,对于实时系统更是如此.
- 异常是指任何打断处理器正常执行,并且迫使处理器进入一个由有特权的特殊指令执行的事件.异常通常可以分成两类:
  - 同步异常和异步异常.由内部事件(像处理器指令运行产生的事件)引起的异常称为同步异常,例如造成被零除的算术运算引发一个异常,又如在某些处理器体系结构中,对于确定的数据尺寸必须从内存的偶数地址进行读和写操作.从一个奇数内存地址的读或写操作将引起存储器存取一个错误事件并引起一个异常(称为校准异常).
  - 异步异常主要是指由于外部异常源产生的异常,是一个由外部硬件装置产生的事件引起的异步异常.同步异常不同于异步异常的地方是事件的来源,同步异常事件是由于执行某些指令而从处理器内部产生的,而异步异常事件的来源是外部硬件装置.例如按下设备某个按钮产生的事件.同步异常与异步异常的区别还在于,同步异常触发后,系统必须立刻进行处理而不能够依然执行原有的程序指令步骤;而异步异常则可以延缓处理甚至是忽略,例如按键中断异常,虽然中断异常触发了,但是系统可以忽略它继续运行(同样也忽略了相应的按键事件).
- 中断,中断属于异步异常.所谓中断是指中央处理器CPU正在处理某件事的时候,外部发生了某一事件,请求CPU迅速处理,CPU暂时中断当前的工作,转入处理所发生的事件,处理完后,再回到原来被中断的地方,继续原来的工作,这样的过程称为中断.
- 中断能打断任务的运行,无论该任务具有什么样的优先级,因此中断一般用于处理比较紧急的事件,而且只做简单处理,例如标记该事件,在使用μC/OS系统时,一般建议使用信号量,消息或事件标志组等标志中断的发生,将这些内核对象发布给处理任务,处理任务再做具体处理.
- 通过中断机制,在外设不需要CPU介入时,CPU可以执行其他任务,而当外设需要CPU时通过产生中断信号使CPU立即停止当前任务转而来响应中断请求.这样可以使CPU避免把大量时间耗费在等待,查询外设状态的操作上,因此将大大提高系统实时性以及执行效率.
- μC/OS源码中有许多处临界段的地方,临界段虽然保护了关键代码的执行不被打断,但也会影响系统的实时,任何使用了操作系统的中断响应都不会比裸机快.比如,某个时候有一个任务在运行中,并且该任务部分程序将中断屏蔽掉,也就是进入临界段中,这个时候如果有一个紧急的中断事件被触发,这个中断就会被挂起,不能得到及时响应,必须等到中断开启才可以得到响应,如果屏蔽中断时间超过了紧急中断能够容忍的限度,危害是可想而知的.操作系统的中断在某些时候会产生必要的中断延迟,因此调用中断屏蔽函数进入临界段的时候,也需快进快出.
- μC/OS的中断管理支持:

  - 开/关中断.

  - 恢复中断.
  - 中断启用.
  - 中断屏蔽.
  - 中断嵌套.
  - 中断延迟发布.

#### 中断的介绍

- 与中断相关的硬件可以划分为三类:
  - 外设:当外设需要请求CPU时,产生一个中断信号,该信号连接至中断控制器.
  - 中断控制器:中断控制器是CPU众多外设中的一个,它一方面接收其他外设中断信号的输入,另一方面,它会发出中断信号给CPU.可以通过对中断控制器编程实现对中断源的优先级,触发方式,打开和关闭源等设置操作.在Cortex-M系列控制器中常用的中断控制器是NVIC(内嵌向量中断控制器Nested Vectored Interrupt Controller).
  - CPU:CPU会响应中断源的请求,中断当前正在执行的任务,转而执行中断处理程序.NVIC最多支持240个中断,每个中断最多256个优先级.

#### 中断相关的名词解释

- 中断号:每个中断请求信号都会有特定的标志,使得计算机能够判断是哪个设备提出的中断请求,这个标志就是中断号.

- 中断请求:"紧急事件"需向CPU提出申请,要求CPU暂停当前执行的任务,转而处理该"紧急事件",这一申请过程称为中断请求.

- 中断优先级:为使系统能够及时响应并处理所有中断,系统根据中断时间的重要性和紧迫程度,将中断源分为若干个级别,称作中断优先级.

- 中断处理程序:当外设产生中断请求后,CPU暂停当前的任务,转而响应中断申请,即执行中断处理程序.

- 中断触发:中断源发出并送给CPU控制信号,将中断触发器置"1",表明该中断源产生了中断,要求CPU去响应该中断,CPU暂停当前任务,执行相应的中断处理程序.

- 中断触发类型:外部中断申请通过一个物理信号发送到NVIC,可以是电平触发或边沿触发.

- 中断向量:中断服务程序的入口地址.

- 中断向量表:存储中断向量的存储区,中断向量与中断号对应,中断向量在中断向量表中按照中断号顺序存储.

- 临界段:代码的临界段也称为临界区,一旦这部分代码开始执行,则不允许任何中断打断.为确保临界段代码的执行不被中断,在进入临界段之前须关中断,而临界段代码执行完毕后,要立即开中断.

### 中断的运作机制

- 当中断产生时,处理机将按如下的顺序执行:

  1. 保存当前处理机状态信息
  2. 载入异常或中断处理函数到PC寄存器
  3. 把控制权转交给处理函数并开始执行
  4. 当处理函数执行完成时,恢复处理器状态信息
  5. 从异常或中断中返回到前一个程序执行点

- 中断使得CPU可以在事件发生时才给予处理,而不必让CPU连续不断地查询是否有相应的事件发生.通过两条特殊指令:关中断和开中断可以让处理器不响应或响应中断,在关闭中断期间,通常处理器会把新产生的中断挂起,当中断打开时立刻进行响应,所以会有适当的延时响应中断,故用户在进入临界区的时候应快进快出.

- 中断发生的环境有两种情况:在任务的上下文中,在中断服务函数处理上下文中.

  - 在任务的上下文中![中断发生的环境1](.\Image\中断发生的环境1.png)

    - 任务在工作的时候,如果此时发生了一个中断,无论中断的优先级是多大,都会打断当前

      任务的执行,从而转到对应的中断服务函数中执行.

    - 中断发生在任务上下文:①,③:在任务运行的时候发生了中断,那么中断会打断任务的运

      行,那么操作系统将先保存当前任务的上下文环境,转而去处理中断服务函数.

    - 中断发生在任务上下文:②,④:当且仅当中断服务函数处理完的时候才恢复任务的上下文

      环境,继续运行任务.

  - 在中断服务函数处理上下文中

    ![中断发生的环境2](.\Image\中断发生的环境2.png)

    - 在执行中断服务例程的过程中,如果有更高优先级别的中断源触发中断,由于当前处于中断处理上下文环境中,根据不同的处理器构架可能有不同的处理方式,比如新的中断等待挂起直到当前中断处理离开后再行响应;或新的高优先级中断打断当前中断处理过程,而去直接响应这个更高优先级的新中断源.后面这种情况,称之为中断嵌套.在硬实时环境中,前一种情况是不允许发生的,不能使响应中断的时间尽量的短.而在软件处理(软实时环境)上,μC/OS允许中断嵌套,即在一个中断服务例程期间,处理器可以响应另外一个优先级更高的中断,过程如上图"中断嵌套发生"所示.

    - 中断嵌套发生①:当中断1的服务函数在处理的时候发生了中断2,由于中断2的优先级比中断1更高,所以发生了中断嵌套,那么操作系统将先保存当前中断服务函数的上下文环境,并且转向处理中断2,当且仅当中断2执行完的时候图中断嵌套发生②,才能继续执行中断1.


### 中断延迟的概念

- 即使操作系统的响应很快了,但对于中断的处理仍然存在着中断延迟响应的问题,我们称之为中断延迟 (Interrupt Latency).

- 中断延迟是指从硬件中断发生到开始执行中断处理程序第一条指令之间的这段时间.也就是:系统接收到中断信号到操作系统作出响应,并完成换到转入中断服务程序的时间.也可以简单地理解为:(外部)硬件(设备)发生中断,到系统执行中断服务子程序(ISR)的第一条指令的时间.

- 中断的处理过程是:外界硬件发生了中断后,CPU到中断处理器读取中断向量,并且查找中断向量表,找到对应的中断服务子程序(ISR)的首地址,然后跳转到对应的ISR去做相应处理.这部分时间,我称之为:识别中断时间.

- 在允许中断嵌套的实时操作系统中,中断也是基于优先级的,允许高优先级中断抢断正在处理的低优先级中断,所以,如果当前正在处理更高优先级的中断,即使此时有低优先级的中断,也系统不会立刻响应,而是等到高优先级的中断处理完之后,才会响应.而即使在不支持中断嵌套,即中断是没有优先级的,中断是不允许被中断的,所以,如果当前系统正在处理一个中断,而此时另一个中断到来了,系统也是不会立即响应的,而只是等处理完当前的中断之后,才会处理后来的中断.此部分时间,称为:等待中断打开时间.

- 在操作系统中,很多时候我们会主动进入临界段,系统不允许当前状态被中断打断,故而在临界区发生的中断会被挂起,直到退出临界段时候打开中断.此部分时间,称为:关闭中断时间.

- 中断延迟可以定义为,从中断开始的时刻到中断服务例程开始执行的时刻之间的时间段.
  
  中断延迟 = 识别中断时间 + [等待中断打开时间] + [关闭中断时间].
  
  PS:注意:"[ ]"的时间是不一定都存在的,此处为最大可能的中断延迟时间.此外,中断恢复时间定义为:执行完ISR 中最后一句代码后到恢复到任务级代码的这段时间.任务延迟时间定义为:中断发生到恢复到任务级代码的这段时间.
  

### 中断的应用场景

- 中断在嵌入式处理器中应用非常之多,没有中断的系统不是一个好系统,因为有中断,才能启动或者停止某件事情,从而转去做另一间事情.我们可以举一个日常生活中的例子来说明,假如你正在给朋友写信,电话铃响了,这时你放下手中的笔去接电话,通话完毕再继续写信.这个例子就表现了中断及其处理的过程:电话铃声使你暂时中止当前的工作,而去处理更为急需处理的事情,当把急需处理的事情处理完毕之后,再回过头来继续原来的事情.在这个例子中,电话铃声就可以称为"中断请求",而你暂停写信去接电话就叫作"中断响应",那么接电话的过程就是"中断处理".由此我们可以看出,在计算机执行程序的过程中,由于出现某个特殊情况(或称为"特殊事件"),使得系统暂时中止现行程序,而转去执行处理这一特殊事件的程序,处理完毕之后再回到原来程序的中断点继续向下执行.

- 没有中断的系统不是好系统,举一个例子.假设有一个朋友来拜访你,但是由于不知何时到达,你只能在门口等待,于是什么事情也干不了;但如果在门口装一个门铃,你就不必在门口等待而可以在家里去做其他的工作,朋友来了按门铃通知你,这时你才中断手中的工作去开门,这就避免了不必要的等待.CPU也是一样,如果时间都浪费在查询的事情上,那这个CPU啥也干不了.在嵌入式系统中合理利用中断,能更好利用CPU的资源.


### 中断管理讲解

- ARM Cortex-M系列内核的中断是由硬件管理的,而μC/OS是软件,它并不接管由硬件管理的相关中断(接管简单来说就是,所有的中断都由RTOS的软件管理,硬件来了中断时,由软件决定是否响应,可以挂起中断,延迟响应或者不响应),只支持简单的开关中断等,所以μC/OS中的中断使用其实跟裸机差不多的,需要我们自己配置中断,并且启用中断,编写中断服务函数,在中断服务函数中使用内核IPC通信机制,一般建议使用信号量,消息或事件标志组等标志事件的发生,将事件发布给处理任务,等退出中断后再由相关处理任务具体处理中断,当然μC/OS为能让系统更快退出中断,它支持中断延迟发布,将中断级的发布变成任务级.

- ARM Cortex-M NVIC支持中断嵌套功能:当一个中断触发并且系统进行响应时,处理器硬件会将当前运行的部分上下文寄存器自动压入中断栈中,这部分的寄存器包括 PSR,R0,R1,R2,R3以及R12寄存器.当系统正在服务一个中断时,如果有一个更高优先级的中断触发,那么处理器同样的会打断当前运行的中断服务例程,然后把老的中断服务例程上下文的PSR,R0,R1,R2,R3和R12寄存器自动保存到中断栈中.这些部分上下文寄存器保存到中断栈的行为完全是硬件行为,这一点是与其他ARM处理器最大的区别(以往都需要依赖于软件保存上下文).

- 在 ARM Cortex-M系列处理器上,所有中断都采用中断向量表的方式进行处理,即当一个中断触发时,处理器将直接判定是哪个中断源,然后直接跳转到相应的固定位置进行处理.而在ARM7,ARM9中,一般是先跳转进入IRQ入口,然后再由软件进行判断是哪个中断源触发,获得了相对应的中断服务例程入口地址后,再进行后续的中断处理.ARM7,ARM9的好处在于,所有中断它们都有统一的入口地址,便于OS的统一管理.而ARM Cortex-M系列处理器则恰恰相反,每个中断服务例程必须排列在一起放在统一的地址上(这个地址必须要设置到NVIC的中断向量偏移寄存器中).中断向量表一般由一个数组定义(或在起始代码中给出).在STM32上,默认起始代码给出.

  ```assembly
  ;中断向量表(部分)
  __Vectors DCD __initial_sp ; Top of Stack
  DCD Reset_Handler ; Reset Handler
  DCD NMI_Handler ; NMI Handler
  DCD HardFault_Handler ; Hard Fault Handler
  DCD MemManage_Handler ; MPU Fault Handler
  DCD BusFault_Handler ; Bus Fault Handler
  DCD UsageFault_Handler ; Usage Fault Handler
  DCD 0 ; Reserved
  DCD 0 ; Reserved
  DCD 0 ; Reserved
  DCD 0 ; Reserved
  DCD SVC_Handler ; SVCall Handler
  DCD DebugMon_Handler ; Debug Monitor Handler
  DCD 0 ; Reserved
  DCD PendSV_Handler ; PendSV Handler
  DCD SysTick_Handler ; SysTick Handler
  
  ; External Interrupts
  DCD WWDG_IRQHandler ; Window Watchdog
  DCD PVD_IRQHandler ; PVD through EXTI Line detect
  DCD TAMPER_IRQHandler ; Tamper
  DCD RTC_IRQHandler ; RTC
  DCD FLASH_IRQHandler ; Flash
  DCD RCC_IRQHandler ; RCC
  DCD EXTI0_IRQHandler ; EXTI Line 0
  DCD EXTI1_IRQHandler ; EXTI Line 1
  DCD EXTI2_IRQHandler ; EXTI Line 2
  DCD EXTI3_IRQHandler ; EXTI Line 3
  DCD EXTI4_IRQHandler ; EXTI Line 4
  DCD DMA1_Channel1_IRQHandler ; DMA1 Channel 1
  DCD DMA1_Channel2_IRQHandler ; DMA1 Channel 2
  DCD DMA1_Channel3_IRQHandler ; DMA1 Channel 3
  DCD DMA1_Channel4_IRQHandler ; DMA1 Channel 4
  DCD DMA1_Channel5_IRQHandler ; DMA1 Channel 5
  DCD DMA1_Channel6_IRQHandler ; DMA1 Channel 6
  DCD DMA1_Channel7_IRQHandler ; DMA1 Channel 7
  
  ………
  
  ```
  
  μC/OS在Cortex-M系列处理器上也遵循与裸机中断一致的方法,当用户需要使用自定义的中断服务例程时,只需要定义相同名称的函数覆盖弱化符号即可.所以,μC/OS在Cortex-M系列处理器的中断控制其实与裸机没什么差别,不过在进入中断与退出中断的时候需要调用一下`OSIntEnter()`函数与`OSIntExit()`函数,方便中断嵌套管理.
  

### 中断延迟发布

#### 中断延迟发布的概念

- μC/OS有两种方法处理来自于中断的事件,直接发布(或者称为释放)和延迟发布.通过os_cfg.h中的`OS_CFG_ISR_POST_DEFERRED_EN`来选择,当设置为0时,μC/OS使用直接发布的方法.当设置为1时,使用延迟发布方法,用户可以根据自己设计系统的应用选择其中一种方法即可.

- 启用中断延时发布,可以将中断级发布转换成任务级发布,而且在进入临界段时也可以使用锁调度器代替关中断,这就大大减小了关中断时间,有利于提高系统的实时性(能实时响应中断而不受中断屏蔽导致响应延迟).在前面提到的 `OSTimeTick()`,`OSSemPost()`,`OSQPost()`,`OSFlagPost()`,`OSTaskSemPost()`,`OSTaskQPost()`,`OSTaskSuspend()`和`OSTaskResume()`等这些函数,如果没有使用中断延迟发布,那么调用这些函数意味着进入一段很长的临界段,也就要关中断很长时间.在启用中断延时发布后,如果在中断中调用这些函数,系统就会将这些post提交函数必要的信息保存到中断延迟提交的变量中去,为了配合中断延迟,μC/OS还将创建了优先级最高(优先级为0)的任务:中断发布函数`OS_IntQTask`,退出中断后根据之前保存的参数,在任务中再次进行post相关操作.这个过程其实就是把中断中的临界段放到任务中来实现,这个时候进入临界段就可以用锁住调度器的方式代替了关中断,因此大大减少了关中断的时间,系统将post操作延迟了,中断延迟就是这么来的.

- 进入临界段的方式可以是关中断或者锁住调度器,系统中有些变量不可能在中断中被访问,所以只要保证其他任务不要使用这些变量即可,这个时候就可以用锁调度启动的方式,用锁住调度代替关中断,大大减少了关中断的时间,也能达到进入临界段的目的.中断延迟就是利用这种思想,让本该在中断中完成的事情切换到任务中完成,而且进入临界段的方式是锁定调度器,这样子中断就不会被屏蔽,系统能随时响应中断,并且,整个中断延迟发布的过程是不影响post的效果,因为μC/OS已经设定中断发布任务的优先级为最高,在退出中断后会马上进行post操作,这与在中断中直接进行post操作的时间基本一致.

  PS:操作系统内核相关函数一般为了保证其操作的完整性,一般都会进入或长或短的临界段,所以在中断的要尽量少调用内核函数,部分μC/OS提供的函数是不允许在中断中调用的.

- 在直接发布方式中,μC/OS访问临界段时是采用关中断方式.然而,在延迟提交方式中,μC/OS访问临界段时是采用锁调度器方式.在延迟提交方式中,访问中断队列时μC/OS仍需要关中断进入临界段,但是这段关中断时间是非常短的且是固定的.

- 中断延迟发布与直接发布的区别

  - 中断延时发布

    ![中断延迟发布](.\Image\中断延迟发布.png)

    ①:进入中断,在中断中需要发布一个内核对象(如消息队列,信号量等),但是使用了中断延迟发布,在中断中值执行`OS_IntQPost()`函数,在这个函数中,采用关中断方式进入临界段,因此在这个时间段是不能响应中断的.

    ②:已经将内核对象发布到中断消息队列,那么将唤醒`OS_IntQTask`任务,因为该任务是最高优先级任务,所以能立即被唤醒,然后转到`OS_IntQTask`任务中发布内核对象,在该任务中,调用`OS_IntQRePost()`函数进行发布内核对象,进入临界段的方式采用锁调度器方式,那么在这个阶段,中断是可以被响应的.

    ③:系统正常运行,任务按优先级进行切换.

  - 中断直接发布

    ![中断直接发布](.\Image\中断直接发布.png)

    ①,②:而采用中断直接发布的情况是在中断中直接屏蔽中断以进入临界段,这段时间中,都不会响应中断,直到发布完成,系统任务正常运行才开启中断.

    ③:系统正常运行,任务按照优先级正常切换.从两个图中可以看出,采用中断延迟发布的效果更好,将本该在中断中的处理转变成为在任务中处理,系统关中断的时间大大降低,使得系统能很好地响应外部中断,如果在应用中关中断时间是关键性的,应用中有非常频繁的中断源,且应用不能接受直接发布方式那样较长的关中断时间,推荐使用中断延迟发布方式.


#### 中断队列控制块

如果启用中断延迟发布,在中断中调用内核对象发布(释放)函数,系统会将发布的内容存放在中断队列中控制块中.

中断队列信息块.

```c
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
struct os_int_q
{
    OS_OBJ_TYPE Type; //用于发布的内核对象类型
    OS_INT_Q *NextPtr; //指向下一个中断队列控制块
    void *ObjPtr; //指向内核对象变量指针
    void *MsgPtr; //如果发布的是任务消息或者是内核对象消息,指向发布消息的指针
    OS_MSG_SIZE MsgSize; //如果发布的是任务消息或者是内核对象消息,记录发布的消息的字节大小
    OS_FLAGS Flags; //如果发布的是事件标志,该成员变量记录要设置事件的标志位
    OS_OPT Opt; //记录发布内核对象时的选项
    CPU_TS TS; //记录时间戳
};
#endif

```

#### 中断延时发布相关函数

| 函数名称          | 函数作用               |
| ----------------- | ---------------------- |
| OS_IntQTaskInit() | 中断延迟发布任务初始化 |
| OS_IntQPost()     |                        |
| OS_IntQTask()     |                        |

- 中断延迟发布任务初始化OS_IntQTaskInit()

  在系统初始化的时候,如果我们启用了中断延迟发布,那么系统会根据我们自定义配置中断延迟发布任务的宏定义`OS_CFG_INT_Q_SIZE`与`OS_CFG_INT_Q_TASK_STK_SIZE`进行相关初始化,这两个宏定义在os_cfg_app.h文件中.

  ```c
  void OS_IntQTaskInit (OS_ERR *p_err)
  {
      OS_INT_Q *p_int_q;
      OS_INT_Q *p_int_q_next;
      OS_OBJ_QTY i;
  
  #ifdef OS_SAFETY_CRITICAL
      if (p_err == (OS_ERR *)0)
      {
          OS_SAFETY_CRITICAL_EXCEPTION();
          return;
      }
  #endif
  
      /* 清空延迟提交过程中溢出的计数值 */
      OSIntQOvfCtr = (OS_QTY)0u;
  
      //延迟发布信息队列的基地址必须不为空指针
      if (OSCfg_IntQBasePtr == (OS_INT_Q *)0)
      {
          *p_err = OS_ERR_INT_Q;
          return;
      }
  
  	//延迟发布队列成员必须不小于 2 个
      if (OSCfg_IntQSize < (OS_OBJ_QTY)2u)
      {
          *p_err = OS_ERR_INT_Q_SIZE;
          return;
      }
  
      //初始化延迟发布任务每次运行的最长时间记录变量
      OSIntQTaskTimeMax = (CPU_TS)0;
  
      //将定义的数据连接成一个单向链表
      p_int_q = OSCfg_IntQBasePtr;
      p_int_q_next = p_int_q;
      p_int_q_next++;
      for (i = 0u; i < OSCfg_IntQSize; i++)
      {
          //每个信息块都进行初始化
          p_int_q->Type = OS_OBJ_TYPE_NONE;
          p_int_q->ObjPtr = (void *)0;
          p_int_q->MsgPtr = (void *)0;
          p_int_q->MsgSize = (OS_MSG_SIZE)0u;
          p_int_q->Flags = (OS_FLAGS )0u;
          p_int_q->Opt = (OS_OPT )0u;
          p_int_q->NextPtr = p_int_q_next;
          p_int_q++;
          p_int_q_next++;
      }
      //将单向链表的首尾相连组成一个“圈
      p_int_q--;
      p_int_q_next = OSCfg_IntQBasePtr;
      p_int_q->NextPtr = p_int_q_next;
  
      //队列出口和入口都指向第一个
      OSIntQInPtr = p_int_q_next;
      OSIntQOutPtr = p_int_q_next;
  
      //清空延迟发布队列中需要进行发布的内核对象个数
      OSIntQNbrEntries = (OS_OBJ_QTY)0u;
      //清空延迟发布队列中历史发布的内核对象最大个数
      OSIntQNbrEntriesMax = (OS_OBJ_QTY)0u;
  
  
      if (OSCfg_IntQTaskStkBasePtr == (CPU_STK *)0)
      {
          *p_err = OS_ERR_INT_Q_STK_INVALID;
          return;
      }
  
      if (OSCfg_IntQTaskStkSize < OSCfg_StkSizeMin)
      {
          *p_err = OS_ERR_INT_Q_STK_SIZE_INVALID;
          return;
      }
      //创建延迟发布任务
      OSTaskCreate((OS_TCB *)&OSIntQTaskTCB,
      			 (CPU_CHAR *)((void *)"μC/OS-III ISR Queue Task"),
                   (OS_TASK_PTR )OS_IntQTask,
                   (void *)0,
                   (OS_PRIO )0u, //优先级最高
                   (CPU_STK *)OSCfg_IntQTaskStkBasePtr,
                   (CPU_STK_SIZE)OSCfg_IntQTaskStkLimit,
                   (CPU_STK_SIZE)OSCfg_IntQTaskStkSize,
                   (OS_MSG_QTY )0u,
                   (OS_TICK )0u,
                   (void *)0,
                   (OS_OPT )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                   (OS_ERR *)p_err);
  }
  #endif
  
  ```
  
  
  
- 中断延迟发布过程OS_IntQPost()

  如果启用了中断延迟发布,并且发送消息的函数是在中断中被调用,此时就不该立即发送消息,而是将消息的发送放在指定发布任务中,此时系统就将消息发布到租单消息队列中,等待到中断发布任务唤醒再发送消息.

  ```c
  void OS_IntQPost (OS_OBJ_TYPE type, //内核对象类型
  				  void *p_obj, //被发布的内核对象
                    void *p_void, //消息队列或任务消息
                    OS_MSG_SIZE msg_size, //消息的数目
                    OS_FLAGS flags, //事件
                    OS_OPT opt, //发布内核对象时的选项
                    CPU_TS ts, //发布内核对象时的时间戳
                    OS_ERR *p_err){
  	CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
  #ifdef OS_SAFETY_CRITICAL //如果启用(默认禁用)了安全检测
      if (p_err == (OS_ERR *)0) { //如果错误类型实参为空
          OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
          return; //返回,不继续执行
      }
  #endif
  
      CPU_CRITICAL_ENTER(); //关中断
      if (OSIntQNbrEntries < OSCfg_IntQSize) { //如果中断队列未占满
          OSIntQNbrEntries++;
          //更新中断队列的最大使用数目的历史记录
          if (OSIntQNbrEntriesMax < OSIntQNbrEntries) {
              	OSIntQNbrEntriesMax = OSIntQNbrEntries;
              }
              /* 将要重新提交的内核对象的信息放入到中断队列入口的信息记录块 */
              OSIntQInPtr->Type = type; /* 保存要发布的对象类型 */
              OSIntQInPtr->ObjPtr = p_obj; /* 保存指向要发布的对象的指针 */
              OSIntQInPtr->MsgPtr = p_void;/* 将信息保存到消息块的中 */
              OSIntQInPtr->MsgSize = msg_size; /* 保存信息的大小 */
              OSIntQInPtr->Flags = flags; /* 如果发布到事件标记组,则保存标志 */
              OSIntQInPtr->Opt = opt; /* 保存选项 */
              OSIntQInPtr->TS = ts; /* 保存时间戳信息 */
  
              OSIntQInPtr = OSIntQInPtr->NextPtr; //指向下一个中断队列入口
              /* 让中断队列管理任务 OSIntQTask 就绪 */
              OSRdyList[0].NbrEntries = (OS_OBJ_QTY)1; //更新就绪列表上的优先级 0 的任务数为 1 个
              //就绪列表的头尾指针都指向 OSIntQTask 任务
              OSRdyList[0].HeadPtr = &OSIntQTaskTCB;
              OSRdyList[0].TailPtr = &OSIntQTaskTCB;
              OS_PrioInsert(0u); //在优先级列表中增加优先级 0
              if (OSPrioCur != 0) { //如果当前运行的不是 OSIntQTask 任务
              	OSPrioSaved = OSPrioCur; //保存当前任务的优先级
  		}
  		*p_err = OS_ERR_NONE; //返回错误类型为“无错误”
  	} else { //如果中断队列已占满
  		OSIntQOvfCtr++; //中断队列溢出数目加 1
  		*p_err = OS_ERR_INT_Q_FULL;//返回错误类型为“中断队列已满”
  	}
  	CPU_CRITICAL_EXIT(); //开中断
  }
  
  ```

  

- 中断延迟发布任务OS_IntQTask()

  在中断中将消息放入中断队列,那么接下来又怎么样进行发布内核对象呢?μC/OS在中断中只是将要提交的内核对象的信息都暂时保存起来,然后就绪优先级最高的中断延迟发布任务,接着继续执行中断,在退出所有中断嵌套后,第一个执行的任务就是延迟发布任务.

  ```c
  void OS_IntQTask (void *p_arg)
  {
      CPU_BOOLEAN done;
      CPU_TS ts_start;
      CPU_TS ts_end;
      CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏
  
      p_arg = p_arg;
      while (DEF_ON) //进入死循环
      {
          done = DEF_FALSE;
          while (done == DEF_FALSE)
          {
              CPU_CRITICAL_ENTER(); //关中断
              if (OSIntQNbrEntries == (OS_OBJ_QTY)0u)
          	{
                  //如果中断队列里的内核对象发布完毕
                  //从就绪列表移除中断队列管理任务 OS_IntQTask
                  OSRdyList[0].NbrEntries = (OS_OBJ_QTY)0u;
                  OSRdyList[0].HeadPtr = (OS_TCB *)0;
                  OSRdyList[0].TailPtr = (OS_TCB *)0;
                  OS_PrioRemove(0u); //从优先级表格移除优先级 0
                  CPU_CRITICAL_EXIT(); //开中断
                  OSSched(); //任务调度
                  done = DEF_TRUE; //退出循环
          	} else {//如果中断队列里还有内核对象
                  CPU_CRITICAL_EXIT(); //开中断
                  ts_start = OS_TS_GET(); //获取时间戳
                  OS_IntQRePost(); //发布中断队列里的内核对象
                  ts_end = OS_TS_GET() - ts_start; //计算该次发布时间
                  if (OSIntQTaskTimeMax < ts_end)
                  //更新中断队列发布内核对象的最大时间的历史记录
                  {
                      OSIntQTaskTimeMax = ts_end;
                  }
                  CPU_CRITICAL_ENTER(); //关中断
                  OSIntQOutPtr = OSIntQOutPtr->NextPtr;//处理下一个
                  OSIntQNbrEntries--; //中断队列的成员数目减 1
                  CPU_CRITICAL_EXIT(); //开中断
      		}
      	}
      }
  }
  
  ```

  

### 中断管理实验

中断管理实验是在μC/OS中创建了两个任务分别获取信号量与消息队列,定义WKUP按键的触发方式为中断触发,其触发的中断服务函数则跟裸机一样,在中断触发的时候通过消息队列将消息传递给任务,任务接收到消息就将信息通过串口调试助手显示出来。而且中断管理实验也实现了一个串口的DMA传输+空闲中断功能,当串口接收完不定长的数据之后产生一个空闲中断,在中断中将信号量传递给任务,任务在收到信号量的时候将串口的数据读取出来并且在串口调试助手中回显.

1. 开启延时中断

   ![中断实验1](.\Image\中断实验1.png)

2. 定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![中断实验2_1](.\Image\中断实验2_1.png)

   ![中断实验2_2](.\Image\中断实验2_2.png)

3. 定义任务函数

   ![中断实验3](.\Image\中断实验3.png)

4. 中断服务函数编写

   ![中断实验4](.\Image\中断实验4.png)

5. 任务启动函数编写

   ![中断实验5](.\Image\中断实验5.png)

6. 结果现象

   ![中断实验6](.\Image\中断实验6.png)



## CPU利用率及栈检测统计

### CPU利用率的基本概念及作用

- CPU利用率其实就是系统运行的程序占用的CPU资源,表示机器在某段时间程序运行的情况,如果这段时间中,程序一直在占用CPU的使用权,那么可以认为CPU的利用率是100%.CPU的利用率越高,说明机器在这个时间上运行了很多程序,反之较少.利用率的高低与CPU性能强弱有直接关系,就像一段一模一样的程序,如果使用运算速度很慢的CPU,它可能要运行1000ms,而使用很运算速度很快的CPU可能只需要10ms,那么在1000ms这段时间中,前者的CPU利用率就是100%,而后者的CPU利用率只有1%,因为1000ms内前者都在使用CPU做运算,而后者只使用10ms的时间做运算,剩下的时间CPU可以做其他事情.
- μC/OS是多任务操作系统,对CPU都是分时使用的:比如A任务占用10ms,然后B任务占用30ms,然后空闲60ms,再又是A任务占10ms,B任务占30ms,空闲60ms;如果在一段时间内都是如此,那么这段时间内的利用率为40%,因为整个系统中只有40%的时间是CPU处理数据的时间.
- 一个系统设计的好坏,可以使用CPU利用率来衡量,一个好的系统必然是能完美响应急需的处理,并且系统的资源不会过于浪费(性价比高).举个例子,假设一个系统的CPU利用率经常在90%~100%徘徊,那么系统就很少有空闲的时候,这时候突然有一些事情急需CPU的处理,但是此时CPU都很可能被其他任务在占用了,那么这个紧急事件就有可能无法被相应,即启用被相应,那么占用CPU的任务又处于等待状态,这种系统就是不够完美的,因为资源处理得太过于紧迫;反过来,假如CPU的利用率在1%以下,那么我们就可以认为这种产品的资源过于浪费,搞一个那么好的CPU去干着没啥意义的活(大部分时间处于空闲状态),作为产品的设计,既不能让资源过于浪费,也不能让资源过于紧迫,这种设计才是完美的,在需要的时候能及时处理完突发事件,而且资源也不会过剩,性价比更高.
- μC/OS提供的CPU利用率统计是一个可选功能,只有将`OS_CFG_STAT_TASK_EN`宏定义启用后用户才能使用CPU利用率统计相关函数,该宏定义位于os_cfg.h文件中.

### CPU利用率统计初始化

- CPU利用率统计的原理很简单,系统中必须存在空闲任务,当且仅当CPU空闲的时候才会去执行空闲任务,那么我们就可以让CPU在空闲任务中一直做加法运算,假设某段时间T中CPU一直都在空闲任务中做加法运算(变量自加),那么这段时间算出来的值就是CPU空闲时候的最大值,我们假设为100,那么当系统中有其他任务的时候,CPU就不可能一直处于空闲任务做运算了,那么同样的一段时间T里,空闲任务算出来的值变成了80,那么是不是可以说明空闲任务只占用了系统的80%的资源,剩下的20%被其他任务占用了,这是显而易见的,同样的,利用这个原理,我们就能知道CPU的利用率大约是多少了(这种计算不会很精确),假设CPU在T时间内空闲任务中运算的最大值为OSStatTaskCtrMax(100),而有其他任务参与时候 T 时间内空闲任务运算的值为80(OSStatTaskCtr),那么CPU的利用率CPUUsage的公式应该为:

  CPUUsage(%)= 100*(1-OSStatTaskCtr / OSStatTaskCtrMax).

  假设有一次空闲任务运算的值为100(OSStatTaskCtr),说明没有其他任务参与,那么CPU的利用率就是0%,如果 OSStatTaskCtr的值为0,那么表示这段时间里CPU都没在空闲任务中运算,那么CPU的利用率自然就是100%.

  PS:一般情况下时间T由`OS_CFG_STAT_TASK_RATE_HZ`宏定义决定,是我们自己在os_cfg_app.h文件中定义的,我们的例程定义为10,该宏定义决定了统计任务的执行频率,即决定了更新一次CPU利用率的时间为1/OS_CFG_STAT_TASK_RATE_HZ,单位是秒.此外,统计任务的时钟节拍与软件定时器任务的时钟节拍一样,都是由系统时钟节拍分频得到的,如果统计任务运行的频率设定不是时钟节拍整数倍,那么统计任务实际运行的频率跟设定的就会有误差,这点跟定时器是一样的.

- 在统计CPU利用率之前必须先调用`OSStatTaskCPUUsageInit()`函数进行相关初始化,这个函数的

  目的就是为了计算只有空闲任务时CPU在某段时间内的运算最大值,也就是`OSStatTaskCtrMax`.

### 栈溢出检测概念及作用

- 如果处理器有MMU或者MPU,检测栈是否溢出是非常简单的,MMU和MPU是处理器上特殊的硬件设施,可以检测非法访问,如果任务企图访问未被允许的内存空间的话,就会产生警告,但是我们使用的STM32是没有MMU和MPU的,但是可以使用软件模拟栈检测,但是软件的模拟比较难以实现,但是 μC/OS 为我们提供了栈使用情况统计的功能,直接使用即可,如果需要使用栈溢出检测的功能,就需要用户自己在`App_OS_TaskSwHook()`钩子函数中自定义实现(我们不实现该功能),需要使用μC/OS为我们提供的栈检测功能,想要使用该功能就需要在os_cfg_app.h文件中将`OS_CFG_STAT_TASK_STK_CHK_EN`宏定义配置为1.

- 某些处理器中有一些栈溢出检测相关的寄存器,当CPU的栈指针小于(或大于,取决于栈的生长方向)设置于这个寄存器的值时,就会产生一个异常(中断),异常处理程序就需要确保未允许访问空间代码的安全(可能会发送警告给用户,或者其他处理).任务控制块中的成员变量`StkLimitPtr`就是为这种目的而设置的.每个任务的栈必须分配足够大的内存空间供任务使用,在大多数情况下,`StkLimitPtr`指针的值可以设置接近于栈顶(&TaskStk[0],假定栈是从高地址往低地址生长的,事实上STM32的栈生长方向就是向下生长,从高地址向低地址生长),`StkLimitPtr`的值在创建任务的时候由用户指定.

  ![栈溢出检测](.\Image\栈溢出检测.png)

  PS:此处的栈检测是对于带有MPU的处理器.

### 栈溢出检测过程

- 中断管理实验是在μC/OS中创建了两个任务分别获取信号量与消息队列,并且定义了两个按键KEY1与KEY2的触发方式为中断触发,其触发的中断服务函数则跟裸机一样,在中断触发的时候通过消息队列将消息传递给任务,任务接收到消息就将信息通过串口调试助手显示出来.而且中断管理实验也实现了一个串口的DMA传输+空闲中断功能,当串口接收完不定长的数据之后产生一个空闲中断,在中断中将信号量传递给任务,任务在收到信号量的时候将串口的数据读取出来并且在串口调试助手中回显.

- μC/OS 是怎么检测任务使用了多少栈的呢?以STM32的栈生长方向为例子(高地址向低地址生长),在任务初始化的时候先将任务所有的栈都置 0,使用后的栈不为 0,在检测的时候只需从栈的低地址开始对为 0 的栈空间进行计数统计,然后通过计算就可以得出任务的栈使用了多少,这样子用户就可以根据实际情况进行调整任务栈的大小,这些信息同样也会在统计任务每隔1/OSCfg_StatTaskRate_Hz 秒就进行更新.

  ![栈检测示意图](.\Image\栈检测示意图.png)

### 统计任务OS_StatTask()

μC/OS提供了统计任务的函数, 该函数为系统内部函数(任务),在启用宏定义`OS_CFG_STAT_TASK_EN`后,系统会自动创建一个统计任务`OS_StatTask()`,它会在任务中计算整个系统的CPU利用率,各个任务的CPU利用率和各个任务的栈使用信息.

```c
void OS_StatTask (void *p_arg) //统计任务函数
{
#if OS_CFG_DBG_EN > 0u
#if OS_CFG_TASK_PROFILE_EN > 0u
    OS_CPU_USAGE usage;
    OS_CYCLES cycles_total;
    OS_CYCLES cycles_div;
    OS_CYCLES cycles_mult;
    OS_CYCLES cycles_max;
#endif
    OS_TCB *p_tcb;
#endif
    OS_TICK ctr_max;
    OS_TICK ctr_mult;
    OS_TICK ctr_div;
    OS_ERR err;
    OS_TICK dly;
    CPU_TS ts_start;
    CPU_TS ts_end;
    CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏

	p_arg = p_arg;
	//没意义,仅为预防编译器警告
	while (OSStatTaskRdy != DEF_TRUE) //如果统计任务没被允许运行
	{
		OSTimeDly(2u * OSCfg_StatTaskRate_Hz, //一直延时
				  OS_OPT_TIME_DLY,
				  &err);
	}
    OSStatReset(&err); //如果统计任务已被就绪,复位统计,继续执行
    /* 根据设置的宏计算统计任务的执行节拍数 */
	dly = (OS_TICK)0;
	if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz)
	{
    	dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == (OS_TICK)0)
    {
    	dly = (OS_TICK)(OSCfg_TickRate_Hz / (OS_RATE_HZ)10);
    }

    while (DEF_ON) //进入任务体
    {
		ts_start = OS_TS_GET(); //获取时间戳
#ifdef CPU_CFG_INT_DIS_MEAS_EN//如果要测量关中断时间
		OSIntDisTimeMax = CPU_IntDisMeasMaxGet(); //获取最大的关中断时间
#endif

        CPU_CRITICAL_ENTER(); //关中断
        OSStatTaskCtrRun = OSStatTaskCtr; //获取上一次空闲任务的计数值
        OSStatTaskCtr = (OS_TICK)0; //进行下一次空闲任务计数清零
        CPU_CRITICAL_EXIT(); //开中断
        /* 计算 CPU 利用率 */
        if (OSStatTaskCtrMax > OSStatTaskCtrRun) 
        //如果空闲计数值小于最大空闲计数值
        {
            if (OSStatTaskCtrMax < 400000u)
            //这些分类是为了避免计算 CPU 利用率过程中
            {
                ctr_mult = 10000u; //产生溢出,就是避免相乘时超出 32 位寄存器.
                ctr_div = 1u;
            }
            else if (OSStatTaskCtrMax < 4000000u)
            {
                ctr_mult = 1000u;
                ctr_div = 10u;
            }
            else if (OSStatTaskCtrMax < 40000000u)
            {
                ctr_mult = 100u;
                ctr_div = 100u;
            }
            else if (OSStatTaskCtrMax < 400000000u)
            {
                ctr_mult = 10u;
                ctr_div = 1000u;
            }
            else
            {
                ctr_mult = 1u;
                ctr_div = 10000u;
            }
            ctr_max = OSStatTaskCtrMax / ctr_div;
            OSStatTaskCPUUsage = (OS_CPU_USAGE)((OS_TICK)10000u -
            ctr_mult * OSStatTaskCtrRun / ctr_max);
            if (OSStatTaskCPUUsageMax < OSStatTaskCPUUsage)
            //更新 CPU 利用率的最大历史记录
            {
                OSStatTaskCPUUsageMax = OSStatTaskCPUUsage;
            }
        } else {//如果空闲计数值大于或等于最大空闲计数值
            OSStatTaskCPUUsage = (OS_CPU_USAGE)10000u; //那么 CPU 利用率为 0
        }

        OSStatTaskHook(); //用户自定义的钩子函数

/* 下面计算各个任务的 CPU 利用率,原理跟计算整体 CPU 利用率相似 */
#if OS_CFG_DBG_EN > 0u//如果启用了调试代码和变量
#if OS_CFG_TASK_PROFILE_EN > 0u
    	//如果启用了允许统计任务信息
        cycles_total = (OS_CYCLES)0;

        CPU_CRITICAL_ENTER(); //关中断
        p_tcb = OSTaskDbgListPtr; //获取任务双向调试列表的首个任务
        CPU_CRITICAL_EXIT(); //开中断
        while (p_tcb != (OS_TCB *)0) //如果该任务非空
        {
            OS_CRITICAL_ENTER(); //进入临界段
            p_tcb->CyclesTotalPrev = p_tcb->CyclesTotal; //保存任务的运行周期
            p_tcb->CyclesTotal = (OS_CYCLES)0;
            //复位运行周期,为下次运行做准备
            OS_CRITICAL_EXIT(); //退出临界段

            cycles_total+=p_tcb->CyclesTotalPrev;//所有任务运行周期的总和

            CPU_CRITICAL_ENTER(); //关中断
            p_tcb = p_tcb->DbgNextPtr;
            //获取列表的下一个任务,进行下一次循环
            CPU_CRITICAL_EXIT(); //开中断
        }
#endif

/* 使用算法计算各个任务的 CPU 利用率和任务栈用量 */
#if OS_CFG_TASK_PROFILE_EN > 0u
    //如果启用了任务的统计功能
    if (cycles_total > (OS_CYCLES)0u) //如果有任务占用过 CPU
    {
        if (cycles_total < 400000u)
        //这些分类是为了避免计算 CPU 利用率过程中
        {
            cycles_mult = 10000u; //产生溢出,就是避免相乘时超出 32 位寄存器.
            cycles_div = 1u;
        }
        else if (cycles_total < 4000000u)
        {
            cycles_mult = 1000u;
            cycles_div = 10u;
        }
        else if (cycles_total < 40000000u)
        {
            cycles_mult = 100u;
            cycles_div = 100u;
        }
        else if (cycles_total < 400000000u)
        {
            cycles_mult = 10u;
            cycles_div = 1000u;
        }
        else
        {
            cycles_mult = 1u;
            cycles_div = 10000u;
        }
            cycles_max = cycles_total / cycles_div;
        }
        else//如果没有任务占用过 CPU
        {
            cycles_mult = 0u;
            cycles_max = 1u;
        }
#endif
        CPU_CRITICAL_ENTER(); //关中断
        p_tcb = OSTaskDbgListPtr;
        //获取任务双向调试列表的首个任务
        CPU_CRITICAL_EXIT(); //开中断
        while (p_tcb != (OS_TCB *)0) //如果该任务非空
        {
#if OS_CFG_TASK_PROFILE_EN > 0u
    //如果启用了任务控制块的简况变量
            usage = (OS_CPU_USAGE)(cycles_mult * //计算任务的 CPU 利用率
            p_tcb->CyclesTotalPrev / cycles_max);
            if (usage > 10000u) //任务的 CPU 利用率为 100%
            {
                usage = 10000u;
            }
            p_tcb->CPUUsage = usage; //保存任务的 CPU 利用率
            if (p_tcb->CPUUsageMax < usage) //更新任务的最大 CPU 利用率的历史记录
            {
                p_tcb->CPUUsageMax = usage;
            }
#endif
            /* 栈检测 */
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u//如果启用了任务栈检测
            OSTaskStkChk(p_tcb, //计算被激活任务的栈用量
                		 &p_tcb->StkFree,
			             &p_tcb->StkUsed,
			             &err);
#endif

            CPU_CRITICAL_ENTER(); //关中断
            p_tcb = p_tcb->DbgNextPtr;
            //获取列表的下一个任务,进行下一次循环
            CPU_CRITICAL_EXIT(); //开中断
    	}
#endif

        if (OSStatResetFlag == DEF_TRUE) //如果需要复位统计
        {
            OSStatResetFlag = DEF_FALSE;
            OSStatReset(&err); //复位统计
        }

        ts_end = OS_TS_GET() - ts_start; //计算统计任务的执行时间
        if (OSStatTaskTimeMax < ts_end)//更新统计任务的最大执行时间的历史记录
        {
            OSStatTaskTimeMax = ts_end;
        }

        OSTimeDly(dly,//按照先前计算的执行节拍数延时
                  OS_OPT_TIME_DLY,
                  &err);
    }
}

```



### 栈检测OSTaskStkChk()

μC/OS提供`OSTaskStkChk()`函数用来进行栈检测, 在使用之前必须将宏定义`OS_CFG_STAT_TASK_STK_CHK_EN`配置为1,对于需要进行任务栈检测的任务,在其被`OSTaskCreate()`函数创建时,选项参数`opt`还需包含`OS_OPT_TASK_STK_CHK`.统计任	务会以我们设定的运行频率不断更新栈使用的情况并且保存到任务控制块的`StkFree`和`StkUsed`成员变量中,这两个变量分别表示任务栈的剩余空间与已使用空间大小,单位为任务栈大小的单位(在STM32中采用4字节).

```c
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u//如果启用了任务栈检测
void OSTaskStkChk (OS_TCB *p_tcb, //目标任务控制块的指针
                   CPU_STK_SIZE *p_free, //返回空闲栈大小
                   CPU_STK_SIZE *p_used, //返回已用栈大小
                   OS_ERR *p_err){
    CPU_STK_SIZE free_stk;
    CPU_STK *p_stk;
    CPU_SR_ALLOC(); //使用到临界段(在关/开中断时)时必须用到该宏

#ifdef OS_SAFETY_CRITICAL//如果启用了安全检测
    if (p_err == (OS_ERR *)0) //如果 p_err 为空
    {
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return; //返回,停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u//如果启用了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) //如果该函数是在中断中被调用
    {
        *p_err = OS_ERR_TASK_STK_CHK_ISR; //错误类型为“在中断中检测栈”
        return; //返回,停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u//如果启用了参数检测
    if (p_free == (CPU_STK_SIZE*)0) //如果 p_free 为空
    {
        *p_err = OS_ERR_PTR_INVALID; //错误类型为“指针非法”
        return; //返回,停止执行
    }

    if (p_used == (CPU_STK_SIZE*)0) //如果 p_used 为空
    {
        *p_err = OS_ERR_PTR_INVALID; //错误类型为“指针非法”
        return; //返回,停止执行
    }
#endif

CPU_CRITICAL_ENTER(); //关中断
    if (p_tcb == (OS_TCB *)0) //如果 p_tcb 为空
    {
        p_tcb = OSTCBCurPtr;//目标任务为当前运行任务(自身)
    }

    if (p_tcb->StkPtr == (CPU_STK*)0) //如果目标任务的栈为空
    {
        CPU_CRITICAL_EXIT(); //开中断
        *p_free = (CPU_STK_SIZE)0; //清零 p_free
        *p_used = (CPU_STK_SIZE)0; //清零 p_used
        *p_err = OS_ERR_TASK_NOT_EXIST; //错误类型为“任务不存在
        return; //返回,停止执行
    }
    /* 如果目标任务的栈非空 */
    if ((p_tcb->Opt & OS_OPT_TASK_STK_CHK) == (OS_OPT)0) //如果目标任务没选择检测栈
    {
        CPU_CRITICAL_EXIT(); //开中断
        *p_free = (CPU_STK_SIZE)0; //清零 p_free
        *p_used = (CPU_STK_SIZE)0; //清零 p_used
        *p_err = OS_ERR_TASK_OPT;
        //错误类型为“任务选项有误”
        return; //返回,停止执行
    }
    CPU_CRITICAL_EXIT();//如果任务选择了检测栈,开中断
    /* 开始计算目标任务的栈的空闲数目和已用数目 */
    free_stk = 0u; //初始化计算栈工作
#if CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO
    //如果 CPU 的栈是从高向低增长
    p_stk = p_tcb->StkBasePtr;
    //从目标任务栈最低地址开始计算
    while (*p_stk == (CPU_STK)0) //计算值为 0 的栈数目
    {
        p_stk++;
        free_stk++;
    }
#else
    //如果 CPU 的栈是从低向高增长
    p_stk = p_tcb->StkBasePtr + p_tcb->StkSize - 1u;
    //从目标任务栈最高地址开始计算
    while (*p_stk == (CPU_STK)0) //计算值为 0 的栈数目
    {
        free_stk++;
        p_stk--;
    }
    #endif
    *p_free = free_stk;
    //返回目标任务栈的空闲数目
    *p_used = (p_tcb->StkSize - free_stk);
    //返回目标任务栈的已用数目
    *p_err = OS_ERR_NONE; //错误类型为“无错误”
}
#endif

```



### 任务栈大小的确定

- 任务栈的大小取决于该任务的需求,设定栈大小时,我们就需要考虑:所有可能被栈调用的函数及其函数的嵌套层数,相关局部变量的大小,中断服务程序所需要的空间,另外,栈还需存入CPU寄存器,如果处理器有浮点数单元FPU寄存器的话还需存入FPU寄存器.
- 嵌入式系统的潜规则,避免写递归函数,这样子可以人为计算出一个任务需要的栈空间大小,逐级嵌套所有可能被调用的函数,计数被调用函数中所有的参数,计算上下文切换时的CPU寄存器空间,计算切换到中断时所需的CPU寄存器空间(假如CPU没有独立的栈用于处理中断),计算处理中断服务函数(ISR)所需的栈空间,将这些值相加即可得到任务最小的需求空间,但是我们不可能计算出精确的栈空间,我们通常会将这个值再乘以1.5到2.0以确保任务的安全运行.这个计算的值是假定在任务所有的执行路线都是已知的情况下的,但这在真正的应用中并不太可能,比如说,如果调用printf()函数或者其他的函数,这些函数所需要的空间是很难测得或者说就是不可能知道的,在这种情况下,我们这种人为计算任务栈大小的方法就变得不太可能了,那么我们可以在刚开始创建任务的时候给任务设置一个较大的栈空间,并监测该任务运行时栈空间的实际使用量,运行一段时间后得到任务的最大栈使用情况(或者叫任务栈最坏结果),然后用该值乘1.5到2.0作为栈空间大小就差不多可以作为任务栈的空间大小,这样子得到的值就会比较精确一点,在调试阶段可以这样子进行测试,发现崩溃就增大任务的栈空间,直到任务能正常稳定运行为止.

### CPU利用率及栈检测统计实验

CPU利用率及栈检测统计实验是在μC/OS中创建了四个任务,其中三个任务是普通任务,另一个任务用于获取CPU利用率与任务相关信息并通过串口打印出来.

1. 启用宏定义`OS_CFG_STAT_TASK_EN`.

   ![统计任务实验1](.\Image\统计任务实验1.png)

2. 声明内存管理对象和内存分区,定义任务空间栈的大小以及任务栈数组,任务控制块和优先级.

   ![统计任务实验1_1](.\Image\统计任务实验2_1.png)

   ![统计任务实验1_2](.\Image\统计任务实验2_2.png)

3. 定义任务函数

   ![统计任务实验3](.\Image\统计任务实验3.png)

4. 任务启动函数编写

   ![统计任务实验4](.\Image\统计任务实验4.png)

5. 结果现象

   ![统计任务实验5](.\Image\统计任务实验5.png)

#include "app.h"

OS_TCB AppTaskLED1TCB;
OS_TCB AppTaskLED2TCB;
OS_TCB AppTaskUSART1TCB;

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
		




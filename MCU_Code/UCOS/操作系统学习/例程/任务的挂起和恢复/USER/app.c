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
			printf("�ָ�LED2����\r\n");
			OSTaskResume(&AppTaskUSART1TCB, &err);
			printf("�ָ�USART1����\r\n");
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
		
		value = OSTaskRegGet ( 0, 0, & err );            //��ȡ��������Ĵ���ֵ
		
		if ( value < 5 )                                 //�������Ĵ���ֵ<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //�����ۼ�����Ĵ���ֵ
		}
		else                                             //����ۼӵ�5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //������Ĵ���ֵ��0
			printf("����LED2����������\r\n");
			OSTaskSuspend (&AppTaskLED2TCB, & err );       //�������� OSTaskSuspend()��һ��������0ʱ��������
			
		}
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		
	}
}

void  AppTaskUsart1( void * p_arg ){
	OS_ERR      err;
	OS_REG      value;


	(void)p_arg;


	while (1) {                                 //�����壬ͨ��д��һ����ѭ��                            
		printf("AppTaskUsart Running\n");
		value = OSTaskRegGet ( 0, 0, & err );            //��ȡ��������Ĵ���ֵ
		
		if ( value < 5 )                                 //�������Ĵ���ֵ<5
		{
			OSTaskRegSet ( 0, 0, ++ value, & err );        //�����ۼ�����Ĵ���ֵ
		}
		else                                             //����ۼӵ�5
		{
			OSTaskRegSet ( 0, 0, 0, & err );               //������Ĵ���ֵ����
			printf("����Usart����������\n");
			OSTaskSuspend ( 0, & err );                    //��������
			
		}

		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );      //�������ʱ1000��ʱ�ӽ��ģ�1s��
		}
}
		




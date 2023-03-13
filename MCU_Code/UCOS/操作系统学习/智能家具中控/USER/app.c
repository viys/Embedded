#include "app.h"
#include "lcd.h"
#include "lcd_init.h"
#include "nrf24l01.h"
#include "bsp_usart.h"
#include "bsp_key.h"

OS_TCB AppTaskNRFTCB; 	//3
OS_TCB AppTaskLCDTCB; 	//4
OS_TCB AppTaskWKUPTCB;	//5

CPU_STK AppTaskNRFStk[APP_TASK_NRF_STK_SIZE];
CPU_STK AppTaskLCDStk[APP_TASK_LCD_STK_SIZE];
CPU_STK AppTaskWKUPStk[APP_TASK_WKUP_STK_SIZE];

void AppTaskLCD(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OS_CRITICAL_ENTER();
//		LCD_ShowString(0,16,"NRF",WHITE,BLACK,16,0);
		LCD_Fill(0,0,8,16,WHITE);
		LCD_Fill(8,8,8,16,BLACK);
		LCD_Fill(16,16,8,16,BLUE);
		LCD_Fill(24,24,8,16,BRED);
//		LCD_Fill(32,0,8,16,GRED);
//		LCD_Fill(40,0,8,16,GBLUE);
//		LCD_Fill(48,0,8,16,RED);
//		LCD_Fill(56,0,8,16,MAGENTA); 
//		LCD_Fill(64,0,8,16,GREEN      );
//		LCD_Fill(72,0,8,16,CYAN       );
//		LCD_Fill(80,0,8,16,YELLOW     );
//		LCD_Fill(88,0,8,16,BROWN 		);
//		LCD_Fill(0,0,8,16,BRRED 		);
//		LCD_Fill(0,0,8,16,GRAY  		);
//		LCD_Fill(0,0,8,16,DARKBLUE   );
//		LCD_Fill(0,0,8,16,LIGHTBLUE  );
//		LCD_Fill(0,0,8,16,GRAYBLUE   );
//		LCD_Fill(0,0,8,16,LIGHTGREEN );
//		LCD_Fill(0,0,8,16,LGRAY 		);
//		LCD_Fill(0,0,8,16,LGRAYBLUE  );
//		LCD_Fill(0,0,8,16,LBBLUE     );
		
		
		
		
		

		OS_CRITICAL_EXIT();
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);

	}
}

void AppTaskWKUP(void * p_arg){
	OS_ERR err;
	(void)p_arg;
	
	CPU_SR_ALLOC();
	
	while(DEF_TRUE){
		OSTaskSemPend((OS_TICK)0,
					  (OS_OPT)OS_OPT_PEND_BLOCKING,
					  (CPU_TS *)0,
					  (OS_ERR *)&err);
		
		OS_CRITICAL_ENTER();
		LCD_Off();
		printf("´¥·¢°´¼üÖÐ¶Ï\r\n");
		OS_CRITICAL_EXIT();
	}
}

void AppTaskNRF(void *p_arg){
	OS_ERR err;
	(void)p_arg;
	CPU_SR_ALLOC();
	
	u8 rx_buff[5]={0};
	while(DEF_TRUE){
		OS_CRITICAL_ENTER();
		NRF24L01_RX_Mode();
		if(NRF24L01_RxPacket(rx_buff)==0){
			for(int i=0;i<5;i++){
				printf("RX_BUFF[%d]:%x \r\n",i,rx_buff[i]);
			}
			//LCD_ShowString(10,50,"NRF_OK",RED,WHITE,16,0);
		}
		OS_CRITICAL_EXIT();
//		LCD_ShowString(10,50,"NRF_OK",RED,WHITE,16,0);
		OSTimeDly(100,OS_OPT_TIME_DLY,&err);

	}
}


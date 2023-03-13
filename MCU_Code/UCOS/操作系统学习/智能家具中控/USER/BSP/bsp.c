#include "bsp.h"
#include "lcd.h"
#include "lcd_init.h"
#include "bsp_relays.h"
#include "nrf24l01.h"
#include "systick.h"
#include "bsp_usart.h"
#include "bsp_key.h"

//BSP��ʼ������
void BSP_Init(void){
	
	USART_Config(USART_1,USART_MODE_COMMON,115200); //���ڳ�ʼ��
	
	LCD_Init();//LCD��ʼ��
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
	LCD_ShowString(0,0,"UCOSIII_RUN",WHITE,BLACK,16,0);
	EXTI_Key_Config(WKUP,1,1);
	
	NRF24L01_Init(); //������Ƶģ��
	while(NRF24L01_Check()){
		printf("NRF_ERR\r\n");
		SysTick_Delay(500);
	}


	Relays_Config(RELAYS1); //�̵���1
	Relays_Config(RELAYS2); //�̵���2
	Relays_State(RELAYS1,RELAYS_ON);
	Relays_State(RELAYS2,RELAYS_ON);
}

CPU_INT32U BSP_CPU_ClkFreq(void){
	RCC_ClocksTypeDef Clocks_InitStructure;
	
	RCC_GetClocksFreq(&Clocks_InitStructure);
	
	return ((CPU_INT32U)Clocks_InitStructure.HCLK_Frequency);
}


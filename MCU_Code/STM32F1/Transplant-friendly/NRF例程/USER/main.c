#include "stm32f10x.h"
#include "usart.h"
#include "spi.h"
#include "nrf24l01.h"
#include "systick.h"
#include "led.h"

u8 tx_buff[5]={0x11,0x22,0x33,0x44,0x55};
u8 rx_buff[5]={0};

#define TX_MODE 0
#define RX_MODE 1

int main(void){
	HSE_SetSysClock(RCC_PLLMul_9);//启用外部时钟
	
	NRF24L01_Init();
	USART_Config(USART_1,USART_MODE_EXTI,115200);
	LED_GPIO_Config(LED3);
	printf("HELLO\r\n");
	

	while(NRF24L01_Check()){
		printf("NRF24L01 Error\r\n");
		SysTick_Delay(500);
	}
	printf("NRF24L01 OK\r\n"); 
	
#if	RX_MODE
	
	NRF24L01_RX_Mode();
	
	while(1){		
		if(NRF24L01_RxPacket(rx_buff)==0){
			for(int i=0;i<5;i++){
				printf("RX_BUFF[%d]:%x \r\n",i,rx_buff[i]);
			}
			LED_State_Config(LED3, TOGGLE);
		}
		
		SysTick_Delay(100);
	}
#endif

#if TX_MODE
	
	NRF24L01_TX_Mode();
	
	while(1){
		if(NRF24L01_TxPacket(tx_buff)==TX_OK){
			LED_State_Config(LED3, TOGGLE);
			printf("发射成功\r\n");

		}else{
			printf("发射失败\r\n");
		}
		SysTick_Delay(300);
	}
	
#endif
}


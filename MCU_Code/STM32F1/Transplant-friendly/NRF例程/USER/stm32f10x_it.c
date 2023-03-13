#include "stm32f10x_it.h"
#include "usart.h"
#include "spi.h"

void SysTick_Handler(){

}

//void USART1_IRQHandler(void){
//	uint8_t ucTemp;
//	uint8_t ucTemp1;
//	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){		
//		ucTemp = USART_ReceiveData(USART1);
//		SPI_I2S_SendData(SPI1,ucTemp);
//		ucTemp1=SPI_I2S_ReceiveData(SPI1);
//		USART_SendData(USART1,ucTemp1);
//	}	 
//}


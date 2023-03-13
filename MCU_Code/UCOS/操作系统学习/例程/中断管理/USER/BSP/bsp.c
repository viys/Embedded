#include "bsp.h"



/*!
    \brief      BSP初始化程序
    \Note	    none			
    \param[in]  none
    \retval     none
*/
void BSP_Init(void){
	EXTI_Key_Config(WKUP,0,0);
	//USART_Config(USART_1,USART_MODE_COMMON,115200);
	USARTx_Config ();   //初始化 USART1
	USARTx_DMA_Config();
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

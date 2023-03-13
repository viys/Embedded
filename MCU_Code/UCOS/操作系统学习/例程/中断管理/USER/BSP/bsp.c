#include "bsp.h"



/*!
    \brief      BSP��ʼ������
    \Note	    none			
    \param[in]  none
    \retval     none
*/
void BSP_Init(void){
	EXTI_Key_Config(WKUP,0,0);
	//USART_Config(USART_1,USART_MODE_COMMON,115200);
	USARTx_Config ();   //��ʼ�� USART1
	USARTx_DMA_Config();
}

/*!
    \brief      ʱ�Ӷ�ȡ����
    \Note	    none			
    \param[in]  none
    \retval     ʱ��Ƶ��
*/
CPU_INT32U BSP_CPU_ClkFreq(void){
	RCC_ClocksTypeDef Clocks_InitStructure;
	
	RCC_GetClocksFreq(&Clocks_InitStructure);
	
	return ((CPU_INT32U)Clocks_InitStructure.HCLK_Frequency);
}

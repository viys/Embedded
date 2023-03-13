#include "bsp.h"



/*!
    \brief      BSP��ʼ������
    \Note	    none			
    \param[in]  none
    \retval     none
*/
void BSP_Init(void){
	LED_GPIO_Config(LED1);
	LED_GPIO_Config(LED2);
	LED_State_Config(LED1,OFF);
	LED_State_Config(LED2,OFF);
	USART_Config(USART_1,USART_MODE_COMMON,115200);
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

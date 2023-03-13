#include "rccclk.h"

/*!
    \brief      ϵͳʱ�����ú���
    \Note	    PLLCLK = HSE(8M) * RCC_PLLMul_x
    \param[in]  none
    \retval     none
*/
void HSE_SetSysClock(uint32_t RCC_PLLMul_x)
{
	ErrorStatus HSEStatus;

	//��rcc�Ĵ�����λ�ɸ�λֵ
	RCC_DeInit();

	//ʹ��HSE
	RCC_HSEConfig(RCC_HSE_ON);

	//�ȴ�HSE����
	HSEStatus = RCC_WaitForHSEStartUp();

	//�ȴ�HSE�ȶ�
	if (SUCCESS == HSEStatus)
	{
		//ʹ��Ԥȡַ
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);
		//�������ߵķ�Ƶ����
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		//���໷������ PLLCLK = HSE * RCC_PLLMul_x
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_x);
		
		//ʹ��PLL
		RCC_PLLCmd(ENABLE);

		//�ȴ�PLL�ȶ�
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//ѡ��ϵͳʱ��
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		while (RCC_GetSYSCLKSource() != 0x08);
	}
	else
	{
		//�������ʧ��.�û�������������Ӵ������Ĵ���
		while (1);
	}
}

/*!
    \brief      ϵͳʱ�����ú���
    \Note	    PLLCLK = HSI(4M) * RCC_PLLMul_x
    \param[in]  RCC_PLLMul_2 ...
    \retval     none
*/
void HSI_SetSysClock(uint32_t RCC_PLLMul_x)
{
	//��rcc�Ĵ�����λ�ɸ�λֵ
	RCC_DeInit();

	//ʹ��HSI
	RCC_HSICmd(ENABLE);
	
	//�ȴ�HSI����
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)== RESET);
	
    //ʹ��Ԥȡַ
//    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
//    FLASH_SetLatency(FLASH_Latency_2);

    //�������ߵķ�Ƶ����
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
	//���໷������ PLLCLK = HSE * RCC_PLLMul_x
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_x);

    //����PLL 
    RCC_PLLCmd(ENABLE);

    //�ȴ�PLL�ȶ�
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    //ѡ��ϵͳʱ��
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while (RCC_GetSYSCLKSource() != 0x08);

}

/*!
    \brief      MOC��ʼ������
    \Note	    void RCC_MCOConfig(uint8_t RCC_MCO); ֮��ʹ�øú�����MOC�������øù����ܽ�STM32�ڲ���ʱ��ͨ��PA.8���			
    \param[in]  none
    \retval     none
*/
void MCO_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

#include "bsp_clkcongfig.h"

void User_HSE_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ)
{
	/* ����RCC_WaitForHSEStartUp()�����ķ���ֵ */
	ErrorStatus HSE_ErrorStatus = ERROR ;
	/* ��λRCC�����мĴ��� */
	RCC_DeInit ();
	
	/* ʹ��HSE */
	RCC_HSEConfig(RCC_HSE_ON) ;
	
	/* �ж�HES�Ƿ������ɹ� */
	HSE_ErrorStatus = RCC_WaitForHSEStartUp() ;
	
	if(HSE_ErrorStatus == SUCCESS )
	{
		/* ѡ���ѹ��������ģʽΪ1 */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS ; 
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);
		
		/* �������໷��Ƶ���� */
		RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);
		
		/* ʹ�� */
		RCC_PLLCmd(ENABLE );
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET )
		{
		}
		
		/* ѡ����ϵͳʱ�ӣ�����FLASHԤȡַ��ָ��棬�ȴ����ڣ� */
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS ;
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK );
		
		while(RCC_GetSYSCLKSource () != 0x08)
		{
		}
		
	}
	else
	{
		/* HSE����ʧ�� */
	}

}

void User_HEI_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ)
{
	volatile uint32_t  HSI_ErrorStatus = 0 ;
	/* ��λRCC�����мĴ��� */
	RCC_DeInit ();
	
	/* ʹ��HSI */
	RCC_HSICmd(ENABLE ) ;
	
	HSI_ErrorStatus = RCC->CR & RCC_CR_HSIRDY ;
	
	/* �ж�HES�Ƿ������ɹ� */
	HSI_ErrorStatus = RCC_WaitForHSEStartUp() ;
	
	if(HSI_ErrorStatus == SUCCESS )
	{
		/* ѡ���ѹ��������ģʽΪ1 */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS ; 
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);
		
		/* �������໷ */
		RCC_PLLConfig(RCC_PLLSource_HSI, PLLM, PLLN, PLLP, PLLQ);
		
		/* ʹ�� */
		RCC_PLLCmd(ENABLE );
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET )
		{
		}
		
		/* ѡ����ϵͳʱ�ӣ�����FLASHԤȡַ��ָ��棬�ȴ����ڣ� */
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS ;
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK );
		
		while(RCC_GetSYSCLKSource () != 0x08)
		{
		}
		
	}
	else
	{
		/* HSE����ʧ�� */
	}

}

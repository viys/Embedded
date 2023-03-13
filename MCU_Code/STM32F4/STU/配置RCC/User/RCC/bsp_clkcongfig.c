#include "bsp_clkcongfig.h"

void User_HSE_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ)
{
	/* 保存RCC_WaitForHSEStartUp()函数的返回值 */
	ErrorStatus HSE_ErrorStatus = ERROR ;
	/* 复位RCC的所有寄存器 */
	RCC_DeInit ();
	
	/* 使能HSE */
	RCC_HSEConfig(RCC_HSE_ON) ;
	
	/* 判断HES是否启动成功 */
	HSE_ErrorStatus = RCC_WaitForHSEStartUp() ;
	
	if(HSE_ErrorStatus == SUCCESS )
	{
		/* 选择电压调节器的模式为1 */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS ; 
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);
		
		/* 配置锁相环倍频因子 */
		RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);
		
		/* 使能 */
		RCC_PLLCmd(ENABLE );
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET )
		{
		}
		
		/* 选择主系统时钟（配置FLASH预取址，指令缓存，等待周期） */
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS ;
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK );
		
		while(RCC_GetSYSCLKSource () != 0x08)
		{
		}
		
	}
	else
	{
		/* HSE启动失败 */
	}

}

void User_HEI_SetSysClock(uint32_t PLLM, uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ)
{
	volatile uint32_t  HSI_ErrorStatus = 0 ;
	/* 复位RCC的所有寄存器 */
	RCC_DeInit ();
	
	/* 使能HSI */
	RCC_HSICmd(ENABLE ) ;
	
	HSI_ErrorStatus = RCC->CR & RCC_CR_HSIRDY ;
	
	/* 判断HES是否启动成功 */
	HSI_ErrorStatus = RCC_WaitForHSEStartUp() ;
	
	if(HSI_ErrorStatus == SUCCESS )
	{
		/* 选择电压调节器的模式为1 */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS ; 
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);
		
		/* 配置锁相环 */
		RCC_PLLConfig(RCC_PLLSource_HSI, PLLM, PLLN, PLLP, PLLQ);
		
		/* 使能 */
		RCC_PLLCmd(ENABLE );
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET )
		{
		}
		
		/* 选择主系统时钟（配置FLASH预取址，指令缓存，等待周期） */
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS ;
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK );
		
		while(RCC_GetSYSCLKSource () != 0x08)
		{
		}
		
	}
	else
	{
		/* HSE启动失败 */
	}

}

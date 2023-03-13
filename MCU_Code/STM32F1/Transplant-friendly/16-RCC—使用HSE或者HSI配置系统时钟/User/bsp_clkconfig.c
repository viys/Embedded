#include "bsp_clkconfig.h"

/*!
    \brief      系统时钟配置函数
    \Note	    PLLCLK = HSE(8M) * RCC_PLLMul_x
    \param[in]  none
    \retval     none
*/
void HSE_SetSysClock(uint32_t RCC_PLLMul_x)
{
	ErrorStatus HSEStatus;

	//把rcc寄存器复位成复位值
	RCC_DeInit();

	//使能HSE
	RCC_HSEConfig(RCC_HSE_ON);

	//等待HSE就绪
	HSEStatus = RCC_WaitForHSEStartUp();

	//等待HSE稳定
	if (SUCCESS == HSEStatus)
	{
		//使能预取址
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);
		//配置总线的分频因子
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		//锁相环的配置 PLLCLK = HSE * RCC_PLLMul_x
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_x);
		
		//使能PLL
		RCC_PLLCmd(ENABLE);

		//等待PLL稳定
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//选择系统时钟
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		while (RCC_GetSYSCLKSource() != 0x08);
	}
	else
	{
		//如果启动失败.用户可以在这里添加处理错误的代码
		while (1);
	}
}

/*!
    \brief      系统时钟配置函数
    \Note	    PLLCLK = HSI(4M) * RCC_PLLMul_x
    \param[in]  RCC_PLLMul_2 ...
    \retval     none
*/
void HSI_SetSysClock(uint32_t RCC_PLLMul_x)
{
	//把rcc寄存器复位成复位值
	RCC_DeInit();

	//使能HSI
	RCC_HSICmd(ENABLE);
	
	//等待HSI就绪
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)== RESET);
	
    //使能预取址
//    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
//    FLASH_SetLatency(FLASH_Latency_2);

    //配置总线的分频因子
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
	//锁相环的配置 PLLCLK = HSE * RCC_PLLMul_x
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_x);

    //开启PLL 
    RCC_PLLCmd(ENABLE);

    //等待PLL稳定
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    //选择系统时钟
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while (RCC_GetSYSCLKSource() != 0x08);

}

/*!
    \brief      MOC初始化函数
    \Note	    void RCC_MCOConfig(uint8_t RCC_MCO); 之后使用该函数对MOC进行配置该功能能将STM32内部的时钟通过PA.8输出			
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

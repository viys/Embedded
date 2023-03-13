#include "systick.h"

static __IO u32 TimingDelay;


void TimingDelay_Decrement(void){
	if (TimingDelay != 0x00){ 
		TimingDelay--;
	}
}

void SysTick_Init(void){

	if (SysTick_Config(72000)){ 
		/* Capture error */ 
		while (1);
	}
}


//ϵͳ�δ�ʱ��
void SysTick_Delay(uint32_t ms){
	SysTick_Config(72000);
	
	for(uint32_t i = 0; i < ms; i++){
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// �ر�SysTick��ʱ��
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}

void HSE_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

	// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
  RCC_DeInit();

  //ʹ��HSE�������ⲿ����Ұ�𿪷����õ���8M
  RCC_HSEConfig(RCC_HSE_ON);

  // �ȴ� HSE �����ȶ�
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	// ֻ�� HSE �ȶ�֮�����������ִ��
  if (HSEStartUpStatus == SUCCESS)
  {
//----------------------------------------------------------------------//
    // ʹ��FLASH Ԥ��ȡ������
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
		// ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
		// ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
		// 0��0 < SYSCLK <= 24M
		// 1��24< SYSCLK <= 48M
		// 2��48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------���ø���Ƶ����Ҫ��������������-------------------//
    // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
		// PLLCLK = 8MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
//------------------------------------------------------------------//

    // ����PLL 
    RCC_PLLCmd(ENABLE);

    // �ȴ� PLL�ȶ�
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // ���HSE����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
		// ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
		// HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
    while (1)
    {
    }
  }
}
//�����Ҫ���Ӳ���ϵͳʱ�ӣ����������¼��Ӳ����˿ڣ��������õ���PA(8)��

void MCO_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;      
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;   
	GPIO_Init(GPIOA,&GPIO_InitStructure);          
}

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
  RCC_DeInit();

  //ʹ��HSI
	RCC_HSICmd(ENABLE);
	
  // �ȴ� HSI ����
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	
	// ֻ�� HSI����֮�����������ִ��
  if (HSIStartUpStatus == RCC_CR_HSIRDY)
  {
//----------------------------------------------------------------------//
    // ʹ��FLASH Ԥ��ȡ������
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
		// ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
		// ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
		// 0��0 < SYSCLK <= 24M
		// 1��24< SYSCLK <= 48M
		// 2��48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//
 
    // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
//-----------------���ø���Ƶ����Ҫ��������������-------------------//
    // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
		// PLLCLK = 4MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
//------------------------------------------------------------------//

    // ����PLL 
    RCC_PLLCmd(ENABLE);

    // �ȴ� PLL�ȶ�
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // ���HSI����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
		// ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
		// HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
    while (1)
    {
    }
  }
}

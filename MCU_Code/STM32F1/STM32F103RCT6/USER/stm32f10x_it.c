#include "stm32f10x_it.h"
#include "usart.h"
#include "fatfsuser.h"
#include "led.h"
#include "adc.h"
#include "basictim.h"
#include "gentim.h"
#include "advancetim.h"
#include "wdg.h"

void HardFault_Handler(void)
{
	printf("hardfault error\r\n");
	while(1)
	{
		
	}
}

void SysTick_Handler(void)
{
	//�õ��жϱ���Ҫ���жϺ���
	//TimingDelay_Decrement();	
}


void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT);//ι��
	
	WWDG_ClearFlag();
	LED_State_Config(LED3, TOGGLE);
}


//KEY�жϷ�����
char fpath[100]; //���浱ǰɨ��·��

void EXTI0_IRQHandler(void)
{
	if(RESET != EXTI_GetITStatus(EXTI_Line0))
	{
		LED_State_Config(LED3, TOGGLE);
		
//		strcpy(fpath,"1:");
//		scan_files(fpath) ;
//		printf("The file scan is complete\r\n");
		
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}


/**
@brief	����1�����жϷ�����
@Note		
@param	None
@retval	None
*/
void USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(USART1);
		USART_SendData(USART1,ucTemp);    
	}	 
}

//ADCx�жϷ�����
uint16_t ADC_ConvertedValue;
void ADC1_2_IRQHandler(void)
{
	if(SET == ADC_GetITStatus(ADCx, ADC_IT_EOC))
	{
		ADC_ConvertedValue = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx, ADC_IT_EOC);
}

uint16_t basictime = 0;
void BASIC_TIM_IRQHandler(void)
{
	if(RESET != TIM_GetITStatus(BASIC_TIM, TIM_IT_Update))
	{
		basictime++;
		TIM_ClearITPendingBit(BASIC_TIM, TIM_FLAG_Update);
	}
}

void GENERAL_CAP_TIM_INT_FUN(void)
{
	// ��Ҫ��������źŵ����ڴ��ڶ�ʱ�������ʱʱ����ʱ���ͻ���������������ж�
	// ���ʱ��������Ҫ�������Ķ�ʱ���ڼӵ������źŵ�ʱ������ȥ
	if ( TIM_GetITStatus ( GENERAL_CAP_TIM, TIM_IT_Update) != RESET )               
	{	
		TIM_ICUserValueStructure.Capture_Period ++;		
		TIM_ClearITPendingBit ( GENERAL_CAP_TIM, TIM_FLAG_Update ); 		
	}

	// �����ز����ж�
	if ( TIM_GetITStatus (GENERAL_CAP_TIM, GENERAL_CAP_TIM_IT_CCx ) != RESET)
	{
		// ��һ�β���
		if ( TIM_ICUserValueStructure.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( GENERAL_CAP_TIM, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			TIM_ICUserValueStructure.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			TIM_ICUserValueStructure.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			GENERAL_CAP_TIM_OCxPolarityConfigCmd(GENERAL_CAP_TIM, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			TIM_ICUserValueStructure.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			TIM_ICUserValueStructure.Capture_CcrValue = 
			GENERAL_CAP_TIM_GetCapturexCmd(GENERAL_CAP_TIM);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			GENERAL_CAP_TIM_OCxPolarityConfigCmd(GENERAL_CAP_TIM, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			TIM_ICUserValueStructure.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			TIM_ICUserValueStructure.Capture_FinishFlag = 1;		
		}

		TIM_ClearITPendingBit (GENERAL_CAP_TIM,GENERAL_CAP_TIM_IT_CCx);	    
	}		
}

__IO uint16_t IC2Value = 0;
__IO uint16_t IC1Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;

/*
 * ����ǵ�һ���������жϣ��������ᱻ��λ�����浽CCR1�Ĵ�����ֵ��0��CCR2�Ĵ�����ֵҲ��0
 * �޷�����Ƶ�ʺ�ռ�ձȡ����ڶ��������ص�����ʱ��CCR1��CCR2���񵽵Ĳ�����Ч��ֵ������
 * CCR1��Ӧ�������ڣ�CCR2��Ӧ����ռ�ձȡ�
 */
void ADVANCE_CAP_TIM_IRQHandler(void)
{
  /* ����жϱ�־λ */
  TIM_ClearITPendingBit(ADVANCE_CAP_TIM, TIM_IT_CC1);

  /* ��ȡ���벶��ֵ */
  IC1Value = TIM_GetCapture1(ADVANCE_CAP_TIM);
  IC2Value = TIM_GetCapture2(ADVANCE_CAP_TIM);
	
  // ע�⣺����Ĵ���CCR1��CCR2��ֵ�ڼ���ռ�ձȺ�Ƶ�ʵ�ʱ������1
	if (IC1Value != 0)
  {
    /* ռ�ձȼ��� */
    DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

    /* Ƶ�ʼ��� */
    Frequency = (72000000/(ADVANCE_CAP_TIM_PSC+1))/(float)(IC1Value+1);
		printf("ռ�ձȣ�%0.2f%%   Ƶ�ʣ�%0.2fHz\n",DutyCycle,Frequency);
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
}


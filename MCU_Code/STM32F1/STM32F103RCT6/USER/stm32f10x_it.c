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
	//用到中断必须要有中断函数
	//TimingDelay_Decrement();	
}


void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT);//喂狗
	
	WWDG_ClearFlag();
	LED_State_Config(LED3, TOGGLE);
}


//KEY中断服务函数
char fpath[100]; //保存当前扫描路径

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
@brief	串口1接收中断服务函数
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

//ADCx中断服务函数
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
	// 当要被捕获的信号的周期大于定时器的最长定时时，定时器就会溢出，产生更新中断
	// 这个时候我们需要把这个最长的定时周期加到捕获信号的时间里面去
	if ( TIM_GetITStatus ( GENERAL_CAP_TIM, TIM_IT_Update) != RESET )               
	{	
		TIM_ICUserValueStructure.Capture_Period ++;		
		TIM_ClearITPendingBit ( GENERAL_CAP_TIM, TIM_FLAG_Update ); 		
	}

	// 上升沿捕获中断
	if ( TIM_GetITStatus (GENERAL_CAP_TIM, GENERAL_CAP_TIM_IT_CCx ) != RESET)
	{
		// 第一次捕获
		if ( TIM_ICUserValueStructure.Capture_StartFlag == 0 )
		{
			// 计数器清0
			TIM_SetCounter ( GENERAL_CAP_TIM, 0 );
			// 自动重装载寄存器更新标志清0
			TIM_ICUserValueStructure.Capture_Period = 0;
      // 存捕获比较寄存器的值的变量的值清0			
			TIM_ICUserValueStructure.Capture_CcrValue = 0;

			// 当第一次捕获到上升沿之后，就把捕获边沿配置为下降沿
			GENERAL_CAP_TIM_OCxPolarityConfigCmd(GENERAL_CAP_TIM, TIM_ICPolarity_Falling);
      // 开始捕获标准置1			
			TIM_ICUserValueStructure.Capture_StartFlag = 1;			
		}
		// 下降沿捕获中断
		else // 第二次捕获
		{
			// 获取捕获比较寄存器的值，这个值就是捕获到的高电平的时间的值
			TIM_ICUserValueStructure.Capture_CcrValue = 
			GENERAL_CAP_TIM_GetCapturexCmd(GENERAL_CAP_TIM);

			// 当第二次捕获到下降沿之后，就把捕获边沿配置为上升沿，好开启新的一轮捕获
			GENERAL_CAP_TIM_OCxPolarityConfigCmd(GENERAL_CAP_TIM, TIM_ICPolarity_Rising);
      // 开始捕获标志清0		
			TIM_ICUserValueStructure.Capture_StartFlag = 0;
      // 捕获完成标志置1			
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
 * 如果是第一个上升沿中断，计数器会被复位，锁存到CCR1寄存器的值是0，CCR2寄存器的值也是0
 * 无法计算频率和占空比。当第二次上升沿到来的时候，CCR1和CCR2捕获到的才是有效的值。其中
 * CCR1对应的是周期，CCR2对应的是占空比。
 */
void ADVANCE_CAP_TIM_IRQHandler(void)
{
  /* 清除中断标志位 */
  TIM_ClearITPendingBit(ADVANCE_CAP_TIM, TIM_IT_CC1);

  /* 获取输入捕获值 */
  IC1Value = TIM_GetCapture1(ADVANCE_CAP_TIM);
  IC2Value = TIM_GetCapture2(ADVANCE_CAP_TIM);
	
  // 注意：捕获寄存器CCR1和CCR2的值在计算占空比和频率的时候必须加1
	if (IC1Value != 0)
  {
    /* 占空比计算 */
    DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

    /* 频率计算 */
    Frequency = (72000000/(ADVANCE_CAP_TIM_PSC+1))/(float)(IC1Value+1);
		printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
}


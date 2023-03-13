/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "gd32e10x.h"
#include "MechanicalKey.h"
#include "FreeRTOS.h"
#include "task.h"
#include "myQueue.h"
//--------------------- variable declare ----------------------
static unsigned extiKeyWakeUp = 0;							// ���ڱ�־�������ѱ�־


/*******************************************************************************
Function: 
Description: 
*******************************************************************************/
void isr_exti_key(void)
{
	extiKeyWakeUp = 1;
}

/*******************************************************************************
Function: 
Description: 
*******************************************************************************/
int is_ExitKey_Waupup_sys(void)
{
	if(1==extiKeyWakeUp)	return(0);
	else					return(-1);
}

/*******************************************************************************
Function: 
Description: 
*******************************************************************************/
void cleanExitKey_Waupup_Flag(void)
{
	extiKeyWakeUp = 0;
}


/*****************************************************************************************
    \brief      PA1��Ϊ�������룬ʹ���ⲿ�ж�
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void wakeup_key_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA); 
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, GPIO_PIN_1);

	// ���ð������ⲿ�жϹ���
	/* enable the key EXTI clock */
	rcu_periph_clock_enable(RCU_AF);
	/* enable and set key EXTI interrupt to the lowest priority */
	/* connect key EXTI line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_1);
	/* configure key EXTI line */
	exti_init(EXTI_1, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(EXTI_1);

	nvic_irq_enable(EXTI1_IRQn, 3U, 0U);
}


/*******************************************************************************
Function: 
Description: 
*******************************************************************************/
void MechanicalKey_init(void)
{
	KEY_ENTER_ININT();
	KEY_FUN_ININT();
	
	wakeup_key_init();					// ʹ���ⲿ�жϣ����ڻ��ѵ�Ƭ��
}


/*******************************************************************************
Function: 
Description: 
*******************************************************************************/
unsigned int get_key_Mechanical(void)
{
	if(!READ_KEY_ENTER())				return(0x01);
	else if(READ_KEY_FUNCTION())		return(0x02);
	else								return(0xffff);
	
}



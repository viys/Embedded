#include "led.h"
#include "key.h"
#include "systick.h"

/**
@brief	LED��ʼ��
@note		LED1 -- PC4 /LED2 -- PC5 /LED3 -- PC6 /LED4 -- PC7
@param 	led:1 /2 /3 /4
@retval None
*/
void LED_Init(u8 led)
{
	if(5 == led)
	{
		for(int i = 1;i <5;i++)
			LED_Init(i);
		return ;
	}

	RCC->AHB1ENR |= 1 << 2;									//��ʱ��

	GPIOC->MODER &= ~(3 << (2 * (led + 3)));
	GPIOC->MODER |= 1 << (2 * (led + 3));		//����Ϊ���ģʽ
	
	GPIOC->OTYPER &= ~(1 << (led + 3));			//�������
	
	GPIOC->OSPEEDR &= ~(3 << (2 * (led + 3)));
	GPIOC->OSPEEDR |= 2 << (2 * (led + 3));	//50MHZ
	
	GPIOC->PUPDR &= ~(3 << (2 * (led + 3)));//��������
		
	GPIOC->ODR |= 1 << (led + 3);//����

}

/**
@brief	����LED״̬
@note		LED_OFF --0 /LED_ON -- 1 /LED_TOGGLE -- 2
@param	led:1 /2 /3 /4	| state:LED_OFF /LED_ON /LED_TOGGLE
@retval	None
*/
void LED_State(u8 led, u8 state)
{
	switch (state)
  {
  	case LED_OFF:
			GPIOC->ODR |= 1 << (led + 3);
  		break;
		
  	case LED_ON :
			GPIOC->ODR &= ~(1 << (led + 3));
  		break;
		
	case LED_TOGGLE :
			GPIOC->ODR ^= 1 << (led + 3);
  		break;
		
	case LED_ALL_OFF :
			for(int i = 1;i < 5;i++)
				GPIOC->ODR |= 1 << (i + 3);
  		break;
	
	case LED_ALL_ON :
			for(int i = 1;i < 5;i++)
				GPIOC->ODR &= ~(1 << (i + 3));
  		break;
	case LED_ALL_TOGGLE :
			for(int i = 1;i < 5;i++)
				GPIOC->ODR ^= 1 << (i + 3);
  		break;
	
  	default:		
  		break;
  }
}

/**
@brief	LED��ˮ��
@note   �Ӷ�Ӧ������ʼ��������ѭ����times��Ӧѭ������
@param	keystate:1 /2 /3 /4 | times: any
@retval	None
*/
void LED_Running(u8 keystate,u8 times)
{
	while(times)
	{
		for(int i = keystate ;i < 5;i++)
		{
			LED_State (i ,LED_ON );
			SysTick_ms (100);
			LED_State (i ,LED_OFF );
		}
		for(int i = 1 ;i < keystate;i++)
		{
			LED_State (i ,LED_ON );
			SysTick_ms (100);
			LED_State (i ,LED_OFF );
		}
		times--;
	}
}

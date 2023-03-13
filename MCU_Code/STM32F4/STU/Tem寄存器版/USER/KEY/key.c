#include "key.h"
#include "systick.h"
#include "nvic.h"

//extern u8 keystate;//����״̬��
u8 KEYARR[4] = {0,2,3,4};


/*
@brief	key��ʼ��
@note	GPIOC:key1 -- arr[0]/GPIOE:key2 -- arr[1] /key1 -- arr[2] /key1 -- arr[3]
@param	key:1 /2 /3 /4
@retval	None
*/
void KEY_Init(u8 key)
{
	if( KEY_ALL == key)
	{
		for(int i = 1;i < 5;i++)
		KEY_Init (i);
		return ;
	}
	if(1 == key)
	{
		RCC->AHB1ENR |= 1 << 0;  //ʹ��GPIOAʱ��	
		/* ����GPIOxģʽ */
		GPIOA->MODER &= ~(3 << (2 * KEYARR [key - 1])); //����ģʽ
		/* ���������� */
		GPIOA->PUPDR &= ~(3 << (2 * KEYARR [key - 1])); //��������
	}
	else if(2 == key || 3 == key || 4 == key)
	{
		RCC->AHB1ENR |= 1 << 4;
		
		GPIOE->MODER &= ~(3 << (2 * KEYARR [key - 1])); 
		GPIOE->PUPDR &= ~(3 << (2 * KEYARR [key - 1])); 
	}

}	


/*
@brief	keyɨ��
@note   key1 -- PA0   û�а����ǵ͵�ƽ  ����Ϊ�ߵ�ƽ
				key2 -- PE2   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
				key3 -- PE3   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
				key4 -- PE4   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
@param	None
@retval	None
*/

u8 key_Scan(void)
{
	  static u8 flag = 1;
	  u8 key = 0;
	
	  if((KEY1 || KEY2 || KEY3 || KEY4) && (flag == 1))
		{ 
			  SysTick_ms(10); //10ms-20ms
			  if(KEY1)
				{
					  flag = 0;  //��־λ����
					  key = 1;
				}
				else if(KEY2)
				{
					 flag = 0;
					 key = 2;
				}
				else if(KEY3)
				{
					 flag = 0;
					 key = 3;
				}
				else if(KEY4)
				{
					 flag = 0;
					 key = 4;
				}
		}
		
		if(!KEY1 && !KEY2 && !KEY3 && !KEY4 && (flag == 0)) //û�а�������
		{
			 flag = 1;
		}
		return key;
}





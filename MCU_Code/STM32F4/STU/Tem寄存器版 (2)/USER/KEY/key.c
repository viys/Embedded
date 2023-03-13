#include "key.h"
#include "systick.h"

/*
�������ܣ�key��ʼ��
����ֵ����
�βΣ���
��ע��key1 -- PA0  û�а����ǵ͵�ƽ  ����Ϊ�ߵ�ƽ
*/
void KEY_Init(void)
{
	/* ʱ��ʹ�� */
	RCC->AHB1ENR |= 1 << 0;  //ʹ��GPIOAʱ��
	RCC->AHB1ENR |= 1 << 4;  //ʹ��GPIOEʱ��
	
	/* ����GPIOģʽ */
	GPIOA->MODER &= ~(3 << (2 * 0)); //����ģʽ
	/* ���������� */
	GPIOA->PUPDR &= ~(3 << (2 * 0));  //��������
	
		/* ����GPIOE.2ģʽ */
	GPIOE->MODER &= ~(3 << (2 * 2)); //����ģʽ
	/* ���������� */
	GPIOE->PUPDR &= ~(3 << (2 * 2));
	
	
		/* ����GPIOE.3ģʽ */
	GPIOE->MODER &= ~(3 << (2 * 3)); //����ģʽ
	/* ���������� */
	GPIOE->PUPDR &= ~(3 << (2 * 3));
	
	
		/* ����GPIOE.4ģʽ */
	GPIOE->MODER &= ~(3 << (2 * 4)); //����ģʽ
	/* ���������� */
	GPIOE->PUPDR &= ~(3 << (2 * 4));
	
}


/*
�������ܣ�keyɨ��
����ֵ��1,2,3,4
�βΣ���
��ע��key1 -- PA0   û�а����ǵ͵�ƽ  ����Ϊ�ߵ�ƽ
      key2 -- PE2   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
      key3 -- PE3   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
			key4 -- PE4   û�а����Ǹߵ�ƽ  ����Ϊ�͵�ƽ
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





/*****************************************************************************
  �ļ�: main.c
  ����: Zhengyu https://gzwelink.taobao.com
  �汾: V1.0.0
  ʱ��: 20210401
	ƽ̨:MINI-GD32F103RCT6������
	΢�ź�:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "NRF24L01.h"
#include "Oled.h"


uint8_t tmp_buf[5];//������������
int main(void)
{
	uint8_t i;
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
	systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ�� 
	delay_1ms(1000);//�ȴ�1��
	OLED_Init();		//OLED��ʼ�� 
	OLED_Clear();   //OLED���� 
	Init_NRF24L01();//NRF24L01��ʼ��
	while(NRF24L01_Check())//24L01��ѯ
	{

	}
	
	TX_Mode();//����ģʽ
  while (1)
  {
    OLED_Clear();
    OLED_ShowCHinese(18,0,0);//��
		OLED_ShowCHinese(36,0,1);//��
		OLED_ShowCHinese(54,0,2);//��
		OLED_ShowCHinese(72,0,3);//��
		OLED_ShowCHinese(90,0,4);//��
	
		OLED_ShowString(6,3,"NRF2401 TEST");
		OLED_ShowString(0,6,"SEND:");  
 
    for( i=0;i<5;i++)
		{
			tmp_buf[i]=i;
			
			OLED_ShowNum(40+i*8,6,tmp_buf[i],1,16);//��ʾ��������
		}

	
		if(NRF24L01_TxPacket(tmp_buf)==TX_OK)		//NRF2401����
		{
			
		   OLED_ShowString(96,6,"OK"); 
			 HAL_Delay(3000);					
		}			
		else						   			
		{		
		  OLED_ShowString(96,6,"ERR"); 			
			 HAL_Delay(3000);	    
		}
    /* USER CODE BEGIN 3 */
  }
}

/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "NRF24L01.h"
#include "Oled.h"


uint8_t tmp_buf[5];//发送内容数组
int main(void)
{
	uint8_t i;
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能 
	delay_1ms(1000);//等待1秒
	OLED_Init();		//OLED初始化 
	OLED_Clear();   //OLED清屏 
	Init_NRF24L01();//NRF24L01初始化
	while(NRF24L01_Check())//24L01查询
	{

	}
	
	TX_Mode();//发送模式
  while (1)
  {
    OLED_Clear();
    OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
	
		OLED_ShowString(6,3,"NRF2401 TEST");
		OLED_ShowString(0,6,"SEND:");  
 
    for( i=0;i<5;i++)
		{
			tmp_buf[i]=i;
			
			OLED_ShowNum(40+i*8,6,tmp_buf[i],1,16);//显示发送内容
		}

	
		if(NRF24L01_TxPacket(tmp_buf)==TX_OK)		//NRF2401发送
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

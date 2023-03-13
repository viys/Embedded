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
#include "rtc.h"

__IO uint32_t timedisplay;//ʱ����ʾ��־λ��RTC_IRQHandler��������λ
int main(void)
{
		rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB��Ƶ��1��Ƶ
		systick_config();//ϵͳ��Ƶ108MHZ,�����ⲿ����,�����������(__SYSTEM_CLOCK_108M_PLL_HXTAL��HXTAL_VALUE)
		rcu_periph_clock_enable(RCU_AF);//�ܽŸ���ʱ��alternate function clockʹ��
		delay_1ms(1000);//�ȴ�1��
	
		nvic_configuration();//�ж�����
		rtc_configuration();//RTC����
		rcu_periph_clock_enable(RCU_PMU);//��Դ����ʹ��
		pmu_backup_write_enable();//backupдʹ��
        
		 /* wait for RTC registers synchronization */
		rtc_register_sync_wait();
		/* enable the RTC second */
		rtc_interrupt_enable(RTC_INT_SECOND);
		/* wait until last write operation on RTC registers has finished */
		rtc_lwoff_wait();
    while(1)
    {
      delay_1ms(1000);//�ȴ�1��
      time_show();//ʱ���ȡ
    }
}

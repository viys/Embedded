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
#include "rtc.h"

__IO uint32_t timedisplay;//时间显示标志位，RTC_IRQHandler函数中置位
int main(void)
{
		rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
		systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
		rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能
		delay_1ms(1000);//等待1秒
	
		nvic_configuration();//中断配置
		rtc_configuration();//RTC配置
		rcu_periph_clock_enable(RCU_PMU);//电源管理使能
		pmu_backup_write_enable();//backup写使能
        
		 /* wait for RTC registers synchronization */
		rtc_register_sync_wait();
		/* enable the RTC second */
		rtc_interrupt_enable(RTC_INT_SECOND);
		/* wait until last write operation on RTC registers has finished */
		rtc_lwoff_wait();
    while(1)
    {
      delay_1ms(1000);//等待1秒
      time_show();//时间读取
    }
}

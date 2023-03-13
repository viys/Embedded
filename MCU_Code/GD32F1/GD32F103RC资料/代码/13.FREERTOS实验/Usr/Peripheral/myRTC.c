/********************************************************************************************
file name:
Description:
********************************************************************************************/
//----------------------------- include files -----------------------------
#include "myRTC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "myQueue.h"
//---------------------------- varialbe define -----------------------------
unsigned int YearOfDate = DEFAULT_YEAR;										// 用于存储日期的年份，同时还要保存到FLASH中。
myAlarmData_t myAlarmTimer1 = {0};											// 闹铃1时间
myAlarmData_t myAlarmTimer2 = {0};											// 闹铃2时间
myAlarmData_t myAlarmPowSave = {0};											// 低功耗闹铃
myAlarmData_t myAlarmWholeYear = {0};											// RTC计满1整年中断
myAlarm_t myAlarm1 = {0};													// 闹铃1日期
myAlarm_t myAlarm2 = {0};													// 闹铃2日期


/*******************************************************************************
Function:
Descripton:
	将日期更新到RTC计数器
*******************************************************************************/
void date_load_to_rtc(unsigned char alarmNum, unsigned short int year, unsigned short int month, unsigned short int day, unsigned short int hour, unsigned short int minute)
{
	unsigned int cnt_temp = time_regulate(year, month, day, hour, minute, 0);;
	
	rtc_lwoff_wait();
	rtc_counter_set(cnt_temp);
	rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
	闹铃开关控制,在闹铃开关控制界面设置
	alarmNum: 闹铃编号，1号和2号，只支持两个闹铃
	on_off: 闹铃闹铃开关控制，1=开，0=关
*******************************************************************************/
void myAlarmTimer_switch(unsigned char alarmNum, unsigned char on_off)
{
	switch(alarmNum)
	{
		case 1:											// 1号闹铃设置
		{
			myAlarmTimer1.en = on_off;
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 1号闹铃设置
		{
			myAlarmTimer2.en = on_off;
			myAlarmTimer2.timeout = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

/*******************************************************************************
Function:
Descripton:
	设置闹铃时间，在闹铃设置界面中调用
	如果当前系统时间还未达到闹铃时间，则写入闹铃时间对应的计数值，
	如果当前系统时间已经超过闹铃时间，则写入下一天闹铃时间对应的数值。
*******************************************************************************/
void myAlarmTimer_time_set(unsigned char alarmNum, unsigned short int hour, unsigned short int minute)
{
	unsigned int sysYear,sysMonth,sysDay,sysHour,sysMinute,sysSecond;
	unsigned int cur_rtc_cnt,cnt_temp;
	unsigned int max_counter;

	get_sys_date_from_rtc(&sysYear,&sysMonth,&sysDay,&sysHour,&sysMinute,&sysSecond);
	cur_rtc_cnt = grab_rtc_counter();	// 当前计数值
	cnt_temp = time_regulate(sysYear, sysMonth, sysDay, hour, minute, 0);					// 得到闹铃时间对应的计数值
	max_counter = IS_LEAP_YEAR(sysYear) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

	// 判断闹铃时间是否已过?如果已过,则写入下一天的闹铃时间
	if(cnt_temp<=cur_rtc_cnt)													// 今天的时间已经晚于闹铃时间,闹铃要设置为明天的时间
	{
		cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
		cnt_temp += 24*3600*SECOND_CNT_RTC;									// 加一天
		cnt_temp += (hour*3600+minute*60)*SECOND_CNT_RTC;						// 小时和分钟(第二天的闹铃时间)
		if(cnt_temp>max_counter)												// 如加一天的闹铃时间超过了一年计数值
		{
			cnt_temp = (hour*3600+minute*60)*SECOND_CNT_RTC;					// 小时和分钟
		}
	}
	
	switch(alarmNum)
	{
		case 1:											// 1号闹铃设置
		{
			myAlarmTimer1.alarm_cnt = cnt_temp;
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 2号闹铃设置
		{
			myAlarmTimer2.alarm_cnt = cnt_temp;
			myAlarmTimer2.timeout = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
int is_myAlarmTimer_timeout(unsigned alarmNum)
{
	int ret = -1;
	
	switch(alarmNum)
	{
		case 1:											// 1号闹铃设置
		{
			if(myAlarmTimer1.timeout)	ret =  0;
			else						ret = -1;
			break;
		}
		case 2:											// 2号闹铃设置
		{
			if(myAlarmTimer2.timeout)	ret = 0;
			else						ret = -1;
			break;
		}
		default:
		{
			ret = -1;
			break;
		}
	}
	return(ret);
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
void clean_myAlarmTimer_timeout_flag(unsigned alarmNum)
{
	switch(alarmNum)
	{
		case 1:											// 1号闹铃设置
		{
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 2号闹铃设置
		{
			myAlarmTimer2.timeout = 0;
			break;
		}
		default:
		{
			break;
		}
	}
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
void diasble_myAlarmTimer(unsigned alarmNum)
{
	switch(alarmNum)
	{
		case 1:											// 1号闹铃设置
		{
			myAlarmTimer1.en= 0;
			break;
		}
		case 2:											// 2号闹铃设置
		{
			myAlarmTimer2.en= 0;
			break;
		}
		default:
		{
			break;
		}
	}
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myPowerSaveTimer_time_set(unsigned int time_ms)
{
	unsigned int temp = (time_ms*1000/7812);
	unsigned int cnt_temp;
		
	myRTC_APB_sync();											// RTC与APB同步
	//myRTC_enter_config_mod();
	taskENTER_CRITICAL();
	temp += grab_rtc_counter();
	taskEXIT_CRITICAL();
	myAlarmPowSave.en = 1;
	myAlarmPowSave.alarm_cnt = temp;
	myAlarmPowSave.timeout = 0;
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
int is_powerSave_alarm_timeout(void)
{
	if(myAlarmPowSave.timeout)	return 0;
	else						return -1;
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
void clean_powerSave_alarm_timeout_flag(void)
{
	myAlarmPowSave.timeout = 0;
}

/*******************************************************************************
Function:
Descripton:
	判断低功耗定时中断是否超时
*******************************************************************************/
void diasble_powerSave_alarm(void)
{
	myAlarmPowSave.en= 0;
}


/*******************************************************************************
Function:
Descripton:
	根据闹铃编号查询闹铃使能状态。
*******************************************************************************/
int check_alarm_status(int alarm_num)
{
	int ret = -1;
	
	switch(alarm_num)
	{
		case 1:
		{
			if(myAlarmTimer1.en)	ret = 0;
			break;
		}
		case 2:
		{
			if(myAlarmTimer2.en)	ret = 0;
			break;
		}
		default:
			break;
	}
	return(ret);
}

/*******************************************************************************
Function:
Descripton:
	通过闹铃计数值得到闹铃日期
*******************************************************************************/
void myAlarm_date_get(unsigned char alarm_num, unsigned int* year,unsigned int* month,unsigned int* day,unsigned int* hour,unsigned int* minute,unsigned int* second)
{
	unsigned int rtc_cnt;
	unsigned int week;
	
	switch(alarm_num)
	{
		case 1:
		{
			rtc_cnt = myAlarmTimer1.alarm_cnt;
			break;
		}
		case 2:
		{
			rtc_cnt = myAlarmTimer2.alarm_cnt;
			break;
		}
		default:
			break;
	}
	
	get_calender_from_rtcCount(rtc_cnt/SECOND_CNT_RTC,year,month,day,hour,minute,second,&week);
}

/*******************************************************************************
Function:
Descripton:
	计算手环闹铃的alarm计数值
	先判断闹铃时间是否能在当天触发，如果当天不能触发的话，要将计数值改为下一天的相同时刻，
	如果超过一年的最大时间，则改为后一年相同时刻。
	注意:此函数只能在修改手环闹铃时间时调用一次
*******************************************************************************/
void get_valid_rtc_count_of_alarmTimer(unsigned char alarmNum)
{
	unsigned int sysYear,sysMonth,sysDay,sysHour,sysMinute,sysSecond;
	unsigned int cur_rtc_cnt,cnt_temp;
	unsigned int max_counter;

	get_sys_date_from_rtc(&sysYear,&sysMonth,&sysDay,&sysHour,&sysMinute,&sysSecond);
	cur_rtc_cnt = grab_rtc_counter();	// 当前计数值
	max_counter = IS_LEAP_YEAR(sysYear) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

	switch(alarmNum)
	{
		case 1:
		{
			// calculate the rtc counter of alarm1
			if(myAlarm1.en) // 闹铃1开启,计算闹铃1下次中断的时间
			{
				cnt_temp = time_regulate(sysYear, sysMonth, sysDay, myAlarm1.time.hour, myAlarm1.time.minute, 0); // 得到闹铃时间对应的计数值
				// 判断闹铃时间是否已过?如果已过,则写入下一天的闹铃时间
				if(cnt_temp<=cur_rtc_cnt)													// 今天的时间已经晚于闹铃时间,闹铃要设置为明天的时间
				{
					cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
					cnt_temp += 24*3600*SECOND_CNT_RTC; 								// 加一天
					cnt_temp += (myAlarm1.time.hour*3600+myAlarm1.time.minute*60)*SECOND_CNT_RTC;						// 小时和分钟(第二天的闹铃时间)
					if(cnt_temp>max_counter)												// 如加一天的闹铃时间超过了一年计数值
					{
						cnt_temp = (myAlarm1.time.hour*3600+myAlarm1.time.minute*60)*SECOND_CNT_RTC;					// 下一年的时间(小时和分钟)
					}
				}
				myAlarmTimer1.alarm_cnt = cnt_temp; // 保存闹铃下次中断时间
				myAlarmTimer1.en = 1;
			}
			break;
		}
		case 2:
		{
			// calculate the rtc counter of alarm1
			if(myAlarm2.en) // 闹铃1开启,计算闹铃2下次中断的时间
			{
				cnt_temp = time_regulate(sysYear, sysMonth, sysDay, myAlarm2.time.hour, myAlarm2.time.minute, 0); // 得到闹铃时间对应的计数值
				// 判断闹铃时间是否已过?如果已过,则写入下一天的闹铃时间
				if(cnt_temp<=cur_rtc_cnt)													// 今天的时间已经晚于闹铃时间,闹铃要设置为明天的时间
				{
					cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
					cnt_temp += 24*3600*SECOND_CNT_RTC; 								// 加一天
					cnt_temp += (myAlarm2.time.hour*3600+myAlarm2.time.minute*60)*SECOND_CNT_RTC;						// 小时和分钟(第二天的闹铃时间)
					if(cnt_temp>max_counter)												// 如加一天的闹铃时间超过了一年计数值
					{
						cnt_temp = (myAlarm2.time.hour*3600+myAlarm2.time.minute*60)*SECOND_CNT_RTC;					// 下一年的时间(小时和分钟)
					}
				}
				myAlarmTimer2.alarm_cnt = cnt_temp; // 保存闹铃下次中断时间
				myAlarmTimer2.en = 1;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}


/*******************************************************************************
Function:
Descripton:
	计算低功耗闹铃的alarm值。
	当设定的计数大于一年的最大计数值时，将计数值减去整年计数值
	注意:此函数只能在修改低功耗计数值时调用一次
*******************************************************************************/
void get_valid_rtc_count_of_powerSaveAlarm(void)
{
	unsigned int max_counter;

	// calculate the rtc counter of powersave
	if(myAlarmPowSave.en)
	{
		max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		if(myAlarmPowSave.alarm_cnt>max_counter)	// 计数值大于整年
		{
			myAlarmPowSave.alarm_cnt = max_counter-myAlarmPowSave.alarm_cnt;
		}
	}
}

/*******************************************************************************
Function:
Descripton:
	更新整年闹铃计数器
*******************************************************************************/
void get_valid_rtc_count_of_WholeYearAlarm(unsigned int val)
{
	myAlarmWholeYear.alarm_cnt = val;
}

/*******************************************************************************
Function:
Descripton:
	将所有闹铃:手环闹铃计数值，低功耗闹铃计数值,整年计数值放到数组中排序
	注意:只有大于当前RTC计数值的数据参于排序。
*******************************************************************************/
void alarm_timeout_handle_in_isr(unsigned int cur_rtc_cnt)
{
	if( (myAlarmTimer2.en) )
	{
		if( (myAlarmTimer2.alarm_cnt<cur_rtc_cnt+50)&&(myAlarmTimer2.alarm_cnt+50>cur_rtc_cnt) )
		{
			myAlarmTimer2.en = 0;
			myAlarmTimer2.timeout = 1;
		}
	}
	
	if( (myAlarmTimer1.en) )
	{
		if( (myAlarmTimer1.alarm_cnt<cur_rtc_cnt+50)&&(myAlarmTimer1.alarm_cnt+50>cur_rtc_cnt) )
		{
			myAlarmTimer1.en = 0;
			myAlarmTimer1.timeout = 1;
		}
	}

	if( (myAlarmPowSave.en))
	{

		if( (myAlarmPowSave.alarm_cnt<cur_rtc_cnt+50)&&(myAlarmPowSave.alarm_cnt+50>cur_rtc_cnt) )
		{
			myAlarmPowSave.en = 0;
			myAlarmPowSave.timeout = 1;
		}
	}

}

/*******************************************************************************
Function:
Descripton:
	将所有闹铃:手环闹铃计数值，低功耗闹铃计数值,整年计数值放到数组中排序
	注意:只有大于当前RTC计数值的数据参于排序。
*******************************************************************************/
void load_new_alarm_cnt_to_rtc(void)
{
	unsigned int cnt_array[5];
	unsigned char i,j,k;
	unsigned int cur_rtc_cnt;
	unsigned int temp;

	i = 0;
	cur_rtc_cnt = grab_rtc_counter();	// 当前计数值
	if(myAlarmTimer2.en)
	{
		if(myAlarmTimer2.alarm_cnt>cur_rtc_cnt)
		{
			*(cnt_array+i) = myAlarmTimer2.alarm_cnt;
			i++;
		}
	}

	if(myAlarmTimer1.en)
	{
		if(myAlarmTimer1.alarm_cnt>cur_rtc_cnt)
		{
			*(cnt_array+i) = myAlarmTimer1.alarm_cnt;
			i++;
		}
	}

	if(myAlarmPowSave.en)
	{
		if(myAlarmPowSave.alarm_cnt>cur_rtc_cnt)
		{
			*(cnt_array+i) = myAlarmPowSave.alarm_cnt;
			i++;
		}
	}

	*(cnt_array+i) = myAlarmWholeYear.alarm_cnt;
	i++;

	// 冒泡排序，选出最小值
	for(j=0;j<i;j++)
	{
		for(k=j+1;k<i;k++)
		{
			if( cnt_array[j]>cnt_array[k] )
			{
				temp = cnt_array[j];
				cnt_array[j] = cnt_array[k];
				cnt_array[k] = temp;
			}
		}
	}
	
	reload_rtc_alarm(cnt_array[0]);
}


/*******************************************************************************
Function:
Descripton:
	设置低功耗闹铃定时，用于待机时进入低功耗接收模式定时。
*******************************************************************************/
void powSaveAlarm_set(unsigned int time_ms)
{
	
}

/*******************************************************************************
Function:
Descripton:
	用于闹铃中断中，切换闹铃计数器的值。
	当闹铃中断后，如果还有更多闹铃任务时，要将其它闹铃时间重新写入RTC闹铃计数器
*******************************************************************************/
void rtc_alarm_cnt_update(void)
{
	
}


/*******************************************************************************
Function:
Descripton:
	rtc秒中断服务程序
*******************************************************************************/
void isr_rtc_second(void)
{
	Msg_RTC_t	MsgRTC;
    uint32_t temp = 0, max_counter = 0, reset_value = 0, year = 0;
    
    if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_SECOND))
	{
        /* clear the RTC second interrupt flag*/
        rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);    

        temp = rtc_counter_get();
		if( 0==temp%SECOND_CNT_RTC )												// 计满一秒
		{
			MsgRTC.type = MSG_RTC_SECOND_INT;
			MsgRTC.rtcCount = temp/SECOND_CNT_RTC;									// 除以秒计数
			xQueueSendFromISR(myRtcQueue,&MsgRTC,0);
		}

		// 判断RTC计数器是否计满1年?
        year = YearOfDate;
        max_counter = IS_LEAP_YEAR(year) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		// 当RTC计满1年时,重置RTC计数器
        /* reset RTC counter when time is 31st Dec. 23:59:59 */
        if( temp >= (max_counter - 1) )												// 计满1年
		{
            YearOfDate++;
            reset_value = 0;

            /* set new year's value to RTC counter register */
            rtc_lwoff_wait();
            rtc_counter_set(reset_value);
            rtc_lwoff_wait();
        }
    }
}


/*****************************************************************************************
    \brief      RTC闹铃中断服务程序
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void isr_rtc_alarm(void)
{
    uint32_t temp = 0, max_counter = 0, reset_value = 0, year = 0;

	if(RESET != rtc_interrupt_flag_get(RTC_INT_FLAG_ALARM))
	{
        temp = rtc_counter_get();

        /* clear the RTC alarm interrupt flag*/
        rtc_interrupt_flag_clear(RTC_INT_FLAG_ALARM);
		
		// 先置位超时标志
		alarm_timeout_handle_in_isr(temp);											// 超时处理

		// 判断RTC计数器是否计满1年?
        year = YearOfDate;
        max_counter = IS_LEAP_YEAR(year) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		// 当RTC计满1年时,重置RTC计数器
        /* reset RTC counter when time is 31st Dec. 23:59:59 */
        if( temp >= (max_counter - 1) )												// 计满1年
		{
            YearOfDate++;
            reset_value = 0;

            /* set new year's value to RTC counter register */
            rtc_lwoff_wait();
            rtc_counter_set(reset_value);
            rtc_lwoff_wait();

			// 下一年的整年计数值
			max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);
			get_valid_rtc_count_of_WholeYearAlarm(max_counter);
        }		

		load_new_alarm_cnt_to_rtc();											// 中断时更新alarm
    }
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void rtc_port_init(void)
{
	IO_OSC32IN_INIT();
	IO_OSC32OUT_INIT();
}


/*******************************************************************************
Function:
Descripton:
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*******************************************************************************/
void nvic_rtc_config(void)
{
	// 配置RTC外部中断功能
	rtc_lwoff_wait();
	rtc_interrupt_enable(RTC_INT_SECOND);
	rtc_interrupt_enable(RTC_INTEN_ALRMIE);
	rtc_lwoff_wait();
	nvic_irq_enable(RTC_ALARM_IRQn,2,0);							// 开启闹铃外部中断
	nvic_irq_enable(RTC_IRQn,7,0);									// 开启RTC秒中断
}


/*******************************************************************************
Function:
Descripton:
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*******************************************************************************/
void rtc_alarm_switch(unsigned char flag)
{
	if(flag)
	{
		nvic_irq_enable(RTC_ALARM_IRQn,3,0);							// 使能RTC唤醒中断
		exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
		//rtc_interrupt_enable(RTC_INT_ALARM);							// rtc闹铃中断
		exti_interrupt_enable(EXTI_17);
		exti_interrupt_flag_clear(EXTI_17);
	}
	else
	{
		nvic_irq_disable(RTC_ALARM_IRQn);							// 使能RTC唤醒中断
		rtc_interrupt_disable(RTC_INT_ALARM);							// rtc闹铃中断
		exti_interrupt_flag_clear(EXTI_17);
	}
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);			// RCU_APB1EN - 开启BKP时钟
    rcu_periph_clock_enable(RCU_PMU);			// RCU_APB1EN - 开启PMU时钟
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable RTC Clock */
    rcu_periph_clock_disable(RCU_RTC);			// 开启RTC时钟
	__nop();__nop();__nop();__nop();

    /* enable LXTAL */
    rcu_osci_off(RCU_LXTAL);					// 打开低速时钟开关
	__nop();__nop();__nop();__nop();

    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);						// 打开低速时钟开关
	
    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);						// 打开低速时钟开关
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);				// 等待低速时钟稳定
    
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);		// 设置RTC时钟源

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);			// 开启RTC时钟

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();					// 等待RTC寄存器与ABP1时钟同步

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();							// 等待RTC寄存器操作完成

    /* enable the RTC second and alarm interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);
    //rtc_interrupt_enable(RTC_INT_ALARM);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    //rtc_prescaler_set(32767);
    rtc_prescaler_set(VAL_RTC_PRESCAL);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();							// 等待RTC寄存器操作完成
}


/*******************************************************************************
    \brief      return the time entered by user, using Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     current time of RTC counter value
*******************************************************************************/
uint32_t time_regulate(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
	uint32_t sum_days = 0;

	sum_days = calc_days_num_by_date(year, month, day);

	/* return the value which will be stored in RTC counter register */
	/* year information will be stored in the high 7 bits, total second information will be stored in the low 25 bits */
	return ( ( ((sum_days - 1) * 24 * 3600) + (hour * 3600 + minute * 60 + second) ) * SECOND_CNT_RTC );
}


/*******************************************************************************
Function:
Descripton:
    \brief      calculate how many days have passed in this year by date
    \param[in]  year, month, day : constitute the date
    \param[out] none
    \retval     the days number
*******************************************************************************/
uint32_t calc_days_num_by_date(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t i;
    uint32_t sum_days = 0;
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if((month < 1) || (month > 12)){
        return 0;
    }

    days_of_month[1] = IS_LEAP_YEAR(year) ? 29 : 28;

    for(i = 0; i < (month - 1); i++){
        sum_days += days_of_month[i];
    }

    sum_days += day;

    return sum_days;
}


/*******************************************************************************
Function:
Descripton:
    \brief      adjust time 
    \param[in]  none
    \param[out] none
    \retval     none
*******************************************************************************/
void time_adjust(void)
{
    uint32_t temp = 0;
	
    /* wait until last write operation on RTC registers has finished */
	// 根据默认日期得到RTC的计数初值
    temp = time_regulate(DEFAULT_YEAR, DEFAULT_MONTH, DEFAULT_DAY, DEFAULT_HOUR, DEFAULT_MINUTE, DEFAULT_SECOND);					
    /* change the current time */
    rtc_lwoff_wait();
    rtc_counter_set(temp);					// 重新写入RTC计数值
    rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
	初始化RTC计数器
*******************************************************************************/
void rtc_process(void)
{
	Msg_RTC_t	MsgRTC;
    uint32_t rtc_count = 0;
	unsigned int max_counter;

    if(bkp_data_read(BKP_DATA_0) != BACKUP_DATA)
	{
        /* RTC configuration */
        rtc_configuration();
        // 使用默认时间RTC计数值初始化
        time_adjust();
        bkp_data_write(BKP_DATA_0, BACKUP_DATA);
    }
	else
	{
        /* allow access to BKP domain */
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();
    
        /* wait for RTC registers synchronization */
        rtc_register_sync_wait();
        rtc_lwoff_wait();
        /* enable the RTC second and alarm interrupt*/
        rtc_interrupt_enable(RTC_INT_SECOND);
        //rtc_interrupt_enable(RTC_INT_ALARM);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
    }

	// 整年计数值
	max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);
	get_valid_rtc_count_of_WholeYearAlarm(max_counter);
	
	rtc_count = rtc_counter_get();												// 上电时先发送时钟到界面
	MsgRTC.type = MSG_RTC_SECOND_INT;
	MsgRTC.rtcCount = rtc_count/SECOND_CNT_RTC;
	xQueueSendFromISR(myRtcQueue,&MsgRTC,0);
}

/*******************************************************************************
Function:
Descripton:
    \brief      Enter a certain day a year, determine the day is this year 's first few days?
    \param[in]  year: 
    \param[in]  month: 
    \param[in]  day: 
    \retval     first few days	
*******************************************************************************/
unsigned int get_first_few_days(uint32_t year,uint32_t month,uint32_t day)
{
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	unsigned char i;
	unsigned int sum_day = 0;

    days_of_month[1] = IS_LEAP_YEAR(year) ? 29 : 28;
	
	for(i=0;i<(month-1);i++)
	{
		sum_day += days_of_month[i];
	}
	sum_day += day;
	return(sum_day);
}


/*******************************************************************************
Function:
Descripton:
    \brief      get week from date
    \param[in]  year: 
    \param[in]  month: 
    \param[in]  day: 
    \retval     week
	formula: [Y-1]	+	 [(Y-1)/4]	-	[(Y-1)/100]		+	[(Y-1)/400]	+	D
	
*******************************************************************************/
unsigned char get_week_from_date(uint32_t year,uint32_t month,uint32_t day)
{
	unsigned int sum = 0;
	
	sum += year-1;
	sum += (year-1)/4;
	sum -= (year-1)/100;
	sum += (year-1)/400;
	sum += get_first_few_days(year,month,day);
	return(sum%7);
}


/*******************************************************************************
Function:
Descripton:
    \brief      get calendar from rtc count
    \param[in]  rtc_counter: value read from RTC counter register
    \param[out] year
    \param[out] month
    \param[out] day
    \param[out] hour
    \param[out] minute
    \param[out] second
    \param[out] week
    \retval     none
*******************************************************************************/
void get_calender_from_rtcCount(uint32_t rtc_counter,uint32_t* year,uint32_t* month,uint32_t* day,uint32_t* hour,uint32_t* minute,uint32_t* second,uint32_t* week)
{
    uint32_t i = 0, day_num = 0, temp = 0;
    uint32_t days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* compute years */
    *year = YearOfDate;
    
    /* compute months */
    day_num  = rtc_counter / (3600 * 24);
    days_of_month[1] = IS_LEAP_YEAR(*year) ? 29 : 28;
    for(i = 0; i < 12; i++)
	{
        if(day_num >= days_of_month[i])
		{
            day_num -= days_of_month[i];
            continue;
        }
		else
		{
            break;
        }
    }
    *month = ((i >= 12) ? 12 : (i + 1));
    /* compute days */
    *day = day_num + 1;

    temp = rtc_counter % (3600 * 24);
    /* compute  hours */
    *hour = temp / 3600;
    /* compute minutes */
    *minute = (temp % 3600) / 60;
    /* compute seconds */
    *second = (temp % 3600) % 60;
	
	// get week
	*week = get_week_from_date(*year,*month,*day);
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
unsigned int grab_rtc_counter(void)
{
	unsigned int rtc_cnt;
	
	rtc_lwoff_wait();
	rtc_cnt = rtc_counter_get();
	rtc_lwoff_wait();
	return( rtc_cnt );						// 获取RTC计数器
}


/*******************************************************************************
Function:
Descripton:
	取出RTC计数值，然后转换为日期和时间
*******************************************************************************/
void get_sys_date_from_rtc(uint32_t* year,uint32_t* month,uint32_t* day,uint32_t* hour,uint32_t* minute,uint32_t* second)
{
	unsigned int rtc_cnt;
	unsigned int week;
	
	rtc_cnt = grab_rtc_counter();
	get_calender_from_rtcCount(rtc_cnt/SECOND_CNT_RTC,year,month,day,hour,minute,second,&week);
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void reload_rtc_counter(unsigned int val)
{
	rtc_lwoff_wait();
	rtc_counter_set(val);									// 设置RTC
	rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void reload_rtc_alarm(unsigned int val)
{
	rtc_lwoff_wait();
	rtc_alarm_config(val);									// 设置RTC
	rtc_lwoff_wait();
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void rtc_exti_init(void)
{
    // 配置RTC外部中断功能
    exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_17);
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myRTC_APB_sync(void)
{
	rtc_register_sync_wait();
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myRTC_enter_config_mod(void)
{
	rtc_lwoff_wait();
	rtc_configuration_mode_enter();
	rtc_lwoff_wait();
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myRTC_exit_config_mod(void)
{
	rtc_lwoff_wait();
	rtc_configuration_mode_exit();
	rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myRTC_init(void)
{
	rtc_port_init();							// IO口初始化，设置为OSC32口
    // RTC配置，分为首次上电和唤本模式
    rtc_process();
	
	/* clear reset flags */
	rcu_all_reset_flag_clear();

	rtc_exti_init();							// 使能RTC外部中断

	nvic_rtc_config();
}




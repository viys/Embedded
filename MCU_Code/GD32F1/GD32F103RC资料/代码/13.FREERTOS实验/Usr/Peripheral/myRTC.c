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
unsigned int YearOfDate = DEFAULT_YEAR;										// ���ڴ洢���ڵ���ݣ�ͬʱ��Ҫ���浽FLASH�С�
myAlarmData_t myAlarmTimer1 = {0};											// ����1ʱ��
myAlarmData_t myAlarmTimer2 = {0};											// ����2ʱ��
myAlarmData_t myAlarmPowSave = {0};											// �͹�������
myAlarmData_t myAlarmWholeYear = {0};											// RTC����1�����ж�
myAlarm_t myAlarm1 = {0};													// ����1����
myAlarm_t myAlarm2 = {0};													// ����2����


/*******************************************************************************
Function:
Descripton:
	�����ڸ��µ�RTC������
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
	���忪�ؿ���,�����忪�ؿ��ƽ�������
	alarmNum: �����ţ�1�ź�2�ţ�ֻ֧����������
	on_off: �������忪�ؿ��ƣ�1=����0=��
*******************************************************************************/
void myAlarmTimer_switch(unsigned char alarmNum, unsigned char on_off)
{
	switch(alarmNum)
	{
		case 1:											// 1����������
		{
			myAlarmTimer1.en = on_off;
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 1����������
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
	��������ʱ�䣬���������ý����е���
	�����ǰϵͳʱ�仹δ�ﵽ����ʱ�䣬��д������ʱ���Ӧ�ļ���ֵ��
	�����ǰϵͳʱ���Ѿ���������ʱ�䣬��д����һ������ʱ���Ӧ����ֵ��
*******************************************************************************/
void myAlarmTimer_time_set(unsigned char alarmNum, unsigned short int hour, unsigned short int minute)
{
	unsigned int sysYear,sysMonth,sysDay,sysHour,sysMinute,sysSecond;
	unsigned int cur_rtc_cnt,cnt_temp;
	unsigned int max_counter;

	get_sys_date_from_rtc(&sysYear,&sysMonth,&sysDay,&sysHour,&sysMinute,&sysSecond);
	cur_rtc_cnt = grab_rtc_counter();	// ��ǰ����ֵ
	cnt_temp = time_regulate(sysYear, sysMonth, sysDay, hour, minute, 0);					// �õ�����ʱ���Ӧ�ļ���ֵ
	max_counter = IS_LEAP_YEAR(sysYear) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

	// �ж�����ʱ���Ƿ��ѹ�?����ѹ�,��д����һ�������ʱ��
	if(cnt_temp<=cur_rtc_cnt)													// �����ʱ���Ѿ���������ʱ��,����Ҫ����Ϊ�����ʱ��
	{
		cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
		cnt_temp += 24*3600*SECOND_CNT_RTC;									// ��һ��
		cnt_temp += (hour*3600+minute*60)*SECOND_CNT_RTC;						// Сʱ�ͷ���(�ڶ��������ʱ��)
		if(cnt_temp>max_counter)												// ���һ�������ʱ�䳬����һ�����ֵ
		{
			cnt_temp = (hour*3600+minute*60)*SECOND_CNT_RTC;					// Сʱ�ͷ���
		}
	}
	
	switch(alarmNum)
	{
		case 1:											// 1����������
		{
			myAlarmTimer1.alarm_cnt = cnt_temp;
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 2����������
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
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
int is_myAlarmTimer_timeout(unsigned alarmNum)
{
	int ret = -1;
	
	switch(alarmNum)
	{
		case 1:											// 1����������
		{
			if(myAlarmTimer1.timeout)	ret =  0;
			else						ret = -1;
			break;
		}
		case 2:											// 2����������
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
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
void clean_myAlarmTimer_timeout_flag(unsigned alarmNum)
{
	switch(alarmNum)
	{
		case 1:											// 1����������
		{
			myAlarmTimer1.timeout = 0;
			break;
		}
		case 2:											// 2����������
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
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
void diasble_myAlarmTimer(unsigned alarmNum)
{
	switch(alarmNum)
	{
		case 1:											// 1����������
		{
			myAlarmTimer1.en= 0;
			break;
		}
		case 2:											// 2����������
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
		
	myRTC_APB_sync();											// RTC��APBͬ��
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
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
int is_powerSave_alarm_timeout(void)
{
	if(myAlarmPowSave.timeout)	return 0;
	else						return -1;
}

/*******************************************************************************
Function:
Descripton:
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
void clean_powerSave_alarm_timeout_flag(void)
{
	myAlarmPowSave.timeout = 0;
}

/*******************************************************************************
Function:
Descripton:
	�жϵ͹��Ķ�ʱ�ж��Ƿ�ʱ
*******************************************************************************/
void diasble_powerSave_alarm(void)
{
	myAlarmPowSave.en= 0;
}


/*******************************************************************************
Function:
Descripton:
	���������Ų�ѯ����ʹ��״̬��
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
	ͨ���������ֵ�õ���������
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
	�����ֻ������alarm����ֵ
	���ж�����ʱ���Ƿ����ڵ��촥����������첻�ܴ����Ļ���Ҫ������ֵ��Ϊ��һ�����ͬʱ�̣�
	�������һ������ʱ�䣬���Ϊ��һ����ͬʱ�̡�
	ע��:�˺���ֻ�����޸��ֻ�����ʱ��ʱ����һ��
*******************************************************************************/
void get_valid_rtc_count_of_alarmTimer(unsigned char alarmNum)
{
	unsigned int sysYear,sysMonth,sysDay,sysHour,sysMinute,sysSecond;
	unsigned int cur_rtc_cnt,cnt_temp;
	unsigned int max_counter;

	get_sys_date_from_rtc(&sysYear,&sysMonth,&sysDay,&sysHour,&sysMinute,&sysSecond);
	cur_rtc_cnt = grab_rtc_counter();	// ��ǰ����ֵ
	max_counter = IS_LEAP_YEAR(sysYear) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

	switch(alarmNum)
	{
		case 1:
		{
			// calculate the rtc counter of alarm1
			if(myAlarm1.en) // ����1����,��������1�´��жϵ�ʱ��
			{
				cnt_temp = time_regulate(sysYear, sysMonth, sysDay, myAlarm1.time.hour, myAlarm1.time.minute, 0); // �õ�����ʱ���Ӧ�ļ���ֵ
				// �ж�����ʱ���Ƿ��ѹ�?����ѹ�,��д����һ�������ʱ��
				if(cnt_temp<=cur_rtc_cnt)													// �����ʱ���Ѿ���������ʱ��,����Ҫ����Ϊ�����ʱ��
				{
					cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
					cnt_temp += 24*3600*SECOND_CNT_RTC; 								// ��һ��
					cnt_temp += (myAlarm1.time.hour*3600+myAlarm1.time.minute*60)*SECOND_CNT_RTC;						// Сʱ�ͷ���(�ڶ��������ʱ��)
					if(cnt_temp>max_counter)												// ���һ�������ʱ�䳬����һ�����ֵ
					{
						cnt_temp = (myAlarm1.time.hour*3600+myAlarm1.time.minute*60)*SECOND_CNT_RTC;					// ��һ���ʱ��(Сʱ�ͷ���)
					}
				}
				myAlarmTimer1.alarm_cnt = cnt_temp; // ���������´��ж�ʱ��
				myAlarmTimer1.en = 1;
			}
			break;
		}
		case 2:
		{
			// calculate the rtc counter of alarm1
			if(myAlarm2.en) // ����1����,��������2�´��жϵ�ʱ��
			{
				cnt_temp = time_regulate(sysYear, sysMonth, sysDay, myAlarm2.time.hour, myAlarm2.time.minute, 0); // �õ�����ʱ���Ӧ�ļ���ֵ
				// �ж�����ʱ���Ƿ��ѹ�?����ѹ�,��д����һ�������ʱ��
				if(cnt_temp<=cur_rtc_cnt)													// �����ʱ���Ѿ���������ʱ��,����Ҫ����Ϊ�����ʱ��
				{
					cnt_temp = time_regulate(sysYear, sysMonth, sysDay, 0, 0, 0);
					cnt_temp += 24*3600*SECOND_CNT_RTC; 								// ��һ��
					cnt_temp += (myAlarm2.time.hour*3600+myAlarm2.time.minute*60)*SECOND_CNT_RTC;						// Сʱ�ͷ���(�ڶ��������ʱ��)
					if(cnt_temp>max_counter)												// ���һ�������ʱ�䳬����һ�����ֵ
					{
						cnt_temp = (myAlarm2.time.hour*3600+myAlarm2.time.minute*60)*SECOND_CNT_RTC;					// ��һ���ʱ��(Сʱ�ͷ���)
					}
				}
				myAlarmTimer2.alarm_cnt = cnt_temp; // ���������´��ж�ʱ��
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
	����͹��������alarmֵ��
	���趨�ļ�������һ���������ֵʱ��������ֵ��ȥ�������ֵ
	ע��:�˺���ֻ�����޸ĵ͹��ļ���ֵʱ����һ��
*******************************************************************************/
void get_valid_rtc_count_of_powerSaveAlarm(void)
{
	unsigned int max_counter;

	// calculate the rtc counter of powersave
	if(myAlarmPowSave.en)
	{
		max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		if(myAlarmPowSave.alarm_cnt>max_counter)	// ����ֵ��������
		{
			myAlarmPowSave.alarm_cnt = max_counter-myAlarmPowSave.alarm_cnt;
		}
	}
}

/*******************************************************************************
Function:
Descripton:
	�����������������
*******************************************************************************/
void get_valid_rtc_count_of_WholeYearAlarm(unsigned int val)
{
	myAlarmWholeYear.alarm_cnt = val;
}

/*******************************************************************************
Function:
Descripton:
	����������:�ֻ��������ֵ���͹����������ֵ,�������ֵ�ŵ�����������
	ע��:ֻ�д��ڵ�ǰRTC����ֵ�����ݲ�������
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
	����������:�ֻ��������ֵ���͹����������ֵ,�������ֵ�ŵ�����������
	ע��:ֻ�д��ڵ�ǰRTC����ֵ�����ݲ�������
*******************************************************************************/
void load_new_alarm_cnt_to_rtc(void)
{
	unsigned int cnt_array[5];
	unsigned char i,j,k;
	unsigned int cur_rtc_cnt;
	unsigned int temp;

	i = 0;
	cur_rtc_cnt = grab_rtc_counter();	// ��ǰ����ֵ
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

	// ð������ѡ����Сֵ
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
	���õ͹������嶨ʱ�����ڴ���ʱ����͹��Ľ���ģʽ��ʱ��
*******************************************************************************/
void powSaveAlarm_set(unsigned int time_ms)
{
	
}

/*******************************************************************************
Function:
Descripton:
	���������ж��У��л������������ֵ��
	�������жϺ�������и�����������ʱ��Ҫ����������ʱ������д��RTC���������
*******************************************************************************/
void rtc_alarm_cnt_update(void)
{
	
}


/*******************************************************************************
Function:
Descripton:
	rtc���жϷ������
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
		if( 0==temp%SECOND_CNT_RTC )												// ����һ��
		{
			MsgRTC.type = MSG_RTC_SECOND_INT;
			MsgRTC.rtcCount = temp/SECOND_CNT_RTC;									// ���������
			xQueueSendFromISR(myRtcQueue,&MsgRTC,0);
		}

		// �ж�RTC�������Ƿ����1��?
        year = YearOfDate;
        max_counter = IS_LEAP_YEAR(year) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		// ��RTC����1��ʱ,����RTC������
        /* reset RTC counter when time is 31st Dec. 23:59:59 */
        if( temp >= (max_counter - 1) )												// ����1��
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
    \brief      RTC�����жϷ������
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
		
		// ����λ��ʱ��־
		alarm_timeout_handle_in_isr(temp);											// ��ʱ����

		// �ж�RTC�������Ƿ����1��?
        year = YearOfDate;
        max_counter = IS_LEAP_YEAR(year) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);

		// ��RTC����1��ʱ,����RTC������
        /* reset RTC counter when time is 31st Dec. 23:59:59 */
        if( temp >= (max_counter - 1) )												// ����1��
		{
            YearOfDate++;
            reset_value = 0;

            /* set new year's value to RTC counter register */
            rtc_lwoff_wait();
            rtc_counter_set(reset_value);
            rtc_lwoff_wait();

			// ��һ����������ֵ
			max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);
			get_valid_rtc_count_of_WholeYearAlarm(max_counter);
        }		

		load_new_alarm_cnt_to_rtc();											// �ж�ʱ����alarm
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
	// ����RTC�ⲿ�жϹ���
	rtc_lwoff_wait();
	rtc_interrupt_enable(RTC_INT_SECOND);
	rtc_interrupt_enable(RTC_INTEN_ALRMIE);
	rtc_lwoff_wait();
	nvic_irq_enable(RTC_ALARM_IRQn,2,0);							// ���������ⲿ�ж�
	nvic_irq_enable(RTC_IRQn,7,0);									// ����RTC���ж�
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
		nvic_irq_enable(RTC_ALARM_IRQn,3,0);							// ʹ��RTC�����ж�
		exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
		//rtc_interrupt_enable(RTC_INT_ALARM);							// rtc�����ж�
		exti_interrupt_enable(EXTI_17);
		exti_interrupt_flag_clear(EXTI_17);
	}
	else
	{
		nvic_irq_disable(RTC_ALARM_IRQn);							// ʹ��RTC�����ж�
		rtc_interrupt_disable(RTC_INT_ALARM);							// rtc�����ж�
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
    rcu_periph_clock_enable(RCU_BKPI);			// RCU_APB1EN - ����BKPʱ��
    rcu_periph_clock_enable(RCU_PMU);			// RCU_APB1EN - ����PMUʱ��
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable RTC Clock */
    rcu_periph_clock_disable(RCU_RTC);			// ����RTCʱ��
	__nop();__nop();__nop();__nop();

    /* enable LXTAL */
    rcu_osci_off(RCU_LXTAL);					// �򿪵���ʱ�ӿ���
	__nop();__nop();__nop();__nop();

    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);						// �򿪵���ʱ�ӿ���
	
    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);						// �򿪵���ʱ�ӿ���
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);				// �ȴ�����ʱ���ȶ�
    
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);		// ����RTCʱ��Դ

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);			// ����RTCʱ��

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();					// �ȴ�RTC�Ĵ�����ABP1ʱ��ͬ��

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();							// �ȴ�RTC�Ĵ����������

    /* enable the RTC second and alarm interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);
    //rtc_interrupt_enable(RTC_INT_ALARM);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    //rtc_prescaler_set(32767);
    rtc_prescaler_set(VAL_RTC_PRESCAL);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();							// �ȴ�RTC�Ĵ����������
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
	// ����Ĭ�����ڵõ�RTC�ļ�����ֵ
    temp = time_regulate(DEFAULT_YEAR, DEFAULT_MONTH, DEFAULT_DAY, DEFAULT_HOUR, DEFAULT_MINUTE, DEFAULT_SECOND);					
    /* change the current time */
    rtc_lwoff_wait();
    rtc_counter_set(temp);					// ����д��RTC����ֵ
    rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
	��ʼ��RTC������
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
        // ʹ��Ĭ��ʱ��RTC����ֵ��ʼ��
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

	// �������ֵ
	max_counter = IS_LEAP_YEAR(YearOfDate) ? (366ul * 24ul * 3600ul * SECOND_CNT_RTC) : (365ul * 24ul * 3600ul * SECOND_CNT_RTC);
	get_valid_rtc_count_of_WholeYearAlarm(max_counter);
	
	rtc_count = rtc_counter_get();												// �ϵ�ʱ�ȷ���ʱ�ӵ�����
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
	return( rtc_cnt );						// ��ȡRTC������
}


/*******************************************************************************
Function:
Descripton:
	ȡ��RTC����ֵ��Ȼ��ת��Ϊ���ں�ʱ��
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
	rtc_counter_set(val);									// ����RTC
	rtc_lwoff_wait();
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void reload_rtc_alarm(unsigned int val)
{
	rtc_lwoff_wait();
	rtc_alarm_config(val);									// ����RTC
	rtc_lwoff_wait();
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void rtc_exti_init(void)
{
    // ����RTC�ⲿ�жϹ���
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
	rtc_port_init();							// IO�ڳ�ʼ��������ΪOSC32��
    // RTC���ã���Ϊ�״��ϵ�ͻ���ģʽ
    rtc_process();
	
	/* clear reset flags */
	rcu_all_reset_flag_clear();

	rtc_exti_init();							// ʹ��RTC�ⲿ�ж�

	nvic_rtc_config();
}




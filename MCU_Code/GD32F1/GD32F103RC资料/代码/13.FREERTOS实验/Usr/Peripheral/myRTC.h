/********************************************************************************************
file name:
Description:
********************************************************************************************/
#ifndef		__MY_RTC_H__
#define		__MY_RTC_H__
//----------------------------- include files -----------------------------
#include "gd32e10x.h"
#include <stdio.h>
//----------------------------- macro define ------------------------------
#define		IO_OSC32IN				PC14
#define		IO_OSC32OUT				PC15

#define		IO_OSC32IN_AF_RCU		RCU_AF
#define		IO_OSC32IN_RCU			RCU_GPIOC
#define		IO_OSC32IN_PORT			GPIOC
#define		IO_OSC32IN_PIN			GPIO_PIN_14

#define		IO_OSC32OUT_AF_RCU		RCU_AF
#define		IO_OSC32OUT_RCU			RCU_GPIOC
#define		IO_OSC32OUT_PORT		GPIOC
#define		IO_OSC32OUT_PIN			GPIO_PIN_15

#define		IO_OSC32IN_INIT()	do{								\
									rcu_periph_clock_enable(IO_OSC32IN_AF_RCU);	\
									rcu_periph_clock_enable(IO_OSC32IN_RCU);		\
									gpio_init(IO_OSC32IN_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, IO_OSC32IN_PIN);	\
									gpio_bit_set(IO_OSC32IN_PORT,IO_OSC32IN_PIN);		\
								}while(0)

#define		IO_OSC32OUT_INIT()	do{								\
									rcu_periph_clock_enable(IO_OSC32OUT_AF_RCU);	\
									rcu_periph_clock_enable(IO_OSC32OUT_RCU);		\
									gpio_init(IO_OSC32OUT_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, IO_OSC32OUT_PIN);	\
									gpio_bit_set(IO_OSC32OUT_PORT,IO_OSC32OUT_PIN);		\
								}while(0)

#define		IO_OSC32IN_IN()	do{								\
									rcu_periph_clock_enable(IO_OSC32IN_AF_RCU);	\
									rcu_periph_clock_enable(IO_OSC32IN_RCU);		\
									gpio_init(IO_OSC32IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, IO_OSC32IN_PIN);	\
									gpio_bit_set(IO_OSC32IN_PORT,IO_OSC32IN_PIN);		\
								}while(0)

#define		IO_OSC32OUT_IN()	do{								\
									rcu_periph_clock_enable(IO_OSC32OUT_AF_RCU);	\
									rcu_periph_clock_enable(IO_OSC32OUT_RCU);		\
									gpio_init(IO_OSC32OUT_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, IO_OSC32OUT_PIN);	\
									gpio_bit_set(IO_OSC32OUT_PORT,IO_OSC32OUT_PIN);		\
								}while(0)

								
#define BACKUP_DATA   (0xABCD)

#define DIGIT_TO_CHAR_OFFSET    (48)
#define MIN_YEAR                (1951)
#define YEAR_SPAN               (100)
#define MAX_YEAR                (MIN_YEAR + YEAR_SPAN - 1)
#define IS_LEAP_YEAR(year)      ((year % 4 == 0) && ( year % 100 != 0) || (year % 400 == 0))

#define DEFAULT_YEAR            (2020)
#define DEFAULT_MONTH           (10)
#define DEFAULT_DAY          	(1)
#define DEFAULT_HOUR          	(12)
#define DEFAULT_MINUTE         	(0)
#define DEFAULT_SECOND        	(0)
//----------------------------- macro define ------------------------------
#define	FRQ_RTC_LXTAL		32768ul
#define	VAL_RTC_PRESCAL		256ul
#define	SECOND_CNT_RTC		(FRQ_RTC_LXTAL/VAL_RTC_PRESCAL)								
#define	EXTI_RTC			EXTI_17
//----------------------------- macro define ------------------------------
#define	LEN_LIST_ALARM_CNT		10
//----------------------------- macro define ------------------------------
typedef	struct __myAlarm_data_type__
{
	unsigned char en;
	unsigned char timeout;
	unsigned int alarm_cnt;
}myAlarmData_t;
typedef struct __listDataAlarm_type__
{
	unsigned char empty;
	unsigned int count;
}listDataAlarm_t;
typedef struct __ListAlarmCnt_type__
{
	struct __ListAlarmCnt_type__ *next;
	struct __ListAlarmCnt_type__ *prev;
	
	listDataAlarm_t data;
}ListAlarmCnt_t;
typedef struct __myAlarmTime_type__
{
	unsigned int hour;
	unsigned int minute;
}myAlarmTime_t;
typedef struct __myAlarm_type__
{
	unsigned char en;
	myAlarmTime_t time;
}myAlarm_t;
//--------------------------- varaible declare ----------------------------
extern unsigned int YearOfDate;										// 用于存储日期的年份，同时还要保存到FLASH中。
extern  myAlarmData_t myAlarmTimer1;											// 闹铃1时间
extern  myAlarmData_t myAlarmTimer2;											// 闹铃2时间
extern  myAlarmData_t myAlarmPowSave;											// 低功耗闹铃
extern  myAlarmData_t myAlarmWholeYear;											// RTC计满1整年中断
extern  myAlarm_t myAlarm1;													// 闹铃1日期
extern  myAlarm_t myAlarm2;													// 闹铃2日期
//--------------------------- function declare ----------------------------
void rtc_port_init(void);
void nvic_rtc_config(void);
void rtc_alarm_switch(unsigned char flag);
void rtc_configuration(void);
uint32_t time_regulate(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second);
void time_adjust(void);
uint32_t calc_days_num_by_date(uint32_t year, uint32_t month, uint32_t day);
void rtc_process(void);
void get_calender_from_rtcCount(uint32_t rtc_counter,uint32_t* year,uint32_t* month,uint32_t* day,uint32_t* hour,uint32_t* minute,uint32_t* second,uint32_t* week);
unsigned int grab_rtc_counter(void);
void reload_rtc_counter(unsigned int val);
void reload_rtc_alarm(unsigned int val);
void myRTC_init(void);
void myRTC_APB_sync(void);
void myRTC_enter_config_mod(void);
void myRTC_exit_config_mod(void);
void date_load_to_rtc(unsigned char alarmNum, unsigned short int year, unsigned short int month, unsigned short int day, unsigned short int hour, unsigned short int minute);
void myAlarmTimer_switch(unsigned char alarmNum, unsigned char on_off);
void myAlarmTimer_time_set(unsigned char alarmNum, unsigned short int hour, unsigned short int minute);
void rtc_alarm_cnt_update(void);
void get_sys_date_from_rtc(uint32_t* year,uint32_t* month,uint32_t* day,uint32_t* hour,uint32_t* minute,uint32_t* second);
int check_alarm_status(int alarm_num);
void myAlarm_date_get(unsigned char alarm_num, unsigned int* year,unsigned int* month,unsigned int* day,unsigned int* hour,unsigned int* minute,unsigned int* second);
void get_valid_rtc_count_of_alarmTimer(unsigned char alarmNum);
void get_valid_rtc_count_of_powerSaveAlarm(void);
void load_new_alarm_cnt_to_rtc(void);
void alarm_timeout_handle_in_isr(unsigned int cur_rtc_cnt);
void get_valid_rtc_count_of_WholeYearAlarm(unsigned int val);
int is_powerSave_alarm_timeout(void);
void clean_powerSave_alarm_timeout_flag(void);
void diasble_powerSave_alarm(void);
int is_myAlarmTimer_timeout(unsigned alarmNum);
void clean_myAlarmTimer_timeout_flag(unsigned alarmNum);
void diasble_myAlarmTimer(unsigned alarmNum);
void myPowerSaveTimer_time_set(unsigned int time_ms);


#endif



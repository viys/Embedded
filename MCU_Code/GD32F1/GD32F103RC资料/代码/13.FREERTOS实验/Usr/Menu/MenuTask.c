/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "MenuTask.h"
#include "FreeRTOS.h"
#include "myQueue.h"
#include "task.h"
#include <stdio.h>
#include "SoftTimer.h"
#include "myRTC.h"
//#include "oled.h"
//#include "oled_128x64.h"
#include "oled_ch1115_spi.h"
#include "bmp.h"
#include "mypmu.h"
#include "rfdecode.h"
#include "miscellaneousTask.h"
//--------------------- variable declare ----------------------
static const char Char_SlaveName[INDX_SLAVE_NAME_CHR_MAX+1] = 
{
	'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'-',
};

unsigned int yearRtc,monthRtc,dayRtc,weekRtc;
unsigned int hourRtc,minuteRtc,secondRtc;

unsigned char PrevousMenu = MENU_MAX;
unsigned char CurrentMenu = MENU_TURN_OFF;
unsigned char MenuIndex = MENU_TURN_OFF;
unsigned char SettingItem = SET_ITEM_ADD_SLAVE;								// 默认进入设置时选择第一项
unsigned char preSettingItem = SET_ITEM_ADD_SLAVE;							// 默认进入设置时选择第一项

static unsigned cursorMenu = 0;												// 光标
unsigned char myMenuStageCnt = 0;											// 界面显示步骤
unsigned short int SlaveCount = 0;

unsigned char NumDisStyle = NUM_STYLE_0;									// 分机号码显示格式
unsigned char Alarm1_Status = DIS_STATUS;									// 闹铃开关标志
unsigned char Alarm2_Status = DIS_STATUS;									// 闹铃开关标志

unsigned char AutoCleanTime = AUTO_CLEAN_TIME_MIN;
unsigned int AutoStandbyCnt = 0;											// 自动进入待机计时
unsigned int AutoDisplayOffCnt = 0;											// 自动熄屏计时
unsigned char FG_PowerSave = 0;												// 省电标志

myWaitNumQue_t	myWaitNumQue = {0};

const char mac_version[] = "V1.0";
static unsigned char stage_powSave = STAGE_STANDBY_RX_START;				// 待机状态标志



/*****************************************************************************************
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void power_save_init(void)
{
	PMU_init();
}

/*****************************************************************************************
Function: 
Description: 
*****************************************************************************************/
void set_rtc_alarm_time(unsigned int time_ms)
{
	unsigned int temp = (time_ms*1000/7812);
	
	myRTC_APB_sync();											// RTC与APB同步
	//myRTC_enter_config_mod();
	taskENTER_CRITICAL();
	reload_rtc_alarm(grab_rtc_counter()+temp);
	taskEXIT_CRITICAL();
	nvic_rtc_config();
}


/*****************************************************************************************
Function: 
Description: 
*****************************************************************************************/
int getCharIndex(char inChar)
{
	int i;
	
	for( i=0;i<(sizeof(Char_SlaveName)/sizeof(char));i++ )
	{
		if(Char_SlaveName[i]==inChar)	return(i);
	}
	
	return(-1);
}


/*****************************************************************************************
Function: 
Description: 
*****************************************************************************************/
void switch_menu(unsigned char menuInx)
{
	MenuIndex = menuInx;
	myMenuStageCnt = 0;
}

/*****************************************************************************************
Function:
Description:
*****************************************************************************************/
unsigned int get_sysTicks_interval(unsigned int currentTick,unsigned int prevousTick)
{
	if(currentTick<prevousTick)
	{
		return( (currentTick+0xffffff)-prevousTick );
	}
	else 
	{
		return(currentTick-prevousTick);
	}
}

/*****************************************************************************************
Function:
Descripton:
	进入休眠前先判断要将什么值写入RTC闹铃计数器中。
	判断的对象包括：系统唤醒中断，手环闹铃，RTC整年唤醒中断
*****************************************************************************************/
void enter_sleep_rtc_prepare(void)
{
	
}

/*****************************************************************************************
Function: 
Description: 
	关机界面，进入关机界面后MCU和LCD、RF均进入休眠状态。
*****************************************************************************************/
void menu_off(void)
{
	Msg_RTC_t	MsgRTC;
	int msg_key;
	BaseType_t	ret;
	Msg_RfCtrl_t msgRfCtrl;
	static unsigned char Time_Sleep_Cnt = 0;									// 关机时进入睡眠状态延时计数
	static unsigned char Flag_KeyWait = 0;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_DisPlay_Off();
			RF_module_sleep();
			//ClockColonFlash_timer_stop();
			msgRfCtrl = MSG_RF_TURN_OFF;										// 关闭RF接收
			ret = xQueueSend(myRfCtrlQueue,&msgRfCtrl,0);

			Time_Sleep_Cnt = 0;
			Flag_KeyWait = 0;
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			vTaskDelay(pdMS_TO_TICKS(100));
			Time_Sleep_Cnt++;
			
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)														// 有按键
			{
				if(MSG_KEY_ENT_TOUCHING==msg_key)								// 确认键正在按下
				{
					Flag_KeyWait = 1;
					Time_Sleep_Cnt = 0;
				}
				else if(MSG_KEY_ENT_L_TOUCH==msg_key)
				{
					// 接收RTC日期
					ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
					if(pdTRUE==ret)
					{
						if(MSG_RTC_SECOND_INT==MsgRTC.type)
						{
							get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
						}
					}
					Time_Sleep_Cnt = 0;
					switch_menu(MENU_STANDBY);										// 切换到待机界面
				}		
			}

			// is the time to enter sleep mode?
			if(Time_Sleep_Cnt>(Flag_KeyWait?TIME_ENTER_SLP1:TIME_ENTER_SLP2))		// 检测ENT键有按下动作,进入睡眠的等待时间延长
			{
				ret = xQueueReset(myKeyQueue);
				OLED_DisPlay_Off();													// 显示关闭
				RF_module_sleep();													// RF睡眠
				mcu_goto_deepsleep();												// 单片机进入睡眠状态
				
				Time_Sleep_Cnt = 0;													// 再次被唤醒
				Flag_KeyWait = 0;
				taskENTER_CRITICAL();
				recover_sys_clock();												// 恢复系统时钟
				taskEXIT_CRITICAL();
			}
			
			// 手环闹铃超时,自动进入待机界面
			if(0==is_myAlarmTimer_timeout(1)||0==is_myAlarmTimer_timeout(2))
			{
				// 接收RTC日期
				ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
				if(pdTRUE==ret)
				{
					if(MSG_RTC_SECOND_INT==MsgRTC.type)
					{
						get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
					}
				}
				Time_Sleep_Cnt = 0;
				switch_menu(MENU_STANDBY);											// 切换到待机界面
			}

			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	无开机界面，开机即进入待机
*****************************************************************************************/
void menu_powerOn(void)
{

}

/*****************************************************************************************
Function: 
Description: 
	RF低功耗模式运行 	
*****************************************************************************************/
void RF_Receive_LowPower_Runing(void)
{
	static unsigned char rf_decode_success = 0;
	static unsigned int PreTicks_Sty;
	static Msg_VaildFrame_t stbyRxFrame;
	int signalQuality;
	Bk_Cmd_t myBkCmd;
	Bk_CmdReq_t mCmdReq; 
	BaseType_t	ret;
	
	switch(stage_powSave)
	{
		// ***************** RF接收开始,最长接收时间为300MS **************
		case STAGE_STANDBY_RX_START:	// 开始接收
		{
			RF_module_goto_rx();
			rf_decode_control(1);
			ret = xQueueReset(myRfObserverQue); 										// RF侦测消息清空
			ret = xQueueReset(myRfFrameQueue);											// 清空RF帧消息
			rf_decode_success = 0;
			PreTicks_Sty = xTaskGetTickCount(); 										// 保存当前系统滴答
			stage_powSave = STAGE_STANDBY_RX_BAD_SIG;
			break;
		}
		// ******************* 5MS侦测无效信号立刻进入休眠 *****************
		case STAGE_STANDBY_RX_BAD_SIG:
		{
			ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// 接收RF侦测消息
			if(pdTRUE==ret)
			{
				if(RF_SIG_BAD==signalQuality)											// 侦测到无效信号-当前为噪声或干扰严重
				{
					stage_powSave = STAGE_STANDBY_SLEEP;
					break;
				}
				else																	// 侦测到有效信号-需要延时接收
				{
					ResetBadSignalCnt();														// 无效信号计数清0
					stage_powSave = STAGE_STANDBY_RX_APPEND_DELAY;
					break;
				}
			}

			// 5ms超时未侦测到无效信号，转到有效信号侦测
			if( get_sysTicks_interval(xTaskGetTickCount(),PreTicks_Sty)>=pdMS_TO_TICKS(5) )
			{
				ResetBadSignalCnt();														// 无效信号计数清0
				stage_powSave = STAGE_STANDBY_RX_VALID_SIG;
			}
			break;
		}
		// ******************* 前5MS内未侦测到无效信号，延时接收有效信号35ms *****************
		case STAGE_STANDBY_RX_VALID_SIG:
		{
			ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// 接收RF侦测消息
			if(pdTRUE==ret)
			{
				if(RF_SIG_BAD==signalQuality)											// 侦测到无效信号-当前为噪声或干扰严重
				{
					stage_powSave = STAGE_STANDBY_SLEEP;
					break;
				}
				else																	// 侦测到有效信号-需要延时接收
				{
					ResetBadSignalCnt();														// 无效信号计数清0
					stage_powSave = STAGE_STANDBY_RX_APPEND_DELAY;
					break;
				}
			}

			if( get_sysTicks_interval(xTaskGetTickCount(),PreTicks_Sty)>=pdMS_TO_TICKS(40) )
			{
				stage_powSave = STAGE_STANDBY_SLEEP;
			}
			break;
		}
		// ******************* 侦测到有效信号后延时接收有效帧260ms *****************
		case STAGE_STANDBY_RX_APPEND_DELAY:
		{
			ret = xQueueReceive(myDecodeQueue,&stbyRxFrame,0);							// 收到解码成功的数据帧
			if(pdTRUE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SID_SEARCH;
				break;
			}
			else																		// 超时判断
			{
				ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// 接收RF侦测消息
				if(pdTRUE==ret)
				{
					if(RF_SIG_BAD==signalQuality)											// 侦测到无效信号-当前为噪声或干扰严重
					{
						stage_powSave = STAGE_STANDBY_SLEEP;
						break;
					}
				}
			}
			
			if(get_sysTicks_interval(xTaskGetTickCount(),PreTicks_Sty)>=pdMS_TO_TICKS(300))
			{
				stage_powSave = STAGE_STANDBY_SLEEP;
			}
			break;
		}
		// *****************// 熄屏睡眠200MS**************
		case STAGE_STANDBY_SLEEP:	// 熄屏睡眠200MS
		{
			// 关闭RF,关闭显示
			rf_decode_control(0);
			RF_module_sleep();
			// 开启RTC闹铃，设置闹铃时间120ms
			if(rf_decode_success)	myPowerSaveTimer_time_set(1500);						// 设置低功耗时间
			else	myPowerSaveTimer_time_set(200);
			get_valid_rtc_count_of_powerSaveAlarm();									// 调整ALARM计数值
			load_new_alarm_cnt_to_rtc();												// 将所有闹铃值排序，然后将最近的值写入RTC
			// 休眠
			if(1==check_powerSave_status())												// enter power save mode or not?
			{
				mcu_goto_deepsleep();													// 单片机进入睡眠状态
				// 退出睡眠状态
				taskENTER_CRITICAL();
				recover_sys_clock();												// 恢复系统时钟
				taskEXIT_CRITICAL();

				RF_module_goto_rx();
				diasble_powerSave_alarm();
				clean_powerSave_alarm_timeout_flag();
				stage_powSave = STAGE_STANDBY_RX_START;
			}
			else
			{
				stage_powSave = STAGE_STANDBY_SLP_WAIT;
			}
			break;
		}
		// ******************* 等待超时 *****************
		case STAGE_STANDBY_SLP_WAIT:
		{
			// 等待ALARM中断
			if(0==is_powerSave_alarm_timeout()) 										// 判断闹铃是否超时
			{
				diasble_powerSave_alarm();
				clean_powerSave_alarm_timeout_flag();
				RF_module_goto_rx();
				stage_powSave = STAGE_STANDBY_RX_START;
			}
			break;
		}
		// ******************* 接收到有效数据帧后搜索ID是否注册过 *****************
		case STAGE_STANDBY_SID_SEARCH:
		{
			rf_decode_control(0);
			RF_module_sleep();
			myBkCmd.cmd = BK_CMD_SEARCH_SID_PREPARE;
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));			// 发送搜索准备命令
			if(pdFALSE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SLEEP;
				break;
			}
			myBkCmd.cmd = BK_CMD_SEARCH_SID;
			myBkCmd.SlaveID.Integer = stbyRxFrame.value&0xfffff0;					// 屏蔽键值
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));			// 发送搜索命令和SID
			if(pdFALSE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SLEEP;
				break;
			}
			ret = xQueueReceive(myCmdReqQue,&mCmdReq,pdMS_TO_TICKS(1000));			// 返回查询结果
			if(pdTRUE==ret)
			{
				if(BK_CMD_SEARCH_SID_OK==mCmdReq.cmd)								// 已经注册过
				{
					// 切换到提醒状态
					myWaitNumQue.WaitNumArry[myWaitNumQue.Tail].WaitNum = mCmdReq.SlaveName.Integer;
					myWaitNumQue.WaitNumArry[myWaitNumQue.Tail].KeyValue = myBkCmd.SlaveID.Integer&0x0f;
					myWaitNumQue.Amount++;
					if(myWaitNumQue.Amount>NUM_WAIT_NUM_QUE)
					{
						myWaitNumQue.Amount = NUM_WAIT_NUM_QUE;
					}
					myWaitNumQue.Tail = (myWaitNumQue.Tail+1)%NUM_WAIT_NUM_QUE;
					switch_menu(MENU_NOFITY);										// 进入提醒界面
					break;
				}
			}
			
			rf_decode_success = 1;
			stage_powSave = STAGE_STANDBY_SLEEP;
			break;
		}
		// ******************* 呼叫成功后禁用2S *****************
		case STAGE_STANDBY_RF_OFF_DELAY:
		{
			
			break;
		}
		// ******************* 默认状态 *****************
		default:
		{
			break;
		}
	}
}


/*****************************************************************************************
Function: 
Description: 
				待机界面
	进入待机后,若超时进入熄屏省电模式，则MCU进入休眠状态。
*****************************************************************************************/
void menu_standby(void)
{
	Msg_RTC_t	MsgRTC;
	Msg_RfCtrl_t msgRfCtrl;
	Msg_SecondFlash_t	MsgSecFlash;
	BaseType_t	ret;
	int msg_key;
	char str_temp[10] = {0};
	char str_temp1[10] = {0};
	char str_hour[2] = {0};
	char str_minute[2] = {0};
	unsigned char LCD_Fresh = 0;
	static unsigned char Flag_Colon = 1;
	volatile unsigned int count_temp;
	static unsigned char Flag_alarm_Tout = 0,Flag_alarm_icon_flash = 1;

	switch(myMenuStageCnt)
	{
		//--------------------------------- 待机初如化 --------------------------------------------
		case MENU_STAGE_INIT:
		{
			OLED_DisPlay_On();
			OLED_Clear();			
			
			// 接收RTC日期
			ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
			if(pdTRUE==ret)
			{
				if(MSG_RTC_SECOND_INT==MsgRTC.type)
				{
					get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
				}
			}
			
			sprintf( str_temp, "%02d/%02d", monthRtc,dayRtc );						// 日月
			sprintf( str_hour, "%02d", hourRtc );									// 小时
			sprintf( str_minute, "%02d", minuteRtc );								// 分
			
			OLED_ShowString(2,0,(unsigned char*)str_hour,32,1);						// 刷-小时
			OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// 刷-:
			OLED_ShowString(2+32/2*3,0,(unsigned char*)str_minute,32,1);				// 刷-分钟
			
			OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1);			// 刷闹钟图标
			OLED_ShowPicture(114,2,13,7,(unsigned char*)BMP_bat_13x7,1);			// 电池图标
			
			OLED_ShowString(98,19,(unsigned char*)str_temp,12,1);
			OLED_Refresh();

			//ClockColonFlash_timer_init();											// 开启秒闪定时
			//ClockColonFlash_timer_reset();

			msgRfCtrl = MSG_RF_TURN_ON;												// 开启RF接收
			ret = xQueueSend(myRfCtrlQueue,&msgRfCtrl,0);
			ret = xQueueReset(myRtcQueue);											// 清空秒中断消息
			ret = xQueueReset(mySecFlashQueue);

			reset_powerSave_counter();												// 禁止进入省电状态
			powerSave_flag_set(0);

			stage_powSave = STAGE_STANDBY_RX_START;
			
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		//--------------------------------- 待机运行 --------------------------------------------
		case MENU_STAGE_RUN:
		{
			// 判断是否进入低功耗？
			if( is_powerSave_time() )
			{
				powerSave_flag_set(1);
				LCD_Fresh = 1;
			}
			
			// 接收RTC日期
			ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
			if(pdTRUE==ret)
			{
				if(MSG_RTC_SECOND_INT==MsgRTC.type)
				{
					get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
					
					sprintf( str_temp, "%02d/%02d", monthRtc,dayRtc );						// 日月
					sprintf( str_hour, "%02d", hourRtc );									// 小时
					sprintf( str_minute, "%02d", minuteRtc );								// 分
					
					OLED_ShowString(2,0,(unsigned char*)str_hour,32,1);						// 刷-小时
					OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// 刷-:
					OLED_ShowString(2+32/2*3,0,(unsigned char*)str_minute,32,1);				// 刷-分钟
					OLED_ShowString(98,19,(unsigned char*)str_temp,12,1);					// 刷日月
					
					LCD_Fresh = 1;
				}
			}

			// 秒闪中断
			ret = xQueueReceive(mySecFlashQueue,&MsgSecFlash,0);
			if(pdTRUE==ret)
			{
				// 中间时间
				if(Flag_Colon)
				{
					Flag_Colon = 0;
					OLED_ShowString(2+32/2*2,0,(unsigned char*)" ",32,1);					// 刷-
				}
				else
				{
					Flag_Colon = 1;
					OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// 刷-:
				}

				// 闹铃图标
				if(Flag_alarm_Tout)
				{
					if(Flag_alarm_icon_flash)
					{
						Flag_alarm_icon_flash = 0;
						OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_Null_10x12,1); 		// 刷闹钟图标
					}
					else
					{
						Flag_alarm_icon_flash = 1;
						OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); 		// 刷闹钟图标
					}
				}
				LCD_Fresh = 1;
			}

			// 按键检测
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:
					{
						if(1==Flag_alarm_Tout)												// 闹铃超时
						{
							Flag_alarm_Tout = 0;
							OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); // 刷闹钟图标
							LCD_Fresh = 1;
						}
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:
					{
						if(1==Flag_alarm_Tout)												// 闹铃超时
						{
							Flag_alarm_Tout = 0;
							OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); // 刷闹钟图标
							LCD_Fresh = 1;
						}
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_TURN_OFF);											// 切换到关机界面
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:
					{
						SettingItem = SET_ITEM_ADD_SLAVE;
						switch_menu(MENU_SET_ITEM);											// 进入设置界面
						break;
					}
					default:
						break;
				}
				
				reset_powerSave_counter();													// 禁止进入省电状态
				powerSave_flag_set(0);
				LCD_Fresh = 1;
			}
			
			if(LCD_Fresh)																	// 刷新LCD
			{
				LCD_Fresh = 0;
				if(check_powerSave_status())												// 省电状态下关闭显示
				{
					OLED_DisPlay_Off();
				}
				else
				{
					OLED_DisPlay_On();
					OLED_Refresh();
				}
			}

			// RF receive running
			RF_Receive_LowPower_Runing();													// RF低功耗运行

			// 手环闹铃超时,闹铃图标闪烁
			if(0==is_myAlarmTimer_timeout(1)||0==is_myAlarmTimer_timeout(2))
			{
				clean_myAlarmTimer_timeout_flag(1);
				clean_myAlarmTimer_timeout_flag(2);
				Flag_alarm_Tout = 1;														// 闹铃图标闪烁
				Flag_alarm_icon_flash = 1;
				reset_powerSave_counter();													// 禁止进入省电状态
				powerSave_flag_set(0);
			}

			// when external key wake up MCU
			if(0==is_key_wakeup_system())
			{
				is_key_wakeup_system();
				reset_powerSave_counter();													// 禁止进入省电状态
				powerSave_flag_set(0);
			}
			break;
		}
		//--------------------------------- 默认情况 --------------------------------------------
		default:
		{
			break;
		}
	}
}


/*****************************************************************************************
Function: 
Description: 
	呼叫提醒界面。显示呼叫号码
*****************************************************************************************/
void menu_notify(void)
{
	myInt5CharInx_t slaveNum;
	unsigned char keyValue;
	signed char strSlaveNum[6] = {0};
	unsigned char index;
	int ret;
	int msg_key;
	unsigned char fg_LedFresh = 0;
	
	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_DisPlay_On();
			// 取出第一个排队的号码
			index = (myWaitNumQue.Tail+NUM_WAIT_NUM_QUE-myWaitNumQue.Amount)%NUM_WAIT_NUM_QUE;
			slaveNum.Integer = myWaitNumQue.WaitNumArry[index].WaitNum;
			keyValue = myWaitNumQue.WaitNumArry[index].KeyValue;
			
			// 取出呼叫号码的字符
			strSlaveNum[0] = Char_SlaveName[slaveNum.mChar.indx0];
			strSlaveNum[1] = Char_SlaveName[slaveNum.mChar.indx1];
			strSlaveNum[2] = Char_SlaveName[slaveNum.mChar.indx2];
			strSlaveNum[3] = Char_SlaveName[slaveNum.mChar.indx3];
			strSlaveNum[4] = Char_SlaveName[slaveNum.mChar.indx4];
			
			// 显示字符
			OLED_Clear();
			OLED_ShowString(34,4,(unsigned char*)strSlaveNum,24,1);
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			// 按键检测
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:
					{
						if(myWaitNumQue.Amount>1)
						{
							myWaitNumQue.Amount--;
							fg_LedFresh = 1;
						}
						else
						{
							myWaitNumQue.Amount = 0;
							switch_menu(MENU_STANDBY);									// 切换到待机界面
						}
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:
					{
						myWaitNumQue.Amount = 0;
						switch_menu(MENU_STANDBY);									// 切换到待机界面
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:
					{
						break;
					}
					default:
						break;
				}
			}
			
			// 刷新显示
			if(fg_LedFresh)
			{
				fg_LedFresh = 0;
				
				// 取出第一个排队的号码
				if(0<myWaitNumQue.Amount)
				{
					index = (myWaitNumQue.Tail+NUM_WAIT_NUM_QUE-myWaitNumQue.Amount)%NUM_WAIT_NUM_QUE;
					slaveNum.Integer = myWaitNumQue.WaitNumArry[index].WaitNum;
					keyValue = myWaitNumQue.WaitNumArry[index].KeyValue;
					
					// 取出呼叫号码的字符
					strSlaveNum[0] = Char_SlaveName[slaveNum.mChar.indx0];
					strSlaveNum[1] = Char_SlaveName[slaveNum.mChar.indx1];
					strSlaveNum[2] = Char_SlaveName[slaveNum.mChar.indx2];
					strSlaveNum[3] = Char_SlaveName[slaveNum.mChar.indx3];
					strSlaveNum[4] = Char_SlaveName[slaveNum.mChar.indx4];
					
					// 显示字符
					OLED_Clear();
					OLED_ShowString(34,4,(unsigned char*)strSlaveNum,24,1);
					OLED_Refresh();
				}
			}
			break;
		}
		default:
			break;
	}
}


/*****************************************************************************************
Function: 
Description: 
	设置功能选择界面
*****************************************************************************************/
void menu_setItem_select(void)
{
	Msg_RfCtrl_t msgRfCtrl;
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			msgRfCtrl = MSG_RF_TURN_OFF;												// 关闭RF接收
			ret = xQueueSend(myRfCtrlQueue,&msgRfCtrl,0);
			RF_module_sleep();

			OLED_Clear();
			sprintf(stringTemp,"F%02d",SettingItem);
			OLED_ShowString(40,0,(unsigned char*)stringTemp,32,1);
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					//-------------------- 按键处理 ---------------------------
					case MSG_KEY_ENT_S_TOUCH:
					{
						SettingItem++;
						if(SET_ITEM_MAX<=SettingItem)
						{
							SettingItem = SET_ITEM_ADD_SLAVE;
						}
						sprintf(stringTemp,"F%02d",SettingItem);
						OLED_ShowString(40,0,(unsigned char*)stringTemp,32,1);
						OLED_Refresh();
						break;
					}
					//--------------------- 按键处理 ------------------------
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					//--------------------- 按键处理 ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_STANDBY);										// 切换到待机界面
						break;
					}
					//--------------------- 按键处理 ------------------------
					case MSG_KEY_FUN_L_TOUCH:
					{
						switch(SettingItem)
						{
							case SET_ITEM_ADD_SLAVE:
							{
								switch_menu(MENU_ADD_SLAVE);							// 切换到
								break;
							}
							case SET_ITEM_ALERT_TIME:
							{
								switch_menu(MENU_ALERT_TIME);							// 切换到
								break;
							}
							case SET_ITEM_AUTO_CLEAN:
							{
								switch_menu(MENU_AUTO_CLEAN);							// 切换到
								break;
							}
							case SET_ITEM_SLAVE_DELETE:
							{
								switch_menu(MENU_SLAVE_DELETE);							// 切换到
								break;
							}
							case SET_ITEM_DATE_TIME:
							{
								switch_menu(MENU_DATE_TIME);							// 切换到
								break;
							}
							case SET_ITEM_NUMBER_STYLE:
							{
								switch_menu(MENU_NUMBER_STYLE);							// 切换到
								break;
							}
							case SET_ITEM_PASSWORD:
							{
								switch_menu(MENU_PASSWORD);								// 切换到
								break;
							}
							case SET_ITEM_ALARM:
							{
								switch_menu(MENU_ALARM_TIME);							// 切换到
								break;
							}
							case SET_ITEM_ALARM_SWITCH:
							{
								switch_menu(MENU_ALARM_SWITCH);							// 切换到
								break;
							}
							case SET_ITEM_VERSION:
							{
								switch_menu(MENU_MAC_VERSION);							// 切换到
								break;
							}
							default:
								break;
						}
						break;
					//--------------------- 按键处理 -------------------------
					}
					default:
						break;
				}
			}
			break;		
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	添加分机界面
	进入添加分机界面后，默认显示'00001'分机编号，5位分机编号的索引值会被合并成一个32位数存储到FLASH中。
*****************************************************************************************/
void menu_add_slave(void)
{
	int msg_key;
	BaseType_t	ret;
	static char strSlaveNumTemp[6];
	static int CharInx_Num;
	static unsigned char LCD_Fresh;
	static unsigned char NumberFlash;
	Msg_SecondFlash_t	MsgSecFlash;
	static char numTemp = 0;
	Msg_VaildFrame_t ValidFrame;
	Bk_Cmd_t myBkCmd;
	Msg_RfCtrl_t msgRfCtrl;
	Bk_CmdReq_t mCmdReq; 
	static myInt5CharInx_t addNumName_Temp;
	unsigned char flagSidRegister = 0;
	char strRegisterResult[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			addNumName_Temp.mChar.indx0 = getCharIndex('0');			// 保存分机编号的字符索引
			addNumName_Temp.mChar.indx1 = getCharIndex('0');
			addNumName_Temp.mChar.indx2 = getCharIndex('0');
			addNumName_Temp.mChar.indx3 = getCharIndex('0');
			addNumName_Temp.mChar.indx4 = getCharIndex('1');
			cursorMenu = 4;
			CharInx_Num = addNumName_Temp.mChar.indx4;
			LCD_Fresh = 0;
			OLED_Clear();
			strSlaveNumTemp[0] = Char_SlaveName[addNumName_Temp.mChar.indx0];
			strSlaveNumTemp[1] = Char_SlaveName[addNumName_Temp.mChar.indx1];
			strSlaveNumTemp[2] = Char_SlaveName[addNumName_Temp.mChar.indx2];
			strSlaveNumTemp[3] = Char_SlaveName[addNumName_Temp.mChar.indx3];
			strSlaveNumTemp[4] = Char_SlaveName[addNumName_Temp.mChar.indx4];
			OLED_ShowString(34,4,(unsigned char*)strSlaveNumTemp,24,1);
			OLED_Refresh();
			ret = xQueueReset(mySecFlashQueue);
			numTemp = strSlaveNumTemp[cursorMenu];
			// 查询当前号码是否已经注册过分机?
			// 发送查询命令
			myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// 通知查询任务准备
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));				// 发通知
			myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// 发送查询SNUM命令
			myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));				// 将SID发给查询任务
			// 等待查询结果
			flagSidRegister =0;														// 此号码未注册
			ret = xQueueReceive(myCmdReqQue,&mCmdReq,pdMS_TO_TICKS(1000));				// 接收查询结果
			if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM已经注册过	
			{
				flagSidRegister =2;													// 号码已注册	
			}
			sprintf(strRegisterResult,"-%d",flagSidRegister);
			OLED_ShowString(112,8,(unsigned char*)strRegisterResult,16,1);
			OLED_Refresh();
			
			msgRfCtrl = MSG_RF_TURN_ON;														// 开启RF接收
			ret = xQueueSend(myRfCtrlQueue,&msgRfCtrl,0);
			RF_module_goto_rx();
			
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					//-------------------- 按键处理 ---------------------------
					case MSG_KEY_ENT_S_TOUCH:												// 修改字符
					{
						CharInx_Num++;														// 字符索引加1
						if(CharInx_Num>INDX_SLAVE_NAME_CHR_MAX)	CharInx_Num = 0;
						strSlaveNumTemp[cursorMenu] = Char_SlaveName[CharInx_Num];			// 更新字符
						numTemp = strSlaveNumTemp[cursorMenu];
						if(0==cursorMenu)
						{
							addNumName_Temp.mChar.indx0 = CharInx_Num;
						}
						else if(1==cursorMenu)
						{
							addNumName_Temp.mChar.indx1 = CharInx_Num;
						}
						else if(2==cursorMenu)
						{
							addNumName_Temp.mChar.indx2 = CharInx_Num;
						}
						else if(3==cursorMenu)
						{
							addNumName_Temp.mChar.indx3 = CharInx_Num;
						}
						else if(4==cursorMenu)
						{
							addNumName_Temp.mChar.indx4 = CharInx_Num;
						}
						else
						{
							cursorMenu = 0;
						}
						ret = xQueueReset(myDecodeQueue);									// 清空解码消息
						
						strSlaveNumTemp[0] = Char_SlaveName[addNumName_Temp.mChar.indx0];
						strSlaveNumTemp[1] = Char_SlaveName[addNumName_Temp.mChar.indx1];
						strSlaveNumTemp[2] = Char_SlaveName[addNumName_Temp.mChar.indx2];
						strSlaveNumTemp[3] = Char_SlaveName[addNumName_Temp.mChar.indx3];
						strSlaveNumTemp[4] = Char_SlaveName[addNumName_Temp.mChar.indx4];
						
						NumberFlash = 0;
						LCD_Fresh = 1;
						break;
					}
					//--------------------- 按键处理 ------------------------
					case MSG_KEY_FUN_S_TOUCH:												// 切换分机编号位选
					{
						strSlaveNumTemp[cursorMenu] = numTemp;
						if(cursorMenu)	cursorMenu--;
						else			cursorMenu = 4;
						if(0==cursorMenu)
						{
							CharInx_Num = addNumName_Temp.mChar.indx0;
						}
						else if(1==cursorMenu)
						{
							CharInx_Num = addNumName_Temp.mChar.indx1;
						}
						else if(2==cursorMenu)
						{
							CharInx_Num = addNumName_Temp.mChar.indx2;
						}
						else if(3==cursorMenu)
						{
							CharInx_Num = addNumName_Temp.mChar.indx3;
						}
						else if(4==cursorMenu)
						{
							CharInx_Num = addNumName_Temp.mChar.indx4;
						}
						else
						{
							cursorMenu = 0;
							CharInx_Num = addNumName_Temp.mChar.indx0;
						}
						if(0>CharInx_Num)	CharInx_Num = 0;					
						numTemp = strSlaveNumTemp[cursorMenu];

						strSlaveNumTemp[0] = Char_SlaveName[addNumName_Temp.mChar.indx0];
						strSlaveNumTemp[1] = Char_SlaveName[addNumName_Temp.mChar.indx1];
						strSlaveNumTemp[2] = Char_SlaveName[addNumName_Temp.mChar.indx2];
						strSlaveNumTemp[3] = Char_SlaveName[addNumName_Temp.mChar.indx3];
						strSlaveNumTemp[4] = Char_SlaveName[addNumName_Temp.mChar.indx4];
						
						LCD_Fresh = 1;
						break;
					}
					//--------------------- 按键处理 ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);											// 返回
						break;
					}
					default:
						break;
				}
			}

			// 秒闪 - 设置字符
			ret = xQueueReceive(mySecFlashQueue,&MsgSecFlash,0);
			if(pdTRUE==ret)
			{
				// 字符闪烁
				if(NumberFlash)																// 灭
				{
					NumberFlash = 0;
					strSlaveNumTemp[cursorMenu] = ' ';
				}
				else
				{
					NumberFlash = 1;
					strSlaveNumTemp[cursorMenu] = numTemp;
				}
				
				LCD_Fresh = 1;
			}
						
			// 接收分机信号
			ret = xQueueReceive(myDecodeQueue,&ValidFrame,0);								// 收到有效数据帧
			{
				if(pdTRUE==ret)																// 接收到新数据帧
				{
					// 发送查询SID命令
					myBkCmd.cmd = BK_CMD_SEARCH_SID_PREPARE;								// 通知查询任务准备
					ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 发通知
					myBkCmd.cmd = BK_CMD_SEARCH_SID;										// 发送查询数据
					myBkCmd.SlaveID.Integer = ValidFrame.value&0xfffff0;					// 屏蔽键值
					ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 将SID发给查询任务

					// 等待查询结果
					ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// 接收查询结果
					if(BK_CMD_SEARCH_SID_OK==mCmdReq.cmd)									// SID已经注册过	(分机ID已经注册过)					
					{
						if(addNumName_Temp.Integer==mCmdReq.SlaveName.Integer)					// 当前分机已经注册到当前号码下
						{
							flagSidRegister =3;												// 当前分机已经注册到当号码下
						}
						else																// SID注册过，但未注册到当前号码下
						{
							// 查询当前号码SNUM是否注册过？
							// 发送查询命令
							myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// 通知查询任务准备
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 发通知
							myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// 发送查询SNUM命令
							myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 将SID发给查询任务
							// 等待查询结果
							ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// 接收查询结果
							if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM已经注册过	
							{
								flagSidRegister =4;													// 分机和号码都和其它号码和分机注册过								
							}
							else
							{
								flagSidRegister =1;													// 分机注册过，号码未注册						
							}
						}
					}
					else																			// SID未注册过
					{
						// 查询当前号码SNUM是否注册过？
						// 发送查询命令
						myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// 通知查询任务准备
						ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 发通知
						myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// 发送查询SNUM命令
						myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
						ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 将SID发给查询任务
						// 等待查询结果
						ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// 接收查询结果
						if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM已经注册过	
						{
							flagSidRegister =2;													// 号码已注册,呼叫器未注册过	
						}
						else
						{
							// 号码和分机均未注册，马上进行注册
							myBkCmd.cmd = BK_CMD_INSERT_PREPARE;								// 通知注册任务准备
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 发通知
							myBkCmd.cmd = BK_CMD_INSERT_SID;										// 插入命令
							myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
							myBkCmd.SlaveID.Integer = ValidFrame.value&0xfffff0;			// 屏蔽键值
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 将SID发给查询任务
							// 等待查询结果
							ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// 接收查询结果
							if(BK_CMD_INSERT_SID_OK==mCmdReq.cmd)									// SNUM已经注册过	
							{
								flagSidRegister =3;													// 号码已注册	
							}
							else
							{
								flagSidRegister =5;													// 注册失败说明空间已满
							}
						}
					}
					
					sprintf(strRegisterResult,"-%d",flagSidRegister);								// 刷注册结果
					OLED_ShowString(112,8,(unsigned char*)strRegisterResult,16,1);
					LCD_Fresh = 1;
				}
			}

			if(LCD_Fresh)
			{
				LCD_Fresh = 0;
				OLED_ShowString(34,4,(unsigned char*)strSlaveNumTemp,24,1);
				OLED_Refresh();
			}
			break;
		}
		default:
			break;
	}
}


/*****************************************************************************************
Function: 
Description: 
*****************************************************************************************/
void menu_alert_time(void)
{
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	自动清除时间设置
*****************************************************************************************/
void menu_auto_clean(void)
{
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_Clear();
			sprintf(stringTemp,"%02d",AutoCleanTime);
			OLED_ShowString(52,4,(unsigned char*)stringTemp,24,1);
			OLED_Refresh();
			
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					//-------------------- 按键处理 ---------------------------
					case MSG_KEY_ENT_S_TOUCH:
					{
						AutoCleanTime++;
						if(AUTO_CLEAN_TIME_MAX<=AutoCleanTime)
						{
							AutoCleanTime = AUTO_CLEAN_TIME_MIN;
						}
						sprintf(stringTemp,"%02d",AutoCleanTime);
						OLED_ShowString(52,4,(unsigned char*)stringTemp,24,1);
						OLED_Refresh();
						break;
					}
					//--------------------- 按键处理 ------------------------
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					//--------------------- 按键处理 ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);							// 切换到
						break;
					}
					//--------------------- 按键处理 ------------------------
					case MSG_KEY_FUN_L_TOUCH:
					{
						break;
					//--------------------- 按键处理 -------------------------
					}
					default:
						break;
				}
			}
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	删除分机号码
*****************************************************************************************/
void menu_slave_delete(void)
{
	int msg_key;
	BaseType_t	ret;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			break;
		}
		default:
			break;
	}
}


/*****************************************************************************************
Function: 
Description: 
	日期设置界面
*****************************************************************************************/
void menu_date_time(void)
{
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			break;
		}
		default:
			break;
	}
}


/*****************************************************************************************
Function: 
Description: 
			F06 - 设置号码显示格式
*****************************************************************************************/
void menu_number_style(void)
{
	int msg_key;
	BaseType_t	ret;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_Clear();
			if(NUM_STYLE_0==NumDisStyle)
			{
				OLED_ShowString(28,4,(unsigned char*)"style0",24,1);
			}
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:
					{
						if(NUM_STYLE_0==NumDisStyle)
						{
							NumDisStyle = NUM_STYLE_1;
							OLED_ShowString(28,4,(unsigned char*)"style1",24,1);
						}
						else
						{
							NumDisStyle = NUM_STYLE_0;
							OLED_ShowString(28,4,(unsigned char*)"style0",24,1);
						}
						OLED_Refresh();
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);							// 返回
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:
					{
						break;
					}
					default:
						break;
				}
			}			
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	密码设置界面,有效密码为181
*****************************************************************************************/
void menu_password(void)
{
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	闹铃时间设置.
*****************************************************************************************/
void menu_alarm_time(void)
{	
	int msg_key;
	BaseType_t	ret;
	char stringTemp2[6] = {0};
	char stringTemp3[4] = {0};
	char stringTemp4[3] = {0};
	static myAlarm_t alarmTemp;
	static unsigned char alarm_num;
	unsigned char LCD_Fresh = 0;
	Msg_SecondFlash_t	MsgSecFlash;
	static unsigned char alarmItemFlash = 1;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_Clear();
			alarm_num = 1;
			alarmTemp.time.hour = myAlarm1.time.hour;
			alarmTemp.time.minute = myAlarm1.time.minute;
			sprintf(stringTemp2,"%02d:%02d",alarmTemp.time.hour,alarmTemp.time.minute);
			sprintf(stringTemp3,"%02d:",alarm_num);
			OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1);					// 编号
			OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1);				// 时间
			OLED_Refresh();
			
			cursorMenu = CUSOR_ALARM_MINUTE;
			alarmItemFlash = 0;
			ret = xQueueReset(mySecFlashQueue);
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:										// 调整数字
					{
						switch(cursorMenu)
						{
							case CUSOR_ALARM_MINUTE:
							{
								if(++alarmTemp.time.minute>=60)	alarmTemp.time.minute = 0;
								break;
							}
							case CUSOR_ALARM_HOUR:
							{
								if(++alarmTemp.time.hour>=60)	alarmTemp.time.hour= 0;
								break;
							}
							default:
							{
								break;
							}
						}
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:												// 切换光标位置
					{
						if(++cursorMenu>CUSOR_ALARM_HOUR)	cursorMenu = CUSOR_ALARM_MINUTE;
						sprintf(stringTemp2,"%02d:%02d",alarmTemp.time.hour,alarmTemp.time.minute);
						sprintf(stringTemp3,"%02d:",alarm_num);
						OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1); 				// 编号 - 16x8字体
						OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1); 			// 时间
						LCD_Fresh = 1;
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:												// 返回时要保存数据
					{
						if(1==alarm_num)
						{
							//===========================================================
													myAlarm1.en = 1;
							//===========================================================
							myAlarm1.time.hour = alarmTemp.time.hour;
							myAlarm1.time.minute = alarmTemp.time.minute;
							//myAlarmTimer_time_set(alarm_num, myAlarm1.time.hour, myAlarm1.time.minute);	// 将小时和分钟转成RTC计数值
						}
						else
						{
							//===========================================================
													myAlarm2.en = 1;
							//===========================================================
							myAlarm2.time.hour = alarmTemp.time.hour;
							myAlarm2.time.minute = alarmTemp.time.minute;
							//myAlarmTimer_time_set(alarm_num, myAlarm2.time.hour, myAlarm2.time.minute);
						}


						get_valid_rtc_count_of_alarmTimer(alarm_num);
						load_new_alarm_cnt_to_rtc();
						switch_menu(MENU_SET_ITEM);							// 返回
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:								// 长按FUN键切换闹铃,切换时先保存设置值
					{
						if(1==alarm_num)	
						{
							//===========================================================
													myAlarm1.en = 1;
							//===========================================================
							myAlarm1.time.hour = alarmTemp.time.hour;
							myAlarm1.time.minute = alarmTemp.time.minute;
							get_valid_rtc_count_of_alarmTimer(1);
							load_new_alarm_cnt_to_rtc();
							
							alarm_num = 2;
							alarmTemp.time.hour = myAlarm2.time.hour;
							alarmTemp.time.minute = myAlarm2.time.minute;
							sprintf(stringTemp2,"%02d:%02d",alarmTemp.time.hour,alarmTemp.time.minute);
							sprintf(stringTemp3,"%02d:",alarm_num);
						}
						else				
						{
							//===========================================================
													myAlarm2.en = 1;
							//===========================================================
							myAlarm2.time.hour = alarmTemp.time.hour;
							myAlarm2.time.minute = alarmTemp.time.minute;
							get_valid_rtc_count_of_alarmTimer(2);
							load_new_alarm_cnt_to_rtc();
							
							alarm_num = 1;
							alarmTemp.time.hour = myAlarm1.time.hour;
							alarmTemp.time.minute = myAlarm1.time.minute;
							sprintf(stringTemp2,"%02d:%02d",alarmTemp.time.hour,alarmTemp.time.minute);
							sprintf(stringTemp3,"%02d:",alarm_num);
						}
						// 更新时钟缓冲
						OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1);				// 编号
						OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1); 			// 时间
						LCD_Fresh = 1;
						break;
					}
					default:
						break;
				}
			}
			
			ret = xQueueReceive(mySecFlashQueue,&MsgSecFlash,0);
			if(pdTRUE==ret)
			{
				// 字符闪烁
				if(alarmItemFlash)	alarmItemFlash = 0;											// 灭
				else	alarmItemFlash = 1;

				switch(cursorMenu)
				{
					case CUSOR_ALARM_MINUTE:
					{
						if(alarmItemFlash)														// 亮
						{
							sprintf(stringTemp4,"%02d",alarmTemp.time.minute);
							OLED_ShowString(24+8+3*16,0,(unsigned char*)stringTemp4,32,1); 			// 分钟
						}
						else																	// 灭
						{
							OLED_ShowString(24+8+3*16,0,(unsigned char*)"  ",32,1); 			// // 分钟
						}
						break;
					}
					case CUSOR_ALARM_HOUR:
					{
						if(alarmItemFlash)														// 亮
						{
							sprintf(stringTemp4,"%02d",alarmTemp.time.hour);
							OLED_ShowString(24+8+0*8,0,(unsigned char*)stringTemp4,32,1); 			// 小时
						}
						else																	// 灭
						{
							OLED_ShowString(24+8+0*8,0,(unsigned char*)"  ",32,1); 			// 小时
						}
						break;
					}
					default:
					{
						break;
					}
				}
				
				LCD_Fresh = 1;
			}

			if(LCD_Fresh)	{LCD_Fresh = 0;OLED_Refresh();}			
			break;
		}
		default:
		{
			break;
		}
	}
}


/*****************************************************************************************
Function: 
Description: 
	闹铃开/关控制界面
*****************************************************************************************/
void menu_alarm_switch(void)
{
	int msg_key;
	BaseType_t	ret;
	char stringTemp[3] = {0};

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_Clear();
			if(EN_STATUS==Alarm1_Status)
			{
				OLED_ShowString(20,8,(unsigned char*)"1:ON",16,1);
			}
			else
			{
				OLED_ShowString(20,8,(unsigned char*)"1:OFF",16,1);
			}
			
			if(EN_STATUS==Alarm1_Status)
			{
				OLED_ShowString(68,8,(unsigned char*)"2:ON",16,1);
			}
			else
			{
				OLED_ShowString(68,8,(unsigned char*)"2:OFF",16,1);
			}
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:
					{
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);							// 返回
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:
					{
						break;
					}
					default:
						break;
				}
			}				
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	显示软件版本号
*****************************************************************************************/
void menu_mac_verion(void)
{
	int msg_key;
	BaseType_t	ret;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_Clear();
			OLED_ShowString(34,4,(unsigned char*)mac_version,24,1);
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				if(MSG_KEY_ENT_L_TOUCH==msg_key)
				{
					switch_menu(MENU_SET_ITEM);							// 返回
				}
			}
			break;
		}
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	测试界面
*****************************************************************************************/
void menu_rtc_test(void)
{

}

/*****************************************************************************************
Function: 
Description: 
	界面总表
*****************************************************************************************/
void menu_main(void)
{
	switch(MenuIndex)
	{
		case MENU_TURN_OFF:		{ menu_off(); break;	}
		case MENU_STANDBY:		{ menu_standby(); break;}									// 待机界面
		case MENU_NOFITY:		{ menu_notify(); break; }									// 呼叫提醒界面
		case MENU_SET_ITEM:		{ menu_setItem_select(); break;}
		case MENU_ADD_SLAVE:	{ menu_add_slave(); break;}									// 添加分机
		case MENU_ALERT_TIME:	{ menu_alert_time(); break;}								// 修改提醒时间
		case MENU_AUTO_CLEAN:	{ menu_auto_clean(); break;}								// 自动清除时间
		case MENU_SLAVE_DELETE:	{ menu_slave_delete(); break;}								// 删除分机
		case MENU_DATE_TIME:	{ menu_date_time(); break;}									// 日期和时间
		case MENU_NUMBER_STYLE:	{ menu_number_style(); break;}								// 号码显示格式
		case MENU_PASSWORD:		{ menu_password(); break;}									// 密码设置
		case MENU_ALARM_TIME:	{ menu_alarm_time(); break;}								// 闹铃时间设置
		case MENU_ALARM_SWITCH:	{ menu_alarm_switch(); break;}								// 闹铃开关
		case MENU_MAC_VERSION:	{ menu_mac_verion(); break;}
		case MENU_RTC_TEST:		{ menu_rtc_test(); break;}									// RTC测试
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	查看当前是否在省电状态
	return - the satus of the pwower save(1-power save,0-normal)
*****************************************************************************************/
int check_powerSave_status(void)
{
	return(FG_PowerSave);
}

/*****************************************************************************************
Function: 
Description: 
	设置省电模式标志
	Flag - the value which will be set
*****************************************************************************************/
void powerSave_flag_set(unsigned char Flag)
{
	FG_PowerSave = Flag;
}

/*****************************************************************************************
Function: 
Description: 
	复位进入省电模械的倒计时计数器
*****************************************************************************************/
void reset_powerSave_counter(void)
{
	AutoDisplayOffCnt = TIME_AUTO_POW_SAVE;
}

/*****************************************************************************************
Function: 
Description: 
	判断进入省电模式时间到否?
*****************************************************************************************/
int is_powerSave_time(void)
{
	return(AutoDisplayOffCnt>0?0:1);
}

/*****************************************************************************************
Function: 
Description: 
	省电模式倒计时计数值更新	
*****************************************************************************************/
void auto_powerSave_count(void)
{
	if(AutoDisplayOffCnt)		AutoDisplayOffCnt--;
}

/*****************************************************************************************
Function: 
Description: 
	界面任务初始化
*****************************************************************************************/
void menu_init(void)
{
	MenuIndex = MENU_TURN_OFF;
	OLED_Init();
	OLED_ColorTurn(0);									//0正常显示，1 反色显示
	OLED_Refresh();
	OLED_DisPlay_Off();									// 关闭OLED
	ClockColonFlash_timer_init();						// 开启秒闪定时
}


/*****************************************************************************************
Function: 
Description: 
	界面调试函数
*****************************************************************************************/
void MenuTask(void * pvParameters)
{
	vTaskDelay(200);									// 延时等待低优先级任务完成初始化动作。
	power_save_init();
	menu_init();
	switch_menu(MENU_TURN_OFF);
	vTaskDelay(20);
	
	while(1)
	{
		menu_main();									// 界面逻辑
		auto_powerSave_count();							// 自动熄屏计时
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}
}





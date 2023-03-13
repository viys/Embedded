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
unsigned char SettingItem = SET_ITEM_ADD_SLAVE;								// Ĭ�Ͻ�������ʱѡ���һ��
unsigned char preSettingItem = SET_ITEM_ADD_SLAVE;							// Ĭ�Ͻ�������ʱѡ���һ��

static unsigned cursorMenu = 0;												// ���
unsigned char myMenuStageCnt = 0;											// ������ʾ����
unsigned short int SlaveCount = 0;

unsigned char NumDisStyle = NUM_STYLE_0;									// �ֻ�������ʾ��ʽ
unsigned char Alarm1_Status = DIS_STATUS;									// ���忪�ر�־
unsigned char Alarm2_Status = DIS_STATUS;									// ���忪�ر�־

unsigned char AutoCleanTime = AUTO_CLEAN_TIME_MIN;
unsigned int AutoStandbyCnt = 0;											// �Զ����������ʱ
unsigned int AutoDisplayOffCnt = 0;											// �Զ�Ϩ����ʱ
unsigned char FG_PowerSave = 0;												// ʡ���־

myWaitNumQue_t	myWaitNumQue = {0};

const char mac_version[] = "V1.0";
static unsigned char stage_powSave = STAGE_STANDBY_RX_START;				// ����״̬��־



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
	
	myRTC_APB_sync();											// RTC��APBͬ��
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
	��������ǰ���ж�Ҫ��ʲôֵд��RTC����������С�
	�жϵĶ��������ϵͳ�����жϣ��ֻ����壬RTC���껽���ж�
*****************************************************************************************/
void enter_sleep_rtc_prepare(void)
{
	
}

/*****************************************************************************************
Function: 
Description: 
	�ػ����棬����ػ������MCU��LCD��RF����������״̬��
*****************************************************************************************/
void menu_off(void)
{
	Msg_RTC_t	MsgRTC;
	int msg_key;
	BaseType_t	ret;
	Msg_RfCtrl_t msgRfCtrl;
	static unsigned char Time_Sleep_Cnt = 0;									// �ػ�ʱ����˯��״̬��ʱ����
	static unsigned char Flag_KeyWait = 0;

	switch(myMenuStageCnt)
	{
		case MENU_STAGE_INIT:
		{
			OLED_DisPlay_Off();
			RF_module_sleep();
			//ClockColonFlash_timer_stop();
			msgRfCtrl = MSG_RF_TURN_OFF;										// �ر�RF����
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
			if(pdTRUE==ret)														// �а���
			{
				if(MSG_KEY_ENT_TOUCHING==msg_key)								// ȷ�ϼ����ڰ���
				{
					Flag_KeyWait = 1;
					Time_Sleep_Cnt = 0;
				}
				else if(MSG_KEY_ENT_L_TOUCH==msg_key)
				{
					// ����RTC����
					ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
					if(pdTRUE==ret)
					{
						if(MSG_RTC_SECOND_INT==MsgRTC.type)
						{
							get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
						}
					}
					Time_Sleep_Cnt = 0;
					switch_menu(MENU_STANDBY);										// �л�����������
				}		
			}

			// is the time to enter sleep mode?
			if(Time_Sleep_Cnt>(Flag_KeyWait?TIME_ENTER_SLP1:TIME_ENTER_SLP2))		// ���ENT���а��¶���,����˯�ߵĵȴ�ʱ���ӳ�
			{
				ret = xQueueReset(myKeyQueue);
				OLED_DisPlay_Off();													// ��ʾ�ر�
				RF_module_sleep();													// RF˯��
				mcu_goto_deepsleep();												// ��Ƭ������˯��״̬
				
				Time_Sleep_Cnt = 0;													// �ٴα�����
				Flag_KeyWait = 0;
				taskENTER_CRITICAL();
				recover_sys_clock();												// �ָ�ϵͳʱ��
				taskEXIT_CRITICAL();
			}
			
			// �ֻ����峬ʱ,�Զ������������
			if(0==is_myAlarmTimer_timeout(1)||0==is_myAlarmTimer_timeout(2))
			{
				// ����RTC����
				ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
				if(pdTRUE==ret)
				{
					if(MSG_RTC_SECOND_INT==MsgRTC.type)
					{
						get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
					}
				}
				Time_Sleep_Cnt = 0;
				switch_menu(MENU_STANDBY);											// �л�����������
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
	�޿������棬�������������
*****************************************************************************************/
void menu_powerOn(void)
{

}

/*****************************************************************************************
Function: 
Description: 
	RF�͹���ģʽ���� 	
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
		// ***************** RF���տ�ʼ,�����ʱ��Ϊ300MS **************
		case STAGE_STANDBY_RX_START:	// ��ʼ����
		{
			RF_module_goto_rx();
			rf_decode_control(1);
			ret = xQueueReset(myRfObserverQue); 										// RF�����Ϣ���
			ret = xQueueReset(myRfFrameQueue);											// ���RF֡��Ϣ
			rf_decode_success = 0;
			PreTicks_Sty = xTaskGetTickCount(); 										// ���浱ǰϵͳ�δ�
			stage_powSave = STAGE_STANDBY_RX_BAD_SIG;
			break;
		}
		// ******************* 5MS�����Ч�ź����̽������� *****************
		case STAGE_STANDBY_RX_BAD_SIG:
		{
			ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// ����RF�����Ϣ
			if(pdTRUE==ret)
			{
				if(RF_SIG_BAD==signalQuality)											// ��⵽��Ч�ź�-��ǰΪ�������������
				{
					stage_powSave = STAGE_STANDBY_SLEEP;
					break;
				}
				else																	// ��⵽��Ч�ź�-��Ҫ��ʱ����
				{
					ResetBadSignalCnt();														// ��Ч�źż�����0
					stage_powSave = STAGE_STANDBY_RX_APPEND_DELAY;
					break;
				}
			}

			// 5ms��ʱδ��⵽��Ч�źţ�ת����Ч�ź����
			if( get_sysTicks_interval(xTaskGetTickCount(),PreTicks_Sty)>=pdMS_TO_TICKS(5) )
			{
				ResetBadSignalCnt();														// ��Ч�źż�����0
				stage_powSave = STAGE_STANDBY_RX_VALID_SIG;
			}
			break;
		}
		// ******************* ǰ5MS��δ��⵽��Ч�źţ���ʱ������Ч�ź�35ms *****************
		case STAGE_STANDBY_RX_VALID_SIG:
		{
			ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// ����RF�����Ϣ
			if(pdTRUE==ret)
			{
				if(RF_SIG_BAD==signalQuality)											// ��⵽��Ч�ź�-��ǰΪ�������������
				{
					stage_powSave = STAGE_STANDBY_SLEEP;
					break;
				}
				else																	// ��⵽��Ч�ź�-��Ҫ��ʱ����
				{
					ResetBadSignalCnt();														// ��Ч�źż�����0
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
		// ******************* ��⵽��Ч�źź���ʱ������Ч֡260ms *****************
		case STAGE_STANDBY_RX_APPEND_DELAY:
		{
			ret = xQueueReceive(myDecodeQueue,&stbyRxFrame,0);							// �յ�����ɹ�������֡
			if(pdTRUE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SID_SEARCH;
				break;
			}
			else																		// ��ʱ�ж�
			{
				ret = xQueueReceive(myRfObserverQue,&signalQuality,0);						// ����RF�����Ϣ
				if(pdTRUE==ret)
				{
					if(RF_SIG_BAD==signalQuality)											// ��⵽��Ч�ź�-��ǰΪ�������������
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
		// *****************// Ϩ��˯��200MS**************
		case STAGE_STANDBY_SLEEP:	// Ϩ��˯��200MS
		{
			// �ر�RF,�ر���ʾ
			rf_decode_control(0);
			RF_module_sleep();
			// ����RTC���壬��������ʱ��120ms
			if(rf_decode_success)	myPowerSaveTimer_time_set(1500);						// ���õ͹���ʱ��
			else	myPowerSaveTimer_time_set(200);
			get_valid_rtc_count_of_powerSaveAlarm();									// ����ALARM����ֵ
			load_new_alarm_cnt_to_rtc();												// ����������ֵ����Ȼ�������ֵд��RTC
			// ����
			if(1==check_powerSave_status())												// enter power save mode or not?
			{
				mcu_goto_deepsleep();													// ��Ƭ������˯��״̬
				// �˳�˯��״̬
				taskENTER_CRITICAL();
				recover_sys_clock();												// �ָ�ϵͳʱ��
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
		// ******************* �ȴ���ʱ *****************
		case STAGE_STANDBY_SLP_WAIT:
		{
			// �ȴ�ALARM�ж�
			if(0==is_powerSave_alarm_timeout()) 										// �ж������Ƿ�ʱ
			{
				diasble_powerSave_alarm();
				clean_powerSave_alarm_timeout_flag();
				RF_module_goto_rx();
				stage_powSave = STAGE_STANDBY_RX_START;
			}
			break;
		}
		// ******************* ���յ���Ч����֡������ID�Ƿ�ע��� *****************
		case STAGE_STANDBY_SID_SEARCH:
		{
			rf_decode_control(0);
			RF_module_sleep();
			myBkCmd.cmd = BK_CMD_SEARCH_SID_PREPARE;
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));			// ��������׼������
			if(pdFALSE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SLEEP;
				break;
			}
			myBkCmd.cmd = BK_CMD_SEARCH_SID;
			myBkCmd.SlaveID.Integer = stbyRxFrame.value&0xfffff0;					// ���μ�ֵ
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));			// �������������SID
			if(pdFALSE==ret)
			{
				rf_decode_success = 1;
				stage_powSave = STAGE_STANDBY_SLEEP;
				break;
			}
			ret = xQueueReceive(myCmdReqQue,&mCmdReq,pdMS_TO_TICKS(1000));			// ���ز�ѯ���
			if(pdTRUE==ret)
			{
				if(BK_CMD_SEARCH_SID_OK==mCmdReq.cmd)								// �Ѿ�ע���
				{
					// �л�������״̬
					myWaitNumQue.WaitNumArry[myWaitNumQue.Tail].WaitNum = mCmdReq.SlaveName.Integer;
					myWaitNumQue.WaitNumArry[myWaitNumQue.Tail].KeyValue = myBkCmd.SlaveID.Integer&0x0f;
					myWaitNumQue.Amount++;
					if(myWaitNumQue.Amount>NUM_WAIT_NUM_QUE)
					{
						myWaitNumQue.Amount = NUM_WAIT_NUM_QUE;
					}
					myWaitNumQue.Tail = (myWaitNumQue.Tail+1)%NUM_WAIT_NUM_QUE;
					switch_menu(MENU_NOFITY);										// �������ѽ���
					break;
				}
			}
			
			rf_decode_success = 1;
			stage_powSave = STAGE_STANDBY_SLEEP;
			break;
		}
		// ******************* ���гɹ������2S *****************
		case STAGE_STANDBY_RF_OFF_DELAY:
		{
			
			break;
		}
		// ******************* Ĭ��״̬ *****************
		default:
		{
			break;
		}
	}
}


/*****************************************************************************************
Function: 
Description: 
				��������
	���������,����ʱ����Ϩ��ʡ��ģʽ����MCU��������״̬��
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
		//--------------------------------- �������绯 --------------------------------------------
		case MENU_STAGE_INIT:
		{
			OLED_DisPlay_On();
			OLED_Clear();			
			
			// ����RTC����
			ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
			if(pdTRUE==ret)
			{
				if(MSG_RTC_SECOND_INT==MsgRTC.type)
				{
					get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
				}
			}
			
			sprintf( str_temp, "%02d/%02d", monthRtc,dayRtc );						// ����
			sprintf( str_hour, "%02d", hourRtc );									// Сʱ
			sprintf( str_minute, "%02d", minuteRtc );								// ��
			
			OLED_ShowString(2,0,(unsigned char*)str_hour,32,1);						// ˢ-Сʱ
			OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// ˢ-:
			OLED_ShowString(2+32/2*3,0,(unsigned char*)str_minute,32,1);				// ˢ-����
			
			OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1);			// ˢ����ͼ��
			OLED_ShowPicture(114,2,13,7,(unsigned char*)BMP_bat_13x7,1);			// ���ͼ��
			
			OLED_ShowString(98,19,(unsigned char*)str_temp,12,1);
			OLED_Refresh();

			//ClockColonFlash_timer_init();											// ����������ʱ
			//ClockColonFlash_timer_reset();

			msgRfCtrl = MSG_RF_TURN_ON;												// ����RF����
			ret = xQueueSend(myRfCtrlQueue,&msgRfCtrl,0);
			ret = xQueueReset(myRtcQueue);											// ������ж���Ϣ
			ret = xQueueReset(mySecFlashQueue);

			reset_powerSave_counter();												// ��ֹ����ʡ��״̬
			powerSave_flag_set(0);

			stage_powSave = STAGE_STANDBY_RX_START;
			
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		//--------------------------------- �������� --------------------------------------------
		case MENU_STAGE_RUN:
		{
			// �ж��Ƿ����͹��ģ�
			if( is_powerSave_time() )
			{
				powerSave_flag_set(1);
				LCD_Fresh = 1;
			}
			
			// ����RTC����
			ret = xQueueReceive(myRtcQueue,&MsgRTC,0);
			if(pdTRUE==ret)
			{
				if(MSG_RTC_SECOND_INT==MsgRTC.type)
				{
					get_calender_from_rtcCount(MsgRTC.rtcCount,&yearRtc,&monthRtc,&dayRtc,&hourRtc,&minuteRtc,&secondRtc,&weekRtc);
					
					sprintf( str_temp, "%02d/%02d", monthRtc,dayRtc );						// ����
					sprintf( str_hour, "%02d", hourRtc );									// Сʱ
					sprintf( str_minute, "%02d", minuteRtc );								// ��
					
					OLED_ShowString(2,0,(unsigned char*)str_hour,32,1);						// ˢ-Сʱ
					OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// ˢ-:
					OLED_ShowString(2+32/2*3,0,(unsigned char*)str_minute,32,1);				// ˢ-����
					OLED_ShowString(98,19,(unsigned char*)str_temp,12,1);					// ˢ����
					
					LCD_Fresh = 1;
				}
			}

			// �����ж�
			ret = xQueueReceive(mySecFlashQueue,&MsgSecFlash,0);
			if(pdTRUE==ret)
			{
				// �м�ʱ��
				if(Flag_Colon)
				{
					Flag_Colon = 0;
					OLED_ShowString(2+32/2*2,0,(unsigned char*)" ",32,1);					// ˢ-
				}
				else
				{
					Flag_Colon = 1;
					OLED_ShowString(2+32/2*2,0,(unsigned char*)":",32,1);					// ˢ-:
				}

				// ����ͼ��
				if(Flag_alarm_Tout)
				{
					if(Flag_alarm_icon_flash)
					{
						Flag_alarm_icon_flash = 0;
						OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_Null_10x12,1); 		// ˢ����ͼ��
					}
					else
					{
						Flag_alarm_icon_flash = 1;
						OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); 		// ˢ����ͼ��
					}
				}
				LCD_Fresh = 1;
			}

			// �������
			ret = xQueueReceive(myKeyQueue,&msg_key,0);
			if(pdTRUE==ret)
			{
				switch(msg_key)
				{
					case MSG_KEY_ENT_S_TOUCH:
					{
						if(1==Flag_alarm_Tout)												// ���峬ʱ
						{
							Flag_alarm_Tout = 0;
							OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); // ˢ����ͼ��
							LCD_Fresh = 1;
						}
						break;
					}
					case MSG_KEY_FUN_S_TOUCH:
					{
						if(1==Flag_alarm_Tout)												// ���峬ʱ
						{
							Flag_alarm_Tout = 0;
							OLED_ShowPicture(99,0,10,12,(unsigned char*)BMP_Alarm_10x12,1); // ˢ����ͼ��
							LCD_Fresh = 1;
						}
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_TURN_OFF);											// �л����ػ�����
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:
					{
						SettingItem = SET_ITEM_ADD_SLAVE;
						switch_menu(MENU_SET_ITEM);											// �������ý���
						break;
					}
					default:
						break;
				}
				
				reset_powerSave_counter();													// ��ֹ����ʡ��״̬
				powerSave_flag_set(0);
				LCD_Fresh = 1;
			}
			
			if(LCD_Fresh)																	// ˢ��LCD
			{
				LCD_Fresh = 0;
				if(check_powerSave_status())												// ʡ��״̬�¹ر���ʾ
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
			RF_Receive_LowPower_Runing();													// RF�͹�������

			// �ֻ����峬ʱ,����ͼ����˸
			if(0==is_myAlarmTimer_timeout(1)||0==is_myAlarmTimer_timeout(2))
			{
				clean_myAlarmTimer_timeout_flag(1);
				clean_myAlarmTimer_timeout_flag(2);
				Flag_alarm_Tout = 1;														// ����ͼ����˸
				Flag_alarm_icon_flash = 1;
				reset_powerSave_counter();													// ��ֹ����ʡ��״̬
				powerSave_flag_set(0);
			}

			// when external key wake up MCU
			if(0==is_key_wakeup_system())
			{
				is_key_wakeup_system();
				reset_powerSave_counter();													// ��ֹ����ʡ��״̬
				powerSave_flag_set(0);
			}
			break;
		}
		//--------------------------------- Ĭ����� --------------------------------------------
		default:
		{
			break;
		}
	}
}


/*****************************************************************************************
Function: 
Description: 
	�������ѽ��档��ʾ���к���
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
			// ȡ����һ���Ŷӵĺ���
			index = (myWaitNumQue.Tail+NUM_WAIT_NUM_QUE-myWaitNumQue.Amount)%NUM_WAIT_NUM_QUE;
			slaveNum.Integer = myWaitNumQue.WaitNumArry[index].WaitNum;
			keyValue = myWaitNumQue.WaitNumArry[index].KeyValue;
			
			// ȡ�����к�����ַ�
			strSlaveNum[0] = Char_SlaveName[slaveNum.mChar.indx0];
			strSlaveNum[1] = Char_SlaveName[slaveNum.mChar.indx1];
			strSlaveNum[2] = Char_SlaveName[slaveNum.mChar.indx2];
			strSlaveNum[3] = Char_SlaveName[slaveNum.mChar.indx3];
			strSlaveNum[4] = Char_SlaveName[slaveNum.mChar.indx4];
			
			// ��ʾ�ַ�
			OLED_Clear();
			OLED_ShowString(34,4,(unsigned char*)strSlaveNum,24,1);
			OLED_Refresh();
			myMenuStageCnt = MENU_STAGE_RUN;
			break;
		}
		case MENU_STAGE_RUN:
		{
			// �������
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
							switch_menu(MENU_STANDBY);									// �л�����������
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
						switch_menu(MENU_STANDBY);									// �л�����������
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
			
			// ˢ����ʾ
			if(fg_LedFresh)
			{
				fg_LedFresh = 0;
				
				// ȡ����һ���Ŷӵĺ���
				if(0<myWaitNumQue.Amount)
				{
					index = (myWaitNumQue.Tail+NUM_WAIT_NUM_QUE-myWaitNumQue.Amount)%NUM_WAIT_NUM_QUE;
					slaveNum.Integer = myWaitNumQue.WaitNumArry[index].WaitNum;
					keyValue = myWaitNumQue.WaitNumArry[index].KeyValue;
					
					// ȡ�����к�����ַ�
					strSlaveNum[0] = Char_SlaveName[slaveNum.mChar.indx0];
					strSlaveNum[1] = Char_SlaveName[slaveNum.mChar.indx1];
					strSlaveNum[2] = Char_SlaveName[slaveNum.mChar.indx2];
					strSlaveNum[3] = Char_SlaveName[slaveNum.mChar.indx3];
					strSlaveNum[4] = Char_SlaveName[slaveNum.mChar.indx4];
					
					// ��ʾ�ַ�
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
	���ù���ѡ�����
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
			msgRfCtrl = MSG_RF_TURN_OFF;												// �ر�RF����
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
					//-------------------- �������� ---------------------------
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
					//--------------------- �������� ------------------------
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					//--------------------- �������� ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_STANDBY);										// �л�����������
						break;
					}
					//--------------------- �������� ------------------------
					case MSG_KEY_FUN_L_TOUCH:
					{
						switch(SettingItem)
						{
							case SET_ITEM_ADD_SLAVE:
							{
								switch_menu(MENU_ADD_SLAVE);							// �л���
								break;
							}
							case SET_ITEM_ALERT_TIME:
							{
								switch_menu(MENU_ALERT_TIME);							// �л���
								break;
							}
							case SET_ITEM_AUTO_CLEAN:
							{
								switch_menu(MENU_AUTO_CLEAN);							// �л���
								break;
							}
							case SET_ITEM_SLAVE_DELETE:
							{
								switch_menu(MENU_SLAVE_DELETE);							// �л���
								break;
							}
							case SET_ITEM_DATE_TIME:
							{
								switch_menu(MENU_DATE_TIME);							// �л���
								break;
							}
							case SET_ITEM_NUMBER_STYLE:
							{
								switch_menu(MENU_NUMBER_STYLE);							// �л���
								break;
							}
							case SET_ITEM_PASSWORD:
							{
								switch_menu(MENU_PASSWORD);								// �л���
								break;
							}
							case SET_ITEM_ALARM:
							{
								switch_menu(MENU_ALARM_TIME);							// �л���
								break;
							}
							case SET_ITEM_ALARM_SWITCH:
							{
								switch_menu(MENU_ALARM_SWITCH);							// �л���
								break;
							}
							case SET_ITEM_VERSION:
							{
								switch_menu(MENU_MAC_VERSION);							// �л���
								break;
							}
							default:
								break;
						}
						break;
					//--------------------- �������� -------------------------
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
	��ӷֻ�����
	������ӷֻ������Ĭ����ʾ'00001'�ֻ���ţ�5λ�ֻ���ŵ�����ֵ�ᱻ�ϲ���һ��32λ���洢��FLASH�С�
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
			addNumName_Temp.mChar.indx0 = getCharIndex('0');			// ����ֻ���ŵ��ַ�����
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
			// ��ѯ��ǰ�����Ƿ��Ѿ�ע����ֻ�?
			// ���Ͳ�ѯ����
			myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// ֪ͨ��ѯ����׼��
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));				// ��֪ͨ
			myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// ���Ͳ�ѯSNUM����
			myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
			ret = xQueueSend(myBkCommandQue,&myBkCmd,pdMS_TO_TICKS(100));				// ��SID������ѯ����
			// �ȴ���ѯ���
			flagSidRegister =0;														// �˺���δע��
			ret = xQueueReceive(myCmdReqQue,&mCmdReq,pdMS_TO_TICKS(1000));				// ���ղ�ѯ���
			if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM�Ѿ�ע���	
			{
				flagSidRegister =2;													// ������ע��	
			}
			sprintf(strRegisterResult,"-%d",flagSidRegister);
			OLED_ShowString(112,8,(unsigned char*)strRegisterResult,16,1);
			OLED_Refresh();
			
			msgRfCtrl = MSG_RF_TURN_ON;														// ����RF����
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
					//-------------------- �������� ---------------------------
					case MSG_KEY_ENT_S_TOUCH:												// �޸��ַ�
					{
						CharInx_Num++;														// �ַ�������1
						if(CharInx_Num>INDX_SLAVE_NAME_CHR_MAX)	CharInx_Num = 0;
						strSlaveNumTemp[cursorMenu] = Char_SlaveName[CharInx_Num];			// �����ַ�
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
						ret = xQueueReset(myDecodeQueue);									// ��ս�����Ϣ
						
						strSlaveNumTemp[0] = Char_SlaveName[addNumName_Temp.mChar.indx0];
						strSlaveNumTemp[1] = Char_SlaveName[addNumName_Temp.mChar.indx1];
						strSlaveNumTemp[2] = Char_SlaveName[addNumName_Temp.mChar.indx2];
						strSlaveNumTemp[3] = Char_SlaveName[addNumName_Temp.mChar.indx3];
						strSlaveNumTemp[4] = Char_SlaveName[addNumName_Temp.mChar.indx4];
						
						NumberFlash = 0;
						LCD_Fresh = 1;
						break;
					}
					//--------------------- �������� ------------------------
					case MSG_KEY_FUN_S_TOUCH:												// �л��ֻ����λѡ
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
					//--------------------- �������� ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);											// ����
						break;
					}
					default:
						break;
				}
			}

			// ���� - �����ַ�
			ret = xQueueReceive(mySecFlashQueue,&MsgSecFlash,0);
			if(pdTRUE==ret)
			{
				// �ַ���˸
				if(NumberFlash)																// ��
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
						
			// ���շֻ��ź�
			ret = xQueueReceive(myDecodeQueue,&ValidFrame,0);								// �յ���Ч����֡
			{
				if(pdTRUE==ret)																// ���յ�������֡
				{
					// ���Ͳ�ѯSID����
					myBkCmd.cmd = BK_CMD_SEARCH_SID_PREPARE;								// ֪ͨ��ѯ����׼��
					ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��֪ͨ
					myBkCmd.cmd = BK_CMD_SEARCH_SID;										// ���Ͳ�ѯ����
					myBkCmd.SlaveID.Integer = ValidFrame.value&0xfffff0;					// ���μ�ֵ
					ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��SID������ѯ����

					// �ȴ���ѯ���
					ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// ���ղ�ѯ���
					if(BK_CMD_SEARCH_SID_OK==mCmdReq.cmd)									// SID�Ѿ�ע���	(�ֻ�ID�Ѿ�ע���)					
					{
						if(addNumName_Temp.Integer==mCmdReq.SlaveName.Integer)					// ��ǰ�ֻ��Ѿ�ע�ᵽ��ǰ������
						{
							flagSidRegister =3;												// ��ǰ�ֻ��Ѿ�ע�ᵽ��������
						}
						else																// SIDע�������δע�ᵽ��ǰ������
						{
							// ��ѯ��ǰ����SNUM�Ƿ�ע�����
							// ���Ͳ�ѯ����
							myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// ֪ͨ��ѯ����׼��
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��֪ͨ
							myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// ���Ͳ�ѯSNUM����
							myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��SID������ѯ����
							// �ȴ���ѯ���
							ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// ���ղ�ѯ���
							if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM�Ѿ�ע���	
							{
								flagSidRegister =4;													// �ֻ��ͺ��붼����������ͷֻ�ע���								
							}
							else
							{
								flagSidRegister =1;													// �ֻ�ע���������δע��						
							}
						}
					}
					else																			// SIDδע���
					{
						// ��ѯ��ǰ����SNUM�Ƿ�ע�����
						// ���Ͳ�ѯ����
						myBkCmd.cmd = BK_CMD_SEARCH_SNUM_PREPARE;								// ֪ͨ��ѯ����׼��
						ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��֪ͨ
						myBkCmd.cmd = BK_CMD_SEARCH_SNUM;										// ���Ͳ�ѯSNUM����
						myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
						ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��SID������ѯ����
						// �ȴ���ѯ���
						ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// ���ղ�ѯ���
						if(BK_CMD_SEARCH_SNUM_OK==mCmdReq.cmd)									// SNUM�Ѿ�ע���	
						{
							flagSidRegister =2;													// ������ע��,������δע���	
						}
						else
						{
							// ����ͷֻ���δע�ᣬ���Ͻ���ע��
							myBkCmd.cmd = BK_CMD_INSERT_PREPARE;								// ֪ͨע������׼��
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��֪ͨ
							myBkCmd.cmd = BK_CMD_INSERT_SID;										// ��������
							myBkCmd.SlaveName.Integer = addNumName_Temp.Integer;
							myBkCmd.SlaveID.Integer = ValidFrame.value&0xfffff0;			// ���μ�ֵ
							ret = xQueueSend(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ��SID������ѯ����
							// �ȴ���ѯ���
							ret = xQueueReceive(myCmdReqQue,&mCmdReq,portMAX_DELAY);				// ���ղ�ѯ���
							if(BK_CMD_INSERT_SID_OK==mCmdReq.cmd)									// SNUM�Ѿ�ע���	
							{
								flagSidRegister =3;													// ������ע��	
							}
							else
							{
								flagSidRegister =5;													// ע��ʧ��˵���ռ�����
							}
						}
					}
					
					sprintf(strRegisterResult,"-%d",flagSidRegister);								// ˢע����
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
	�Զ����ʱ������
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
					//-------------------- �������� ---------------------------
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
					//--------------------- �������� ------------------------
					case MSG_KEY_FUN_S_TOUCH:
					{
						break;
					}
					//--------------------- �������� ----------------------
					case MSG_KEY_ENT_L_TOUCH:
					{
						switch_menu(MENU_SET_ITEM);							// �л���
						break;
					}
					//--------------------- �������� ------------------------
					case MSG_KEY_FUN_L_TOUCH:
					{
						break;
					//--------------------- �������� -------------------------
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
	ɾ���ֻ�����
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
	�������ý���
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
			F06 - ���ú�����ʾ��ʽ
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
						switch_menu(MENU_SET_ITEM);							// ����
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
	�������ý���,��Ч����Ϊ181
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
	����ʱ������.
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
			OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1);					// ���
			OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1);				// ʱ��
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
					case MSG_KEY_ENT_S_TOUCH:										// ��������
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
					case MSG_KEY_FUN_S_TOUCH:												// �л����λ��
					{
						if(++cursorMenu>CUSOR_ALARM_HOUR)	cursorMenu = CUSOR_ALARM_MINUTE;
						sprintf(stringTemp2,"%02d:%02d",alarmTemp.time.hour,alarmTemp.time.minute);
						sprintf(stringTemp3,"%02d:",alarm_num);
						OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1); 				// ��� - 16x8����
						OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1); 			// ʱ��
						LCD_Fresh = 1;
						break;
					}
					case MSG_KEY_ENT_L_TOUCH:												// ����ʱҪ��������
					{
						if(1==alarm_num)
						{
							//===========================================================
													myAlarm1.en = 1;
							//===========================================================
							myAlarm1.time.hour = alarmTemp.time.hour;
							myAlarm1.time.minute = alarmTemp.time.minute;
							//myAlarmTimer_time_set(alarm_num, myAlarm1.time.hour, myAlarm1.time.minute);	// ��Сʱ�ͷ���ת��RTC����ֵ
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
						switch_menu(MENU_SET_ITEM);							// ����
						break;
					}
					case MSG_KEY_FUN_L_TOUCH:								// ����FUN���л�����,�л�ʱ�ȱ�������ֵ
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
						// ����ʱ�ӻ���
						OLED_ShowString(0,8,(unsigned char*)stringTemp3,16,1);				// ���
						OLED_ShowString(24+8,0,(unsigned char*)stringTemp2,32,1); 			// ʱ��
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
				// �ַ���˸
				if(alarmItemFlash)	alarmItemFlash = 0;											// ��
				else	alarmItemFlash = 1;

				switch(cursorMenu)
				{
					case CUSOR_ALARM_MINUTE:
					{
						if(alarmItemFlash)														// ��
						{
							sprintf(stringTemp4,"%02d",alarmTemp.time.minute);
							OLED_ShowString(24+8+3*16,0,(unsigned char*)stringTemp4,32,1); 			// ����
						}
						else																	// ��
						{
							OLED_ShowString(24+8+3*16,0,(unsigned char*)"  ",32,1); 			// // ����
						}
						break;
					}
					case CUSOR_ALARM_HOUR:
					{
						if(alarmItemFlash)														// ��
						{
							sprintf(stringTemp4,"%02d",alarmTemp.time.hour);
							OLED_ShowString(24+8+0*8,0,(unsigned char*)stringTemp4,32,1); 			// Сʱ
						}
						else																	// ��
						{
							OLED_ShowString(24+8+0*8,0,(unsigned char*)"  ",32,1); 			// Сʱ
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
	���忪/�ؿ��ƽ���
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
						switch_menu(MENU_SET_ITEM);							// ����
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
	��ʾ����汾��
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
					switch_menu(MENU_SET_ITEM);							// ����
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
	���Խ���
*****************************************************************************************/
void menu_rtc_test(void)
{

}

/*****************************************************************************************
Function: 
Description: 
	�����ܱ�
*****************************************************************************************/
void menu_main(void)
{
	switch(MenuIndex)
	{
		case MENU_TURN_OFF:		{ menu_off(); break;	}
		case MENU_STANDBY:		{ menu_standby(); break;}									// ��������
		case MENU_NOFITY:		{ menu_notify(); break; }									// �������ѽ���
		case MENU_SET_ITEM:		{ menu_setItem_select(); break;}
		case MENU_ADD_SLAVE:	{ menu_add_slave(); break;}									// ��ӷֻ�
		case MENU_ALERT_TIME:	{ menu_alert_time(); break;}								// �޸�����ʱ��
		case MENU_AUTO_CLEAN:	{ menu_auto_clean(); break;}								// �Զ����ʱ��
		case MENU_SLAVE_DELETE:	{ menu_slave_delete(); break;}								// ɾ���ֻ�
		case MENU_DATE_TIME:	{ menu_date_time(); break;}									// ���ں�ʱ��
		case MENU_NUMBER_STYLE:	{ menu_number_style(); break;}								// ������ʾ��ʽ
		case MENU_PASSWORD:		{ menu_password(); break;}									// ��������
		case MENU_ALARM_TIME:	{ menu_alarm_time(); break;}								// ����ʱ������
		case MENU_ALARM_SWITCH:	{ menu_alarm_switch(); break;}								// ���忪��
		case MENU_MAC_VERSION:	{ menu_mac_verion(); break;}
		case MENU_RTC_TEST:		{ menu_rtc_test(); break;}									// RTC����
		default:
			break;
	}
}

/*****************************************************************************************
Function: 
Description: 
	�鿴��ǰ�Ƿ���ʡ��״̬
	return - the satus of the pwower save(1-power save,0-normal)
*****************************************************************************************/
int check_powerSave_status(void)
{
	return(FG_PowerSave);
}

/*****************************************************************************************
Function: 
Description: 
	����ʡ��ģʽ��־
	Flag - the value which will be set
*****************************************************************************************/
void powerSave_flag_set(unsigned char Flag)
{
	FG_PowerSave = Flag;
}

/*****************************************************************************************
Function: 
Description: 
	��λ����ʡ��ģе�ĵ���ʱ������
*****************************************************************************************/
void reset_powerSave_counter(void)
{
	AutoDisplayOffCnt = TIME_AUTO_POW_SAVE;
}

/*****************************************************************************************
Function: 
Description: 
	�жϽ���ʡ��ģʽʱ�䵽��?
*****************************************************************************************/
int is_powerSave_time(void)
{
	return(AutoDisplayOffCnt>0?0:1);
}

/*****************************************************************************************
Function: 
Description: 
	ʡ��ģʽ����ʱ����ֵ����	
*****************************************************************************************/
void auto_powerSave_count(void)
{
	if(AutoDisplayOffCnt)		AutoDisplayOffCnt--;
}

/*****************************************************************************************
Function: 
Description: 
	���������ʼ��
*****************************************************************************************/
void menu_init(void)
{
	MenuIndex = MENU_TURN_OFF;
	OLED_Init();
	OLED_ColorTurn(0);									//0������ʾ��1 ��ɫ��ʾ
	OLED_Refresh();
	OLED_DisPlay_Off();									// �ر�OLED
	ClockColonFlash_timer_init();						// ����������ʱ
}


/*****************************************************************************************
Function: 
Description: 
	������Ժ���
*****************************************************************************************/
void MenuTask(void * pvParameters)
{
	vTaskDelay(200);									// ��ʱ�ȴ������ȼ�������ɳ�ʼ��������
	power_save_init();
	menu_init();
	switch_menu(MENU_TURN_OFF);
	vTaskDelay(20);
	
	while(1)
	{
		menu_main();									// �����߼�
		auto_powerSave_count();							// �Զ�Ϩ����ʱ
		vTaskDelay(2 / portTICK_PERIOD_MS);
	}
}





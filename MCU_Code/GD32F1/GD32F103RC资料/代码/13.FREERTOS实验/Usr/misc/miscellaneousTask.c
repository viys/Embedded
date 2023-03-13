/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "gd32e10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "powerKey.h"
#include "MechanicalKey.h"
#include "myQueue.h"
#include "miscellaneousTask.h"
#include "motor.h"
#include "battery.h"
//--------------------- variable declare ----------------------



/*************************************************************
Function: 
Description: 
*************************************************************/
void test_io_port_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);	
	gpio_bit_reset(GPIOA,GPIO_PIN_9);
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void myKey_deal(unsigned char keycode,unsigned char keystat)
{
	int msg_key;
	switch(keycode)
	{
		case KEY_CODE_ENTER:
		{
			if(KEY_STAT_PRESSING==keystat)
			{
				msg_key = MSG_KEY_ENT_TOUCHING;
				xQueueSend(myKeyQueue,&msg_key,0);
			}
			else if(KEY_STAT_UP==keystat)
			{
				msg_key = MSG_KEY_ENT_S_TOUCH;
				xQueueSend(myKeyQueue,&msg_key,0);
			}
			else if(KEY_STAT_LONG==keystat)
			{
				msg_key = MSG_KEY_ENT_L_TOUCH;
				xQueueSend(myKeyQueue,&msg_key,0);
			}
			break;
		}
		case KEY_CODE_FUN:
		{
			if(KEY_STAT_UP==keystat)
			{
				msg_key = MSG_KEY_FUN_S_TOUCH;
				xQueueSend(myKeyQueue,&msg_key,0);
			}
			else if(KEY_STAT_LONG==keystat)
			{
				msg_key = MSG_KEY_FUN_L_TOUCH;
				xQueueSend(myKeyQueue,&msg_key,0);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}


/*************************************************************
Function: 
Description: 
	this routine will be invoked per 10ms
*************************************************************/
void key_scan(void)
{
	static unsigned char myKeyEnb = nKEY_SCAN_DIS;
	static unsigned char myKeyDelayCnt = 0;
	
	if(KEY_SCAN_EN==myKeyEnb)
	{
		myKey.current_keyval = get_key_Mechanical();
		get_keycode(myKey.current_keyval,&myKey.keycode,&myKey.keystate);
		myKey_deal(myKey.keycode, myKey.keystate);
	}
	else														// key event will occur when power up,so delay a short time
	{
		myKeyDelayCnt++;
		if(TIME_SCAN_DELAY<myKeyDelayCnt)
		{
			myKeyDelayCnt = 0;
			myKeyEnb = KEY_SCAN_EN;
		}
	}
}

/*****************************************************************************************
Function: 
Description: 
	查询是否有按键唤醒系统
*****************************************************************************************/
int is_key_wakeup_system(void)
{
	return(is_ExitKey_Waupup_sys());
}

/*****************************************************************************************
Function: 
Description: 
	马达控制代码
*****************************************************************************************/
void mot_control(void)
{
	int mot_cmd;
	BaseType_t ret;

	// 接收马达时长控制
	ret = xQueueReceive(myMotCmdQue,&mot_cmd,0);
	if(pdTRUE==ret)
	{
		if(mot_cmd)
			motor_vibration_set(mot_cmd/VIBRATE_TIME_UNIT,1);	// 设置马达转动时间
		else
			motor_vibration_set(0,0);
	}

	mot_vibrate();												// 马达转动计时
}


/*******************************************************************************
Function:
Descripton:
	混杂设备控制任务
*******************************************************************************/
void miscTask(void * pvParameters)
{
	charge_det_init();
	volt_det_init();
	key_port_init();											// 按键初始化
	motor_init();
	test_io_port_init();										// 测试端口初始化
	
	while(1)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		key_scan();												// 按键扫描
		mot_control();											// 马达控制
		get_bat_adc_value();									// 获取ADC值
		battery_volt_det();										// 电压判断
	}
}





/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/

//------------------------------- include files ----------------------------------------
#include "SoftTimer.h"
#include "myQueue.h"
//-------------------------------- macro define ----------------------------------------
//------------------------------ variable declare --------------------------------------
TimerHandle_t xAutoReloadTimer_ClkColon = NULL;						// 秒闪定时器



/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
static void prvSecFlashTimerCallback( TimerHandle_t xTimer )
{
	Msg_SecondFlash_t	MsgSecFlash;

	MsgSecFlash = MSG_SEC_FLASH;
	xQueueSendFromISR(mySecFlashQueue,&MsgSecFlash,0);
}


/*******************************************************************************
Function:
Descripton:
			秒闪定时器初始化
*******************************************************************************/
void ClockColonFlash_timer_init(void)
{
	BaseType_t xTimerStarted;
	BaseType_t	ret;
	

	if(NULL==xAutoReloadTimer_ClkColon)
	{
		xAutoReloadTimer_ClkColon = xTimerCreate(	"AutoReload",					// 名字
													TIME_CLOCK_COLON_FLASH,	// 重载需要时间
													pdTRUE,						// 重载模式
													0,							
													prvSecFlashTimerCallback );	// 回调函数
													
		if( ( NULL==xAutoReloadTimer_ClkColon ) )
		{
			configASSERT( pdFALSE );
		}	
		
		// 启动定时器
		xTimerStarted = xTimerStart( xAutoReloadTimer_ClkColon, 0 );
		if( ( pdFAIL==xTimerStarted) )
		{
			configASSERT( xTimerStarted );
		}
	}
	else
	{
		ret = xTimerIsTimerActive(xAutoReloadTimer_ClkColon);
		if(pdFALSE==ret)
		{
			// 启动定时器
			xTimerStarted = xTimerStart( xAutoReloadTimer_ClkColon, 0 );
			if( ( pdFAIL==xTimerStarted) )
			{
				configASSERT( xTimerStarted );
			}
		}
	}
}

/*******************************************************************************
Function:
Descripton:
			秒闪定时器停止
*******************************************************************************/
void ClockColonFlash_timer_stop(void)
{
	BaseType_t	ret;

	if(NULL!=xAutoReloadTimer_ClkColon)
	{
		ret = xTimerIsTimerActive(xAutoReloadTimer_ClkColon);
		if(pdFALSE!=ret)														// 定时器正在运行
		{
			// 启动定时器
			ret = xTimerStop(xAutoReloadTimer_ClkColon,pdMS_TO_TICKS(10));
			if( ( pdFAIL==ret) )
			{
				configASSERT( ret );
			}
		}
	}
}

/*******************************************************************************
Function:
Descripton:
			秒闪定时器复位
*******************************************************************************/
void ClockColonFlash_timer_reset(void)
{
	if(NULL!=xAutoReloadTimer_ClkColon)
	{
		xTimerReset(xAutoReloadTimer_ClkColon,pdMS_TO_TICKS(5));
	}
}



/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/

//------------------------------- include files ----------------------------------------
#include "SoftTimer.h"
#include "myQueue.h"
//-------------------------------- macro define ----------------------------------------
//------------------------------ variable declare --------------------------------------
TimerHandle_t xAutoReloadTimer_ClkColon = NULL;						// ������ʱ��



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
			������ʱ����ʼ��
*******************************************************************************/
void ClockColonFlash_timer_init(void)
{
	BaseType_t xTimerStarted;
	BaseType_t	ret;
	

	if(NULL==xAutoReloadTimer_ClkColon)
	{
		xAutoReloadTimer_ClkColon = xTimerCreate(	"AutoReload",					// ����
													TIME_CLOCK_COLON_FLASH,	// ������Ҫʱ��
													pdTRUE,						// ����ģʽ
													0,							
													prvSecFlashTimerCallback );	// �ص�����
													
		if( ( NULL==xAutoReloadTimer_ClkColon ) )
		{
			configASSERT( pdFALSE );
		}	
		
		// ������ʱ��
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
			// ������ʱ��
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
			������ʱ��ֹͣ
*******************************************************************************/
void ClockColonFlash_timer_stop(void)
{
	BaseType_t	ret;

	if(NULL!=xAutoReloadTimer_ClkColon)
	{
		ret = xTimerIsTimerActive(xAutoReloadTimer_ClkColon);
		if(pdFALSE!=ret)														// ��ʱ����������
		{
			// ������ʱ��
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
			������ʱ����λ
*******************************************************************************/
void ClockColonFlash_timer_reset(void)
{
	if(NULL!=xAutoReloadTimer_ClkColon)
	{
		xTimerReset(xAutoReloadTimer_ClkColon,pdMS_TO_TICKS(5));
	}
}



/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/

//------------------------------- include files ----------------------------------------
#include "gd32e10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "myQueue.h"
//-------------------------------- macro define ----------------------------------------
//------------------------------ variable declare --------------------------------------
QueueHandle_t	myKeyQueue = NULL;
QueueHandle_t	myRtcQueue = NULL;
QueueHandle_t	mySecFlashQueue = NULL;
QueueHandle_t	myRfCtrlQueue = NULL;
QueueHandle_t	myRfFrameQueue = NULL;
QueueHandle_t	myDecodeQueue = NULL;
QueueHandle_t	myAlertQueue = NULL;
QueueHandle_t	myBkCommandQue = NULL;									// ������BK����������
QueueHandle_t	myBkListRetQue = NULL;									// �����������������ѯ���
QueueHandle_t	myCmdReqQue = NULL;										// ���ڷ�������ִ�н��
QueueHandle_t	myWakeUpReqQue = NULL;									// ���ڻ���ʱ���ͻ�������
QueueHandle_t	myRfObserverQue = NULL;									// ������Ƶ���յ��ź�Ԥ�ж���Ϣ����
QueueHandle_t	myMotCmdQue = NULL;										// ������������Ϣ



/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void myQueue_init(void)
{
	myKeyQueue = xQueueCreate(LENGTH_MSG_KEY,sizeof(int));
	if(NULL==myKeyQueue)
	{
		configASSERT( pdFALSE );
	}
	
	myRtcQueue = xQueueCreate(LENGTH_MSG_RTC,sizeof(Msg_RTC_t));
	if(NULL==myRtcQueue)
	{
		configASSERT( pdFALSE );
	}
	
	mySecFlashQueue = xQueueCreate(LENGTH_MSG_SEC_FLS,sizeof(Msg_SecondFlash_t));
	if(NULL==mySecFlashQueue)
	{
		configASSERT( pdFALSE );
	}

	myRfCtrlQueue = xQueueCreate(LENGTH_MSG_RF_CTRL,sizeof(Msg_RfCtrl_t));
	if(NULL==myRfCtrlQueue)
	{
		configASSERT( pdFALSE );
	}

	myRfFrameQueue = xQueueCreate(LENGTH_MSG_RF_FRAME,sizeof(Msg_RfFrame_t));
	if(NULL==myRfFrameQueue)
	{
		configASSERT( pdFALSE );
	}

	myDecodeQueue = xQueueCreate(LENGTH_MSG_DECODE,sizeof(Msg_VaildFrame_t));
	if(NULL==myDecodeQueue)
	{
		configASSERT( pdFALSE );
	}

	myAlertQueue = xQueueCreate(LENGTH_MSG_ALERT,sizeof(unsigned int));
	if(NULL==myAlertQueue)
	{
		configASSERT( pdFALSE );
	}

	myBkCommandQue = xQueueCreate(LENGTH_MSG_BK_CMD,sizeof(Bk_Cmd_t));
	if(NULL==myBkCommandQue)
	{
		configASSERT( pdFALSE );
	}
	
	myWakeUpReqQue = xQueueCreate(LENGTH_MSG_WKUP,sizeof(int));
	if(NULL==myWakeUpReqQue)
	{
		configASSERT( pdFALSE );
	}

	myCmdReqQue = xQueueCreate(LENGTH_MSG_CMD_REQ,sizeof(Bk_CmdReq_t));
	if(NULL==myCmdReqQue)
	{
		configASSERT( pdFALSE );
	}
	
	myRfObserverQue = xQueueCreate(LENGTH_MSG_RF_OSV,sizeof(int));
	if(NULL==myRfObserverQue)
	{
		configASSERT( pdFALSE );
	}

	myMotCmdQue = xQueueCreate(LENGTH_MSG_MOT_CMD,sizeof(int));
	if(NULL==myMotCmdQue)
	{
		configASSERT( pdFALSE );
	}
}



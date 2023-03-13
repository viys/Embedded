/********************************************************************************************
file name:
Description:
********************************************************************************************/
//----------------------------- include files -----------------------------
#include "BackupData.h"
#include "myRTC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "myQueue.h"
#include "flash_zone.h"
//------------------------------ variable declare --------------------------------------



/*******************************************************************************
Function:
Descripton:
	���籣����س�ʼ��
*******************************************************************************/
void BackUp_Init(void)
{

}

/*******************************************************************************
Function:
Descripton:
	����ֻ�ID�ͷֻ���Ų�ѯ���� 
*******************************************************************************/
void DataBackup_task(void * pvParameters)
{
	BaseType_t	ret;
	static unsigned char SlaveSearchStage = 0;
	Bk_Cmd_t myBkCmd;
	Bk_CmdReq_t mCmdReq; 
	
	vTaskDelay(pdMS_TO_TICKS(20));
	myRTC_init();																		// ��ʼ��RTC��RTCҪ����ϵͳ����
	BkData_Init();																		// ���ݱ��������ʼ��
			
	
	while(1)
	{
		switch(SlaveSearchStage)
		{
			case STAGE_BK_CMD_IDLE:														// �ȴ�����
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ����RF��Ч֡
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SID_PREPARE==myBkCmd.cmd)							// ����׼��
					{	
						SlaveSearchStage = STAGE_BK_CMD_SID_SEARCH;
					}
					else if(BK_CMD_INSERT_PREPARE==myBkCmd.cmd)							// ����׼��
					{
						SlaveSearchStage = STAGE_BK_CMD_SID_INSERT;
					}
					else if(BK_CMD_REMOVE_PREPARE==myBkCmd.cmd)							// ɾ��׼��
					{
						SlaveSearchStage = STAGE_BK_CMD_SID_REMOVE;
					}
					else if(BK_CMD_SEARCH_SNUM_PREPARE==myBkCmd.cmd)					// �����ѯ׼��
					{
						SlaveSearchStage = STAGE_BK_CMD_SNUM_SEARCH;
					}
				}
				
				break;
			}
			case STAGE_BK_CMD_SID_SEARCH:												// ��ѯ�ֻ�ID�Ƿ�ע��
			{
				xQueueReset(myDecodeQueue);												// ��ն���
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK����
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SID==myBkCmd.cmd)
					{
						mCmdReq.SlaveName.Integer = 0;
						ret = BkList_Is_SID_Recorded((myInt4Chars_t*)&mCmdReq.SlaveName,(myInt4Bytes_t*)&myBkCmd.SlaveID);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_SEARCH_SID_OK;
						else		mCmdReq.cmd = BK_CMD_SEARCH_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// ���ز�ѯ���
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// ��ѯ��Ϻ󷵻ؽ�������״̬
				break;
			}
			case STAGE_BK_CMD_SNUM_SEARCH:												// ��ѯ�ֻ�����Ƿ�ע��
			{
				xQueueReset(myDecodeQueue);												// ��ն���
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK����
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SNUM==myBkCmd.cmd)
					{
						mCmdReq.SlaveID.Integer = 0;
						ret = BkList_Is_SNUM_Recorded((myInt4Chars_t*)&myBkCmd.SlaveName,(myInt4Bytes_t*)&mCmdReq.SlaveID);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_SEARCH_SNUM_OK;
						else		mCmdReq.cmd = BK_CMD_SEARCH_SNUM_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// ���ز�ѯ���
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// ��ѯ��Ϻ󷵻ؽ�������״̬
				break;
			}
			case STAGE_BK_CMD_SID_REMOVE:												// ɾ���ֻ���¼
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK����
				if(pdTRUE==ret)
				{
					if(BK_CMD_REMOVE_SID==myBkCmd.cmd)
					{
						ret = BkList_Remove_SlaveNum_Record(myBkCmd.SlaveName.Integer);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_REMOVE_SID_OK;
						else		mCmdReq.cmd = BK_CMD_REMOVE_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// ����ɾ�����
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// ��ѯ��Ϻ󷵻ؽ�������״̬
				break;
			}
			case STAGE_BK_CMD_SID_INSERT:												// ��ӷֻ���¼
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// ����RF��Ч֡
				if(pdTRUE==ret)
				{
					if(BK_CMD_INSERT_SID==myBkCmd.cmd)
					{
						ret = BkList_Insert_Record((myInt4Chars_t*)&myBkCmd.SlaveName, (myInt4Bytes_t*)&myBkCmd.SlaveID);						
						if(0==ret)	mCmdReq.cmd = BK_CMD_INSERT_SID_OK;
						else		mCmdReq.cmd = BK_CMD_INSERT_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// ����ɾ�����
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// ��ѯ��Ϻ󷵻ؽ�������״̬
				break;
			}
			default:
				break;
		}
		
	}
}




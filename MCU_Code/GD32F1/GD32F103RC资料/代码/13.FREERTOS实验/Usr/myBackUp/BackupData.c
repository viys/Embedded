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
	掉电保存相关初始化
*******************************************************************************/
void BackUp_Init(void)
{

}

/*******************************************************************************
Function:
Descripton:
	处理分机ID和分机编号查询任务 
*******************************************************************************/
void DataBackup_task(void * pvParameters)
{
	BaseType_t	ret;
	static unsigned char SlaveSearchStage = 0;
	Bk_Cmd_t myBkCmd;
	Bk_CmdReq_t mCmdReq; 
	
	vTaskDelay(pdMS_TO_TICKS(20));
	myRTC_init();																		// 初始化RTC，RTC要用于系统唤醒
	BkData_Init();																		// 数据备份区域初始化
			
	
	while(1)
	{
		switch(SlaveSearchStage)
		{
			case STAGE_BK_CMD_IDLE:														// 等待命令
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 接收RF有效帧
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SID_PREPARE==myBkCmd.cmd)							// 搜索准备
					{	
						SlaveSearchStage = STAGE_BK_CMD_SID_SEARCH;
					}
					else if(BK_CMD_INSERT_PREPARE==myBkCmd.cmd)							// 插入准备
					{
						SlaveSearchStage = STAGE_BK_CMD_SID_INSERT;
					}
					else if(BK_CMD_REMOVE_PREPARE==myBkCmd.cmd)							// 删除准备
					{
						SlaveSearchStage = STAGE_BK_CMD_SID_REMOVE;
					}
					else if(BK_CMD_SEARCH_SNUM_PREPARE==myBkCmd.cmd)					// 号码查询准备
					{
						SlaveSearchStage = STAGE_BK_CMD_SNUM_SEARCH;
					}
				}
				
				break;
			}
			case STAGE_BK_CMD_SID_SEARCH:												// 查询分机ID是否注册
			{
				xQueueReset(myDecodeQueue);												// 清空队列
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK命令
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SID==myBkCmd.cmd)
					{
						mCmdReq.SlaveName.Integer = 0;
						ret = BkList_Is_SID_Recorded((myInt4Chars_t*)&mCmdReq.SlaveName,(myInt4Bytes_t*)&myBkCmd.SlaveID);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_SEARCH_SID_OK;
						else		mCmdReq.cmd = BK_CMD_SEARCH_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// 返回查询结果
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// 查询完毕后返回接收命令状态
				break;
			}
			case STAGE_BK_CMD_SNUM_SEARCH:												// 查询分机编号是否注册
			{
				xQueueReset(myDecodeQueue);												// 清空队列
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK命令
				if(pdTRUE==ret)
				{
					if(BK_CMD_SEARCH_SNUM==myBkCmd.cmd)
					{
						mCmdReq.SlaveID.Integer = 0;
						ret = BkList_Is_SNUM_Recorded((myInt4Chars_t*)&myBkCmd.SlaveName,(myInt4Bytes_t*)&mCmdReq.SlaveID);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_SEARCH_SNUM_OK;
						else		mCmdReq.cmd = BK_CMD_SEARCH_SNUM_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// 返回查询结果
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// 查询完毕后返回接收命令状态
				break;
			}
			case STAGE_BK_CMD_SID_REMOVE:												// 删除分机记录
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// BK命令
				if(pdTRUE==ret)
				{
					if(BK_CMD_REMOVE_SID==myBkCmd.cmd)
					{
						ret = BkList_Remove_SlaveNum_Record(myBkCmd.SlaveName.Integer);		
						if(0==ret)	mCmdReq.cmd = BK_CMD_REMOVE_SID_OK;
						else		mCmdReq.cmd = BK_CMD_REMOVE_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// 返回删除结果
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// 查询完毕后返回接收命令状态
				break;
			}
			case STAGE_BK_CMD_SID_INSERT:												// 添加分机记录
			{
				ret = xQueueReceive(myBkCommandQue,&myBkCmd,portMAX_DELAY);				// 接收RF有效帧
				if(pdTRUE==ret)
				{
					if(BK_CMD_INSERT_SID==myBkCmd.cmd)
					{
						ret = BkList_Insert_Record((myInt4Chars_t*)&myBkCmd.SlaveName, (myInt4Bytes_t*)&myBkCmd.SlaveID);						
						if(0==ret)	mCmdReq.cmd = BK_CMD_INSERT_SID_OK;
						else		mCmdReq.cmd = BK_CMD_INSERT_SID_FAILD;
						xQueueSend(myCmdReqQue,&mCmdReq,0);								// 返回删除结果
					}
				}
				
				SlaveSearchStage = STAGE_BK_CMD_IDLE;									// 查询完毕后返回接收命令状态
				break;
			}
			default:
				break;
		}
		
	}
}




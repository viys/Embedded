/********************************************************************************************
file name:
Description:
********************************************************************************************/
#ifndef		__BACKUP_DATA_H__
#define		__BACKUP_DATA_H__
//----------------------------- include files -----------------------------
#include "gd32e10x.h"
#include <stdio.h>
//----------------------------- macro define ------------------------------
enum
{
	STAGE_BK_CMD_IDLE = 0,
	STAGE_BK_CMD_SID_SEARCH,			// 分机搜索
	STAGE_BK_CMD_SNUM_SEARCH,			// 分机编号搜索
	STAGE_BK_CMD_SID_INSERT,			// 分机插入
	STAGE_BK_CMD_SNUM_INSERT,			// 分机插入
	STAGE_BK_CMD_SID_REMOVE,			// 分机插入
	STAGE_BK_CMD_SNUM_REMOVE,			// 分机插入
};
//----------------------------- macro define ------------------------------
//--------------------------- function declare ----------------------------
void DataBackup_task(void * pvParameters);


#endif	// __BACKUP_DATA_H__



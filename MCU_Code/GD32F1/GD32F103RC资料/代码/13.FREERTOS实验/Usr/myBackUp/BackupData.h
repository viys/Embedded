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
	STAGE_BK_CMD_SID_SEARCH,			// �ֻ�����
	STAGE_BK_CMD_SNUM_SEARCH,			// �ֻ��������
	STAGE_BK_CMD_SID_INSERT,			// �ֻ�����
	STAGE_BK_CMD_SNUM_INSERT,			// �ֻ�����
	STAGE_BK_CMD_SID_REMOVE,			// �ֻ�����
	STAGE_BK_CMD_SNUM_REMOVE,			// �ֻ�����
};
//----------------------------- macro define ------------------------------
//--------------------------- function declare ----------------------------
void DataBackup_task(void * pvParameters);


#endif	// __BACKUP_DATA_H__



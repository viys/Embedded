/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
#ifndef		__MY_QUEUE_H__
#define		__MY_QUEUE_H__
//------------------------------- include files ----------------------------------------
#include "queue.h"
//-------------------------------- macro define ----------------------------------------
enum
{
	MSG_KEY_ENT_TOUCHING,

	MSG_KEY_ENT_S_TOUCH,
	MSG_KEY_FUN_S_TOUCH,

	MSG_KEY_ENT_L_TOUCH,
	MSG_KEY_FUN_L_TOUCH,
	
	MSG_KEY_ENT_CONTI_TOUCH,
	MSG_KEY_FUN_CONTI_TOUCH,
};

enum
{
	BK_CMD_NULL = 0,						// 无效命令
	
	BK_CMD_RX_SIGNAL,						// 接收RF信号
	BK_CMD_SEARCH_SID,						// 搜索分机ID-查徇分机ID是否注册过
	BK_CMD_SEARCH_SNUM,						// 搜索分机编号-搜索分机编号下是否注册过ID
	BK_CMD_REMOVE_SNUM,						// 删除分机编号记录
	BK_CMD_REMOVE_SID,						// 删除分机ID记录
	BK_CMD_INSERT_SID,						// 插入分机ID记录
	
	BK_CMD_SEARCH_SID_PREPARE,				// 搜索分要ID准备
	BK_CMD_SEARCH_SNUM_PREPARE,				// 搜索分机编号准备
	BK_CMD_REMOVE_PREPARE,					// 删除分机记录准备
	BK_CMD_INSERT_PREPARE,					// 插入准备
	
	BK_CMD_SEARCH_SID_OK,					// 搜索分机ID-查徇分机ID是否注册过
	BK_CMD_SEARCH_SNUM_OK,					// 搜索分机编号-搜索分机编号下是否注册过ID
	BK_CMD_REMOVE_SNUM_OK,					// 删除分机编号记录
	BK_CMD_REMOVE_SID_OK,					// 删除分机ID记录
	BK_CMD_INSERT_SID_OK,					// 插入分机ID记录
	
	BK_CMD_SEARCH_NOT_COMP,					// 搜索 不匹配
	BK_CMD_SEARCH_SID_FAILD,				// 搜索分机ID-查徇分机ID是否注册过
	BK_CMD_SEARCH_SNUM_FAILD,				// 搜索分机编号-搜索分机编号下是否注册过ID
	BK_CMD_REMOVE_SNUM_FAILD,				// 删除分机编号记录
	BK_CMD_REMOVE_SID_FAILD,				// 删除分机ID记录
	BK_CMD_INSERT_SID_FAILD,				// 插入分机ID记录
};

enum
{
	MSG_WKUP_TYPE_ALARM,					// 闹铃唤醒方式
	MSG_WKUP_TYPE_KEY,						// 按键唤醒方式
};

typedef enum _Msg_RTC_Type_
{
	MSG_RTC_SECOND_INT,
	MSG_RTC_ARLARM_INT,
}Msg_RTC_Type_t;

typedef struct _Msg_RTC_
{
	Msg_RTC_Type_t	type;
	unsigned int	rtcCount;
}Msg_RTC_t;

typedef enum _Msg_SecondFlash_Type_
{
	MSG_SEC_FLASH,
}Msg_SecondFlash_t;

typedef enum _Msg_RfCtrl_Type_
{
	MSG_RF_TURN_ON,
	MSG_RF_TURN_OFF,
}Msg_RfCtrl_t;

typedef struct _Msg_RfFrame_Type_
{
	unsigned char* frame;
	unsigned char length;
}Msg_RfFrame_t;

typedef union _Msg_VaildFrame_Type_
{
	unsigned char frame[3];
	unsigned int  value;
}Msg_VaildFrame_t;

typedef	union _mInt4Chars_type_
{
	unsigned int Integer;
	char Chars[sizeof(unsigned int)];
}mInt4Chars_t;

typedef	union _mInt4Bytes_type_
{
	unsigned int Integer;
	unsigned char Bytes[sizeof(unsigned int)];
}mInt4Bytes_t;

enum
{
	BK_LIST_SID_CNT,						// 对应分机编号下的分机ID个数
	BK_LIST_SNAME_CNT,						// 分机编号数据量
};

typedef struct _BkList_Ret_type_
{
	unsigned int type;
	unsigned int count;
}BkList_Ret_t;

typedef struct _Bk_Cmd_type_
{
	unsigned char cmd;
	mInt4Chars_t SlaveName;
	mInt4Bytes_t SlaveID;
}Bk_Cmd_t;

typedef struct _Bk_CmdReq_type_
{
	unsigned char cmd;
	mInt4Chars_t SlaveName;
	mInt4Bytes_t SlaveID;
}Bk_CmdReq_t;

enum
{
	RF_SIG_NULL,
	RF_SIG_BAD,
	RF_SIG_OK,
};
//-------------------------------- macro define ----------------------------------------
#define		LENGTH_MSG_KEY		10
#define		LENGTH_MSG_RTC		5
#define		LENGTH_MSG_SEC_FLS	5
#define		LENGTH_MSG_RF_CTRL	5
#define		LENGTH_MSG_RF_FRAME	3
#define		LENGTH_MSG_DECODE	10
#define		LENGTH_MSG_ALERT	5
#define		LENGTH_MSG_BK_CMD	5
#define		LENGTH_MSG_BK_RET	5
#define		LENGTH_MSG_CMD_REQ	5
#define		LENGTH_MSG_WKUP		5
#define		LENGTH_MSG_CMD_REQ	5
#define		LENGTH_MSG_RF_OSV	5
#define		LENGTH_MSG_MOT_CMD	5
//------------------------------ varialbe declare --------------------------------------
extern QueueHandle_t	myKeyQueue;
extern QueueHandle_t	myRtcQueue;
extern QueueHandle_t	mySecFlashQueue;
extern QueueHandle_t	myRfCtrlQueue;
extern QueueHandle_t	myRfFrameQueue;
extern QueueHandle_t	myDecodeQueue;
extern QueueHandle_t	myAlertQueue;
extern QueueHandle_t	myBkCommandQue;									// 用于向BK任务发送命令
extern QueueHandle_t	myBkListRetQue;									// 用于向界面任务反馈查询结果
extern QueueHandle_t	myCmdReqQue;									// 用于返回命令执行结果
extern QueueHandle_t	myWakeUpReqQue;									// 用于唤醒时发送唤醒类型
extern QueueHandle_t	myRfObserverQue;								// 用于射频接收的信号预判断消息发送
extern QueueHandle_t	myMotCmdQue;										// 用于马达控制消息
//------------------------------ function declare --------------------------------------
void myQueue_init(void);





#endif	// __MY_QUEUE_H__



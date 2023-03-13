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
	BK_CMD_NULL = 0,						// ��Ч����
	
	BK_CMD_RX_SIGNAL,						// ����RF�ź�
	BK_CMD_SEARCH_SID,						// �����ֻ�ID-���߷ֻ�ID�Ƿ�ע���
	BK_CMD_SEARCH_SNUM,						// �����ֻ����-�����ֻ�������Ƿ�ע���ID
	BK_CMD_REMOVE_SNUM,						// ɾ���ֻ���ż�¼
	BK_CMD_REMOVE_SID,						// ɾ���ֻ�ID��¼
	BK_CMD_INSERT_SID,						// ����ֻ�ID��¼
	
	BK_CMD_SEARCH_SID_PREPARE,				// ������ҪID׼��
	BK_CMD_SEARCH_SNUM_PREPARE,				// �����ֻ����׼��
	BK_CMD_REMOVE_PREPARE,					// ɾ���ֻ���¼׼��
	BK_CMD_INSERT_PREPARE,					// ����׼��
	
	BK_CMD_SEARCH_SID_OK,					// �����ֻ�ID-���߷ֻ�ID�Ƿ�ע���
	BK_CMD_SEARCH_SNUM_OK,					// �����ֻ����-�����ֻ�������Ƿ�ע���ID
	BK_CMD_REMOVE_SNUM_OK,					// ɾ���ֻ���ż�¼
	BK_CMD_REMOVE_SID_OK,					// ɾ���ֻ�ID��¼
	BK_CMD_INSERT_SID_OK,					// ����ֻ�ID��¼
	
	BK_CMD_SEARCH_NOT_COMP,					// ���� ��ƥ��
	BK_CMD_SEARCH_SID_FAILD,				// �����ֻ�ID-���߷ֻ�ID�Ƿ�ע���
	BK_CMD_SEARCH_SNUM_FAILD,				// �����ֻ����-�����ֻ�������Ƿ�ע���ID
	BK_CMD_REMOVE_SNUM_FAILD,				// ɾ���ֻ���ż�¼
	BK_CMD_REMOVE_SID_FAILD,				// ɾ���ֻ�ID��¼
	BK_CMD_INSERT_SID_FAILD,				// ����ֻ�ID��¼
};

enum
{
	MSG_WKUP_TYPE_ALARM,					// ���廽�ѷ�ʽ
	MSG_WKUP_TYPE_KEY,						// �������ѷ�ʽ
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
	BK_LIST_SID_CNT,						// ��Ӧ�ֻ�����µķֻ�ID����
	BK_LIST_SNAME_CNT,						// �ֻ����������
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
extern QueueHandle_t	myBkCommandQue;									// ������BK����������
extern QueueHandle_t	myBkListRetQue;									// �����������������ѯ���
extern QueueHandle_t	myCmdReqQue;									// ���ڷ�������ִ�н��
extern QueueHandle_t	myWakeUpReqQue;									// ���ڻ���ʱ���ͻ�������
extern QueueHandle_t	myRfObserverQue;								// ������Ƶ���յ��ź�Ԥ�ж���Ϣ����
extern QueueHandle_t	myMotCmdQue;										// ������������Ϣ
//------------------------------ function declare --------------------------------------
void myQueue_init(void);





#endif	// __MY_QUEUE_H__



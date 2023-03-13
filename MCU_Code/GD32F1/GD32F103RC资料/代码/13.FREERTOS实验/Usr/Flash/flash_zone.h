/*************************************************************************************************************
File Name:
Descripton:
*************************************************************************************************************/
#ifndef		__FLASH_ZONE_H__
#define		__FLASH_ZONE_H__
//------------------------------- include files ----------------------------------------
#include "gd32e10x.h" 
//-------------------------------- macro define ----------------------------------------
#define		BK_DATA_START_ADDRESS	0x08000400							// 备份数据超页地址

#define		FLASH_PAGE_SIZE			1024
#define		BK_DATA_PAGES			(4)									// 备份空间页数
#define		BK_DATA_LENGTH			(1024*BK_DATA_PAGES)				// 备份空间长度

#define		BYTES_OF_WORD			4
#define		BYTES_OF_MISC			32									// 其它备份资料
#define		BYTES_OF_SLAVE_RECORD	(BK_DATA_LENGTH-BYTES_OF_MISC)		// 分机号码务份资料

#define		FG_BK_ZONE_WRITED			0xA5														// 备份写入标志。

#define		ADDR_BK_WRITE_FG			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 备份标志，首次上电时写入
#define		ADDR_HIGH_WORD_RTC			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 备份RTC年数
#define		ADDR_MASTER_ID				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 备份区域最后一字节存主机ID
#define		ADDR_ALERT_MOD				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 提醒方式
#define		ADDR_ALERT_TIME				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 提醒时间
#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 射频接收/飞行模式

#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 字体颜色
#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 自动熄屏
#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 语言

#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 自动清除时间
#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 超时提示时间
#define		ADDR_RF_STATUS				(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-1))					// 管理员密码

#define		ADDR_KEY_NAME_ORDER			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-点单
#define		ADDR_KEY_NAME_CALL			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-呼叫
#define		ADDR_KEY_NAME_BILL			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-结帐
#define		ADDR_KEY_NAME_CANCEL		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-取消
#define		ADDR_KEY_NAME_KITCHEN		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-厨房
#define		ADDR_KEY_NAME_SOUP			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-取消
#define		ADDR_KEY_NAME_POWER			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-调火

#define		ADDR_KEY_NAME_SERVE			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-服务
#define		ADDR_KEY_NAME_WATER			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-加水
#define		ADDR_KEY_NAME_COFFEE		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-结帐
#define		ADDR_KEY_NAME_JUICE			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-果汁

#define		ADDR_KEY_NAME_NICE			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-好评
#define		ADDR_KEY_NAME_DISLIKE		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-差评

#define		ADDR_KEY_NAME_TREATMENT		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-换药
#define		ADDR_KEY_NAME_SOS			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-求救
#define		ADDR_KEY_NAME_WITHDRAW		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-拔针
#define		ADDR_KEY_NAME_DOCTOR		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-医生

#define		ADDR_KEY_NAME_CHECKOUT		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-退房
#define		ADDR_KEY_NAME_CLEAN			(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-打扫
#define		ADDR_KEY_NAME_TISSUE		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-纸巾
#define		ADDR_KEY_NAME_SANITARY		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 键值-卫生巾

#define		ADDR_KEY_NAME_CHECKOUT		(BK_DATA_START_ADDRESS+(BK_DATA_LENGTH-2))					// 呼叫记录
//-------------------------------- macro define ----------------------------------------
#define		FG_BIG_ENDIAN			0xff
#define		FG_LITTLE_ENDIAN		0x00
	
#define		BYTES_OF_NODE			16
//-------------------------------- macro define ----------------------------------------
#ifndef		__INDX_OF_5CHAR__
	#define		__INDX_OF_5CHAR__
	typedef	struct _Indx5Char_									// 存储5个字符的索值
	{
		unsigned int indx0		:6;
		unsigned int indx1		:6;
		unsigned int indx2		:6;
		unsigned int indx3		:6;
		unsigned int indx4		:6;
		unsigned int reserve	:2;
	}IndxOf5Char;
	typedef	union _myInt5CharInx_type_
	{
		unsigned int Integer;
		IndxOf5Char mChar;
	}myInt5CharInx_t;
#endif
typedef struct _ListPoint_type_
{
	unsigned short int Previous;
	unsigned short int Next;
}ListPoint_t;
typedef	union _myInt4Chars_type_
{
	unsigned int Integer;
	char Chars[sizeof(unsigned int)];
}myInt4Chars_t;
typedef	union _myInt4Bytes_type_
{
	unsigned int Integer;
	unsigned char Bytes[sizeof(unsigned int)];
}myInt4Bytes_t;
typedef struct _myBkData_type_
{
	unsigned int empty;							// 4字节
	myInt5CharInx_t IdName;						// 4字节(存5个字符的索引)
	myInt4Bytes_t SlaveID;						// 4字节
}myBkData_t;

typedef struct _myNode_type_
{
	ListPoint_t Point;					// 4字节
	myBkData_t Data;							// 12字节
}myNode_t;
typedef	union _myBkList_type_
{
	unsigned char mNode2Bytes[BYTES_OF_NODE];		// 12字节
	myNode_t mNode;								// 12字节
}myBkList_t;
//-------------------------------- macro define ----------------------------------------
#define		NUM_OF_SLAVE_RECORD		(BYTES_OF_SLAVE_RECORD/sizeof(struct Node))
//-------------------------------- macro define ----------------------------------------
#define		FG_NODE_INVALID 		0xff
#define		FG_NODE_VALID 			0xF5

#define		NODES_OF_PAGE_MAX		(FLASH_PAGE_SIZE/sizeof(myBkList_t))			// 一页能存储的最大节点数
//-------------------------------- macro define ----------------------------------------
extern const unsigned char Bk_DataZone[BK_DATA_LENGTH];
//------------------------------ function declare --------------------------------------
void BkData_Init(void);
int BkList_Is_SID_Recorded(myInt4Chars_t *SlaveNumber,myInt4Bytes_t *SlaveID);
int BkList_Is_SNUM_Recorded(myInt4Chars_t *SlaveNumber,myInt4Bytes_t *SlaveID);
signed char BkList_Remove_Slave_ID_Record(unsigned int SID);
signed char BkList_Remove_SlaveNum_Record(unsigned int SlaveName);
signed char BkList_Insert_Record(myInt4Chars_t *SlaveNumber, myInt4Bytes_t *SID);
unsigned int BkList_Search_sidCount_sNumber(myInt4Chars_t *SlaveNumber);


#endif	// __FLASH_ZONE_H__



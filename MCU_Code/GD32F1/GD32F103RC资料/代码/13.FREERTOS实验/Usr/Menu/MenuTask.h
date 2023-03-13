#ifndef		__MENU_TASK_H__
#define		__MENU_TASK_H__

//--------------------- include files ---------------------
//--------------------- macro define ----------------------
enum 
{
	// 一级菜单
	MENU_TURN_OFF,			
	MENU_POWER_ON,
	MENU_STANDBY,								// 待机界面
	MENU_MAIN,
	MENU_SET_ITEM,
	MENU_NOFITY,								// 呼叫提醒界面
	
	MENU_ADD_SLAVE,								// 添加分机
	MENU_ALERT_TIME,							// 设置提醒时间
	MENU_AUTO_CLEAN,							// 设置自动清除时间
	MENU_SLAVE_DELETE,							// 删除分机
	MENU_DATE_TIME,								// 设置日期和时间
	MENU_NUMBER_STYLE,							// 设置号码显示格式
	MENU_PASSWORD,								// 密码设置
	MENU_ALARM_TIME,							// 设置闹铃时间
	MENU_ALARM_SWITCH,							// 闹铃开关
	MENU_MAC_VERSION,							// 机器版本
	MENU_RTC_TEST,								// RTC测试界面
	
	MENU_MAX,
};
typedef struct _MENU_COMPONENTS_
{
	unsigned char menuName;                 		// which menu will show?
	unsigned int Para0;              			// 
	void (*MenuStartSet)(void);                  // menu init
	void (*MenuHand)(void);                     // 
}MenuComponets_t;
enum 
{
	SET_ITEM_MIN,
	SET_ITEM_ADD_SLAVE,							// 添加分机	
	SET_ITEM_ALERT_TIME,						// 提醒时间
	SET_ITEM_AUTO_CLEAN,						// 自动清除时间
	SET_ITEM_SLAVE_DELETE,						// 删除号码
	SET_ITEM_DATE_TIME,							// 日期和时间
	SET_ITEM_NUMBER_STYLE,						// 号码显示格式
	SET_ITEM_PASSWORD,							// 密码
	SET_ITEM_ALARM,								// 闹铃设置
	SET_ITEM_ALARM_SWITCH,						// 闹铃开关
	SET_ITEM_VERSION,							// 版本信息
	SET_ITEM_MAX,
};
enum 
{
	ADM_ITEM_MIN,
	ADM_ITEM_ADD_SENDER,							// 添加发射器
	ADM_ITEM_KEY_FUN,								// 按键功能设置
	ADM_ITEM_MAC_NUM,								// 本机号码
	ADM_ITEM_AUTO_CLEAN,							// 自动清除时间
	ADM_ITEM_TIMEOUT_ALERT,							// 超时提醒时间
	ADM_ITEM_FLY_MOD,								// 飞行模式
	ADM_ITEM_PASSWORD_SET,							// 管理员密码设置
	ADM_ITEM_DEFAULT_MAC,							// 恢复出厂设置
	ADM_ITEM_MAX,
};
enum
{
	MENU_STAGE_INIT = 0,
	MENU_STAGE_RUN,
};
#define		NUM_SLAVE_NAME		5
#define		NUM_WAIT_NUM_QUE	10
//--------------------- macro define ----------------------
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
typedef struct _myWaitNum_type_									// 用于存储排队的呼叫号码
{
	unsigned int WaitNum;
	unsigned char KeyValue;
}myWaitNum_t;
typedef struct __myWaitNumQue_type_								// 用于存储排队的呼叫号码
{
	myWaitNum_t	WaitNumArry[NUM_WAIT_NUM_QUE];
	unsigned char Amount;
	unsigned char Tail;
}myWaitNumQue_t;
//--------------------- macro define ----------------------
#define		LCD_COLUMN_MAX				128
#define		LCD_ROW_MAX					128

#define		SET_ONE_REGION_HIGTH			25

#define		COOR_SET_REGION1_X		0
#define		COOR_SET_REGION1_Y		0
#define		COOR_SET_REGION2_X		0
#define		COOR_SET_REGION2_Y		(COOR_SET_REGION1_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION3_X		0
#define		COOR_SET_REGION3_Y		(COOR_SET_REGION2_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION4_X		0
#define		COOR_SET_REGION4_Y		(COOR_SET_REGION3_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION5_X		0
#define		COOR_SET_REGION5_Y		(COOR_SET_REGION4_Y+SET_ONE_REGION_HIGTH)

#define		COOR_SET_REGION1_END_X		LCD_COLUMN_MAX
#define		COOR_SET_REGION1_END_Y		(SET_ONE_REGION_HIGTH-1)
#define		COOR_SET_REGION2_END_X		LCD_COLUMN_MAX
#define		COOR_SET_REGION2_END_Y		(COOR_SET_REGION1_END_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION3_END_X		LCD_COLUMN_MAX
#define		COOR_SET_REGION3_END_Y		(COOR_SET_REGION2_END_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION4_END_X		LCD_COLUMN_MAX
#define		COOR_SET_REGION4_END_Y		(COOR_SET_REGION3_END_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_REGION5_END_X		LCD_COLUMN_MAX
#define		COOR_SET_REGION5_END_Y		(COOR_SET_REGION4_END_Y+SET_ONE_REGION_HIGTH)

#define		COOR_SET_TITLE_X		48
#define		COOR_SET_TITLE_Y		4
#define		COOR_SET_INDEX_ITEMS_X		(COOR_SET_TITLE_X+16*2+8)
#define		COOR_SET_INDEX_ITEMS_Y		(COOR_SET_TITLE_Y+16-7)
#define		COOR_SET_ROW1_X			4
#define		COOR_SET_ROW1_Y			(COOR_SET_TITLE_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_ROW2_X			4
#define		COOR_SET_ROW2_Y			(COOR_SET_ROW1_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_ROW3_X			4
#define		COOR_SET_ROW3_Y			(COOR_SET_ROW2_Y+SET_ONE_REGION_HIGTH)
#define		COOR_SET_ROW4_X			4
#define		COOR_SET_ROW4_Y			(COOR_SET_ROW3_Y+SET_ONE_REGION_HIGTH)

#define		COOR_ADM_OPT_TITLE_X		24
#define		COOR_ADM_OPT_TITLE_Y		4
#define		COOR_ADM_INDEX_ITEMS_X		(COOR_ADM_OPT_TITLE_X+16*5+4)
#define		COOR_ADM_INDEX_ITEMS_Y		(COOR_ADM_OPT_TITLE_Y+16-7)
#define		COOR_ADM_OPT_ROW1_X			4
#define		COOR_ADM_OPT_ROW1_Y			(COOR_SET_TITLE_Y+SET_ONE_REGION_HIGTH)
#define		COOR_ADM_OPT_ROW2_X			4
#define		COOR_ADM_OPT_ROW2_Y			(COOR_SET_ROW1_Y+SET_ONE_REGION_HIGTH)
#define		COOR_ADM_OPT_ROW3_X			4
#define		COOR_ADM_OPT_ROW3_Y			(COOR_SET_ROW2_Y+SET_ONE_REGION_HIGTH)
#define		COOR_ADM_OPT_ROW4_X			4
#define		COOR_ADM_OPT_ROW4_Y			(COOR_SET_ROW3_Y+SET_ONE_REGION_HIGTH)

#define		COOR_ADD_SENDER_X			24								// 抬头座标
#define		COOR_ADD_SENDER_Y			4
#define		COOR_ADD_SENDER_NUM_X		48								// 分机编号座标
#define		COOR_ADD_SENDER_NUM_Y		42
#define		COOR_ADD_SENDER_CNT_X		28								// 当前分机号的分机数和总分机数
#define		COOR_ADD_SENDER_CNT_Y		(75-7)
#define		COOR_ADD_SENDER_CNT1_X		(COOR_ADD_SENDER_CNT_X+56)		// "个"
#define		COOR_ADD_SENDER_CNT1_Y		(75-10)
#define		COOR_ADD_DELAY_X			(60)							// 倒计时
#define		COOR_ADD_DELAY_Y			(100-7)
#define		COOR_ADD_DELAY1_X			(24)							// 倒计时
#define		COOR_ADD_DELAY1_Y			(100-5)
//-------------------------------- 菜单名称 -------------------------------------
// 二级菜单
#define		SETTING_TITLE_EN				"setting"
#define		SETTING_TITLE_CHS				"设置"
#define		SETTING_COUNTER_EN				"Counter"
#define		SETTING_COUNTER_CHS				"计时器"
#define		SETTING_AUTO_ON_OFF_EN			"Auto on/off"
#define		SETTING_AUTO_ON_OFF_CHS			"定时开关机"
#define		SETTING_ALARM_EN				"Alarm"
#define		SETTING_ALARM_CHS				"闹铃"
#define		SETTING_DATE_TIME_EN			"Date & Time"
#define		SETTING_DATE_TIME_CHS			"日期和时间"
#define		SETTING_ALERT_MOD_EN			"Alert mode"
#define		SETTING_ALERT_MOD_CHS			"呼叫提示模式"
#define		SETTING_ALERT_TIMES_EN			"Alert times"
#define		SETTING_ALERT_TIMES_CHS			"呼叫提示次数"
#define		SETTING_FONT_COLOR_EN			"Font color"
#define		SETTING_FONT_COLOR_CHS			"字体颜色"
#define		SETTING_SAVE_POW_EN				"Save power"
#define		SETTING_SAVE_POW_CHS			"自动熄屏"
#define		SETTING_LANGUAGE_EN				"Language"
#define		SETTING_LANGUAGE_CHS			"语言"
#define		SETTING_HISTORY_EN				"Call history"
#define		SETTING_HISTORY_CHS				"呼叫记录"
#define		SETTING_ADMIN_EN				"Adminitor"
#define		SETTING_ADMIN_CHS				"管理员选项"
#define		SETTING_ABOUT_EN				"About"
#define		SETTING_ABOUT_CHS				"关于本机"
// 三级菜单
#define		ADMIN_ADD_SENDER_EN				"setting"// 添加发射器
#define		ADMIN_ADD_SENDER_CHS			"添加发射器"
#define		ADMIN_KEY_SET_EN				"Counter"// 按键功能设置
#define		ADMIN_KEY_SET_CHS				"按键功能设置"
#define		ADMIN_MAC_NUM_EN				"Auto on/off"// 本机号码
#define		ADMIN_MAC_NUM_CHS				"本机号码"
#define		ADMIN_AUTO_CLEAN_EN				"Alarm"// 自动清除时间
#define		ADMIN_AUTO_CLEAN_CHS			"自动清除时间"
#define		ADMIN_TIMEOUT_ALERT_EN			"Date & Time"// 超时提醒时间
#define		ADMIN_TIMEOUT_ALERT_CHS			"超时提醒时间"
#define		ADMIN_FLY_MOD_EN				"Alert mode"// 飞行模式
#define		ADMIN_FLY_MOD_CHS				"飞行模式"
#define		ADMIN_PASSWORD_EN				"Alert mode"// 管理员密码
#define		ADMIN_PASSWORD_CHS				"管理员密码"
#define		ADMIN_DEFAULT_MAC_EN			"Alert mode"// 恢复出厂设置
#define		ADMIN_DEFAULT_MAC_CHS			"恢复出厂设置"
//--------------------- macro define ----------------------
#define		MAX_ITEM_OF_SET					12
#define		MAX_ITEM_ADM_OPTION				8
//--------------------- variable declare ------------------
#define		INDX_SLAVE_NAME_CHR_MIN			0
#define		INDX_SLAVE_NAME_CHR_MAX			36
//--------------------- macro define ----------------------
#define		NUM_STYLE_0						0X0					// xxx显示格式
#define		NUM_STYLE_1						0X01				// xxx-xx显示格式

#define		EN_STATUS						0X00		
#define		DIS_STATUS						0XFF

#define		AUTO_CLEAN_TIME_MIN				3
#define		AUTO_CLEAN_TIME_MAX				99

#define		TIME_ENTER_SLP1					50					// 5S
#define		TIME_ENTER_SLP2					10					// 1S

#define		TIME_AUTO_POW_SAVE				5000U				// 10S

enum
{
	STAGE_STANDBY_NULL,
	STAGE_STANDBY_NORMAL,
	STAGE_STANDBY_RX_START,
	STAGE_STANDBY_RX_ONLY,
	STAGE_STANDBY_RX_BAD_SIG,									// 等待无效信号消息
	STAGE_STANDBY_RX_VALID_SIG,									// 等待有效信号消息
	STAGE_STANDBY_RX_APPEND_DELAY,								// 当接收到有效后数据脉冲后，延时等待接收完整数据帧
	STAGE_STANDBY_SLEEP,
	STAGE_STANDBY_SLP_WAIT,
	STAGE_STANDBY_SID_SEARCH,									// 接收成功后搜索SID是否注册？
	STAGE_STANDBY_RF_OFF_DELAY,										// 呼叫成功后RF停止2S
};

#define		CUSOR_ALARM_MINUTE		0		// 光标位置定义
#define		CUSOR_ALARM_HOUR		1

//--------------------- function declare ------------------
void menu_off_init(void);
void menu_off(void);
void menu_main_init(void);
void menu_main(void);
void MenuTask(void * pvParameters);
void menu_PowerOn_init(void);
void menu_powerOn(void);
void menu_setting_init(void);
void menu_setting(void);
int check_powerSave_status(void);
void powerSave_flag_set(unsigned char Flag);
void reset_powerSave_counter(void);
int is_powerSave_time(void);
void auto_powerSave_count(void);
void menu_rtc_test(void);


#endif	//__MENU_TASK_H__


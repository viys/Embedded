#ifndef		__RFDECODE_H__
#define		__RFDECODE_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
#define		IO_RF_SIGNAL			PB15

#define		RF_SIGNAL_RCU			RCU_GPIOB
#define		RF_SIGNAL_PORT			GPIOB
#define		RF_SIGNAL_PIN			GPIO_PIN_15
#define		RF_SIGNAL_AF_MODE		GPIO_AF_1

#define		IO_RF_SIGNAL_INIT()		do{		\
										rcu_periph_clock_enable(RF_SIGNAL_RCU);		\
										gpio_init(RF_SIGNAL_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, RF_SIGNAL_PIN);		\
									}while(0)										
#define		IO_RF_SIGNAL_INIT1()	do{		\
										rcu_periph_clock_enable(RF_SIGNAL_RCU);		\
										gpio_init(RF_SIGNAL_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, RF_SIGNAL_PIN);		\
									}while(0)										
									
#define		READ_RF_SIGNAL()		gpio_input_bit_get(RF_SIGNAL_PORT,RF_SIGNAL_PIN)
									
#define		RF_TIMER_INIT()			timer11_capture_configuration()
#define		RF_TIMER_ENB(FLAG)		timer11_control(FLAG)
#define		RF_TIMER_NVIC_SET()		nvic_irq_enable(TIMER7_BRK_TIMER11_IRQn, 6, 0)

#define		LN_0_L_LEVL_MIN			(33-11)				// unit = 25us
#define		LN_0_L_LEVL_MAX			(33+11)				// unit = 25us
#define		LN_0_H_LEVL_MIN			(11-5)				// unit = 25us
#define		LN_0_H_LEVL_MAX			(11+10)				// unit = 25us

#define		LN_1_L_LEVL_MIN			(11-5)				// unit = 25us
#define		LN_1_L_LEVL_MAX			(11+10)				// unit = 25us
#define		LN_1_H_LEVL_MIN			(33-11)				// unit = 25us
#define		LN_1_H_LEVL_MAX			(33+11)				// unit = 25us

#define		LN_SYNC_H_LEVL_MIN		(11-6)				// unit = 25us
#define		LN_SYNC_H_LEVL_MAX		(11+11)				// unit = 25us
#define		LN_SYNC_L_LEVL_MIN		(341-41)			// unit = 25us
#define		LN_SYNC_L_LEVL_MAX		(341+41)			// unit = 25us
//-------------------------------分隔线----------------------------------------
#define		cRF_TICK					300u								// 300us

#define		cLN_0_L_LEVL_MIN			( cRF_TICK*3-cRF_TICK-cRF_TICK/2 )		// + 1.5cRF_TICK
#define		cLN_0_L_LEVL_MAX			( cRF_TICK*3+cRF_TICK+cRF_TICK/2 )		// - 1.5cRF_TICK
#define		cLN_0_H_LEVL_MIN			(cRF_TICK-cRF_TICK/2)					// unit = 1us
#define		cLN_0_H_LEVL_MAX			(cRF_TICK+cRF_TICK)						// unit = 1us

#define		cLN_1_L_LEVL_MIN			(cRF_TICK-cRF_TICK/2)					// unit = 1us
#define		cLN_1_L_LEVL_MAX			(cRF_TICK+cRF_TICK)						// unit = 1us
#define		cLN_1_H_LEVL_MIN			( cRF_TICK*3-cRF_TICK-cRF_TICK/2 )		// + 1.5cRF_TICK
#define		cLN_1_H_LEVL_MAX			( cRF_TICK*3+cRF_TICK+cRF_TICK/2 )		// - 1.5cRF_TICK

#define		cLN_SYNC_H_LEVL_MIN			(cRF_TICK-cRF_TICK/2)					// unit = 1us
#define		cLN_SYNC_H_LEVL_MAX			(cRF_TICK+cRF_TICK)						// unit = 1us
#define		cLN_SYNC_L_LEVL_MIN			(cRF_TICK*31-cRF_TICK*4)				// unit = 1us
#define		cLN_SYNC_L_LEVL_MAX			(cRF_TICK*31+cRF_TICK*4)				// unit = 1us


#define		RF_FRAME_LENGTH			3
#define		RF_RX_FRAME_MAX			20

enum
{
	RF_STAGE_RX_SYNC = 0,									// receive sync
	RF_STAGE_RX_DATA,									// receive data
	RF_STAGE_RX_END,
};

#define	CMD_RF_CALL_SLAVE			0x9		// call slave
#define	CMD_RF_MID_TO_SLAVE			0XA		// set master ID to slave
#define	CMD_RF_SID_TO_SLAVE			0XB		// set slave ID to slave
#define	CMD_RF_ALERT_TIME			0XC		// set slave ID to slave
#define	CMD_RF_ALERT_MODE			0XD		// set slave ID to slave
#define	CMD_RF_SLAVE_OFF			0XE		// set slave ID to slave

#define	CONST_MASTER_ID					107
#define	CONST_SLAVE_ID					1

enum
{
	ALERT_MODE_NULL = 0,							// null
	ALERT_MODE_1 = 0xE1,							// led 
	ALERT_MODE_2 = 0XE2,							// buzzer
	ALERT_MODE_3 = 0XE3,							// vibration
	ALERT_MODE_4 = 0XE4,							// buzzer + vibration
	ALERT_MODE_5 = 0XE5,							// led + buzzer
	ALERT_MODE_6 = 0XE6,							// led + vibration
	ALERT_MODE_7 = 0XE7,							// led + buzzer + vibration
};

#define		CHECK_CODE_12BIT			0xCE7		// 用于设置命令时填充无效数据位

#define		EN_POW_OFF				0x0
#define		DIS_POW_OFF				0xff
#define		EN_PARA_SET				0x0
#define		DIS_PARA_SET			0xff
#define		EN_RF_RX				0x00
#define		DIS_RF_RX				0xff

#define		RF_CMD_SUCCESS			0X0
#define		RF_CMD_FAILED			0XFF

#define		RF_BASE_TIME_MIN		120							// 120US
#define		RF_BASE_TIME_MAX		600							// 600US
#define		INVALID_RF_BASE			0xffff

#define		LEVEL_HALF_TIME_MIN		200ul							// 50US
#define		LEVEL_HALF_TIME_MAX		400ul							// 600US
//----------------------- macro define ------------------------
enum
{
	FRAME_FILT_STAGE_PREPARE,
	FRAME_FILT_STAGE_CACHE,
	FRAME_FILT_STAGE_COMPARE,									// 帧比对
	FRAME_FILT_STAGE_CONTI,
	FRAME_FILT_STAGE_TIMEOUT,
};
//----------------------- macro define ------------------------
typedef	union _MyFameBuff_type_
{
	unsigned char arry[3];
	unsigned int value;
}MyFameBuff_t;
typedef	struct _MyFramePool_type_											// 定义一个环形帧缓冲区
{
	unsigned char Head;
	unsigned char Tail;
	unsigned char length;
	MyFameBuff_t	frames[RF_RX_FRAME_MAX];
}MyFramePool_t;

//--------------------- macro define ----------------------
#define	TIME_IDLE_BUFFER_CLEAN	(1000 / portTICK_PERIOD_MS)
#define	TIME_RF_BUFFER_CLEAN	(3000 / portTICK_PERIOD_MS)
//--------------------- macro define ----------------------
#define	PULSE_BAD_SIGNAL		100ul												// 无效信号脉宽
#define	VAL_BAD_SIG_NUM			3												// 无效信号个数判断
#define	VAL_OK_SIG_NUM			2												// 无效信号个数判断
//--------------------- macro define ----------------------
extern unsigned char MasterID;
extern unsigned int SlaveID;
extern unsigned int AlertTime;
extern unsigned char AlertMode;

//--------------------- function declare ----------------------
void rf_receiver(void);
unsigned char rf_cmd_execute(unsigned char *cmd);
void setting_enable(unsigned char flag);
void rf_decode_PinInt_isr(unsigned int levelTime);
void rf_decode_init(void);
void power_off_enable(unsigned char flag);
void setting_enable(unsigned char flag);
int my_compare_arrays(unsigned char *Arry1,int Ln1,unsigned char *Arry2,int Ln2);
void rf_frame_filter_task(void * pvParameters);
void clean_rf_frame_buffer(void);
int RF_module_sleep(void);
int RF_module_goto_rx(void);
void ResetBadSignalCnt(void);
void ResetOkSignalCnt(void);
void rf_decode_control(unsigned char flag);

extern void timer1_rf_init(void);
extern void timer1_rf_start(void);


#endif	//__RFDECODE_H__


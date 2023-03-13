#ifndef		__POEWR_KEY_H__
#define		__POEWR_KEY_H__

//--------------------- include files ---------------------
#include "gd32e10x.h"
//--------------------- macro define ----------------------
/*	P1 = PAD11,		P2 = PAD8,		P3 = PAD6,		P4 = PAD4,		P5 = PAD1,		P6 = PAD10,
	P7 = PAD9,		P8 = PAD7,		P9 = PAD2,		P10 = PAD5,		P11 = PAD0,		P12 = PAD3,
	PAD0 - PAD1 - PAD2 - PAD3 - PAD4 - PAD5 - PAD6 - PAD7 - PAD8 - PAD9 - PAD10 - PAD11	*/

// the following KEY_PAD0~KEY_PAD11 are corresponding to the chip specification
enum{KEY_PAD0, KEY_PAD1, KEY_PAD2, KEY_PAD3, KEY_PAD4, KEY_PAD5, KEY_PAD6, KEY_PAD7, KEY_PAD8, KEY_PAD9, KEY_PAD10, KEY_PAD11,};

// KEYVAL_P1~KEYVAL_P12 are corresponding to the board route
#define	KEYVAL_P1		(~(0x8000>>KEY_PAD11))							// PAD11
#define	KEYVAL_P2		(~(0x8000>>KEY_PAD8))							// PAD8
#define	KEYVAL_P3		(~(0x8000>>KEY_PAD6))							// PAD6
#define	KEYVAL_P4		(~(0x8000>>KEY_PAD4))							// PAD4
#define	KEYVAL_P5		(~(0x8000>>KEY_PAD1))							// PAD1
#define	KEYVAL_P6		(~(0x8000>>KEY_PAD10))							// PAD10
#define	KEYVAL_P7		(~(0x8000>>KEY_PAD9))							// PAD9
#define	KEYVAL_P8		(~(0x8000>>KEY_PAD7))							// PAD7
#define	KEYVAL_P9		(~(0x8000>>KEY_PAD2))							// PAD2
#define	KEYVAL_P10		(~(0x8000>>KEY_PAD5))							// PAD5
#define	KEYVAL_P11		(~(0x8000>>KEY_PAD0))							// PAD0
#define	KEYVAL_P12		(~(0x8000>>KEY_PAD3))							// PAD3
#define	KEYVAL_P9_a_P11	( ~((0x8000>>KEY_PAD2)+(0x8000>>KEY_PAD0)) )	// PAD2 + PAD0

#define	KEYVAL_POW	0						// 0
#define	KEYVAL_S1	0X45					// 0
#define	KEYVAL_S2	0X4D					// 1
#define	KEYVAL_S3	0X55					// 2
#define	KEYVAL_S4	0X5D					// 3
#define	KEYVAL_S5	0X46					// 4
#define	KEYVAL_S6	0X4E					// 5
#define	KEYVAL_S7	0X56					// 6
#define	KEYVAL_S8	0X5E					// 7
#define	KEYVAL_S9	0X4F					// 8
#define	KEYVAL_S10	0X57					// 9
#define	KEYVAL_S11	0X5F					// del
#define	KEYVAL_S12	0X47					// call
#define	KEYVAL_MOD	KEYVAL_P9_a_P11				// mode

#define	KEYVAL_ENTER		((0x0001<<KEY_PAD0))							// 
#define	KEYVAL_FUNCTION		((0x0001<<KEY_PAD1))							// 

enum
{
	STAGE_KEYDET_NULL,
	STAGE_KEYDET_START,
	STAGE_KEYDET_PRESSING,
	STAGE_KEYDET_DOWN,
	STAGE_KEYDET_LONG,
	STAGE_KEYDET_CONTI,
	STAGE_KEYDET_UP,
};
enum
{
	KEY_CODE_ENTER,	
	KEY_CODE_FUN, 
	KEY_MAX_NUM,
};

#define	KEY_CODE_INVALID		0xff

enum
{
	KEY_STAT_NULL,
	KEY_STAT_PRESSING,
	KEY_STAT_DOWN,
	KEY_STAT_UP,
	KEY_STAT_LONG,
	KEY_STAT_CONTINU,			// continuous key
};
typedef	struct _mKey_t_
{	
	unsigned int current_keyval;
	unsigned char keycode;
	unsigned char keystate;
}mKey_t;
//--------------------- macro define ----------------------
#define	KEY_TIME_PRESSING	2									// 2*10 = 20ms
#define	KEY_TIME_DOWN		10									// 10*10 = 100ms
#define	KEY_TIME_UP			3									// 3*10 = 30ms
#define	KEY_TIME_LONG		(200-KEY_TIME_DOWN)					// 200*10 = 1000ms
#define	KEY_TIME_CONTINU	10									// 10*10 = 100ms

#define	KEY_SCAN_EN			0X0
#define	nKEY_SCAN_DIS		0xff
#define	TIME_SCAN_DELAY		30									// 30*10 = 300ms
//--------------------- variable declare ----------------------
extern const unsigned char digit_tab[];
extern mKey_t	myKey;
//--------------------- function declare ----------------------
void key_scan(void);
void key_port_init(void);
unsigned int get_key_value(void);
unsigned char get_keycode(unsigned int current_keyval,unsigned char *keycode,unsigned char *keystate);



#endif	//__POEWR_KEY_H__


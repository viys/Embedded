/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "powerKey.h"
#include "MechanicalKey.h"
//--------------------- variable declare ----------------------
const unsigned int valid_keyval[] = 
{
	KEYVAL_ENTER,		KEYVAL_FUNCTION,
};
const unsigned char digit_tab[]= 
{// KEY_CODE_0, KEY_CODE_1, KEY_CODE_2, KEY_CODE_3, KEY_CODE_4, KEY_CODE_5, KEY_CODE_6, KEY_CODE_7, KEY_CODE_8, KEY_CODE_9,
		0,			1,			2,			3,			4,			5,			6,			7,			8,			9,
};
mKey_t	myKey = {0};


/*************************************************************
Function: 
Description: 
*************************************************************/
unsigned char is_valid_key(unsigned int keyval)
{
	unsigned char keycode;
	
	for(keycode=0;keycode<KEY_MAX_NUM;keycode++)
	{
		if(keyval==valid_keyval[keycode])
		{
			return(keycode);
		}
	}
	
	return(0xff);										// invalid code
}

/**********************************************************************************************
Function: 
Description: 
	P1 = PAD11,		P2 = PAD8,		P3 = PAD6,		P4 = PAD4,		P5 = PAD1,		P6 = PAD10,
	P7 = PAD9,		P8 = PAD7,		P9 = PAD2,		P10 = PAD5,		P11 = PAD0,		P12 = PAD3,
	PAD0 - PAD1 - PAD2 - PAD3 - PAD4 - PAD5 - PAD6 - PAD7 - PAD8 - PAD9 - PAD10 - PAD11
**********************************************************************************************/
unsigned char get_keycode(unsigned int current_keyval,unsigned char *keycode,unsigned char *keystate)
{
	static unsigned int pre_keyval = 0xffff;
	static int press_time = 0;
	static unsigned char DetStage = STAGE_KEYDET_START;
	static unsigned char temp_code = 0;
	static int release_time = 0;
	
	*keycode = KEY_CODE_INVALID;
	*keystate = KEY_STAT_NULL;
	switch(DetStage)
	{
		//-------------------------------------------------------------------------------
		case STAGE_KEYDET_START:
		{
			temp_code = is_valid_key(current_keyval);
			if(KEY_CODE_INVALID!=temp_code)							// valid key code 
			{
				pre_keyval = current_keyval;
				press_time = 0;
				release_time = 0;
				DetStage = STAGE_KEYDET_PRESSING;
			}
			break;
		}
		case STAGE_KEYDET_PRESSING:									// pressing detect
		{
			if(pre_keyval==current_keyval)
			{
				if(++press_time>=KEY_TIME_PRESSING)					// 20ms
				{
					press_time = 0;
					*keycode = temp_code;							// store key code for function key-deal
					*keystate = KEY_STAT_PRESSING;
					DetStage = STAGE_KEYDET_DOWN;
				}
			}
			else
			{
				if(press_time)		press_time--;
				if(0==press_time)	DetStage = STAGE_KEYDET_START;	// return of first step
			}
			break;
		}	
		//-------------------------------------------------------------------------------
		case STAGE_KEYDET_DOWN:														// key down detect
		{
			if(pre_keyval==current_keyval)
			{
				if(++press_time>=KEY_TIME_DOWN)						// 100ms
				{
					press_time = 0;
					*keycode = temp_code;							// store key code for function key-deal
					*keystate = KEY_STAT_DOWN;
					DetStage = STAGE_KEYDET_LONG;
				}
			}
			else
			{
				if(press_time)		press_time--;
				if(0==press_time)	DetStage = STAGE_KEYDET_START;						// return of first step
			}
			break;
		}	
		//-------------------------------------------------------------------------------
		case STAGE_KEYDET_LONG:														// long press detect
		{
			if(++press_time>=KEY_TIME_LONG)							// 
			{
				press_time = 0;
				*keycode = temp_code;								// store key code for function key-deal
				*keystate = KEY_STAT_LONG;
				DetStage = STAGE_KEYDET_CONTI;
			}
			
			if(pre_keyval!=current_keyval)							// release
			{
				if(++release_time>=KEY_TIME_UP)
				{
					press_time = 0;
					release_time = 0;
					DetStage = STAGE_KEYDET_UP;
				}
			}
			else
			{
				release_time = 0;
			}
			break;
		}	
		//-------------------------------------------------------------------------------
		case STAGE_KEYDET_CONTI:														// continuous press detect
		{
			if(++press_time>=KEY_TIME_CONTINU)						// 
			{
				press_time = 0;
				*keycode = temp_code;								// store key code for function key-deal
				*keystate = KEY_STAT_CONTINU;
			}
			
			if(pre_keyval!=current_keyval)							// release
			{
				if(++release_time>=KEY_TIME_UP)
				{
					press_time = 0;
					release_time = 0;
					DetStage = STAGE_KEYDET_START;
				}
			}
			else
			{
				release_time = 0;
			}
			break;
		}	
		//-------------------------------------------------------------------------------
		case STAGE_KEYDET_UP:														// release detect
		{
			*keycode = temp_code;									// store key code for function key-deal
			*keystate = KEY_STAT_UP;
			press_time = 0;
			release_time = 0;
			DetStage = STAGE_KEYDET_START;
			break;
		}
		default:
			break;
	}
	
	return 0;
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void key_port_init(void)
{
	MechanicalKey_init();
	
	myKey.current_keyval = 0xffff;
	myKey.keycode = KEY_CODE_INVALID;
	myKey.keystate = KEY_STAT_NULL;
}





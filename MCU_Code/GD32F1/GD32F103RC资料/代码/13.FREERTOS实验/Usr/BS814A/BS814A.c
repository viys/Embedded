/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "BS814A.h"
//--------------------- variable declare ----------------------


/*************************************************************
Function: 
Description: 
	AIO01 active when low level
*************************************************************/
unsigned int get_key_bs814a(void)
{
	if(!READ_KEY_SET())			return(0x01);
	else if(!READ_KEY_UP())		return(0x02);
	else if(!READ_KEY_DOWN())	return(0x04);
	else if(!READ_KEY_RETURN())	return(0x08);
	else						return(0xffff);
	
}


/*************************************************************
Function: 
Description: 
*************************************************************/
void bs814a_port_init(void)
{
	KEY_SET_ININT();
	KEY_UP_ININT();
	KEY_DOWN_ININT();
	KEY_RETURN_ININT();
}


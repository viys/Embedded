#ifndef		__IAP_ONCHIP_H__
#define		__IAP_ONCHIP_H__

//--------------------- include files ---------------------
#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_define.h"
//--------------------- macro define ----------------------
//--------------------- variable declare ------------------
//--------------------- function declare ------------------
void read_from_APROM(unsigned int u16AddrAprom, unsigned char *buff, unsigned int length);
void write_to_APROM(unsigned int u16AddrAprom, unsigned char *buff, unsigned int length);




#endif	//__IAP_ONCHIP_H__


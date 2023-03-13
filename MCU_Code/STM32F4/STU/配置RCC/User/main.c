#include "stm32f4XX.h"
#include "bsp_clkcongfig.h"
int main(void)
{
	/* 此处写入程序 */
	
	User_HSE_SetSysClock(25,336,2,7);
	User_HEI_SetSysClock(16,336,2,7);
	
	while(1)
	{
		
	}
}


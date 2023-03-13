#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "rccclk.h"
#include "systick.h"
#include "stm32f10x_it.h"
#include "usart.h"
#include <stdio.h>
#include "dma.h"
#include "i2c.h"
#include "flash_spi.h"
#include "adc.h"
#include "adc_dma.h"
#include "basictim.h"
#include "gentim.h"
#include "remote.h"
#include "advancetim.h"
#include "wdg.h"
#include "oled.h"
#include "bmp.h"

extern uint16_t ADC_ConvertedValue;
#define FILE_SYSTEM 0 //是否挂载文件系统
void conection(void);
extern uint8_t Revolve;
#if FILE_SYSTEM
#include "ff.h"
#include "fatfsuser.h"
FATFS fsObject;
FIL fp;
const char wData[] = "wellcome";
char rData[4096] = "";
//char fpath[100]; //保存当前扫描路径
DIR* dp;
UINT bw;
UINT br;
#endif

int main()
{
	//HSI_SetSysClock(RCC_PLLMul_2);
	
	uint16_t Current_Temp;
	extern uint16_t ADC_Temp_ConvertedValue;
	extern led_page_typedef_enum oled_page;
	//配置系统时钟为72M
	//HSE_SetSysCLK(RCC_PLLMul_9);
	//MCO_GPIO_Config();
	//LED_GPIO_Config(LED3);
	//LED_State_Config(LED3, ON);
	//EXTI_Key_Config(WKUP,1,1);
	USART_Config(USART_1, USART_MODE_COMMON,115200);
	
#if FILE_SYSTEM
	
	//挂载文件系统
	FRESULT res;
	res = f_mount(&fsObject, "1:", 1);
	printf("f_mount res =%d\r\n",res);
	if(res == FR_NO_FILESYSTEM)
	{
		f_mkfs("1:", 0, 0);
		res = f_mount(NULL, "1:", 1);
		res = f_mount(&fsObject, "1:", 1);
		printf("sencond f_mount res =%d\r\n",res);
	}
	
	res = f_open(&fp, "1:demo.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	printf("sencond f_open res =%d\r\n",res);
	if(FR_OK == res)
	{
		res = f_write(&fp, "wellcome", sizeof(wData), &bw);
		
		if(FR_OK == res)
		{
			f_lseek(&fp, 0);
			res = f_read(&fp, rData, f_size(&fp), &br);
			if(FR_OK == res)
			{
				printf("文件内容: %s\r\n",rData);
			}
		}
	}
	
	f_close(&fp);
	f_mkdir("1:/2022");
	f_open(&fp, "1:/2022/demo.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	EXTI_Key_Config(WKUP, 1, 1);
	f_close(&fp);
#endif

    

	Remote_Init();
	GENERAL_TIM3_Init(CH4,1,240,120);
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	//ADC1_temperature_Init();
	ADCx_Init();
	
	while(1)
    {	
		printf("ADC:%d\r\n",ADC_ConvertedValue);
		OLED_Clear();
		if(oled_page == PAGE_HOME)
		{	
			//OLED_Clear();
			Current_Temp=(V25-ADC_Temp_ConvertedValue)/AVG_SLOPE+25;
			OLED_ShowChinese(0,36,15,16,1);//温
			OLED_ShowChinese(18,36,16,16,1);//度
			OLED_ShowString(38,36,":",16,1);  
			OLED_ShowNum(56,36,Current_Temp,2,16,1);
			OLED_ShowChinese(74,36,17,16,1);//机
			OLED_ShowString(80,36,"C",16,1);
			OLED_ShowChinese(0,0,0,16,1);//舵
			OLED_ShowChinese(18,0,1,16,1);//机
			if(0==Revolve)
			{
				OLED_ShowChinese(36,0,5,16,1);//停
				OLED_ShowChinese(54,0,6,16,1);//止
			}else if(1==Revolve)
			{
				OLED_ShowChinese(36,0,14,16,1);//顺
				OLED_ShowChinese(54,0,4,16,1);//转
			}else if(2==Revolve)
			{
				OLED_ShowChinese(36,0,13,16,1);//逆
				OLED_ShowChinese(54,0,4,16,1);//转
			}
			OLED_ShowChinese(0,18,7,16,1);//灯
			OLED_ShowChinese(18,18,8,16,1);//光
			if(LED_ON == LED_Scan(LED3))
			{
				OLED_ShowChinese(36,18,9,16,1);//打
				OLED_ShowChinese(54,18,10,16,1);//开
			}else
			{
				OLED_ShowChinese(36,18,11,16,1);//打
				OLED_ShowChinese(54,18,12,16,1);//开
			}
			OLED_Refresh();
			OLED_ShowString(108,48,"P0",16,1);
		}else if(oled_page == PAGE_1)
		{
			OLED_ShowChinese(18,0,18,16,1);//第
			OLED_ShowChinese(36,0,19,16,1);//二
			OLED_ShowChinese(54,0,20,16,1);//页
			OLED_ShowString(108,48,"P1",16,1);
			OLED_Refresh();
		}else if(oled_page == PAGE_END)
		{
			OLED_ShowString(80,18,"END",16,1);
			
			OLED_ShowString(108,36,"P2",16,1);
			OLED_Refresh();
		}
		SysTick_Delay(100);
    }

}


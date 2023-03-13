/* Includes ------------------------------------------------------------------*/
#include "gd32e10x.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Lcd_Driver.h"
#include "GUI.h"
#include "Picture.h"
#include "QDTFT_demo.h"
#include "Icon.h"
//--------------------- function declare ----------------------
#ifndef		delay_1ms
#define		delay_1ms(x)		vTaskDelay(x / portTICK_PERIOD_MS);
#endif
//--------------------- variable declare ----------------------
unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"电子技术");
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"液晶测试程序");

	DisplayButtonUp(15,38,113,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,40,YELLOW,GRAY0,"颜色填充测试");

	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,70,BLUE,GRAY0,"文字显示测试");

	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,100,RED,GRAY0,"图片显示测试");
	delay_1ms(1500);
}

void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	delay_1ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+5,RED,GRAY0,Num[i+1]);
	delay_1ms(100);
	}
	
}

void Font_Test(void)
{
	Lcd_Clear(BLUE);
//	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"文字显示测试");

//	delay_1ms(1000);
//	Lcd_Clear(GRAY0);
//	Gui_DrawFont_GBK16(16,30,BLACK,GRAY0,"我我我电我我我");
//	Gui_DrawFont_GBK16(16,50,BLUE,GRAY0,"专注液晶批发");
//	Gui_DrawFont_GBK16(16,70,RED,GRAY0, "全程技术支持");
//	Gui_DrawFont_GBK16(0,100,BLUE,GRAY0,"Tel:11111111111");
//	Gui_DrawFont_GBK16(0,130,RED,GRAY0, "QQ:111111111");	
//	delay_1ms(1800);
	
//	LCD_DrawRegion(0,30,128,49,ORRANGE);
//	Gui_DrawFont_GBK16(16,30,BLACK,ORRANGE,"测试1");
	
// 24点	
//	Gui_DrawFont_GBK24(40,0,WHITE,BLUE,"设置");	
//	
//	LCD_DrawRegion(0,26,128,50,ORRANGE);
//	Gui_DrawFont_GBK24(0,26,WHITE,ORRANGE,"计时器");	
//	
//	Gui_DrawFont_GBK24(0,51,WHITE,BLUE,"定时开关机");	
//	Gui_DrawFont_GBK24(0,76,WHITE,BLUE,"定时开关机");	
//	Gui_DrawFont_GBK24(0,101,WHITE,BLUE,"定时开关机");	

// 16点	
	//Gui_DrawFont_GBK16(48,4,WHITE,BLUE,"设置");	
	
	//LCD_DrawRegion(0,26,128,50,ORRANGE);
	//Gui_DrawFont_GBK16(4,26+4,WHITE,ORRANGE,"计时器");	
	
	//Gui_DrawFont_GBK16(4,51+4,WHITE,BLUE,"定时开关机");	
	//Gui_DrawFont_GBK16(4,76+4,WHITE,BLUE,"123开关机");	
	//Gui_DrawFont_GBK16(4,101+4,WHITE,BLUE,"定计时开关机");
	//Gui_DrawFont_GBK12_8(4,101+4,WHITE,BLUE,"123456");	
	
	//Gui_DrawFont_GBK32_16(4,51+4,WHITE,BLUE,"123456");
	
	//drawIcon_20_20(0,0,WHITE,BLUE,icon_shakeOnly_20_20);
	//drawIcon_20_20(21,0,WHITE,BLUE,icon_silence_20_20);
	//drawIcon_20_20(81,0,WHITE,BLUE,icon_voice_20_20);
	//drawIcon_20_20(101,0,WHITE,BLUE,icon_shakeRing_20_20);
	//drawIcon_20_20(101,0,WHITE,BLUE,icon_antenna_20_20);
	//drawIcon_20_20(101,0,WHITE,BLUE,icon_fly_mod_20_20);
	//drawIcon_20_20(81,0,WHITE,BLUE,icon_alarm_20_20);
	//drawIcon_20_20(101,0,WHITE,BLUE,icon_alarm1_20_20);
	

// 18点	
//	Gui_DrawFont_GBK18(46,4,WHITE,BLUE,"设置");	
//	
//	LCD_DrawRegion(0,26,128,50,ORRANGE);
//	Gui_DrawFont_GBK18(4,26+3,WHITE,ORRANGE,"计时器");	
//	
//	Gui_DrawFont_GBK18(4,51+3,WHITE,BLUE,"定时开关机");	
//	Gui_DrawFont_GBK18(4,76+3,WHITE,BLUE,"定时开关机");	
//	Gui_DrawFont_GBK18(4,101+3,WHITE,BLUE,"定时开关机");	


// 待机界面

// 顶上图标
	//Gui_DrawFont_GBK16(2,4,WHITE,BLUE,"999");	
	Gui_DrawFont_GBK16(2,4,WHITE,BLUE,"DMY");	
	//drawIcon_20_20(28,2,WHITE,BLUE,icon_sandClock_20_20);
	//drawIcon_20_20(48,2,WHITE,BLUE,icon_alarm1_20_20);
	drawIcon_20_20(68,2,WHITE,BLUE,icon_shakeRing_20_20);
	drawIcon_20_20(88,3,WHITE,BLUE,icon_fly_mod_20_20);
	drawIcon_20_20(108,2,WHITE,BLUE,icon_batteryFrame_20_20);
// 刷电量区域：xStart=113,xEnd=124;yStart=8,yEnd=15
	LCD_DrawRegion(113,8,124,15,GREEN);
	
// 中间时间
	Gui_DrawFont_GBK32_16(19,46,WHITE,BLUE,"12");
	Gui_DrawFont_GBK32_16(19+36,46,WHITE,BLUE,":");
	Gui_DrawFont_GBK32_16(19+36+18,46,WHITE,BLUE,"00");
	
// 底下日期
	Gui_DrawFont_GBK32_16(19,46,WHITE,BLUE,"12");
	Gui_DrawFont_GBK16(2,110,WHITE,BLUE,"2020/10/21");	
	Gui_DrawFont_GBK16(2+6*10+28,110,WHITE,BLUE,"开机");	
	
	
// 刷滑动开关 	
//	drawIcon_32_14(32,88,GREEN,BLUE,icon_scrollSwitchL_32_14);
//	drawIcon_32_14(32,88,BLACK,BLACK,icon_scrollSwitch_Frame_32_14);
//	
//	drawIcon_32_14(32+32+10,88,GREEN,BLUE,icon_scrollSwitchR_ON_32_14);
//	drawIcon_32_14(32+32+10,88,BLACK,BLACK,icon_scrollSwitch_Frame_32_14);
//	
//	drawIcon_16_8(2,88,GREEN,BLUE,icon_littleScrollSwitch_OFF_16_8);
//	drawIcon_16_8(2,88+10,GREEN,BLUE,icon_littleScrollSwitch_ON_16_8);

//  0-9小数字
	Gui_DrawFont_asc2_5x7(5,88,GREEN,BLUE,"0123456789");
	
	Gui_DrawFont_asc12x24(80,88,GREEN,BLUE,"012");

// 小对勾
	drawIcon_16x16(15,25,GREEN,BLUE,icon_tick_empty_16_16);
	//drawIcon_16x16(15,25,GREEN,BLUE,icon_tick_select_16_16);
	drawIcon_16x16(15+20,25,GREEN,BLUE,icon_tick_select_16_16);
	drawIcon_16x16(15+20+20,25,GREEN,BLUE,icon_circle_empty_16_16);
	drawIcon_16x16(15+20+20+20,25,GREEN,BLUE,icon_tick_empty1_16_16);
	
	
}

void Color_Test(void)
{
	u8 i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	delay_1ms(500);

	while(i--)
	{
		Lcd_Clear(WHITE);
		delay_1ms(500);
		Lcd_Clear(BLACK);
		delay_1ms(500);
		Lcd_Clear(RED);
		delay_1ms(500);
	  	Lcd_Clear(GREEN);
		delay_1ms(500);
	  	Lcd_Clear(BLUE);
		delay_1ms(500);
	}		
}

//取模方式 水平扫描 从左到右 低位在前
void showimage(const unsigned char *p) //显示40*40 QQ图片
{
  	int i,j,k; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //清屏  
	
	for(k=0;k<3;k++)
	{
	   	for(j=0;j<3;j++)
		{	
			Lcd_SetRegion(40*j,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}

/****************************************************************************************
Function:
Description:
			刷20像素*20像素图标
****************************************************************************************/
void drawIcon_16x16(u16 xStart, u16 yStart, u16 fc, u16 bc,const unsigned char *p)
{
	#define	ICON_16X16_ROW		16
 	#define	ICON_16X16_COLUMN	16
 	int i,j; 

	for(i=0;i<ICON_16X16_COLUMN;i++)													// 刷16行
	{
		for(j=0;j<8;j++)															// 先取前半行数据
		{
			if( p[i*2]&(0x80>>j) )	Gui_DrawPoint(xStart+j,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*2+1]&(0x80>>j) )	Gui_DrawPoint(xStart+j+8,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+8,yStart+i,bc);
			}
		}
	}		
}

/****************************************************************************************
Function:
Description:
			刷20像素*20像素图标
****************************************************************************************/
void drawIcon_20_20(u16 xStart, u16 yStart, u16 fc, u16 bc,const unsigned char *p)
{
	#define	ICON_20_COLUMN	20
  	int i,j; 

	for(i=0;i<ICON_20_COLUMN;i++)
	{
		for(j=0;j<8;j++)															// 先取前半行数据
		{
			if( p[i*3]&(0x80>>j) )	Gui_DrawPoint(xStart+j,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*3+1]&(0x80>>j) )	Gui_DrawPoint(xStart+j+8,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+8,yStart+i,bc);
			}
		}
		for(j=0;j<4;j++)															// 再取后半行数据
		{
			if( p[i*3+2]&(0x80>>j) )	Gui_DrawPoint(xStart+j+16,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+16,yStart+i,bc);
			}
		}
	}		
}


/****************************************************************************************
Function:
Description:
			刷32像素*14像素图标
****************************************************************************************/
void drawIcon_32_14(u16 xStart, u16 yStart, u16 fc, u16 bc,const unsigned char *p)
{
	#define	ICON_14_COLUMN	14
	
  	int i,j; 
	int local_column = ICON_14_COLUMN;

	for(i=0;i<local_column;i++)
	{
		for(j=0;j<8;j++)															// 先取前半行数据
		{
			if( p[i*4]&(0x80>>j) )	Gui_DrawPoint(xStart+j,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*4+1]&(0x80>>j) )	Gui_DrawPoint(xStart+j+8,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+8,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*4+2]&(0x80>>j) )	Gui_DrawPoint(xStart+j+16,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+16,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*4+3]&(0x80>>j) )	Gui_DrawPoint(xStart+j+24,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+24,yStart+i,bc);
			}
		}
	}
	
}

/****************************************************************************************
Function:
Description:
			刷32像素*14像素图标
****************************************************************************************/
void drawIcon_16_8(u16 xStart, u16 yStart, u16 fc, u16 bc,const unsigned char *p)
{
	#define	ICON_8_COLUMN	8
	
  	int i,j; 
	int local_column = ICON_8_COLUMN;

	for(i=0;i<local_column;i++)
	{
		for(j=0;j<8;j++)															// 先取前半行数据
		{
			if( p[i*2]&(0x80>>j) )	Gui_DrawPoint(xStart+j,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j,yStart+i,bc);
			}
		}
		for(j=0;j<8;j++)															// 再取后半行数据
		{
			if( p[i*2+1]&(0x80>>j) )	Gui_DrawPoint(xStart+j+8,yStart+i,fc);
			else 
			{
				if (fc!=bc) Gui_DrawPoint(xStart+j+8,yStart+i,bc);
			}
		}
	}
	
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
void QDTFT_Test_Demo(void)
{
	Lcd_Init();
	LCD_LED_SET;//通过IO控制背光亮				
	//Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
	//Color_Test();//简单纯色填充测试
	//Num_Test();//数码管字体测试
	Font_Test();//中英文显示测试		
	//showimage(gImage_qq);//图片显示示例
	delay_1ms(1200);
	//LCD_LED_CLR;//IO控制背光灭	
	//delay_1ms(120);
}

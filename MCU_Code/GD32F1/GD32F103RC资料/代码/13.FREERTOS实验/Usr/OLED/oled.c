/****************************************************************************************
file name:
Description:
****************************************************************************************/
//---------------------------------- include files ----------------------------
#include "oledPort.h"
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "bmp.h"
//--------------------------------- variable declare --------------------------
unsigned char OLED_GRAM[144][4] = {0};


/****************************************************************************************
Function:
Description:
****************************************************************************************/
static void delay_1ms(unsigned int time)
{
	unsigned int i,j;
	
	for(i=0;i<time;i++)
	{
		for(j=0;j<5000;j++)
		{
			__nop();__nop();__nop();__nop();
			__nop();__nop();__nop();__nop();
			__nop();__nop();__nop();__nop();
			__nop();__nop();__nop();__nop();
		}
	}
}

/****************************************************************************************
Function:
Description:
//反显函数
****************************************************************************************/
void OLED_ColorTurn(unsigned char i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

/****************************************************************************************
Function:
Description:
//屏幕旋转180度
****************************************************************************************/
void OLED_DisplayTurn(unsigned char i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

/****************************************************************************************
Function:
Description:
//延时
****************************************************************************************/
void IIC_delay(void)
{
	unsigned char t=3;
	while(t--);
}

/****************************************************************************************
Function:
Description:
//起始信号
****************************************************************************************/
void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

/****************************************************************************************
Function:
Description:
//结束信号
****************************************************************************************/
void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

/****************************************************************************************
Function:
Description:
//等待信号响应
****************************************************************************************/
void I2C_WaitAck(void) //测数据信号的电平
{
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}

/****************************************************************************************
Function:
Description:
//写入一个字节
****************************************************************************************/
void Send_Byte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDA_Set();
    }
		else
		{
			OLED_SDA_Clr();
    }
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();//将时钟信号设置为低电平
		dat<<=1;
  }
}

/****************************************************************************************
Function:
Description:
//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
****************************************************************************************/
void OLED_WR_Byte(unsigned char dat,unsigned char mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

/****************************************************************************************
Function:
Description:
//开启OLED显示 
****************************************************************************************/
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

/****************************************************************************************
Function:
Description:
//关闭OLED显示 
****************************************************************************************/
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}

/****************************************************************************************
Function:
Description:
//更新显存到OLED	
****************************************************************************************/
void OLED_Refresh(void)
{
	unsigned char i,n;
	for(i=0;i<4;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}


/****************************************************************************************
Function:
Description:
//清屏函数
****************************************************************************************/
void OLED_Clear(void)
{
	unsigned char i,n;
	for(i=0;i<4;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}

/****************************************************************************************
Function:
Description:
//清屏函数
****************************************************************************************/
void OLED_Pour(void)
{
	unsigned char i,n;
	for(i=0;i<4;i++)
	{
	   for(n=0;n<128;n++)
	   {
		  OLED_GRAM[n][i]=0xff;//数据全部置位
	   }
    }
	OLED_Refresh();//更新显示
}


/****************************************************************************************
Function:
Description:
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	
****************************************************************************************/
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
	unsigned char i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

/****************************************************************************************
Function:
Description:
//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
****************************************************************************************/
void OLED_DrawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char mode)
{
	unsigned short int t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/****************************************************************************************
Function:
Description:
//x,y:圆心坐标
//r:圆的半径
****************************************************************************************/
void OLED_DrawCircle(unsigned char x,unsigned char y,unsigned char r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}


/****************************************************************************************
Function:
Description:
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char size1,unsigned char mode)
{
	unsigned char i,m,temp,size2,chr1;
	unsigned char x0=x,y0=y,yStart=y;
	
	if( size1==8 )	size2=6;
	else 			
		size2=(size1/8+((size1%8)?1:0))*(size1/2);				//得到字体一个字符对应点阵集所占的字节数	
	chr1=chr - ' ';												//计算偏移后的值	
	
	for(i=0;i<size2;i++)										// 行循环，size2=行数
	{
		if(size1==8)
		{temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else if(size1==32)
		{temp=asc2_3216[chr1][i];} //调用3216字体
		else return;
		
		for(m=0;(m<8&&(y-yStart)<size1);m++)					// 描8点
		{
			if(temp&0x01)	OLED_DrawPoint(x,y,mode);
			else 			OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;												// y座标加1
		}
		
		x++;													// 刷完一列，切换到下一列
		if( (size1!=8)&&((x-x0)==size1/2) )						// 正好刷完8行。
		{
			x=x0;												// 切换回行首
			y0=y0+8;											// 刷第二个8行
		}
		y=y0;
	}
}

/****************************************************************************************
Function:
Description:
// 显示字符串
// x,y:起点坐标  
// size1:字体大小 
// *chr:字符串起始地址 
// mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr,unsigned char size1,unsigned char mode)
{
	while((*chr>=' ')&&(*chr<='~'))				//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)	x+=6;
		else			x+=size1/2;
		chr++;
	}
}

/****************************************************************************************
Function:
Description:
//m^n
****************************************************************************************/
unsigned int OLED_Pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

/****************************************************************************************
Function:
Description:
//显示数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size1,unsigned char mode)
{
	unsigned char t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

/****************************************************************************************
Function:
Description:
//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ShowChinese(unsigned char x,unsigned char y,unsigned char num,unsigned char size1,unsigned char mode)
{
	unsigned char m,temp;
	unsigned char x0=x,y0=y;
	unsigned short int i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//调用16*16字体
		else if(size1==24)
				{temp=Hzk2[num][i];}//调用24*24字体
		else if(size1==32)       
				{temp=Hzk3[num][i];}//调用32*32字体
		else if(size1==64)
				{temp=Hzk4[num][i];}//调用64*64字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}

/****************************************************************************************
Function:
Description:
//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ScrollDisplay(unsigned char num,unsigned char space,unsigned char mode)
{
	unsigned char i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,8,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<4;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //实现左移
		{
			for(n=0;n<4;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

/****************************************************************************************
Function:
Description:
//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
****************************************************************************************/
void OLED_ShowPicture(unsigned char x,unsigned char y,unsigned char sizex,unsigned char sizey,unsigned char BMP[],unsigned char mode)
{
	unsigned short int j=0;
	unsigned char i,n,temp,m;
	unsigned char x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
		 }
	 }
}


/****************************************************************************************
Function:
Description:
//OLED的初始化
****************************************************************************************/
void OLED_Init(void)
{
	oled_port_init();
	
	OLED_RES_Clr();
	delay_1ms(200);
	OLED_RES_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
	OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/ 
	OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/
	OLED_WR_Byte(0x00,OLED_CMD); /*set display start line*/ 
	OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/ 
	OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
	OLED_WR_Byte(0xff,OLED_CMD); /*128*/ 
	OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/ 
	OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/ 
	OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
	OLED_WR_Byte(0x1F,OLED_CMD); /*duty = 1/32*/ 
	OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/ 
	OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
	OLED_WR_Byte(0x00,OLED_CMD); 
	OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
	OLED_WR_Byte(0x80,OLED_CMD); 
	OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
	OLED_WR_Byte(0x1f,OLED_CMD); 
	OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/ 
	OLED_WR_Byte(0x00,OLED_CMD); 
	OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/ 
	OLED_WR_Byte(0x40,OLED_CMD); 
	OLED_WR_Byte(0x8d,OLED_CMD); /*set charge pump enable*/ 
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
void oled_test(void)
{
	//unsigned char t=' ';
	
//	OLED_ShowPicture(0,0,128,32,BMP1,1);
//	OLED_Refresh();
//	delay_1ms(500);
//	OLED_Clear();
//	OLED_ShowChinese(0,0,0,16,1);//中
//	OLED_ShowChinese(18,0,1,16,1);//景
//	OLED_ShowChinese(36,0,2,16,1);//园
//	OLED_ShowChinese(54,0,3,16,1);//电
//	OLED_ShowChinese(72,0,4,16,1);//子
//	OLED_ShowChinese(90,0,5,16,1);//科
//	OLED_ShowChinese(108,0,6,16,1);//技
//	OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
//	OLED_Refresh();
//	delay_1ms(500);
	OLED_Clear();
	OLED_ShowString(0,0,(unsigned char*)"12:00",32,1);
	OLED_ShowPicture(115,0,13,7,(unsigned char*)BMP_bat_13x7,1);
	OLED_ShowPicture(103,0,10,12,(unsigned char*)BMP_Alarm_10x12,1);

//	OLED_Pour();
//	OLED_ShowString(0,16,"ASCII:",16,1);  
//	OLED_ShowString(63,16,"CODE:",16,1);
//	OLED_ShowChar(48,16,t,16,1);//显示ASCII字符	   
//	t++;
//	if(t>'~')t=' ';
//	OLED_ShowNum(103,16,t,3,16,1);
	OLED_Refresh();
//	delay_1ms(500);
//	OLED_Clear();
//	OLED_ShowChinese(0,0,0,16,1); //16*16 中
//	OLED_ShowChinese(16,0,0,24,1);//24*24 中
//	OLED_ShowChinese(40,0,0,32,1);//32*32 中
//	OLED_Refresh();
//	delay_1ms(500);
//	OLED_Clear();
//	OLED_ShowString(0,7,"A",8,1);
//	OLED_ShowString(18,4,"B",12,1);
//	OLED_ShowString(36,2,"C",16,1);
//	OLED_ShowString(56,0,"D",24,1);
//	OLED_Refresh();
//	delay_1ms(500);
//	OLED_ScrollDisplay(11,4,1);
}


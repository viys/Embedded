/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6开发板
	微信号:gzwelink

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "oled.h"

typedef unsigned char u8;
typedef unsigned long u32;
void Init18b20 (void);
void WriteByte (unsigned char wr);  //单字节写入
void read_bytes (unsigned char j);
unsigned char Temp_CRC (unsigned char j);
void GemTemp (void);
void Config18b20 (void);
void ReadID (void);
void TemperatuerResult(void);
void SystemClock_Config(void);
unsigned long Count;
//1us等待，108M主频
void Delay_us(unsigned long i)
{
	unsigned long j;
	for(;i>0;i--)
	{
			for(j=24;j>0;j--);
	}
}
unsigned char  flag;
unsigned int   Temperature;
unsigned char  temp_buff[9]; //存储读取的字节，read scratchpad为9字节，read rom ID为8字节
unsigned char  id_buff[8];
unsigned char  *p;
unsigned char  crc_data;



const unsigned char  CrcTable [256]={
0,  94, 188,  226,  97,  63,  221,  131,  194,  156,  126,  32,  163,  253,  31,  65,
157,  195,  33,  127,  252,  162,  64,  30,  95,  1,  227,  189,  62,  96,  130,  220,
35,  125,  159,  193,  66,  28,  254,  160,  225,  191,  93,  3,  128,  222,  60,  98,
190,  224,  2,  92,  223,  129,  99,  61,  124,  34,  192,  158,  29,  67,  161,  255,
70,  24,  250,  164,  39,  121,  155,  197,  132,  218,  56,  102,  229,  187,  89,  7,
219,  133, 103,  57,  186,  228,  6,  88,  25,  71,  165,  251,  120,  38,  196,  154,
101,  59, 217,  135,  4,  90,  184,  230,  167,  249,  27,  69,  198,  152,  122,  36,
248,  166, 68,  26,  153,  199,  37,  123,  58,  100,  134,  216,  91,  5,  231,  185,
140,  210, 48,  110,  237,  179,  81,  15,  78,  16,  242,  172,  47,  113,  147,  205,
17,  79,  173,  243,  112,  46,  204,  146,  211,  141,  111,  49,  178,  236,  14,  80,
175,  241, 19,  77,  206,  144,  114,  44,  109,  51,  209,  143,  12,  82,  176,  238,
50,  108,  142,  208,  83,  13,  239,  177,  240,  174,  76,  18,  145,  207,  45,  115,
202,  148, 118,  40,  171,  245,  23,  73,  8,  86,  180,  234,  105,  55,  213, 139,
87,  9,  235,  181,  54,  104,  138,  212,  149,  203,  41,  119,  244,  170,  72,  22,
233,  183,  85,  11,  136,  214,  52,  106,  43,  117,  151,  201,  74,  20,  246,  168,
116,  42,  200,  150,  21,  75,  169,  247,  182,  232,  10,  84,  215,  137,  107,  53}; 


/************************************************************
*Function:18B20初始化
*parameter:
*Return:
*Modify:
*************************************************************/
void Init18b20 (void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

	gpio_bit_write(GPIOA, GPIO_PIN_0,SET);
	Delay_us(2); //延时2微秒

 gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);

	Delay_us(490);   //delay 530 uS//80
	 gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);
	Delay_us(100);   //delay 100 uS//14
	if(gpio_input_bit_get(GPIOA ,GPIO_PIN_0)== 0)
		flag = 1;   //detect 1820 success!
	else
		flag = 0;    //detect 1820 fail!
	Delay_us(480);        //延时480微秒
gpio_bit_write(GPIOA, GPIO_PIN_0,SET);
}

/************************************************************
*Function:向18B20写入一个字节
*parameter:
*Return:
*Modify:
*************************************************************/
void WriteByte (unsigned char  wr)  //单字节写入
{
	unsigned char  i;
	for (i=0;i<8;i++)
	{
	 gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);
    	Delay_us(2);
		if(wr&0x01)	 gpio_bit_write(GPIOA, GPIO_PIN_0,SET);
		else   gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);
		Delay_us(45);   //delay 45 uS //5
		
		gpio_bit_write(GPIOA, GPIO_PIN_0,SET);
		wr >>= 1;
	}
}

/************************************************************
*Function:读18B20的一个字节
*parameter:
*Return:
*Modify:
*************************************************************/
unsigned char ReadByte (void)     //读取单字节
{
	unsigned char  i,u=0;
	for(i=0;i<8;i++)
	{		
		gpio_bit_write(GPIOA, GPIO_PIN_0,RESET);
		Delay_us (2);
		u >>= 1;
	
		gpio_bit_write(GPIOA, GPIO_PIN_0,SET);
		Delay_us (4);
		if(gpio_input_bit_get(GPIOA , GPIO_PIN_0) == 1)
		u |= 0x80;
		Delay_us (65);
	}
	return(u);
}

/************************************************************
*Function:读18B20
*parameter:
*Return:
*Modify:
*************************************************************/
void read_bytes (unsigned char  j)
{
	 unsigned char  i;
	 for(i=0;i<j;i++)
	 {
		  *p = ReadByte();
		  p++;
	 }
}

/************************************************************
*Function:CRC校验
*parameter:
*Return:
*Modify:
*************************************************************/
unsigned char Temp_CRC (unsigned char j)
{
   	unsigned char  i,crc_data=0;
  	for(i=0;i<j;i++)  //查表校验
    	crc_data = CrcTable[crc_data^temp_buff[i]];
    return (crc_data);
}

/************************************************************
*Function:读取温度
*parameter:
*Return:
*Modify:
*************************************************************/
void GemTemp (void)
{
   read_bytes (9);
   if (Temp_CRC(9)==0) //校验正确
   {
	  Temperature = temp_buff[1]*0x100 + temp_buff[0];
		Temperature /= 16;
		Delay_us(10);
    }
}

/************************************************************
*Function:内部配置
*parameter:
*Return:
*Modify:
*************************************************************/
void Config18b20 (void)  //重新配置报警限定值和分辨率
{
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0x4e);  //write scratchpad
     WriteByte(0x19);  //上限
     WriteByte(0x1a);  //下限
     WriteByte(0x7f);     //set 12 bit (0.125)
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0x48);  //保存设定值
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0xb8);  //回调设定值
}

/************************************************************
*Function:读18B20ID
*parameter:
*Return:
*Modify:
*************************************************************/
void ReadID (void)//读取器件 id
{
	Init18b20();
	WriteByte(0x33);  //read rom
	read_bytes(8);
}

/************************************************************
*Function:18B20ID全处理
*parameter:
*Return:
*Modify:
*************************************************************/
void TemperatuerResult(void)
{
  	p = id_buff;
  	ReadID();
  Config18b20();
	Init18b20 ();
	delay_1ms(20);
	WriteByte(0xcc);   //skip rom
	WriteByte(0x44);   //Temperature convert

	Init18b20 ();
	delay_1ms(20);
	WriteByte(0xcc);   //skip rom
	WriteByte(0xbe);   //read Temperature
	p = temp_buff;
	GemTemp();
}


void GetTemp(void)
{       
   if(Count == 2) //每隔一段时间读取温度
	{  
		 Count=0;
	   TemperatuerResult();
	}
	Count++;

}
uint32_t TimeCOunter;
int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);//AHB主频是1分频
	systick_config();//系统主频108MHZ,采用外部晶振,由两个宏决定(__SYSTEM_CLOCK_108M_PLL_HXTAL与HXTAL_VALUE)
	rcu_periph_clock_enable(RCU_AF);//管脚复用时钟alternate function clock使能 
	delay_1ms(1000);//等待1秒
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOB时钟使能

	OLED_Init();//OLED初始化  
	OLED_Clear();//OLED清屏 
	GetTemp();
	GetTemp();
	while(1) 
	{		
		GetTemp();//获取温度
	  OLED_Clear();
    OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
		OLED_ShowString(6,3,"DS18B20 Test");
		OLED_ShowString(0,6,"Temperature:");  
		OLED_ShowNum(100,6,Temperature,3,16);//温度值，单位度	   
		delay_1ms(1000);
		
		

	}	 
 
}

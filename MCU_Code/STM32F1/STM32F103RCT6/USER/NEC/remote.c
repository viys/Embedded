#include "remote.h"
#include "systick.h"
#include "usart.h"
#include "led.h"
#include "gentim.h"
#include "ff.h"
#include "fatfsuser.h"
#include "oled.h"

static void GPIO_Config(void)
{
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //使能GPIOB时钟 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 输入   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	     //初始化GPIOB.9
}

static void Tim4_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	   //TIM4 时钟使能 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000-1; //设定计数器自动重装值 最大10ms溢出
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	 0.01s   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

	TIM_ICInitTypeDef  TIM_ICInitStructure;     //输入捕获
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上    ，PB9
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //TIM 输入 4 选择对应地与 IC1 或 IC2 或IC3 或 IC4 相连
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
    TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波，一个定时器周期为（72*10000）72Mhz=0.01s(高于1m的将被率走)
    TIM_ICInit(TIM4, &TIM_ICInitStructure);//初始化定时器输入捕获通道
    
    TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4

}
static void NVIC_Config(void)
{

  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);     //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}

void Remote_Init(void)    			  
{  


    GPIO_Config();  //GPIO初始化
	Tim4_Config();  //TIM4初始化
    NVIC_Config();  //中断优先级设置
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断								 
}

/*NEC码
同步码:由 9ms 低电平+4.5ms 高电平组成
0:560us高，560us低
1:1680us高，560us低
连发码:由 9ms 低电平+2.5ms 高电平+0.56ms 低电平+97.94ms 高电平组成，如果在一帧数据发送完毕之后，
按键仍然没有放开，则发射重复码，即连发码，可以通过统计连发码的次数来标记按键按下的长短/次数。*/
//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	  
//定时器4中断服务程序	
void TIM4_IRQHandler(void)
{ 		    	  
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//上次有数据被接收到了
		{	
			RmtSta&=~0X10;							//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta|=1<<6;	//标记已经完成一次按键的键值信息采集
			}
			if((RmtSta&0X0F)<14)
			{	
				RmtSta++;			
			}
			else
			{
				RmtSta&=~(1<<7);					//清空数据收到标识
				RmtSta&=0XF0;						//清空计数器	
			}								 	   	
		}
	}
	
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)   				//如果捕获到TIM_IT_CC4中断源
	{	  
		if(RDATA)//上升沿捕获
		{
  			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);	//CC4P=1设置为下降沿捕获
			TIM_SetCounter(TIM4,0);								//清空定时器值
			RmtSta|=0X10;										//标记上升沿已经被捕获  
		}
		
		else //下降沿捕获
		{
			Dval=TIM_GetCapture4(TIM4);							//获得 TIMx 输入捕获 4 的值
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);	//CC4P=0设置为上升沿捕获
			if(RmtSta&0X10)										//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)									//接收到了引导码（同步码）
				{
					
					if(Dval>300&&Dval<800)						//560为标准值,560us
					{
						RmtRec<<=1;								//左移一位.
						RmtRec|=0;								//接收到0
					}else if(Dval>1400&&Dval<1800)				//1680为标准值,1680us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}				
					else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						RmtCnt++; 					//按键次数增加1次
						/*此处可放按键次数对应的函数*/
						RmtSta&=0XF0;				//清空计时器在这里知道收集了一个按键的键值信息采集	  
					}
					
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					RmtSta|=1<<7;					//标记成功接收到了引导码（同步码） RmtSta=0x80
					RmtCnt=0;						//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);  						//RmtSta置0
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);//清除 TIMx 的中断待处理位 
	/*此处可以放红外按键对应的事件函数*/
	Remote_Event();
}

#if 0
uint8_t remflag = 0;
uint16_t high_tim;
uint32_t u32date = 0;
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		remflag &= ~0X10;
		if((remflag&0X0F)<15)remflag++;			//一段时间后置位
		else
		{
			remflag&=0X00;//清空数据收到标识
		}
		remflag &= ~(1<<6);
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))//上升沿捕获
		{
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);//设为下降沿捕获
			TIM_SetCounter(TIM4,0);//计时器清零
			remflag|=0X10;
		}else//下降沿捕获
		{
			high_tim = TIM_GetCapture4(TIM4);
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);//设为上升沿捕获
			if(remflag&0x10)
			{
				if(remflag&0x80)
				{
					if(high_tim>300&&high_tim<900)//高电平脉冲为560us 读入0
					{
						u32date <<= 1;
						u32date |= 0;
					}else if(high_tim>1400&&high_tim<1900)//高电平脉冲时间为1680us 读入1
					{
						u32date <<= 1;
						u32date |= 1;
					}else if(high_tim>2200&&high_tim<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						//remflag&=0xF0;				//清空计时器在这里知道收集了一个按键的键值信息采集	此后通过上溢更新中断将其第四位置1 来判断信息采集完成
						remflag|=1<<6;
					}
				}else if(high_tim>4200&&high_tim<4800)//4500同步码的标准值
				{	
					remflag |= 1 <<7;//设置检测到同步码标志位
				}
				remflag &= ~(1<<4);
			}
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);
	if(remflag&0x40)
	{
		u8 sta=0;       
		u8 t1,t2;  
		t1=u32date>>24;			//得到地址码
	    t2=(u32date>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    {
			
	        t1=u32date>>8;   //得到控制码
	        t2=u32date; 	     //得到控制反码
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	remflag&=~(1<<6);//清除接收到有效按键标识
		}
		/*code*/
		switch (t1)
        {
        	case REMOTE_1:printf("1");
        		break;
			case REMOTE_RIGHT:GENERAL_TIM3_Init(CH1,7200,200,20);
        		break;
			case REMOTE_LEFT:GENERAL_TIM3_Init(CH1,7200,200,10);
        		break;
			case REMOTE_OK:GENERAL_TIM3_Init(CH1,7200,200,15);
        		break;
        	default:
        		break;
        }
	}
	remflag &= ~(1<<6);
}
#endif

//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
u8 Remote_Scan(void)//处理红外键盘
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;   //得到控制码
	        t2=RmtRec; 	     //得到控制反码
	        if(t1==(u8)~t2)sta=t1;//键值正确
			RmtSta &= ~(1<<6);
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}
u8 key;
u8 *str=0;
void User_Remote_Scan(void)
{
	key=Remote_Scan();
	if(key)
	{	 
		switch(key)
		{
		
			case 0:str=(u8 *)"ERROR";SysTick_Delay(15);	 break;			   
			case 162:str=(u8 *)"1";SysTick_Delay(15);	 break;	    
			case 98:str=(u8 *)"2";SysTick_Delay(15);	 break;	    
			case 226:str=(u8 *)"3";SysTick_Delay(15);	 break;
			case 34:str=(u8 *)"4";SysTick_Delay(15);	 break;	
			case 2:str=(u8 *)"5";SysTick_Delay(15);		 break;				
			case 194:str=(u8 *)"6";SysTick_Delay(15);	 break;	   	  
			case 224:str=(u8 *)"7";SysTick_Delay(15);	 break;		  
			case 168:str=(u8 *)"8";SysTick_Delay(15);	 break;		   
			case 144:str=(u8 *)"9";SysTick_Delay(15);	 break;		    
			case 104:str=(u8 *)"*";SysTick_Delay(15);	 break;		  
			case 152:str=(u8 *)"0";SysTick_Delay(15);	 break;	   
			case 176:str=(u8 *)"#";SysTick_Delay(15);	 break;	    
			case 24:str=(u8 *)"UP";SysTick_Delay(15);	 break;		    
			case 74:str=(u8 *)"DOWN";SysTick_Delay(15); break;
			case 56:str=(u8 *)"OK";SysTick_Delay(15);	 break;
			case 16:str=(u8 *)"LEFT";SysTick_Delay(15); break;			   						 
			case 90:str=(u8 *)"RIGHT";SysTick_Delay(15);break;	
		}
		printf("%s",str);
	}else SysTick_Delay(12);
}


led_page_typedef_enum oled_page = PAGE_HOME;
static void Turn_Page_UP(void)
{
	if(PAGE_END == oled_page)
	{
		oled_page = PAGE_HOME;
	}else
	{
		oled_page ++;
	}
}
static void Turn_Page_DOWN(void)
{
	if(PAGE_HOME == oled_page)
	{
		oled_page = PAGE_END;
	}else
	{
		oled_page --;
	}
}

uint8_t Revolve = 0;
extern char fpath[100];
//红外遥控按键对应的事件函数
void Remote_Event(void)
{
	u8 remote_num;
	if((remote_num=Remote_Scan()) != 0)
	{
		switch (remote_num)
        {
			case REMOTE_0:printf("HELLO\r\n");
        		break;
        	case REMOTE_1:LED_State_Config(LED3, TOGGLE);
        		break;
			case REMOTE_2:LED_State_Config(LED3, ON);
        		break;
			case REMOTE_3:LED_State_Config(LED3, OFF);
        		break;
			case REMOTE_4:OLED_Clear();
        		break;
			case REMOTE_5:OLED_Refresh();
        		break;
			case REMOTE_6:printf("6\r\n");
        		break;
			case REMOTE_7:printf("7\r\n");
        		break;
			case REMOTE_8:printf("8\r\n");
        		break;
			case REMOTE_9:strcpy(fpath,"1:");scan_files(fpath);printf("The file scan is complete\r\n");
        		break;
			case REMOTE_X:OLED_DisPlay_On();
        		break;
				case REMOTE_J:OLED_DisPlay_Off();
        		break;
				case REMOTE_UP:Turn_Page_UP();
        		break;
				case REMOTE_DOWN:Turn_Page_DOWN();
        		break;
			case REMOTE_RIGHT:GENERAL_TIM3_Init(CH4,7200,200,20);Revolve=2;
        		break;
			case REMOTE_LEFT:GENERAL_TIM3_Init(CH4,7200,200,10);Revolve=1;
        		break;
			case REMOTE_OK:GENERAL_TIM3_Init(CH4,7200,200,15);Revolve=0;
        		break;	
        	default:
        		break;
        }
	}
}

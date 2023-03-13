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
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //ʹ��GPIOBʱ�� 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 ����   
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	     //��ʼ��GPIOB.9
}

static void Tim4_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	   //TIM4 ʱ��ʹ�� 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000-1; //�趨�������Զ���װֵ ���10ms���
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	 0.01s   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

	TIM_ICInitTypeDef  TIM_ICInitStructure;     //���벶��
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // ѡ������� IC4ӳ�䵽TI4��    ��PB9
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //TIM ���� 4 ѡ���Ӧ���� IC1 �� IC2 ��IC3 �� IC4 ����
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
    TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲���һ����ʱ������Ϊ��72*10000��72Mhz=0.01s(����1m�Ľ�������)
    TIM_ICInit(TIM4, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��
    
    TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4

}
static void NVIC_Config(void)
{

  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);     //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
}

void Remote_Init(void)    			  
{  


    GPIO_Config();  //GPIO��ʼ��
	Tim4_Config();  //TIM4��ʼ��
    NVIC_Config();  //�ж����ȼ�����
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�								 
}

/*NEC��
ͬ����:�� 9ms �͵�ƽ+4.5ms �ߵ�ƽ���
0:560us�ߣ�560us��
1:1680us�ߣ�560us��
������:�� 9ms �͵�ƽ+2.5ms �ߵ�ƽ+0.56ms �͵�ƽ+97.94ms �ߵ�ƽ��ɣ������һ֡���ݷ������֮��
������Ȼû�зſ��������ظ��룬�������룬����ͨ��ͳ��������Ĵ�������ǰ������µĳ���/������*/
//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	  
//��ʱ��4�жϷ������	
void TIM4_IRQHandler(void)
{ 		    	  
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;							//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta|=1<<6;	//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			}
			if((RmtSta&0X0F)<14)
			{	
				RmtSta++;			
			}
			else
			{
				RmtSta&=~(1<<7);					//��������յ���ʶ
				RmtSta&=0XF0;						//��ռ�����	
			}								 	   	
		}
	}
	
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)   				//�������TIM_IT_CC4�ж�Դ
	{	  
		if(RDATA)//�����ز���
		{
  			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);	//CC4P=1����Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);								//��ն�ʱ��ֵ
			RmtSta|=0X10;										//����������Ѿ�������  
		}
		
		else //�½��ز���
		{
			Dval=TIM_GetCapture4(TIM4);							//��� TIMx ���벶�� 4 ��ֵ
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);	//CC4P=0����Ϊ�����ز���
			if(RmtSta&0X10)										//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)									//���յ��������루ͬ���룩
				{
					
					if(Dval>300&&Dval<800)						//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;								//����һλ.
						RmtRec|=0;								//���յ�0
					}else if(Dval>1400&&Dval<1800)				//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}				
					else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 					//������������1��
						/*�˴��ɷŰ���������Ӧ�ĺ���*/
						RmtSta&=0XF0;				//��ռ�ʱ��������֪���ռ���һ�������ļ�ֵ��Ϣ�ɼ�	  
					}
					
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					//��ǳɹ����յ��������루ͬ���룩 RmtSta=0x80
					RmtCnt=0;						//�����������������
				}						 
			}
			RmtSta&=~(1<<4);  						//RmtSta��0
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);//��� TIMx ���жϴ�����λ 
	/*�˴����Էź��ⰴ����Ӧ���¼�����*/
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
		if((remflag&0X0F)<15)remflag++;			//һ��ʱ�����λ
		else
		{
			remflag&=0X00;//��������յ���ʶ
		}
		remflag &= ~(1<<6);
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))//�����ز���
		{
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);//��Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);//��ʱ������
			remflag|=0X10;
		}else//�½��ز���
		{
			high_tim = TIM_GetCapture4(TIM4);
			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);//��Ϊ�����ز���
			if(remflag&0x10)
			{
				if(remflag&0x80)
				{
					if(high_tim>300&&high_tim<900)//�ߵ�ƽ����Ϊ560us ����0
					{
						u32date <<= 1;
						u32date |= 0;
					}else if(high_tim>1400&&high_tim<1900)//�ߵ�ƽ����ʱ��Ϊ1680us ����1
					{
						u32date <<= 1;
						u32date |= 1;
					}else if(high_tim>2200&&high_tim<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						//remflag&=0xF0;				//��ռ�ʱ��������֪���ռ���һ�������ļ�ֵ��Ϣ�ɼ�	�˺�ͨ����������жϽ������λ��1 ���ж���Ϣ�ɼ����
						remflag|=1<<6;
					}
				}else if(high_tim>4200&&high_tim<4800)//4500ͬ����ı�׼ֵ
				{	
					remflag |= 1 <<7;//���ü�⵽ͬ�����־λ
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
		t1=u32date>>24;			//�õ���ַ��
	    t2=(u32date>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    {
			
	        t1=u32date>>8;   //�õ�������
	        t2=u32date; 	     //�õ����Ʒ���
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	remflag&=~(1<<6);//������յ���Ч������ʶ
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

//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)//����������
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;   //�õ�������
	        t2=RmtRec; 	     //�õ����Ʒ���
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ
			RmtSta &= ~(1<<6);
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
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
//����ң�ذ�����Ӧ���¼�����
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

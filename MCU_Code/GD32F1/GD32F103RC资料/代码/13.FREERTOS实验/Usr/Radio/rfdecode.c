/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "rfdecode.h"
#include "mytimer.h"
#include "radio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "myQueue.h"
#include "timers.h"
#include "myEventGroup.h"
//--------------------- variable declare ----------------------
volatile unsigned char rx_new_data = 0;
//unsigned char frame_buff[RF_RX_FRAME_MAX][RF_FRAME_LENGTH];
MyFameBuff_t frame_buff[RF_RX_FRAME_MAX];
unsigned char MasterID = CONST_MASTER_ID;
unsigned int SlaveID = CONST_SLAVE_ID;
unsigned int AlertTime = 10;
unsigned char AlertMode = ALERT_MODE_7;
volatile unsigned char Para_set_Enb = DIS_PARA_SET;
volatile unsigned char Slave_Off_Enb = DIS_POW_OFF;
unsigned int Rf_Base_Time = 0;
unsigned char frame_cnt = 0;									// 帧计数器
MyFramePool_t rfFramePool =										// 环形帧缓冲池
{
	.Head = 0,
	.Tail = RF_RX_FRAME_MAX-1,
	.length = 0,
	.frames = {0},
};
TimerHandle_t RfTimerOutTimer = NULL;							// RF超时清除帧缓冲定时器
volatile unsigned char BadSignalCnt = 0;						// 无效信号计数
volatile unsigned char OkSignalCnt = 0;							// 有效信号
volatile unsigned int preBitwidth;								// RF接收中断中，暂存上一次高低电平的宽度
volatile unsigned char firstInRfint = 0;						// 首次进入RF中断标志


/*******************************************************************************
Function:
Descripton:
	无用信号计数器复位
*******************************************************************************/
void ResetBadSignalCnt(void)
{
	BadSignalCnt =0;
}


/*******************************************************************************
Function:
Descripton:
	无用信号计数器加
*******************************************************************************/
void BadSignalCnt_inc(unsigned char step)
{
	if(BadSignalCnt<250)	BadSignalCnt += step;
}

/*******************************************************************************
Function:
Descripton:
	判断无信号计数值是否大于3，如果大于3则说明信号质量差
*******************************************************************************/
int isSignalBad(void)
{
	if(VAL_BAD_SIG_NUM<=BadSignalCnt)	
		return(0);
	else 								
		return(-1);
}

/*******************************************************************************
Function:
Descripton:
	无用信号计数器复位
*******************************************************************************/
void ResetOkSignalCnt(void)
{
	OkSignalCnt =0;
}


/*******************************************************************************
Function:
Descripton:
	无用信号计数器加
*******************************************************************************/
void OkSignalCnt_inc(unsigned char step)
{
	if(OkSignalCnt<250)	OkSignalCnt += step;
}

/*******************************************************************************
Function:
Descripton:
	判断无信号计数值是否大于3，如果大于3则说明信号质量差
*******************************************************************************/
int isSignalOk(void)
{
	if(VAL_OK_SIG_NUM<=OkSignalCnt)	
		return(0);
	else 								
		return(-1);
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
int isBadDataBit(unsigned int HighLevel,unsigned int LowLevel)
{
	int badDatGain = 0;
	
	if( (HighLevel<RF_BASE_TIME_MIN) )
	{
		return(0);
	}

	if(HighLevel>LowLevel)
	{
		badDatGain = (HighLevel+LowLevel)/LowLevel;
	}
	else
	{
		badDatGain = (HighLevel+LowLevel)/HighLevel;
	}

	if( (badDatGain>5) || (badDatGain<2) )
	{
		return(0);
	}
	return(-1);
}



/*******************************************************************************
Function:
Descripton:
	写数到环形缓冲区，写入长度大于数组长度时，覆盖最先写入的数据。
*******************************************************************************/
void Write_rfFramePool(MyFameBuff_t rfFrame)
{
	if(rfFramePool.Tail==(RF_RX_FRAME_MAX+rfFramePool.Head-1)%RF_RX_FRAME_MAX)	// 尾已经和头紧挨着了
	{
		rfFramePool.Head = (rfFramePool.Head+1)%RF_RX_FRAME_MAX;
	}
	rfFramePool.Tail = (rfFramePool.Tail+1)%RF_RX_FRAME_MAX;

    rfFramePool.frames[rfFramePool.Tail].value = rfFrame.value;

	if(++rfFramePool.length > RF_RX_FRAME_MAX)									//判断缓冲区是否已满
	{
		rfFramePool.length = RF_RX_FRAME_MAX;
	}
}

/*******************************************************************************
Function:
Descripton:
	从缓冲池中取一个数据，通过下标指定取哪个数据
*******************************************************************************/
MyFameBuff_t* Read_rfFramePool_by_indx(unsigned char index)
{
	return(&rfFramePool.frames[index]);
}

/*******************************************************************************
Function:
Descripton:
	读出缓冲区数据，每读出一个数据则清除一个。
*******************************************************************************/
signed char Read_rfFramePool(MyFameBuff_t *rfFrame)
{
	if(rfFramePool.length == 0)	return(-1);							//判断非空

	rfFrame->value = rfFramePool.frames[rfFramePool.Head].value;	//先进先出FIFO，从缓冲区头出
	rfFramePool.Head = (rfFramePool.Head+1)%RF_RX_FRAME_MAX;		//防止越界非法访问
	rfFramePool.length--;
	return 0;
}

/*******************************************************************************
Function:
Descripton:
	查询缓冲区，从头往尾部查询
	index - 要查询数据的序号，读出的数据为和Head+index个
*******************************************************************************/
signed char Inquire_rfFramePool_Sequence(MyFameBuff_t *rfFrame,unsigned char index)
{
	if(index>=rfFramePool.length)	return(-1);							// 越界判断

	rfFrame->value = rfFramePool.frames[(rfFramePool.Head+index)%RF_RX_FRAME_MAX].value; 
	return 0;
}


/*******************************************************************************
Function:
Descripton:
	查询缓冲区，从尾部往头部查询
	index - 要查询数据的序号，读出的数据为和Tail-index个
*******************************************************************************/
signed char Inquire_rfFramePool_Reserve(MyFameBuff_t *rfFrame,unsigned char index)
{
	unsigned char position;
	
	if(index>=rfFramePool.length)	return(-1);							// 越界判断

	position = ( ( rfFramePool.Tail + RF_RX_FRAME_MAX ) - index ) % RF_RX_FRAME_MAX;
	rfFrame->value = rfFramePool.frames[position].value;				// 
	return 0;
}


/*******************************************************************************
Function:
Descripton:
	查询缓冲区有效数据长度
*******************************************************************************/
char Inquire_length_rfFramePool(void)
{
	return(rfFramePool.length);
}

/*******************************************************************************
Function:
Descripton:
	清空缓冲区
*******************************************************************************/
void Clean_rfFramePool(void)
{
	rfFramePool.length = 0;
	rfFramePool.Head = 0;
	rfFramePool.Tail = RF_RX_FRAME_MAX-1;
}

/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
static void prvRfTimerOutTimerCallback( TimerHandle_t xTimer )
{
	clean_rf_frame_buffer();
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void clean_rf_buffer_timer_init(void)
{
	if(NULL==RfTimerOutTimer)
	{
		RfTimerOutTimer = xTimerCreate(	"sigle trigger",					// 名字
										TIME_IDLE_BUFFER_CLEAN,			// 重载需要时间
										pdFALSE,						// 不需要重载
										0,							
										prvRfTimerOutTimerCallback );	// 回调函数
													
		if( ( NULL==RfTimerOutTimer ) )
		{
			configASSERT( pdFALSE );
		}	
	}
}

		
/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void clean_rf_buffer_timer_start(void)						// 启动定时器
{
	BaseType_t xTimerStarted;

	xTimerStarted = xTimerStart( RfTimerOutTimer, 0 );
	if( ( pdFAIL==xTimerStarted) )
	{
		configASSERT( xTimerStarted );
	}
}

		
/*******************************************************************************
Function:
Descripton:
		通过接收同步头来设置信号时基，当同步头高电平/低电平 = 31时认为接收到正确的同步头。
		则通过当前接收到的头步头来计算信号时基。
*******************************************************************************/
unsigned int get_rf_base_time(unsigned int sync_head,unsigned int sync_tail)
{
	unsigned int gain = sync_tail/sync_head;
	
	if( (sync_head<RF_BASE_TIME_MIN) || (sync_head>RF_BASE_TIME_MAX) )
	{
		return(INVALID_RF_BASE);
	}
	if( (gain<=36) && (gain>=28) )	
	{
		return(sync_head);
	}
	else 							
	{
		return(INVALID_RF_BASE);
	}
}

/*************************************************************
Function:
Description:
			 invoke the function when RF pin interrupt.
			 use timer input caputure function
*************************************************************/
void rf_decode_PinInt_isr(unsigned int levelTime)
{
	static unsigned int ln_half_bit_h = 0;					// low level time
	static unsigned int ln_half_bit_l = 0;					// time of high level
	static unsigned char bit_cnt = 0;
	static unsigned char byte_cnt = 0;
	static unsigned char DecodeStage = RF_STAGE_RX_SYNC;
	static unsigned char data_temp;
	static unsigned char rx_buff[RF_FRAME_LENGTH] = {0};				// 接收帧缓存(中断中缓存一帧数据)
	MyFameBuff_t	MsgRfFrame = {0};
	int	MsgRfQuality;													// 信号质量标志

		
	// 首次进入中断时先清除无效信号计数和有效信号计数
	if(1==firstInRfint)
	{
		firstInRfint = 0;
		ResetBadSignalCnt();											// 无效信号计数清0
		ResetOkSignalCnt();
		MsgRfFrame.value = 0xffffffff;									// 清空解码任务帧缓冲区命令
		xQueueSendFromISR(myRfFrameQueue,&MsgRfFrame,0);
	}
	
	if( !READ_RF_SIGNAL() )												// falling edge 
	{
		ln_half_bit_h = levelTime;
	}			
	else																// raising edge (receive one bit)
	{
		ln_half_bit_l = levelTime;		
		switch(DecodeStage)
		{
			case RF_STAGE_RX_SYNC:										// receive synchrony signal
			{
				Rf_Base_Time = get_rf_base_time(ln_half_bit_h,ln_half_bit_l); // 获取位时间基准
				if(INVALID_RF_BASE!=Rf_Base_Time)
				{
					bit_cnt = 0;
					byte_cnt = 0;
					ResetBadSignalCnt();
					preBitwidth = 0;
					DecodeStage = RF_STAGE_RX_DATA;
					ResetOkSignalCnt();									// 重新判断
					MsgRfQuality = RF_SIG_OK;
					xQueueSendFromISR(myRfObserverQue,&MsgRfQuality,0);	
				}
				else													// 未接收到同步头时，判断信号质量
				{
					if( 0==isBadDataBit(ln_half_bit_h,ln_half_bit_l) )	// 侦测到无效信号
					{
						BadSignalCnt_inc(1);
						ResetOkSignalCnt();
						preBitwidth = 0;								// 检测到无效信号时不保存数据位周期
						if( 0==isSignalBad() )
						{
							MsgRfQuality = RF_SIG_BAD;
							xQueueSendFromISR(myRfObserverQue,&MsgRfQuality,0);
						}
					}
					else
					{
						if( ((ln_half_bit_h+ln_half_bit_l)<=(preBitwidth+120)) && ((ln_half_bit_h+ln_half_bit_l+120)>=preBitwidth) )
						{
							OkSignalCnt_inc(1);								// 有效计数加1
							if(0==isSignalOk())
							{
								ResetBadSignalCnt();							// 侦测到有效位时，将无效位计数清0
								ResetOkSignalCnt();							// 重新判断
								MsgRfQuality = RF_SIG_OK;
								xQueueSendFromISR(myRfObserverQue,&MsgRfQuality,0);
							}
						}
						preBitwidth = ln_half_bit_h+ln_half_bit_l;
					}
				}
				break;
			}
			case RF_STAGE_RX_DATA:
			{
				data_temp <<= 1;
				if(	( (ln_half_bit_h+ln_half_bit_l)>=(Rf_Base_Time*3) ) &&	\
					( (ln_half_bit_h+ln_half_bit_l)<=(Rf_Base_Time*5) ) )
				{
					if(ln_half_bit_l<=ln_half_bit_h)
					{
						data_temp |= 0x01;
					}
					
					bit_cnt++;
					if(0==bit_cnt%8)
					{
						rx_buff[byte_cnt] = data_temp;
						if(++byte_cnt>=RF_FRAME_LENGTH)					// receive a data frame
						{
							rx_new_data = 1;
							MsgRfFrame.arry[0] = rx_buff[0];
							MsgRfFrame.arry[1] = rx_buff[1];
							MsgRfFrame.arry[2] = rx_buff[2];
							xQueueSendFromISR(myRfFrameQueue,&MsgRfFrame,0);
							DecodeStage = RF_STAGE_RX_SYNC;
						}
					}
				}
				else													// wrong data
				{
					DecodeStage = RF_STAGE_RX_SYNC;
				}
				break;
			}
			default:
				break;
		}
	}
}




/*************************************************************
Function:
Description:
*************************************************************/
signed char traversal_rfFramePool(void)
{
	unsigned char i,j;
	MyFameBuff_t frameTemp0;
	MyFameBuff_t frameTemp1;
		
	for(i=0;i<rfFramePool.length;i++)
	{
		for(j=i+1;j<rfFramePool.length;j++)
		{
			if(Inquire_rfFramePool_Sequence(&frameTemp0,i))	break;			// 缓冲区遍历完
			if(Inquire_rfFramePool_Sequence(&frameTemp1,j))	break;			// 缓冲区遍历完
				
			if(frameTemp0.value == frameTemp1.value)						// 搜索到两帧相同数据
			{
				return(0);
			}
		}
	}
	return(-1);
}
	
/*************************************************************
Function:
Description:
			将最新数据与缓冲区中的其它数据比对，或比对成功即表示帧有效。
			遍历方式为从尾向头反向遍历。
*************************************************************/
signed char traversal_rfFramePool_Reverse(void)
{
	unsigned char i;
	MyFameBuff_t* addrPreFrame;
	MyFameBuff_t* addrNewFrame;
		
	if(rfFramePool.Tail>=3)
	{
		__nop();
	}
	
	if(rfFramePool.length<=1)	return(-1);								// 只有一帧数据，不用比对
	
	addrNewFrame =  Read_rfFramePool_by_indx( rfFramePool.Tail );
	
	for(i=1;i<rfFramePool.length;i++)
	{
		addrPreFrame =  Read_rfFramePool_by_indx( (RF_RX_FRAME_MAX+rfFramePool.Tail-i)%RF_RX_FRAME_MAX );

		if(addrNewFrame->value==addrPreFrame->value)					// 搜索到两帧相同数据
		{
			return(0);
		}
	}
	
	return(-1);
}

/*************************************************************
Function:
Description:
*************************************************************/
unsigned int get_frame_interval(unsigned int currentTick,unsigned int prevousTick)
{
	if(currentTick<prevousTick)
	{
		return( (currentTick+0xffffff)-prevousTick );
	}
	else 
	{
		return(currentTick-prevousTick);
	}
}


/*************************************************************
Function:
Description:
	接收到一个数据帧，然后和之前的进行比对，比对成功则认为数据帧有效
*************************************************************/
void rf_validFrame_check(unsigned char *newFrame)
{
	Msg_VaildFrame_t ValidFrame;
	MyFameBuff_t frame_temp = {0};
	
	// 先将数据存到缓冲池
	frame_temp.arry[0] = newFrame[0];
	frame_temp.arry[1] = newFrame[1];
	frame_temp.arry[2] = newFrame[2];
	Write_rfFramePool(frame_temp);												// 将数据存入缓冲区
	
	// 从缓冲池中取出数据比对
	if( 0==traversal_rfFramePool_Reverse() )									// 帧比对成功
	{
		ValidFrame.value = frame_temp.value;
		xQueueSend(myDecodeQueue,&ValidFrame,0);								// 将有效数据帧发给MENU任务
		Clean_rfFramePool();													// 比对成功则清空缓冲区
	}
		
}


/*******************************************************************************
Function:
Descripton:
*******************************************************************************/
void clean_rf_frame_buffer(void)
{		
	frame_cnt = 0;
}


/****************************************************************************************
Function:
Description: 
    \param[in]  array address:Arry1,Arry2
    \param[in]  array length: Ln1,Ln2
    \param[out] none
    \retval     result of compare(0-success,-1-failed)
****************************************************************************************/
int my_compare_arrays(unsigned char *Arry1,int Ln1,unsigned char *Arry2,int Ln2)
{
	int i;
	
	if(Ln1!=Ln2)	return(-1);

	for(i=0;i<Ln1;i++)
	{
		if(Arry1[i]!=Arry2[i])	return(-1);
	}
	
	return(0);
}


/*************************************************************
Function:
Description:
*************************************************************/
void setting_enable(unsigned char flag)
{
	Para_set_Enb = flag;
}

/*************************************************************
Function:
Description:
*************************************************************/
void power_off_enable(unsigned char flag)
{
	Slave_Off_Enb = flag;
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
int RF_module_sleep(void)
{
	return(CMT2300A_GoSleep());							// RF睡眠
}

/****************************************************************************************
Function:
Description:
****************************************************************************************/
int RF_module_goto_rx(void)
{
	return(CMT2300A_GoRx());							// RF进入接收模式
}

/****************************************************************************************
Function:
Description:
				CMT2219B配置函数
****************************************************************************************/
void CMT2219B_Config(void)
{
	RF_Init();

	if(FALSE==CMT2300A_IsExist()) 								// 芯片检测
	{
		configASSERT( FALSE );
	}

	CMT2300A_GoStby();											// 切换到standby模式

	/* Must clear FIFO after enable SPI to read or write the FIFO */
	CMT2300A_EnableReadFifo();
	CMT2300A_ClearInterruptFlags();
	CMT2300A_ClearRxFifo();
	CMT2300A_GoRx();											// 切换到RX模式
}

/*******************************************************************************
Function:
Descripton:
	cmt2219b的GIPO3被配置为RF信号出口。使用定时器捕捞方式接收信号
	此函数用于清除RF中断中信号侦测计数器
*******************************************************************************/
void rf_decode_control(unsigned char flag)
{
	if(flag)	
	{
		firstInRfint = 1;
	}
	else		
	{
		firstInRfint = 0;
	}
}

/*************************************************************
Function:
Description:
	set timer2 as rf input caputure
*************************************************************/
void rf_decode_init(void)
{
	IO_RF_SIGNAL_INIT1();
	RF_TIMER_INIT();
	CMT2219B_Config();
    RF_TIMER_NVIC_SET();									// enable tf timer interrupt
	RF_TIMER_ENB(1);
}


/*******************************************************************************
Function:
Descripton:
	cmt2219b的GIPO3被配置为RF信号出口。使用定时器捕捞方式接收信号
	remove RF control queue.RF reception is controlled by interrupt mask
*******************************************************************************/
void rf_frame_filter_task(void * pvParameters)
{
	Msg_VaildFrame_t	MsgRfFrame;
	BaseType_t	ret;
	volatile static unsigned int preTickCnt = 0;
	volatile unsigned int frameInterval = 0;

	vTaskDelay(pdMS_TO_TICKS(20));											// ��ʱ20MS
	rf_decode_init();														// RF���ն˿ڳ�ʼ��
		
	while(1)
	{
		// receive queue of RF frame from interrut service routine
		ret = xQueueReceive(myRfFrameQueue,&MsgRfFrame,pdMS_TO_TICKS(100));	// 接收到新的RF帧
		if(pdTRUE==ret)
		{
			// is there a control command?
			if(0xffffffff==MsgRfFrame.value)								// 清帧缓冲命令
			{
				Clean_rfFramePool();											// 清帧缓冲
			}
			else
			{
				rf_validFrame_check(MsgRfFrame.frame);							// 检测是否为有效帧(两帧相同时认为有效)
			}
		}		

	}
}



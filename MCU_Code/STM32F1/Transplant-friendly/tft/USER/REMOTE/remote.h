#ifndef __RED_H_
#define __RED_H_
#include "stm32f10x.h" 


#define RDATA 	 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)	 	//�������������

//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0      		   

#define  REMOTE_ERROR	          0
#define  REMOTE_1                 162
#define  REMOTE_2                 98
#define  REMOTE_3                 226
#define  REMOTE_4                 34
#define  REMOTE_5	              2
#define  REMOTE_6                 194
#define  REMOTE_7                 224
#define  REMOTE_8                 168
#define  REMOTE_9                 144
#define  REMOTE_X	              104
#define  REMOTE_0                 152
#define  REMOTE_J                 176
#define  REMOTE_UP                24
#define  REMOTE_DOWN              74
#define  REMOTE_OK                56
#define  REMOTE_LEFT	          16	   						 
#define  REMOTE_RIGHT	          90

extern u8 RmtCnt;			//�������µĴ���

void Remote_Init(void);    	//���⴫��������ͷ���ų�ʼ��
u8 Remote_Scan(void);	 
void User_Remote_Scan(void);
void Remote_Event(void);

#endif

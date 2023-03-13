/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
 
#include "radio.h"
#include "cmt2300a_params.h"

#include <string.h>

volatile unsigned char cmtReg_Value = 0;

/*************************************************************************************
Function:
Description:
*************************************************************************************/
void RF_Init(void)
{
	u8 tmp;

	CMT2300A_InitGpio();											// 通信端口初始化
	CMT2300A_Init();												// 芯片复位

	/* Config registers */
	CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
	CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
	CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
	CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
	CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
	CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );

	// xosc_aac_code[2:0] = 2
	tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
	CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp|0x02);

	RF_Config();													// RF数据口和中断品配置
	
	cmtReg_Value = 0x00;
	cmtReg_Value = CMT2300A_ReadReg(CMT2300A_CUS_MODE_CTL);
	__nop();
	cmtReg_Value = 0x00;
	cmtReg_Value = CMT2300A_ReadReg(CMT2300A_CUS_CMT2);
	__nop();
//    CMT2300A_WriteReg(CMT2300A_CUS_MODE_CTL, CMT2300A_GO_STBY);
//	__nop();
//	cmtReg_Value = 0x00;
//	cmtReg_Value = CMT2300A_ReadReg(CMT2300A_CUS_MODE_CTL);
//	__nop();
//    CMT2300A_WriteReg(CMT2300A_CUS_MODE_CTL, CMT2300A_GO_SLEEP);
//	__nop();
//	cmtReg_Value = 0x00;
//	cmtReg_Value = CMT2300A_ReadReg(CMT2300A_CUS_MODE_CTL);
//	__nop();
	
	
}

/*************************************************************************************
Function:
Description:
				配置中断口
*************************************************************************************/
void RF_Config(void)
{
#ifdef ENABLE_ANTENNA_SWITCH
    /* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
       and it can't output INT1/INT2 via GPIO1/GPIO2 */
    CMT2300A_EnableAntennaSwitch(0);
    
#else
    /* Config GPIOs */
    CMT2300A_ConfigGpio(
							CMT2300A_GPIO1_SEL_INT1 | /* INT1 > GPIO1 */
							CMT2300A_GPIO2_SEL_INT2 | /* INT2 > GPIO2 */
							CMT2300A_GPIO3_SEL_DOUT
						);
    
	/* Config interrupt 设置INT1和INT2 */
	CMT2300A_ConfigInterrupt(
								CMT2300A_INT_SEL_TX_DONE,  /* Config INT1 */
								CMT2300A_INT_SEL_PKT_DONE  /* Config INT2 */
						);
#endif

	/* Enable interrupt */
	CMT2300A_EnableInterrupt(
								CMT2300A_MASK_TX_DONE_EN  |
								CMT2300A_MASK_PKT_DONE_EN
								//CMT2300A_MASK_PREAM_OK_EN |
								//CMT2300A_MASK_SYNC_OK_EN  |
								//CMT2300A_MASK_NODE_OK_EN  |
								//CMT2300A_MASK_CRC_OK_EN   
							);

    
    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    
    /* Use a single 64-byte FIFO for either Tx or Rx */
    //CMT2300A_EnableFifoMerge(TRUE);
    
    //CMT2300A_SetFifoThreshold(16); // FIFO_TH
    
    /* This is optional, only needed when using Rx fast frequency hopping */
    /* See AN142 and AN197 for details */
    //CMT2300A_SetAfcOvfTh(0x27);
    
    /* Go to sleep for configuration to take effect */
		
		
		
    CMT2300A_GoSleep();
}



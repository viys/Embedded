/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2022 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           PIC24 MPLab Port
*
* File    : os_cpu.h
* Version : V3.08.02
*********************************************************************************************************
*/

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                           INCLUDES
*********************************************************************************************************
*/

#include <p24FJ128GA010.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                         PROCESSOR SPECIFICS
*********************************************************************************************************
*/

#define OS_STK_GROWTH       0                                    /* Stack grows from low to high memory            */

#define OS_TASK_SW()        {__asm__ volatile("call _OSCtxSw");} /* Macro for defining a high level context switch */


/*
*********************************************************************************************************
*                                       TIMESTAMP CONFIGURATION
*
* Note(s) : (1) OS_TS_GET() is generally defined as CPU_TS_Get32() to allow CPU timestamp timer to be of
*               any data type size.
*
*           (2) For architectures that provide 32-bit or higher precision free running counters
*               (i.e. cycle count registers):
*
*               (a) OS_TS_GET() may be defined as CPU_TS_TmrRd() to improve performance when retrieving
*                   the timestamp.
*
*               (b) CPU_TS_TmrRd() MUST be configured to be greater or equal to 32-bits to avoid
*                   truncation of TS.
*********************************************************************************************************
*/

#if      OS_CFG_TS_EN == 1u
#define  OS_TS_GET()               (CPU_TS)CPU_TS_Get32()   /* See Note #2a.                                          */
#else
#define  OS_TS_GET()               (CPU_TS)0u
#endif


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_CPU_EXT  CPU_STK  *OS_CPU_ExceptStkBase;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  OSCtxSw       (void);
void  OSIntCtxSw    (void);
void  OSStartHighRdy(void);


#ifdef __cplusplus
}
#endif

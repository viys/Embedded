/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

#if __DATA_MODEL__ == __DATA_MODEL_FAR__ && __CODE_MODEL__ == __CODE_MODEL_NEAR__
	#warning This port has not been tested with your selected memory model combination. If a far data model is required it is recommended to also use a far code model.
#endif

#if __DATA_MODEL__ == __DATA_MODEL_NEAR__ && __CODE_MODEL__ == __CODE_MODEL_FAR__
	#warning This port has not been tested with your selected memory model combination. If a far code model is required it is recommended to also use a far data model.
#endif
	
/* Type definitions. */

#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  unsigned short
#define portBASE_TYPE   short

#if __DATA_MODEL__ == __DATA_MODEL_FAR__
	#define portPOINTER_SIZE_TYPE unsigned long
#else
	#define portPOINTER_SIZE_TYPE unsigned short
#endif

	
#if ( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned int portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned long portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/	

/* Interrupt control macros. */
#define portDISABLE_INTERRUPTS() __asm ( "DI" )
#define portENABLE_INTERRUPTS()	 __asm ( "EI" )
/*-----------------------------------------------------------*/

/* Critical section control macros. */
#define portNO_CRITICAL_SECTION_NESTING		( ( unsigned portSHORT ) 0 )

#define portENTER_CRITICAL()													\
{																				\
extern volatile unsigned short usCriticalNesting;								\
																				\
	portDISABLE_INTERRUPTS();													\
																				\
	/* Now interrupts are disabled ulCriticalNesting can be accessed */			\
	/* directly.  Increment ulCriticalNesting to keep a count of how many */	\
	/* times portENTER_CRITICAL() has been called. */							\
	usCriticalNesting++;														\
}

#define portEXIT_CRITICAL()														\
{																				\
extern volatile unsigned short usCriticalNesting;								\
																				\
	if( usCriticalNesting > portNO_CRITICAL_SECTION_NESTING )					\
	{																			\
		/* Decrement the nesting count as we are leaving a critical section. */	\
		usCriticalNesting--;													\
																				\
		/* If the nesting level has reached zero then interrupts should be */	\
		/* re-enabled. */														\
		if( usCriticalNesting == portNO_CRITICAL_SECTION_NESTING )				\
		{																		\
			portENABLE_INTERRUPTS();											\
		}																		\
	}																			\
}
/*-----------------------------------------------------------*/

/* Task utilities. */
#define portYIELD()	__asm( "BRK" )
#define portYIELD_FROM_ISR( xHigherPriorityTaskWoken ) if( xHigherPriorityTaskWoken ) vTaskSwitchContext()
#define portNOP()	__asm( "NOP" )
/*-----------------------------------------------------------*/

/* Hardwware specifics. */
#define portBYTE_ALIGNMENT	2
#define portSTACK_GROWTH	( -1 )
#define portTICK_RATE_MS	( ( portTickType ) 1000 / configTICK_RATE_HZ )		
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/* --------------------------------------------------------------------------*/
/* Option-bytes and security ID                                              */
/* --------------------------------------------------------------------------*/
#define OPT_BYTES_SIZE     4
#define SECU_ID_SIZE       10
#define WATCHDOG_DISABLED  0x00
#define LVI_ENABLED        0xFE
#define LVI_DISABLED       0xFF
#define RESERVED_FF        0xFF
#define OCD_DISABLED       0x04
#define OCD_ENABLED        0x81
#define OCD_ENABLED_ERASE  0x80

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

